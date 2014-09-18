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
#include <triqs/arrays.hpp>
#include <triqs/utility/index_generator.hpp>
//#include <string>
//#include <vector>

namespace triqs {
namespace lattice {

 class cyclic_lattice_mesh {

  utility::mini_vector<int, 3> dims;          // the size in each dimension
  size_t _size = dims[0] * dims[1] * dims[2]; // total size
  long s1 = dims[0];                          // stride
  long s2 = dims[0] * dims[1];                // stride

  long _modulo(long r, int i) const {
   long res = r % dims[i];
   return (res >= 0 ? res : res + dims[i]);
  }

  public:
  cyclic_lattice_mesh(int L1 = 1, int L2 = 1, int L3 = 1) : dims{L1, L2, L3} {}

  int rank() const { return (dims[2] > 1 ? 3 : (dims[1] > 1 ? 2 : 1)); }

  utility::mini_vector<int, 3> get_dimensions() const { return dims; }

  /// ---------- Model the domain concept  ---------------------
  using point_t = arrays::vector<int>; // domain concept. PUT on STACK

  /// ----------- Model the mesh concept  ----------------------
  using domain_t = cyclic_lattice_mesh;
  domain_t const& domain() const { return *this; }

  using index_t = utility::mini_vector<long, 3>;
  using linear_index_t = long;

  size_t size() const { return _size; }

  utility::mini_vector<size_t, 1> size_of_components() const {
   return {size()};
  }

  point_t index_to_point(index_t const& i) const {
   return {i[0], i[1], i[2]}; // not very good.
  }

  /// flatten the index
  linear_index_t index_to_linear(index_t const& i) const {
   return _modulo(i[0], 0) + _modulo(i[1], 1) * s1 + _modulo(i[2], 2) * s2;
  }
  // linear_index_t index_to_linear(index_t const& i) const { return i[0] + i[1] * s1 + i[2] * s2; }

  /// The wrapper for the mesh point
  class mesh_point_t : public utility::index3_generator, public utility::arithmetic_ops_by_cast<mesh_point_t, point_t> {
   cyclic_lattice_mesh const* m = nullptr;

   public:
   mesh_point_t() = default;
   // explicit is important for g[ {1,2}] to disambiguate the mesh_point_t and the mesh_index_t
   explicit mesh_point_t(cyclic_lattice_mesh const& mesh, index_t const& index)
      : index3_generator(mesh.get_dimensions(), index), m(&mesh) {}
   mesh_point_t(cyclic_lattice_mesh const& mesh) : mesh_point_t(mesh, {0, 0, 0}) {}
   operator point_t() const { return m->index_to_point(index()); }
   // linear_index_t linear_index() const { return m->index_to_linear(index()); }
   // The mesh point behaves like a vector
   long operator()(int i) const { return index()[i]; }
   long operator[](int i) const { return operator()(i); }
   friend std::ostream& operator<<(std::ostream& out, mesh_point_t const& x) { return out << x.index(); }
  };

  /// Accessing a point of the mesh
  mesh_point_t operator[](index_t i) const {
   return mesh_point_t{*this, i};
  }

  /// Iterating on all the points...
  using const_iterator = gfs::mesh_pt_generator<cyclic_lattice_mesh>;
  const_iterator begin() const { return const_iterator(this); }
  const_iterator end() const { return const_iterator(this, true); }
  const_iterator cbegin() const { return const_iterator(this); }
  const_iterator cend() const { return const_iterator(this, true); }

  /// ----------- End mesh concept  ----------------------

  /// Reduce point modulo to the lattice.
  mesh_point_t modulo_reduce(index_t const& r) const {
   return mesh_point_t{*this, {_modulo(r[0], 0), _modulo(r[1], 1), _modulo(r[2], 2)}};
  }

  /// Write into HDF5
  friend void h5_write(h5::group fg, std::string subgroup_name, cyclic_lattice_mesh const& m) {
   h5::group gr = fg.create_group(subgroup_name);
   h5_write(gr, "dims", m.dims.to_vector());
  }

  /// Read from HDF5
  friend void h5_read(h5::group fg, std::string subgroup_name, cyclic_lattice_mesh& m) {
   h5::group gr = fg.open_group(subgroup_name);
   auto dims = h5::h5_read<std::vector<int>>(gr, "dims");
   m = cyclic_lattice_mesh(dims[0], dims[1], dims[2]);
  }

  //  BOOST Serialization
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive& ar, const unsigned int version) {
   ar& TRIQS_MAKE_NVP("dims", dims);
   ar& TRIQS_MAKE_NVP("_size", _size);
   ar& TRIQS_MAKE_NVP("s2", s2);
   ar& TRIQS_MAKE_NVP("s1", s1);
  }
 };
}
}
