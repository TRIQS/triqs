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

namespace triqs::gfs {

  /// Short cuts. Experimental
  template <typename G> decltype(auto) first_mesh(G &&g) { return std::get<0>(std::forward<G>(g).mesh()); }
  template <typename G> decltype(auto) second_mesh(G &&g) { return std::get<1>(std::forward<G>(g).mesh()); }
  template <typename G> decltype(auto) third_mesh(G &&g) { return std::get<2>(std::forward<G>(g).mesh()); }
  template <typename G> decltype(auto) fourth_mesh(G &&g) { return std::get<3>(std::forward<G>(g).mesh()); }

  namespace details {

    // "Lambda" : X = argument, M = mesh, Tu = tuple of results
    // Action : accumulate the m in tu iif X is a all_t
    // To be used in partial_eval in a fold to filter the tuple of mesh
    struct fw_mesh {
      template <typename X, typename M, typename Tu> auto operator()(X const &x, M const &m, Tu &&tu) const { return std::forward<Tu>(tu); }
      template <typename M, typename Tu> auto operator()(all_t, M const &m, Tu &&tu) const {
        return std::tuple_cat(std::forward<Tu>(tu), std::tie(m));
      }
    };

    // "Lambda" : X = argument, M = mesh, Tu = tuple of results
    // Action : accumulate the linear_index for x in the mesh m in tu iif X is NOT a all_t
    // To be used in partial_eval
    struct fw_mesh_x {
      template <typename M, typename Tu> auto operator()(M const &m, typename M::mesh_point_t const &x, Tu &&tu) const {
        return std::tuple_cat(std::forward<Tu>(tu), std::make_tuple(x.linear_index()));
      }
      template <typename M, typename Tu> auto operator()(M const &m, typename M::index_t const &x, Tu &&tu) const {
        return std::tuple_cat(std::forward<Tu>(tu), std::make_tuple(m.index_to_linear(x)));
      }
      template <typename M, typename Tu> auto operator()(M &&, all_t, Tu &&tu) const {
        return std::tuple_cat(std::forward<Tu>(tu), std::make_tuple(range{}));
      }
    };

    // a simple test for the argument of G[...] to have an early error and short error message.
    template <typename Mesh, typename A> constexpr bool is_ok1() {
      return std::is_same<typename Mesh::mesh_point_t, A>::value || std::is_constructible<long, A>::value || std::is_constructible<_long, A>::value
         || std::is_same<A, all_t>::value || std::is_same<mini_vector<long, 3>, A>::value || std::is_same<matsubara_freq, A>::value;
      // FIXME : all std::array. with NDA clean
    }
    template <typename Mesh, typename... A> struct is_ok { static constexpr bool value = is_ok1<Mesh, std::decay_t<A>...>(); };
    template <typename... T, typename... A> struct is_ok<mesh::prod<T...>, A...> {
      static constexpr bool value = clef::__and(is_ok1<T, std::decay_t<A>>()...);
    };

    ///
    template <typename G, typename... Args> decltype(auto) partial_eval(G &g, Args const &... args) {

      static_assert(is_ok<typename G::mesh_t, std::decay_t<Args>...>::value, "Argument type incorrect");

      if constexpr (not(... or std::is_same_v<all_t, Args>))
        return g.on_mesh(args...);
      else {

        // Filter the mesh of g into m to keep only the meshes corresponding to all_t arguments
        auto m_tuple = triqs::tuple::fold(details::fw_mesh{}, std::make_tuple(args...), g.mesh().components(), std::make_tuple());

        // computing the template parameters of the returned gf, i.e. r_t
        using target_t = typename G::target_t; // unchanged

        // compute the sliced view of the data array
        // xs : filter the arguments which are not all_t and compute their linear_index with the corresponding mesh
        // then in arr2, we unfold the xs tuple into the slice of the data
        auto xs   = triqs::tuple::fold(details::fw_mesh_x{}, g.mesh().components(), std::make_tuple(args...), std::make_tuple());
        auto arr2 = triqs::tuple::apply([&g](auto const &... args) { return g.data()(args..., triqs::arrays::ellipsis()); }, xs);

	// two cases : the tuple is of size 1 or not
        auto mesh = [&m_tuple]() {
          if constexpr (std::tuple_size_v<decltype(m_tuple)> == 1) {
            return std::get<0>(m_tuple);
          } else {
            return mesh::prod{m_tuple};
          }
        }();
	using mesh_t = decltype(mesh);
        if constexpr (G::is_const or std::is_const<G>::value)
          return gf_const_view<mesh_t, target_t>{std::move(mesh), arr2, g->indices()};
        else
          return gf_view<mesh_t, target_t>{std::move(mesh), arr2, g->indices()};
      }
    }
  } // namespace details
} // namespace triqs::gfs
