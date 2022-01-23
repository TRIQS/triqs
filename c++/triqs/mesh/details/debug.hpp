// additional functions for debug sections

namespace triqs::mesh {

  // check if mesh_point mp is compatible with mesh m. Intented for debug mode only.
  template <typename MP, typename T> bool mesh_point_compatible_to_mesh(MP const &mp, T const &m) { return mp.mesh_hash() == m.mesh_hash(); }

  // // special case for imtime : do not check the statistic !
  // template <typename MP> bool mesh_point_compatible_to_mesh(MP const &mp, imtime const &m) {
  //   return (std::abs(m.domain().beta - mp.mesh().domain().beta) < 1.e-15) and (m.size() == mp.mesh().size());
  // }

  // special case for cartesian product, it must be checked for every component.
  template <typename MP, typename... M, size_t... Is>
  bool mesh_point_compatible_to_mesh_impl(std::index_sequence<Is...>, MP const &mp, prod<M...> const &m) {
    return (mesh_point_compatible_to_mesh(std::get<Is>(mp.components_tuple()), std::get<Is>(m.components())) and ...);
  }
  template <typename... M> bool mesh_point_compatible_to_mesh(mesh_point<prod<M...>> const &mp, prod<M...> const &m) {
    return mesh_point_compatible_to_mesh_impl(std::index_sequence_for<M...>{}, mp, m);
  }
} // namespace triqs::mesh
