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

namespace triqs::gfs::details {

  using triqs::arrays::range_all;

  // "Lambda" : X = argument, M = mesh, Tu = tuple of results
  // Action : accumulate the m in tu iif X is a all_t
  // To be used in partial_eval in a fold to filter the tuple of mesh
  struct filter_mesh {
    template <typename X, typename M, typename Tu> auto operator()(X const &x, M const &m, Tu &&tu) const { return std::forward<Tu>(tu); }
    template <typename M, typename Tu> auto operator()(range_all, M const &m, Tu &&tu) const {
      return std::tuple_cat(std::forward<Tu>(tu), std::tie(m));
    }
  };

  // a simple test for the argument of G[...] to have an early error and short error message.
  template <typename Mesh, typename A> constexpr bool is_ok1() {
    return std::is_same<typename Mesh::mesh_point_t, A>::value || std::is_constructible<long, A>::value || std::is_constructible<_long, A>::value
       || std::is_same<A, all_t>::value || std::is_same<std::array<long, 3>, A>::value || std::is_same<matsubara_freq, A>::value;
    // FIXME : all std::array. with NDA clean
  }
  template <typename Mesh, typename... A> struct is_ok { static constexpr bool value = is_ok1<Mesh, std::decay_t<A>...>(); };
  template <typename... T, typename... A> struct is_ok<mesh::prod<T...>, A...> {
    static constexpr bool value = (is_ok1<T, std::decay_t<A>>() and ...);
  };

  // -----------------------------------------
  //           slice_or_access
  //
  //
  // g : a gf, gf_view, gf_const_view container.
  // args must be range_all or long (linear_indices of meshes)
  // returns either g.data[args...] if args contains no  all_t
  // or a new view if some args are all_t
  //
  template <typename G, typename... Args> decltype(auto) slice_or_access(G &g, Args const &... args) {

    static_assert(((std::is_same_v<range_all, Args> or std::is_same_v<long, Args>)and...), "Internal error : unexpected type in slice_or_access");

    if constexpr (not(std::is_same_v<range_all, Args> or ...))
      return g.on_mesh_from_linear_index(args...);
    else {

      // FIXME : use a bit set and a constexpr function
      // Filter the meshes of g into a tuple containing only the ones corresponding to all_t arguments
      auto m_tuple = triqs::tuple::fold(filter_mesh{}, std::make_tuple(args...), g.mesh().components(), std::make_tuple());
      // slice the data
      auto arr2 = g.data()(args..., triqs::arrays::ellipsis());

      // Build the new mesh. If tuple size > 1 , it is a prod mesh, otherwise just extract the mesh from the tuple
      auto mesh = [&m_tuple]() {
        if constexpr (std::tuple_size_v<decltype(m_tuple)> == 1) {
          return std::get<0>(m_tuple);
        } else {
          return mesh::prod{m_tuple};
        }
      }();
      using mesh_t = decltype(mesh);
      if constexpr (G::is_const or std::is_const<G>::value)
        return gf_const_view<mesh_t, typename G::target_t>{std::move(mesh), arr2, g.indices()};
      else
        return gf_view<mesh_t, typename G::target_t>{std::move(mesh), arr2, g.indices()};
    }
  }

  //-----------------------------------------------
  //
  template <typename M> FORCEINLINE auto get_linear_index_from_mesh_and_arg(M const &m, typename M::mesh_point_t const &x) {
    return x.linear_index();
  }
  template <typename M> FORCEINLINE auto get_linear_index_from_mesh_and_arg(M const &m, typename M::index_t const &x) { return m.index_to_linear(x); }
  template <typename M> FORCEINLINE range_all get_linear_index_from_mesh_and_arg(M &&, all_t) { return {}; }

  //-----------------------------------------------

  //
  // Calls slice_or_access, but args cen be all_t, mesh_point, index_t
  //
  template <size_t... Is, typename G, typename... Args>
  decltype(auto) slice_or_access_general_impl(std::index_sequence<Is...>, G &g, Args const &... args) {
    return slice_or_access(g, get_linear_index_from_mesh_and_arg(std::get<Is>(g.mesh()), args)...);
  }
  //-----------------------------------------------

  template <typename G, typename... Args> decltype(auto) slice_or_access_general(G &g, Args const &... args) {

    if constexpr (not(... or std::is_same_v<all_t, Args>))
      return g.on_mesh(args...);
    else {
      static_assert(sizeof...(Args) > 1, "Can not use all_t in a single gf"); // FIXME ? Generalize
      return slice_or_access_general_impl(std::index_sequence_for<Args...>{}, g, args...);
    }
  }

} // namespace triqs::gfs::details
