// additional functions for debug sections

#include "../mesh_concepts.hpp"

namespace triqs::mesh {

  // Check if mesh_point mp is compatible with mesh m. Intended for debug mode only.
  template <MeshPoint MP> bool mesh_point_compatible_to_mesh(MP const &mp, typename MP::mesh_t const &m) { return mp.mesh_hash() == m.mesh_hash(); }

  // special case for cartesian product, it must be checked for every component.
  template <typename MP, typename... M, size_t... Is>
  bool mesh_point_compatible_to_mesh_impl(std::index_sequence<Is...>, MP const &mp, prod<M...> const &m) {
    return (mesh_point_compatible_to_mesh(std::get<Is>(mp.components_tuple()), std::get<Is>(m.components())) and ...);
  }
  template <typename... M> bool mesh_point_compatible_to_mesh(typename prod<M...>::mesh_point_t const &mp, prod<M...> const &m) {
    return mesh_point_compatible_to_mesh_impl(std::index_sequence_for<M...>{}, mp, m);
  }
} // namespace triqs::mesh
