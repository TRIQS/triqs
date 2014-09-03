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
  matsubara_freq_mesh(domain_t dom, long n_pts = 1025, bool positive_only = true)
     : _dom(std::move(dom)), _n_pts(n_pts), _positive_only(positive_only) {
   if (_positive_only) {
    _first_index = 0;
    _last_index = n_pts - 1; // CORRECTION
   } else {
    _last_index = (_n_pts - (_dom.statistic == Boson ? 1 : 2)) / 2;
    _first_index = -(_last_index + (_dom.statistic == Fermion));
   }
   _first_index_window = _first_index;
   _last_index_window = _last_index;
  }

  /// Constructor
  matsubara_freq_mesh() : matsubara_freq_mesh(domain_t(), 0, true){}

  /// Constructor
  matsubara_freq_mesh(double beta, statistic_enum S, int n_pts = 1025, bool positive_only = true)
     : matsubara_freq_mesh({beta, S}, n_pts, positive_only) {}

  /// Copy constructor 
  matsubara_freq_mesh(matsubara_freq_mesh const &) = default;

  /// Scatter a mesh over the communicator c
  friend matsubara_freq_mesh mpi_scatter(matsubara_freq_mesh m, mpi::communicator c, int root) {
   auto m2 = matsubara_freq_mesh{m.domain(), m.size(), m.positive_only()};
   std::tie(m2._first_index_window, m2._last_index_window) = mpi::slice_range(m2._first_index, m2._last_index, c.size(), c.rank());
   return m2;
  }

  friend matsubara_freq_mesh mpi_gather(matsubara_freq_mesh m, mpi::communicator c, int root) {
   return matsubara_freq_mesh{m.domain(), m.size(), m.positive_only()};
  }

  /// The corresponding domain
  domain_t const &domain() const { return _dom; }

  /** \brief First value of the index
   *
   *  0 if positive_only is true
   *  else : 
   *    For fermions : -Nmax - 1
   *    For Bosons : -Nmax
   **/

  /// last Matsubara index 
  int last_index() const { return _last_index;}

  /// first Matsubara index
  int first_index() const { return _first_index;}

  /// last Matsubara index of the window 
  int last_index_window() const { return _last_index_window;}

  /// first Matsubara index of the window
  int first_index_window() const { return _first_index_window;}

  /// Size (linear) of the mesh
  //long size() const { return _n_pts;}

  /// Size (linear) of the mesh of the window
  long size() const { return _last_index_window - _first_index_window + 1; }

  /// From an index of a point in the mesh, returns the corresponding point in the domain
  domain_pt_t index_to_point(index_t ind) const { return 1_j * M_PI * (2 * ind + (_dom.statistic == Fermion)) / _dom.beta; }

  /// Flatten the index in the positive linear index for memory storage (almost trivial here).
  long index_to_linear(index_t ind) const { return ind - first_index_window(); }
  index_t linear_to_index(long lind) const { return lind + first_index_window(); }

  /// Is the mesh only for positive omega_n (G(tau) real))
  bool positive_only() const { return _positive_only;}

  /**
   *  The mesh point
   *  
   *  * NB : the mesh point is also in this case a matsubara_freq.
  **/
  struct mesh_point_t : tag::mesh_point, matsubara_freq {
   mesh_point_t() = default;
   mesh_point_t(matsubara_freq_mesh const &mesh, index_t const &index_)
      : matsubara_freq(index_, mesh.domain().beta, mesh.domain().statistic)
      , first_index_window(mesh.first_index_window())
      , last_index_window(mesh.last_index_window()) {}
   mesh_point_t(matsubara_freq_mesh const &mesh) : mesh_point_t(mesh, mesh.first_index_window()) {}
   void advance() { ++n; }
   long linear_index() const { return n - first_index_window; }
   long index() const { return n; }
   bool at_end() const { return (n == last_index_window + 1); } // at_end means " one after the last one", as in STL
   void reset() { n = first_index_window; }

   private:
   index_t first_index_window, last_index_window;
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
   return (std::make_tuple(_dom, _n_pts, _positive_only) == std::make_tuple(M._dom, M._n_pts, M._positive_only));
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
    h5_write(gr, "start_at_0", (m._positive_only?1:0));
   }
  }

  /// Read from HDF5
  friend void h5_read(h5::group fg, std::string subgroup_name, matsubara_freq_mesh &m) {
   h5::group gr = fg.open_group(subgroup_name);
   typename matsubara_freq_mesh::domain_t dom;
   int L;
   int s = 1;
   h5_read(gr, "domain", dom);
   h5_read(gr, "size", L);
   if (gr.has_key("start_at_0")) h5_read(gr, "start_at_0", s);
   m = matsubara_freq_mesh{std::move(dom), L, (s==1)};
  }

  friend class boost::serialization::access;
  ///  BOOST Serialization
  template <class Archive> void serialize(Archive &ar, const unsigned int version) {
   ar &TRIQS_MAKE_NVP("beta", _dom.beta);
   ar &TRIQS_MAKE_NVP("statistic", _dom.statistic);
   //ar &TRIQS_MAKE_NVP("domain", _dom);
   ar &TRIQS_MAKE_NVP("size", _n_pts);
   ar &TRIQS_MAKE_NVP("kind", _positive_only);
  }

  /// Simple print (just blabla and the size)
  friend std::ostream &operator<<(std::ostream &sout, matsubara_freq_mesh const &m) {
   return sout << "Matsubara Freq Mesh of size " << m.size();
  }

  private:
  domain_t _dom;
  int _n_pts;
  bool _positive_only;
  long _first_index, _last_index, _first_index_window, _last_index_window;
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

