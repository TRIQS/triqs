/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by O. Parcollet
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
#include <triqs/utility/index_generator.hpp>
#include <triqs/h5/vector.hpp>
#include "./brillouin_zone.hpp"
#include "../gfs/tools.hpp"
#include "../gfs/meshes/mesh_tools.hpp"

namespace triqs {
namespace gfs {

 using lattice::brillouin_zone;
 
 template <typename Opt> struct gf_mesh<brillouin_zone, Opt>  { 

  brillouin_zone bz;                             //
  utility::mini_vector<int, 3> dims = {1, 1, 1}; // the size in each dimension
  size_t _size = dims[0] * dims[1] * dims[2];    // total size
  long s1 = dims[0];                             // stride
  long s2 = dims[0] * dims[1];                   // stride
  utility::mini_vector<double, 3> step = {2 * M_PI / dims[0], 2 * M_PI / dims[1], 2 * M_PI / dims[2]};

  using k_t = lattice::k_t;
  public:
  gf_mesh() = default;

  gf_mesh(brillouin_zone const& bz, int n_l)
     : bz(bz), dims{n_l, (bz.lattice().dim() >= 2 ? n_l : 1), (bz.lattice().dim() >= 3 ? n_l : 1)} {}

  int rank() const { return (dims[2] > 1 ? 3 : (dims[1] > 1 ? 2 : 1)); }

  utility::mini_vector<int, 3> get_dimensions() const { return dims; }

  /// ----------- Model the mesh concept  ----------------------
  using domain_t = brillouin_zone;
  using domain_pt_t = typename domain_t::point_t;
  domain_t const& domain() const { return bz; }

  using index_t = utility::mini_vector<long, 3>;
  using linear_index_t = long;

  size_t size() const { return _size; }

  utility::mini_vector<size_t, 1> size_of_components() const {
   return {size()};
  }

  k_t index_to_point(index_t const& i) const {
   return {i[0] * step[0], i[1] * step[1], i[2] * step[2]};
   //return {(i[0] + 0.5) * step[0], (i[1] + 0.5) * step[1], (i[2] + 0.5) * step[2]};
  }

  // flatten the index
  linear_index_t index_to_linear(index_t const& i) const { return i[0] + i[1] * s1 + i[2] * s2; }

  // f (k) -> void where k is a k_t, a point in the BZ
  template <typename F> friend void foreach(gf_mesh const& m, F f) {
   k_t k = {0,0,0}; //{0.5 * m.step[0], 0.5 * m.step[1], 0.5 * m.step[2]};
   for (long i2 = 0; i2 < m.dims[2]; ++i2, k(2) += m.step[2])
    for (long i1 = 0; i1 < m.dims[1]; ++i1, k(1) += m.step[1])
     for (long i0 = 0; i0 < m.dims[0]; ++i0, k(0) += m.step[0]) f(k);
  }

  using mesh_point_t = mesh_point<gf_mesh>;

  /// Accessing a point of the mesh
  mesh_point_t operator[](index_t i) const {
   return {*this, i};
  }

  /// Iterating on all the points...
  using const_iterator = gfs::mesh_pt_generator<gf_mesh>;
  const_iterator begin() const { return const_iterator(this); }
  const_iterator end() const { return const_iterator(this, true); }
  const_iterator cbegin() const { return const_iterator(this); }
  const_iterator cend() const { return const_iterator(this, true); }

  /// Mesh comparison
  //bool operator==(gf_mesh const& M) const { return ((bz == M.bz) && (dims == M.dims)); }
  bool operator==(gf_mesh const& M) const { return ((dims == M.dims)); }
  bool operator!=(gf_mesh const &M) const { return !(operator==(M)); }

  /// ----------- End mesh concept  ----------------------

  /// locate the closest point 
  mesh_point_t locate_neighbours(k_t const& k) const {
   auto l = [&](int i) {
    long r = std::lround(k(i) / step[i]) % dims[i];
    return (r >= 0 ? r : r + dims[i]);
   };
   return {*this, {l(0), l(1), l(2)}};
  }

  /// Write into HDF5
  friend void h5_write(h5::group fg, std::string subgroup_name, gf_mesh const& m) {
   h5::group gr = fg.create_group(subgroup_name);
   h5_write(gr, "domain", m.domain());
   h5_write(gr, "n_pts", m.dims[2]);
   //h5_write(gr, "dims", m.dims.to_vector());
  }

  /// Read from HDF5
  friend void h5_read(h5::group fg, std::string subgroup_name, gf_mesh& m) {
   h5::group gr = fg.open_group(subgroup_name);
   auto bz = h5::h5_read<brillouin_zone>(gr, "domain");
   auto dims = h5::h5_read<int>(gr, "n_pts");
   m = gf_mesh(bz, dims);
   //auto dims = h5::h5_read<std::vector<int>>(gr, "dims");
   //m = gf_mesh(bz, {dims[0], dims[1], dims[2]}); // NOT CORRECT IN GENERAL
  }

  //  BOOST Serialization
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive& ar, const unsigned int version) {
   ar& TRIQS_MAKE_NVP("dims", dims);
   ar& TRIQS_MAKE_NVP("_size", _size);
   ar& TRIQS_MAKE_NVP("s2", s2);
   ar& TRIQS_MAKE_NVP("s1", s1);
   ar& TRIQS_MAKE_NVP("step", step);
  }
  
  friend std::ostream &operator<<(std::ostream &sout, gf_mesh const &m) { return sout << "gf_mesh "; }
 };

 // ---------------------------------------------------------------------------
 //                     The mesh point
 // ---------------------------------------------------------------------------
 template <>
 class mesh_point<gf_mesh<brillouin_zone>> : public utility::index3_generator,
                                             public utility::arithmetic_ops_by_cast<
                                                 mesh_point<gf_mesh<brillouin_zone>>,
                                                 typename gf_mesh<brillouin_zone>::domain_pt_t> {
  using mesh_t = gf_mesh<brillouin_zone>;
  using index_t = mesh_t::index_t;
  using linear_index_t = mesh_t::linear_index_t;
  using domain_pt_t = mesh_t::domain_pt_t;
  mesh_t const* m = nullptr;

  public:
  mesh_point() = default;
  mesh_point(mesh_t const& mesh, index_t const& index) : index3_generator(mesh.get_dimensions(), index), m(&mesh) {}
  mesh_point(mesh_t const& mesh) : mesh_point(mesh, {0, 0, 0}) {}
  operator domain_pt_t() const { return m->index_to_point(index()); }
  linear_index_t linear_index() const { return m->index_to_linear(index()); }
  // The mesh point behaves like a vector // NOT GOOD : take the ith componenet, this is SLOW
  double operator()(int i) const { return index()[i] * m->step[i]; }
  // double operator()(int i) const { return (index()[i] + 0.5) * m->step[i]; }
  double operator[](int i) const { return operator()(i); }
  friend std::ostream& operator<<(std::ostream& out, mesh_point const& x) { return out << (domain_pt_t)x; }
 };

 // for backward compat
 // SHOULD REMOVE THIS 
 using regular_bz_mesh = gf_mesh<brillouin_zone>;

}
}

