// additional functions for debug sections

namespace triqs::gfs {

  template <typename... Ms> struct cartesian_product;

  // check if mesh_point mp is compatible with mesh m. Intented for debug mode only.
  template <typename MP, typename T> bool mesh_point_compatible_to_mesh(MP const &mp, gf_mesh<T> const &m) { return mp.mesh() == m; }

  // special case for imtime : do not check the statistic !
  template <typename MP> bool mesh_point_compatible_to_mesh(MP const &mp, gf_mesh<imtime> const &m) {
    return (std::abs(m.domain().beta - mp.mesh().domain().beta) < 1.e-15) and (m.size() == mp.mesh().size());
  }

  // special case for cartesian product, it must be checked for every component.
  template <typename MP, typename... M, size_t... Is>
  bool mesh_point_compatible_to_mesh_impl(std::index_sequence<Is...>, MP const &mp, gf_mesh<cartesian_product<M...>> const &m) {
    return (mesh_point_compatible_to_mesh(std::get<Is>(mp.components_tuple()), std::get<Is>(m.components())) and ...);
  }
  template <typename... M>
  bool mesh_point_compatible_to_mesh(mesh_point<gf_mesh<cartesian_product<M...>>> const &mp, gf_mesh<cartesian_product<M...>> const &m) {
    return mesh_point_compatible_to_mesh_impl(std::index_sequence_for<M...>{}, mp, m);
  }
} // namespace triqs::gfs
