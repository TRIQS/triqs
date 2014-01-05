/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#include "./mesh_tools.hpp"
#include "../domains/matsubara.hpp"

namespace triqs {
namespace gfs {

 struct matsubara_freq_mesh {

  ///
  using domain_t = matsubara_domain<true>;
  ///
  using index_t = long;
  ///
  using domain_pt_t = typename domain_t::point_t;

  /// Constructor
  matsubara_freq_mesh() : _dom(), n_max(0), _positive_only(true) {}

  /// Constructor
  matsubara_freq_mesh(domain_t dom, int n_pts=1025, bool positive_only = true)
     : _dom(std::move(dom)), n_max(n_pts), _positive_only(positive_only) {}

  /// Constructor
  matsubara_freq_mesh(double beta, statistic_enum S, int Nmax = 1025, bool positive_only = true)
     : matsubara_freq_mesh({beta, S}, Nmax, positive_only) {}

  /// Copy constructor 
  matsubara_freq_mesh(matsubara_freq_mesh const &) = default;

  /// The corresponding domain
  domain_t const &domain() const { return _dom; }

  /** \brief First value of the index
   *
   *  0 if positive_only is true
   *  else : 
   *    For fermions : -Nmax +1
   *    For Bosons : -Nmax
   **/ 
  int index_start() const { return -(_positive_only ? 0 : n_max + (_dom.statistic == Fermion)); }

  /// Size (linear) of the mesh
  long size() const { return (_positive_only ? n_max : 2 * n_max + (_dom.statistic == Boson ? 1 : 2)); }

  /// From an index of a point in the mesh, returns the corresponding point in the domain
  domain_pt_t index_to_point(index_t ind) const { return 1_j * M_PI * (2 * ind + (_dom.statistic == Fermion)) / _dom.beta; }

  /// Flatten the index in the positive linear index for memory storage (almost trivial here).
  long index_to_linear(index_t ind) const { return ind + index_start(); }

  /**
   *  The mesh point
   *  
   *  * NB : the mesh point is also in this case a matsubara_freq.
  **/
  struct mesh_point_t : tag::mesh_point, matsubara_freq {
   mesh_point_t() = default;
   mesh_point_t(matsubara_freq_mesh const &mesh, index_t const &index_)
      : matsubara_freq(index_, mesh.domain().beta, mesh.domain().statistic),
        index_start(mesh.index_start()),
        index_stop(mesh.index_start() + mesh.size() - 1) {}
   void advance() { ++n; }
   long linear_index() const { return n; }
   long index() const { return n; }
   bool at_end() const { return (n == index_stop); }
   void reset() { n = index_start; }

   private:
   index_t index_start, index_stop;
  };

  /// Accessing a point of the mesh from its index
  mesh_point_t operator[](index_t i) const {
   return {*this, i};
  }

  /// Iterating on all the points...
  using const_iterator = mesh_pt_generator<matsubara_freq_mesh>;
  const_iterator begin() const { return const_iterator(this); }
  const_iterator end() const { return const_iterator(this, true); }
  const_iterator cbegin() const { return const_iterator(this); }
  const_iterator cend() const { return const_iterator(this, true); }

  bool operator==(matsubara_freq_mesh const &M) const {
   return (std::make_tuple(_dom, n_max, _positive_only, n_max) == std::make_tuple(M._dom, M.n_max, M._positive_only, n_max));
  }
  bool operator!=(matsubara_freq_mesh const &M) const { return !(operator==(M)); }

  /// Write into HDF5
  friend void h5_write(h5::group fg, std::string subgroup_name, matsubara_freq_mesh const &m) {
   h5::group gr = fg.create_group(subgroup_name);
   h5_write(gr, "domain", m.domain());
   h5_write(gr, "size", m.size());
   if (m._positive_only) {
    // kept ONLY for backward compatibility of archives
    auto beta = m.domain().beta;
    h5_write(gr, "min", Fermion ? M_PI / beta : 0);
    h5_write(gr, "max", ((Fermion ? 1 : 0) + 2 * m.size()) * M_PI / beta);
    h5_write(gr, "kind", 2);
   } else { // A strange way : to preserve backward compatibility for old archive.
    h5_write(gr, "start_at_0", m._positive_only);
   }
  }

  /// Read from HDF5
  friend void h5_read(h5::group fg, std::string subgroup_name, matsubara_freq_mesh &m) {
   h5::group gr = fg.open_group(subgroup_name);
   typename matsubara_freq_mesh::domain_t dom;
   int L;
   bool s = true;
   h5_read(gr, "domain", dom);
   h5_read(gr, "size", L);
   if (gr.has_key("start_at_0")) h5_read(gr, "start_at_0", s);
   m = matsubara_freq_mesh{std::move(dom), L, s};
  }

  friend class boost::serialization::access;
  ///  BOOST Serialization
  template <class Archive> void serialize(Archive &ar, const unsigned int version) {
   ar &boost::serialization::make_nvp("domain", _dom);
   ar &boost::serialization::make_nvp("size", n_max);
   ar &boost::serialization::make_nvp("kind", _positive_only);
  }

  /// Simple print (just blabla and the size)
  friend std::ostream &operator<<(std::ostream &sout, matsubara_freq_mesh const &m) {
   return sout << "Matsubara Freq Mesh of size " << m.size();
  }

  private:
  domain_t _dom;
  int n_max;
  bool _positive_only;
 };

 //-------------------------------------------------------

 /** \brief foreach for this mesh
  *
  *  @param m : a mesh
  *  @param F : a function of synopsis auto F (matsubara_freq_mesh::mesh_point_t)
  *
  *  Calls F on each point of the mesh, in arbitrary order.
  **/
 template <typename Lambda> void foreach(matsubara_freq_mesh const &m, Lambda F) {
  for (auto const &w : m) F(w);
 }
}
}

