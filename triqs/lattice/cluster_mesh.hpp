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
#include <triqs/utility/arithmetic_ops_by_cast.hpp>
#include <triqs/gfs/meshes/mesh_tools.hpp>
#include <triqs/h5/vector.hpp>
#include <triqs/arrays.hpp>

namespace triqs {
namespace gfs {
 using namespace triqs::arrays;
 /// Compute dimensions of a parallelepiped cluster cell using the inverse of the periodization matrix
 /**
   @param inv_n inverse $N^{-1}$ of the periodization matrix
   @return the dimensions of the parallelepiped unit cell

   for a given Bravais lattice (defined by unit vectors ${a_i}_{i=0\dots d-1}$), the periodic boundary conditions are uniquely
   defined by the matrix $N$ such that the super vectors $\tilde{a}_i$ are given by:

   $$\tilde{a}_i = \sum_j N_{ij} a_j$$

   This function computes the dimensions of a parallelepipedic super unit cell (i.e corresponding to the super vectors).

   Example:
    If $N_{ij}$ is diag{n_k1, n_k2, n_k3}, this returns {n_k1, n_k2, n_k3}

   The algorithm used is the following:
   let $C={(0,0,0)}$
   for each dimension $d=1\dots 3$ :
     - Find the lowest nonzero integer $k_d$ such that there exists $mathbf{x}$ in C such for all $d_p$ in $1\dots 3$, $k_d
   mathbf{a}_d - mathbf{x}$ belongs to the superlattice.
     - Update $C = {mathbf{x} + q mathbf{a}_d, mathbf{x}\in C, q=0\dots k_d-1}$
   return {k_d}_{k=1\dots d}
   */
 utility::mini_vector<int, 3> find_cell_dims(arrays::matrix<double> const& inv_n);

 /// A lattice point
 struct lattice_point : public utility::arithmetic_ops_by_cast<lattice_point, arrays::vector<double>> {
  utility::mini_vector<long, 3> index;
  arrays::matrix<double> units; // unit vectors

  lattice_point() : index({0, 0, 0}), units(arrays::make_unit_matrix<double>(3)) {}
  lattice_point(utility::mini_vector<long, 3> const& index_, matrix<double> const& units_) : index(index_), units(units_) {}
  using cast_t = arrays::vector<double>;
  operator cast_t() const {
   cast_t M(3);
   M() = 0.0;
   // slow, to be replaced with matrix vector multiplication
   for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) M(i) += index[j] * units(j, i);
   return M;
  }
  friend std::ostream& operator<<(std::ostream& out, lattice_point const& x) { return out << (cast_t)x; }
 };

 using triqs::arrays::make_unit_matrix;

 struct cluster_mesh {

  matrix<double> units;
  matrix<int> periodization_matrix;
  utility::mini_vector<int, 3> dims; // the size in each dimension
  size_t _size;
  long s1, s2;

  long _modulo(long r, int i) const {
   long res = r % dims[i];
   return (res >= 0 ? res : res + dims[i]);
  }

  public:
  cluster_mesh() = default;
  /// full constructor
  /**
    * @param units matrix X such that the unit vectors (a_i) are given in cartesian coordinates (e_j) by:
          $$ \mathbf{a}_i = \sum_{j} x_{ij} \mathbf{e}_j $$
    * @param periodization_matrix matrix $N$ specifying the periodic boundary conditions:
          $$ \tilde{\mathbf{a}}_i = \sum_j N_{ij} \mathbf{a}_j $$
    */
  cluster_mesh(matrix<double> const& units_, matrix<int> const& periodization_matrix_)
     : units(units_), periodization_matrix(periodization_matrix_) {
   dims = find_cell_dims(inverse(matrix<double>(periodization_matrix)));
   _size = dims[0] * dims[1] * dims[2];
   s1 = dims[2];           // stride
   s2 = dims[1] * dims[2]; // stride
  }

  int rank() const { return (dims[2] > 1 ? 3 : (dims[1] > 1 ? 2 : 1)); }

  utility::mini_vector<int, 3> get_dimensions() const { return dims; }

  /// ---------- Model the domain concept  ---------------------
  using domain_t = cluster_mesh;
  domain_t const& domain() const { return *this; }
  using point_t = arrays::vector<double>; // domain concept. PUT on STACK

  /// ----------- Model the mesh concept  ----------------------

  using index_t = utility::mini_vector<long, 3>;
  using linear_index_t = long;
  using default_interpol_policy = interpol_t::None;

  size_t size() const { return _size; }

  utility::mini_vector<size_t, 1> size_of_components() const { return {size()}; }

  /// from the index (n_i) to the cartesian coordinates
  /** for a point M of coordinates n_i in the {a_i} basis, the cartesian coordinates are
    *     $$ OM_i = \sum_j n_j X_{ji} $$
    * @param index_t the (integer) coordinates of the point (in basis a_i)
    * @warning can be made faster by writing this a matrix-vector multiplication
    */
  point_t index_to_point(index_t const& n) const {
   point_t M(3);
   M() = 0.0;
   for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) M(i) += n[j] * units(j, i);
   return M;
  }

  /// flatten the index
  linear_index_t index_to_linear(index_t const& i) const {
   return _modulo(i[0], 0) * s2 + _modulo(i[1], 1) * s1 + _modulo(i[2], 2);
  }


  /// Is the point in the mesh ? Always true
  template <typename T> bool is_within_boundary(T const&) const { return true; }

  using mesh_point_t = mesh_point<cluster_mesh>;

  /// Accessing a point of the mesh from its index
  inline mesh_point_t operator[](index_t i) const; // impl below

