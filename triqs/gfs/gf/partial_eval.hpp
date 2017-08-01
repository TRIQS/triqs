/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2016 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once

namespace triqs {
 namespace gfs {

  /// The placeholder
  struct var_t {};

  /// Short cuts. Experimental
  template <typename G> decltype(auto) first_mesh(G&& g) { return std::get<0>(std::forward<G>(g).mesh()); }
  template <typename G> decltype(auto) second_mesh(G&& g) { return std::get<1>(std::forward<G>(g).mesh()); }
  template <typename G> decltype(auto) third_mesh(G&& g) { return std::get<2>(std::forward<G>(g).mesh()); }
  template <typename G> decltype(auto) fourth_mesh(G&& g) { return std::get<3>(std::forward<G>(g).mesh()); }

  namespace details {

   // A function to recognize var_t
   inline constexpr bool is_var_t(var_t) { return true; }
   template <typename T> constexpr bool is_var_t(T const&) { return false; }

   // "Lambda" : X = argument, M = mesh, Tu = tuple of results
   // Action : accumulate the m in tu iif X is a var_t
   // To be used in partial_eval in a fold to filter the tuple of meshes
   struct fw_mesh {
    template <typename X, typename M, typename Tu> auto operator()(X const& x, M const& m, Tu&& tu) const {
     return std::forward<Tu>(tu);
    }
    template <typename M, typename Tu> auto operator()(var_t, M const& m, Tu&& tu) const {
     return std::tuple_cat(std::forward<Tu>(tu), std::tie(m));
    }
   };

   // Metafunction:
   // arguments : a tuple of meshes M
   // returns :
   // - if tuple is of size 1, M::var_t
   // - else cartesian_product<M::var_t ...>
   // with a filter function to be apply on the tuple mesh
   template <typename... M> struct mesh_to_var;
   // the & ensures that it is called with & hence no copy of mesh is done before here
   // copy of the mesh is done now
   template <typename M> struct mesh_to_var<std::tuple<M&>> {
    using type = typename M::var_t;
    static M& filter(std::tuple<M&> const& t) { return std::get<0>(t); }
   };
   template <typename M1, typename M2, typename... M> struct mesh_to_var<std::tuple<M1&, M2&, M&...>> {
    using type = cartesian_product<typename M1::var_t, typename M2::var_t, typename M::var_t...>;
    static gf_mesh<type> filter(std::tuple<M1&, M2&, M&...> const& t) { return triqs::tuple::apply_construct<gf_mesh<type>>(t); }
   };

   // "Lambda" : X = argument, M = mesh, Tu = tuple of results
   // Action : accumulate the linear_index for x in the mesh m in tu iif X is NOT a var_t
   // To be used in partial_eval
   struct fw_mesh_x {
    template <typename M, typename Tu> auto operator()(M const& m, typename M::mesh_point_t const& x, Tu&& tu) const {
     return std::tuple_cat(std::forward<Tu>(tu), std::make_tuple(x.linear_index()));
    }
    template <typename M, typename Tu> auto operator()(M const& m, typename M::index_t const& x, Tu&& tu) const {
     return std::tuple_cat(std::forward<Tu>(tu), std::make_tuple(m.index_to_linear(x)));
    }
    template <typename M, typename Tu> auto operator()(M&&, var_t, Tu&& tu) const {
     return std::tuple_cat(std::forward<Tu>(tu), std::make_tuple(range{}));
    }
   };

   // Two helper function for a compile time decision. Workaround
   template <typename Rt, typename M, typename A, typename S> Rt make_r_t(M&& m, A&& a, S&& s, std::true_type) {
    return Rt{std::forward<M>(m), a, s, {}}; // a is view, copy is fine
   }
   template <typename Rt, typename M, typename A, typename S> Rt make_r_t(M&& m, A&& a, S&& s, std::false_type) {
    return Rt{std::forward<M>(m), a};
   }

   // FIXME : C++17 replace this dispatch
   template <typename S, typename A, typename B> auto partial_eval_singularity(std::true_type, S&& s, A const& a, B const&) {
    return s[a];
   }
   template <typename S, typename... A> auto partial_eval_singularity(std::false_type, S&& s, A const&...) { return nothing{}; }

   // FIXME : C++17 replace dispatch by constexpr
   ///
   template <typename G, typename... Args> auto partial_eval1(std::true_type, G* g, Args const&... args) {

    // if constexpr (not (or ... is_var(args))) return g->on_mesh(args...);
    // else ...

    // Filter the mesh of g into m to keep only the meshes corresponding to var_t arguments
    auto m_tuple = triqs::tuple::fold(details::fw_mesh{}, std::make_tuple(args...), g->mesh().components(), std::make_tuple());

    // if m_tuple is of size 1, remove the tuple.
    using mtv = details::mesh_to_var<decltype(m_tuple)>;
    auto m    = mtv::filter(m_tuple); // copy of the meshes is done here, m_tuple was only

    // computing the template parameters of the returned gf, i.e. r_t
    using var_t    = typename mtv::type;   // deduced from the tuple of mesh.
    using target_t = typename G::target_t; // unchanged
    using r_t =
        std14::conditional_t<G::is_const or std::is_const<G>::value, gf_const_view<var_t, target_t>, gf_view<var_t, target_t>>;

    // compute the sliced view of the data array
    // xs : filter the arguments which are not var_t and compute their linear_index with the corresponding mesh
    // then in arr2, we unfold the xs tuple into the slice of the data
    auto xs   = triqs::tuple::fold(details::fw_mesh_x{}, g->mesh().components(), std::make_tuple(args...), std::make_tuple());
    auto arr2 = triqs::tuple::apply([&g](auto const&... args) { return g->data()(args..., triqs::arrays::ellipsis()); }, xs);

    // We now also partial_eval the singularity
    // The rule is :
    // If we have 2 variables AND the var_t is the second arguments, then call singularity[first_argument]
    // This covers the m_tail, used for g(k,omega), g(x,t), ....
    // In all others cases, return nothing
    using TArgs = std::tuple<Args...>;
    // for some mysterious reason, I need to quality var_t on clang or it does not work ... Collision ?
    constexpr bool B = (sizeof...(Args) == 2) and std::is_same<std::tuple_element_t<1, TArgs>, triqs::gfs::var_t>::value and
        not std::is_same<std::tuple_element_t<0, TArgs>, triqs::gfs::var_t>::value;

    auto singv = partial_eval_singularity(std::integral_constant<bool, B>{}, g->singularity(), args...);
    using r_sing_t = typename decltype(singv)::regular_type;
    // finally, we build the view on this data.
    // two cases : if type(singv) == type (r_t.singularity) or not. If not, we rebuild a default singularity

    return details::make_r_t<r_t>(std::move(m), arr2, singv, std::is_same<r_sing_t, typename r_t::_singularity_regular_t>{});

    // FIXME C++17 : remove the dispatch and
    // if constexpr (std::is_same<r_sing_t, typename r_t::_singularity_regular_t>::value)
    // return r_t{m, arr2, singv, {}};
    // else
   }

   ///
   template <typename G, typename... Args> decltype(auto) partial_eval1(std::false_type, G* g, Args const&... args) {
    // FIXME : get_linear_ ... or on_mesh, it depends what is passed !!!
    return g->on_mesh(args...);
   }

   ///
   template <typename G, typename... Args> decltype(auto) partial_eval(G* g, Args const&... args) {
    constexpr bool one_is_var = clef::__or(std::is_same<Args, var_t>::value...);
    return partial_eval1(std::integral_constant<bool, one_is_var>{}, g, args...);
   }

  } // namespace details
 }
}
