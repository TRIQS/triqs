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
namespace gfs {

 struct cyclic_lattice {};

 template <> struct gf_mesh<cyclic_lattice> { 

  utility::mini_vector<int, 3> dims;          // the size in each dimension
  size_t _size = dims[0] * dims[1] * dims[2]; // total size
  long s1 = dims[2];                          // stride
  long s2 = dims[1] * dims[2];                // stride

  long _modulo(long r, int i) const {
   long res = r % dims[i];
   return (res >= 0 ? res : res + dims[i]);
  }

  public:
  gf_mesh(int L1 = 1, int L2 = 1, int L3 = 1) : dims{L1, L2, L3} {}

  int rank() const { return (dims[2] > 1 ? 3 : (dims[1] > 1 ? 2 : 1)); }

  utility::mini_vector<int, 3> get_dimensions() const { return dims; }

  /// ---------- Model the domain concept  ---------------------
  using point_t = arrays::vector<int>; // domain concept. PUT on STACK

  /// ----------- Model the mesh concept  ----------------------
  using domain_t = gf_mesh;
  domain_t const& domain() const { return *this; }

  using index_t = utility::mini_vector<long, 3>;
  using linear_index_t = long;
  using default_interpol_policy = interpol_t::None;

  size_t size() const { return _size; }

  utility::mini_vector<size_t, 1> size_of_components() const {
   return {size()};
  }

  point_t index_to_point(index_t const& i) const {
   return {i[0], i[1], i[2]}; // not very good.
  }

  /// flatten the index
  linear_index_t index_to_linear(index_t const& i) const {
   return _modulo(i[0], 0) * s2 + _modulo(i[1], 1) * s1 + _modulo(i[2], 2);
  }
  // linear_index_t index_to_linear(index_t const& i) const { return i[0] + i[1] * s1 + i[2] * s2; }

  /// Is the point in the mesh ? Always true
  template <typename T> bool is_within_boundary(T const&) const { return true; }

  using mesh_point_t = mesh_point<gf_mesh>;

  /// Accessing a point of the mesh from its index
  inline mesh_point_t operator[](index_t i) const; //impl below

  /// Iterating on all the points...
  using const_iterator = mesh_pt_generator<gf_mesh>;
  inline const_iterator begin() const;  // impl below
  inline const_iterator end() const;    
  inline const_iterator cbegin() const; 
  inline const_iterator cend() const;   

   /// Mesh comparison
  bool operator==(gf_mesh const& M) const { return ((dims == M.dims)); }
  bool operator!=(gf_mesh const &M) const { return !(operator==(M)); }

  /// Reduce point modulo to the lattice.
  // WHY NOT INLINE ??? SLOW !!
  inline mesh_point_t modulo_reduce(index_t const& r) const;

 // mesh_point_t const & modulo_reduce(mesh_point_t const& r) const { return r;}

  // -------------- Evaluation of a function on the grid --------------------------

  /// Reduce index modulo to the lattice.
  index_t index_modulo(index_t const& r) const {
   return index_t{_modulo(r[0], 0), _modulo(r[1], 1), _modulo(r[2], 2)};
  }
  
  using interpol_data_t = index_t;
 
  interpol_data_t get_interpolation_data(default_interpol_policy, index_t const &x) const {
   return index_modulo(x);
  }

  template<typename F>
  auto evaluate(default_interpol_policy, F const & f, index_t const &x) const 
#ifdef TRIQS_CPP11 
->std14::decay_t<decltype(f[0])> 
#endif
  {
   auto id = get_interpolation_data(default_interpol_policy{}, x);
   return f[id];
  }

  // -------------- HDF5  --------------------------
  /// Write into HDF5
  friend void h5_write(h5::group fg, std::string subgroup_name, gf_mesh const& m) {
   h5::group gr = fg.create_group(subgroup_name);
   h5_write(gr, "dims", m.dims.to_vector());
  }

  /// Read from HDF5
  friend void h5_read(h5::group fg, std::string subgroup_name, gf_mesh& m) {
   h5::group gr = fg.open_group(subgroup_name);
   auto dims = h5::h5_read<std::vector<int>>(gr, "dims");
   m = gf_mesh(dims[0], dims[1], dims[2]);
  }

  //  BOOST Serialization
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive& ar, const unsigned int version) {
   ar& TRIQS_MAKE_NVP("dims", dims);
   ar& TRIQS_MAKE_NVP("_size", _size);
   ar& TRIQS_MAKE_NVP("s2", s2);
   ar& TRIQS_MAKE_NVP("s1", s1);
  }

  friend std::ostream &operator<<(std::ostream &sout, gf_mesh const &m) { return sout << "cyclic_lattice of size " << m.dims; }
  
 };

 // ---------------------------------------------------------------------------
 //                     The mesh point
 // ---------------------------------------------------------------------------
 template <>
 class mesh_point<gf_mesh<cyclic_lattice>> : public utility::index3_generator,
                                             public utility::arithmetic_ops_by_cast<mesh_point<gf_mesh<cyclic_lattice>>,
                                                                                    typename gf_mesh<cyclic_lattice>::point_t> {
  using mesh_t = gf_mesh<cyclic_lattice>;
  mesh_t const* m = nullptr;

  public:
  mesh_point() = default;
  // explicit is important for g[ {1,2}] to disambiguate the mesh_point_t and the mesh_index_t
  explicit mesh_point(mesh_t const& mesh, mesh_t::index_t const& index)
     : index3_generator(mesh.get_dimensions(), index), m(&mesh) {}
  mesh_point(mesh_t const& mesh) : mesh_point(mesh, {0, 0, 0}) {}
  operator mesh_t::point_t() const { return m->index_to_point(index()); }
  operator mesh_t::index_t() const { return index(); }
  // linear_index_t linear_index() const { return m->index_to_linear(index()); }
  // The mesh point behaves like a vector
  long operator()(int i) const { return index()[i]; }
  long operator[](int i) const { return operator()(i); }
  friend std::ostream& operator<<(std::ostream& out, mesh_point const& x) { return out << x.index(); }
  mesh_point operator-() const {
   return mesh_point{*m, {m->_modulo(-index()[0], 0), m->_modulo(-index()[1], 1), m->_modulo(-index()[2], 2)}};
  }
 };

 // impl
 inline mesh_point<gf_mesh<cyclic_lattice>> gf_mesh<cyclic_lattice>::operator[](index_t i) const {
  return mesh_point<gf_mesh<cyclic_lattice>>{*this, i};
 }

 inline gf_mesh<cyclic_lattice>::const_iterator gf_mesh<cyclic_lattice>::begin() const { return const_iterator(this); }
 inline gf_mesh<cyclic_lattice>::const_iterator gf_mesh<cyclic_lattice>::end() const { return const_iterator(this, true); }
 inline gf_mesh<cyclic_lattice>::const_iterator gf_mesh<cyclic_lattice>::cbegin() const { return const_iterator(this); }
 inline gf_mesh<cyclic_lattice>::const_iterator gf_mesh<cyclic_lattice>::cend() const { return const_iterator(this, true); }

 /// Reduce point modulo to the lattice.
 inline gf_mesh<cyclic_lattice>::mesh_point_t gf_mesh<cyclic_lattice>::modulo_reduce(index_t const& r) const {
  return mesh_point_t{*this, {_modulo(r[0], 0), _modulo(r[1], 1), _modulo(r[2], 2)}};
 }
}
}
