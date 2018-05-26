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
#include <triqs/utility/arithmetic_ops_by_cast.hpp>
#include <triqs/utility/iterator_facade.hpp>

#include <triqs/arrays.hpp>

namespace triqs {
  namespace gfs {

    using arrays::array;
    using arrays::array_view;
    using arrays::make_shape;
    using arrays::matrix;
    using arrays::matrix_const_view;
    using arrays::matrix_view;
    using arrays::range;
    using triqs::make_clone;
    using dcomplex = std::complex<double>;
    using arrays::ellipsis;
    using arrays::make_shape;
    using arrays::mini_vector;
    using arrays::range;

    using dcomplex = std::complex<double>;

    // the dummy variable
    struct all_t {};

    namespace tag {
      struct composite {};
      struct mesh_point {};
    } // namespace tag

    /** The statistics : Boson or Fermion */
    enum statistic_enum { Boson = 0, Fermion = 1 };

    // 1 for Boson, -1 for Fermion
    inline int sign(statistic_enum s) { return (s == Boson ? 1 : -1); }

    // Boson*Fermion -> Fermion, others -> Boson
    inline statistic_enum operator*(statistic_enum i, statistic_enum j) { return (i == j ? Boson : Fermion); }

    // pretty print
    inline std::ostream &operator<<(std::ostream &sout, statistic_enum x) { return sout << (x == Boson ? "Boson" : "Fermion"); }

    //enum class statistic_enum {_Boson=0, _Fermion = 1};
    //inline statistic_enum Fermion = statistic_enum::_Fermion;
    //inline statistic_enum Boson = statistic_enum::_Boson;
    //statistic_enum operator* (statistic_enum i, statistic_enum j) { return ( i==j ? statistic_enum::_Boson : statistic_enum::_Fermion); }
    //std::ostream &operator<<(std::ostream &sout, statistic_enum x) { return sout << (x==statistic_enum::_Boson ? "Boson" : "Fermion"); }

    // The mesh for each Var
    template <typename Var> struct gf_mesh;

    // The mesh point for each mesh
    template <typename MeshType> struct mesh_point;

    // FIXME : remove boost !
    template <typename MeshType>
    class mesh_pt_generator : public triqs::utility::iterator_facade<mesh_pt_generator<MeshType>, typename MeshType::mesh_point_t,
                                                                     std::forward_iterator_tag, typename MeshType::mesh_point_t const &> {
      friend class triqs::utility::iterator_facade<mesh_pt_generator<MeshType>, typename MeshType::mesh_point_t, std::forward_iterator_tag,
                                                   typename MeshType::mesh_point_t const &>;
      MeshType const *mesh;
      size_t u;
      typename MeshType::mesh_point_t pt;
      typename MeshType::mesh_point_t const &dereference() const { return pt; }
      bool equal(mesh_pt_generator const &other) const { return ((other.u == u)); }
      // do NOT check = of mesh, otherwise e.g. block iterator does not work (infinite loop...)
      //bool equal(mesh_pt_generator const & other) const { return ((mesh == other.mesh) && (other.u==u) );}
      public:
      mesh_pt_generator() : mesh(nullptr), u(0) {}
      mesh_pt_generator(MeshType const *m, bool atEnd = false) : mesh(m), u(atEnd ? m->size() : 0), pt(*m) {}
      void increment() {
        ++u;
        pt.advance();
      }
      bool at_end() const { return (u >= mesh->size()); }
      typename MeshType::domain_t::point_t to_point() const { return pt; }
      mesh_pt_generator &operator+=(int n) {
        for (int i = 0; i < n; ++i) increment();
        return *this;
      }
      friend mesh_pt_generator operator+(mesh_pt_generator lhs, int n) { return lhs += n; }
    };

  } // namespace gfs
} // namespace triqs
