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

 //----------------------------------- The mesh -----------------------------------------
 struct matsubara_freq_mesh {

  using domain_t = matsubara_domain<true>;
  using index_t = int;
  using domain_pt_t = typename domain_t::point_t;

  matsubara_freq_mesh() : _dom(), n_max(0), _positive_only(true) {}

  explicit matsubara_freq_mesh(domain_t dom, int n_pts, bool positive_only = true)
     : _dom(std::move(dom)), n_max(n_pts), _positive_only(positive_only) {}

  domain_t const &domain() const { return _dom; }
  size_t size() const { return (_positive_only ? n_max : 2 * n_max + 1); }

  /// Conversions point <-> index <-> linear_index
  domain_pt_t index_to_point(index_t ind) const {
   return 1_j * M_PI * (2 * ind + (_dom.statistic == Fermion ? 1 : 0)) / _dom.beta;
  }

  public:
  size_t index_to_linear(index_t ind) const { return ind; }

  /// The wrapper for the mesh point
  struct mesh_point_t : tag::mesh_point, public arith_ops_by_cast<mesh_point_t, matsubara_freq> {
   index_t n;
   double beta;
   statistic_enum statistic;
   index_t n_max;

   mesh_point_t()= default;
   mesh_point_t(matsubara_freq_mesh const &mesh, index_t const &index_)
      : n(index_), beta(mesh.domain().beta), statistic(mesh.domain().statistic), n_max(mesh.size()) {}
   mesh_point_t(index_t const &index_, double beta_, statistic_enum stat_, index_t n_max_)
      : n(index_), beta(beta_), statistic(stat_), n_max(n_max_) {}
   void advance() { ++n; }
   using cast_t = domain_pt_t;
   operator cast_t() const { return 1_j * M_PI * (2 * n + (statistic == Fermion ? 1 : 0)) / beta; }
   using cast1_t = matsubara_freq;
   operator cast1_t() const { return {n,beta,statistic};}
   size_t linear_index() const { return n; }
   size_t index() const { return n; }
   bool at_end() const { return (n == n_max); }
   void reset() { n = 0; }
  };

  /// Accessing a point of the mesh
  mesh_point_t operator[](index_t i) const { return mesh_point_t(*this, i); }

  /// Iterating on all the points...
  using const_iterator = mesh_pt_generator<matsubara_freq_mesh>;
  const_iterator begin() const { return const_iterator(this); }
  const_iterator end() const { return const_iterator(this, true); }
  const_iterator cbegin() const { return const_iterator(this); }
  const_iterator cend() const { return const_iterator(this, true); }

  /// Mesh comparison
  bool operator==(matsubara_freq_mesh const &M) const {
   return (std::make_tuple(_dom, n_max, _positive_only, n_max) == std::make_tuple(M._dom, M.n_max, M._positive_only, n_max));
  }
  bool operator!=(matsubara_freq_mesh const &M) const { return !(operator==(M)); }

  /// Write into HDF5
  friend void h5_write(h5::group fg, std::string subgroup_name, matsubara_freq_mesh const &m) {
   h5::group gr = fg.create_group(subgroup_name);
   h5_write(gr, "domain", m.domain());
   h5_write(gr, "size", m.size());
   //h5_write(gr, "start_at_0", m._positive_only);
   // kept for backward compatibility
   auto beta = m.domain().beta;
   h5_write(gr, "min", Fermion ? M_PI / beta : 0);
   h5_write(gr, "max", ((Fermion ? 1 : 0) + 2 * m.size()) * M_PI / beta);
   h5_write(gr, "kind", 2);
  }

  /// Read from HDF5
  friend void h5_read(h5::group fg, std::string subgroup_name, matsubara_freq_mesh &m) {
   h5::group gr = fg.open_group(subgroup_name);
   typename matsubara_freq_mesh::domain_t dom;
   int L;
   bool s = true;
   h5_read(gr, "domain", dom);
   h5_read(gr, "size", L);
   //h5_read(gr, "start_at_0", s);
   m = matsubara_freq_mesh{std::move(dom), L, s};
  }

  //  BOOST Serialization
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) {
   ar &boost::serialization::make_nvp("domain", _dom);
   ar &boost::serialization::make_nvp("size", n_max);
   ar &boost::serialization::make_nvp("kind", _positive_only);
  }

  friend std::ostream &operator<<(std::ostream &sout, matsubara_freq_mesh const &m) {
   return sout << "Matsubara Freq Mesh of size " << m.size();
  }

  private:
  domain_t _dom;
  int n_max;
  bool _positive_only;
 };

 }
}

