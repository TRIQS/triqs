/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#include <itertools/itertools.hpp>

#include <triqs/arrays.hpp>
#include <triqs/utility/arithmetic_ops_by_cast.hpp>
#include <triqs/utility/tuple_tools.hpp>
#include <triqs/utility/mini_vector.hpp>

namespace triqs::mesh {

  using dcomplex = std::complex<double>;

  using arrays::array;
  using arrays::array_view;
  using arrays::ellipsis;
  using arrays::make_shape;
  using arrays::make_unit_matrix;
  using arrays::matrix;
  using arrays::matrix_const_view;
  using arrays::matrix_view;
  using arrays::mini_vector;

  using itertools::range;
  using triqs::make_clone;

  namespace tag {
    struct mesh {};
    struct product : mesh {};
    struct mesh_point {};
  } // namespace tag

  /// Checks if the mesh M is a product
  template <typename M> struct is_product : std::is_base_of<tag::product, M> {};

  ///
  template <typename M> constexpr bool is_product_v = is_product<M>::value;

  // A default implementation. Specialize for non-intrusive usage.
  template <typename M> struct models_mesh_concept : std::is_base_of<tag::mesh, M> {};

  /// Check if M models the mesh concept FIXME 20 : use true concept here
  template <typename M> constexpr bool models_mesh_concept_v = models_mesh_concept<M>::value;

  //
  template <typename Mesh> struct get_n_variables { static const int value = 1; };

  /// A place holder for : or *all*
  struct all_t {};

  /** The statistics : Boson or Fermion */
  enum statistic_enum { Boson = 0, Fermion = 1 };

  // 1 for Boson, -1 for Fermion
  inline int sign(statistic_enum s) { return (s == Boson ? 1 : -1); }

  /// Boson*Fermion -> Fermion, others -> Boson
  inline statistic_enum operator*(statistic_enum i, statistic_enum j) { return (i == j ? Boson : Fermion); }

  // 1 in a completely neutral type for the compiler, which can not optimize 1.0 * x a priori.
  struct _universal_unit_t {};
  template <typename T> auto operator*(_universal_unit_t, T &&x) { return std::forward<T>(x); }
  template <typename T> auto operator*(T &&x, _universal_unit_t) { return std::forward<T>(x); }
  inline _universal_unit_t operator*(_universal_unit_t, _universal_unit_t) { return {}; } // avoid ambiguity

  // FIXME : DOC
  template <typename IndexType> struct interpol_data_0d_t {
    static constexpr int n_pts = 1;
    std::array<IndexType, n_pts> idx;
    std::array<_universal_unit_t, n_pts> w;
    interpol_data_0d_t(IndexType n) : idx{std::move(n)}, w{} {}
  };

  // FIXME CHANGE the order fo the template, it looks strange to accesss W
  template <typename IndexType, int Npts> struct interpol_data_lin_t { // dim R = 1-> 2, R = 2 -> 4, R = 3 -> 8
    static constexpr int n_pts = Npts;
    std::array<IndexType, n_pts> idx;
    std::array<double, n_pts> w;
  };

  // FIXME : DOC
  struct interpol_data_all_t {
    static constexpr int n_pts = 1;
    std::array<all_t, n_pts> idx;
    std::array<_universal_unit_t, n_pts> w;
  };

  // forward : The mesh point for each mesh. To be specialize by each mesh.
  template <typename Mesh> struct mesh_point;

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
