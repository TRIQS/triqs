

#pragma once
#include <itertools/itertools.hpp>
#include <nda/nda.hpp>
#include <triqs/utility/arithmetic_ops_by_cast.hpp>
#include <triqs/utility/tuple_tools.hpp>

namespace triqs::mesh {

  // forward : The mesh point for each mesh. To be specialize by each mesh.
  template <typename Mesh> struct mesh_point;

  //-----------------------------------------------------------------------------------------------------------------------------
  /*
   *
   * A generator for the mesh points of a mesh
   */
  // FIXME : remove ITERATOR FACADE
  template <typename Mesh>
  class mesh_pt_generator : public itertools::iterator_facade<mesh_pt_generator<Mesh>, typename Mesh::mesh_point_t, std::forward_iterator_tag,
                                                              typename Mesh::mesh_point_t const &> {
    friend class itertools::iterator_facade<mesh_pt_generator<Mesh>, typename Mesh::mesh_point_t, std::forward_iterator_tag,
                                            typename Mesh::mesh_point_t const &>;

    static_assert(models_mesh_concept_v<Mesh>, "Logic Error");

    Mesh const *mesh = nullptr;
    size_t u         = 0;
    typename Mesh::mesh_point_t pt;
    typename Mesh::mesh_point_t const &dereference() const { return pt; }

    bool equal(mesh_pt_generator const &other) const { return ((other.u == u)); }
    // do NOT check = of mesh, otherwise e.g. block iterator does not work (infinite loop...)
    //bool equal(mesh_pt_generator const & other) const { return ((mesh == other.mesh) && (other.u==u) );}
    public:
    mesh_pt_generator() = default;

    mesh_pt_generator(Mesh const *m, bool atEnd = false) : mesh(m), u(atEnd ? m->size() : 0), pt(*m) {}

    void increment() {
      ++u;
      pt.advance();
    }

    bool at_end() const { return (u >= mesh->size()); }

    typename Mesh::domain_t::point_t to_point() const { return pt; }

    mesh_pt_generator &operator+=(int n) {
      for (int i = 0; i < n; ++i) increment();
      return *this;
    }

    friend mesh_pt_generator operator+(mesh_pt_generator lhs, int n) { return lhs += n; }
  };

} // namespace triqs::mesh