  /// Iterating on all the points...
  using const_iterator = mesh_pt_generator<cluster_mesh>;
  inline const_iterator begin() const; // impl below
  inline const_iterator end() const;
  inline const_iterator cbegin() const;
  inline const_iterator cend() const;

  /// f (k) -> void where k is a point_t, a point in the BZ
  template <typename F> friend void foreach (cluster_mesh const& m, F f) {
   for (int i = 0; i < m.dims[0]; i++)
    for (int j = 0; j < m.dims[1]; j++)
     for (int k = 0; k < m.dims[2]; k++) f(m.index_to_point({i, j, k}));
  }

  /// Mesh comparison
  bool operator==(cluster_mesh const& M) const { return ((dims == M.dims)); }
  bool operator!=(cluster_mesh const& M) const { return !(operator==(M)); }

  /// Reduce point modulo to the lattice.
  inline mesh_point_t modulo_reduce(index_t const& r) const;

  /// locate the closest point
  inline index_t locate_neighbours(point_t const& x) const {
   auto inv_units = inverse(units);
   index_t x_n({1, 1, 1});
   for (int i = 0; i < 3; i++) {
    double s = 0.0;
    for (int j = 0; j < 3; j++) s += x[j] * inv_units(j, i);
    x_n[i] = std::lrint(s);
   }
   return x_n;
  }

  // -------------- Evaluation of a function on the grid --------------------------

  /// Reduce index modulo to the lattice.
  index_t index_modulo(index_t const& r) const { return index_t{_modulo(r[0], 0), _modulo(r[1], 1), _modulo(r[2], 2)}; }

  using interpol_data_t = index_t;
  interpol_data_t get_interpolation_data(default_interpol_policy, index_t const& x) const { return index_modulo(x); }
  template <typename F>
  auto evaluate(default_interpol_policy, F const& f, index_t const& x) const
#ifdef TRIQS_CPP11
      -> std14::decay_t<decltype(f[0])>
#endif
  {
   auto id = get_interpolation_data(default_interpol_policy{}, x);
   return f[id];
  }

  // -------------- HDF5  --------------------------
  /// Write into HDF5
  friend void h5_write(h5::group fg, std::string subgroup_name, cluster_mesh const& m) {
   h5::group gr = fg.create_group(subgroup_name);
   h5_write(gr, "units", m.units);
   h5_write(gr, "periodization_matrix", m.periodization_matrix);
  }

  /// Read from HDF5
  friend void h5_read(h5::group fg, std::string subgroup_name, cluster_mesh& m) {
   h5::group gr = fg.open_group(subgroup_name);
   auto units = h5::h5_read<matrix<double>>(gr, "units");
   auto periodization_matrix = h5::h5_read<matrix<double>>(gr, "periodization_matrix");
   m = cluster_mesh(units, periodization_matrix);
  }

  //  BOOST Serialization
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive& ar, const unsigned int version) {
   ar& units;
   ar& periodization_matrix;
   ar& dims;
   ar& _size;
   ar& s2;
   ar& s1;
  }

  friend std::ostream& operator<<(std::ostream& sout, cluster_mesh const& m) {
   return sout << "cyclic_lattice of size " << m.dims;
  }
 };

 // ---------------------------------------------------------------------------
 //                     The mesh point
 // ---------------------------------------------------------------------------
 template <>
 struct mesh_point<cluster_mesh> : public utility::index3_generator,
                                   public utility::arithmetic_ops_by_cast<mesh_point<cluster_mesh>, cluster_mesh::index_t> {
  public:
  using mesh_t = cluster_mesh;

  private:
  mesh_t const* m = nullptr;

  public:
  using index_t = mesh_t::index_t;
  using point_t = mesh_t::point_t;
  using linear_index_t = mesh_t::linear_index_t;

  mesh_point() = default;
  explicit mesh_point(mesh_t const& mesh, mesh_t::index_t const& index)
     : index3_generator(mesh.get_dimensions(), index), m(&mesh) {}
  mesh_point(mesh_t const& mesh) : mesh_point(mesh, {0, 0, 0}) {}
  operator mesh_t::point_t() const { return m->index_to_point(index()); }
  operator lattice_point() const { return lattice_point(index(), m->units); }
  operator mesh_t::index_t() const { return index(); }
  linear_index_t linear_index() const { return m->index_to_linear(index()); }
  // The mesh point behaves like a vector
  /// d: component (0, 1 or 2)
  double operator()(int d) const { return m->index_to_point(index())[d]; }
  double operator[](int d) const { return operator()(d); }
  friend std::ostream& operator<<(std::ostream& out, mesh_point const& x) { return out << (lattice_point)x; }
  mesh_point operator-() const { return mesh_point{*m, m->modulo_reduce({-index()[0], -index()[1], -index()[2]})}; }
  mesh_t const& mesh() const { return *m; }
 };

 // impl
 inline mesh_point<cluster_mesh> cluster_mesh::operator[](index_t i) const {
  return mesh_point<cluster_mesh>{*this, this->modulo_reduce(i)};
 }

 inline cluster_mesh::const_iterator cluster_mesh::begin() const { return const_iterator(this); }
 inline cluster_mesh::const_iterator cluster_mesh::end() const { return const_iterator(this, true); }
 inline cluster_mesh::const_iterator cluster_mesh::cbegin() const { return const_iterator(this); }
 inline cluster_mesh::const_iterator cluster_mesh::cend() const { return const_iterator(this, true); }

 /// Reduce point modulo to the lattice.
 inline cluster_mesh::mesh_point_t cluster_mesh::modulo_reduce(index_t const& r) const {
  return mesh_point_t{*this, {_modulo(r[0], 0), _modulo(r[1], 1), _modulo(r[2], 2)}};
 }
}
}
