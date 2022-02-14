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
#include <triqs/utility/arithmetic_ops_by_cast.hpp>
#include <utility>

#include "../details/mesh_tools.hpp"
#include "../details/mesh_point.hpp"

namespace triqs::mesh {

  /**
   * Compute dimensions of a parallelepiped cluster cell using the inverse of the periodization matrix
   *
   * This function computes the dimensions of a parallelepipedic super unit cell (i.e corresponding to the super vectors).
   *
   * for a given Bravais lattice (defined by unit vectors ${a_i}_{i=0\dots d-1}$), the periodic boundary conditions are uniquely
   * defined by the matrix $N$ such that the super vectors $\tilde{a}_i$ are given by:
   *
   * $$\tilde{a}_i = \sum_j N_{ij} a_j$$
   *
   * Example:
   *  If $N_{ij}$ is diag{n_k1, n_k2, n_k3}, this returns {n_k1, n_k2, n_k3}
   *
   * The algorithm used is the following:
   * let $C={(0,0,0)}$
   * for each dimension $d=1\dots 3$ :
   *   - Find the lowest nonzero integer $k_d$ such that there exists $mathbf{x}$ in C such for all $d_p$ in $1\dots 3$, $k_d mathbf{a}_d - mathbf{x}$ belongs to the superlattice.
   *   - Update $C = {mathbf{x} + q mathbf{a}_d, mathbf{x}\in C, q=0\dots k_d-1}$
   * return {k_d}_{k=1\dots d}
   *
   * @param periodization_matrix The periodization matrix
   * @return The dimensions of the parallelepiped unit cell
   */
  std::array<long, 3> find_cell_dims(nda::matrix<long> const &periodization_matrix);

  /// A lattice point
  struct lattice_point : public utility::arithmetic_ops_by_cast<lattice_point, nda::vector<double>> {
    std::array<long, 3> index;
    nda::matrix<double> units;

    lattice_point() : index({0, 0, 0}), units(nda::eye<double>(3)) {}
    lattice_point(std::array<long, 3> const &index_, matrix<double> const &units_) : index(index_), units(units_) {}
    using cast_t = nda::vector<double>;
    operator cast_t() const {
      cast_t M(3);
      M() = 0.0;
      // slow, to be replaced with matrix vector multiplication
      for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) M(i) += index[j] * units(j, i);
      return M;
    }
    friend std::ostream &operator<<(std::ostream &out, lattice_point const &x) { return out << (cast_t)x; }
  };

  struct cluster_mesh : tag::mesh {
    public:
    /// ---------- Model the domain concept  ---------------------
    using domain_t = cluster_mesh;

    domain_t const &domain() const { return *this; }

    using point_t = nda::vector<double>; // domain concept. PUT on STACK

    //  bool contains(point_t const & pt) {return }; // TODO

    // using domain_pt_t = point_t;

    // ------

    using index_t        = std::array<long, 3>;
    using linear_index_t = long;

    struct mesh_point_t : public utility::arithmetic_ops_by_cast<mesh_point_t, typename cluster_mesh::index_t> {
      using mesh_t = cluster_mesh;

      mesh_t const *m = nullptr;

      typename cluster_mesh::index_t index_;
      typename cluster_mesh::domain_t::point_t value_{};
      typename cluster_mesh::linear_index_t linear_index_{};
      std::size_t mesh_hash_ = 0;

      [[nodiscard]] auto index() const { return index_; }
      [[nodiscard]] auto value() const { return value_; }
      [[nodiscard]] auto linear_index() const { return linear_index_; }
      [[nodiscard]] auto mesh_hash() const { return mesh_hash_; }

      mesh_point_t() = default;
      mesh_point_t(typename mesh_t::index_t index, typename mesh_t::domain_t::point_t value, typename mesh_t::linear_index_t linear_index,
                   size_t mesh_hash, mesh_t const &mesh)
         : index_(index), linear_index_(linear_index), value_(std::move(value)), mesh_hash_(mesh_hash), m(&mesh) {}

      using cast_t = cluster_mesh::domain_t::point_t; // FIXME : decide what we want.

      operator cluster_mesh::domain_t::point_t() const { return value_; }
      operator lattice_point() const { return lattice_point(index(), m->units()); }
      operator cluster_mesh::index_t() const { return index_; }

      // The mesh point behaves like a vector
      /// d: component (0, 1 or 2)
      double operator()(int d) const { return value_[d]; }
      double operator[](int d) const { return value_[d]; }

      friend std::ostream &operator<<(std::ostream &out, mesh_point_t const &x) { return out << (lattice_point)x; }
      mesh_point_t operator-() const {
        auto index_new = m->index_modulo({-index()[0], -index()[1], -index()[2]});
        return mesh_point_t{index_new, m->index_to_point(index_new), m->index_to_linear(index_new), mesh_hash_, *m};
      }
    };

    // --- Ctors

    cluster_mesh() = default;

    /**
     * Construct from basis vectors and periodization matrix
     *
     * @param units Matrix $B$ containing as rows the basis vectors that generate mesh points
     * @param periodization_matrix Matrix $N$ specifying the translation vectors for the
     *        periodic boundary conditions
     *      $$ \mathbf{x} \equiv \mathbf{x} + \mathbf{z} \cdot \mathbf{N} \forall \mathbf{n} in \Z^n$
     */
    cluster_mesh(matrix<double> const &units, matrix<long> const &periodization_matrix) : units_(units), periodization_matrix_(periodization_matrix) {
      EXPECTS((periodization_matrix.shape() == std::array{3l, 3l}));

      // The index_modulo operation currently assumes a diagonal periodization matrix by treating each index element separately.
      // It needs to be generalized to use only the periodicity as specified in the periodization matrix, i.e.
      //   $$ (i, j, k) -> (i, j, k) + (n1, n2, n3) * periodization_matrix $$
      auto nvec = nda::diagonal(periodization_matrix);
      if (nda::diag(nvec) != periodization_matrix)
        throw std::runtime_error{"Non-diagonal periodization matrices are currently not supported."};

      dims_   = {nvec[0], nvec[1], nvec[2]};
      size_   = dims_[0] * dims_[1] * dims_[2];
      stride0 = dims_[1] * dims_[2];
      stride1 = dims_[2];
    }

    // -------------------- Index Bijection -------------------

    /// flatten the index
    linear_index_t index_to_linear(index_t const &i) const {
      EXPECTS(i == index_modulo(i));
      return i[0] * stride0 + i[1] * stride1 + i[2];
    }

    // unflatten index
    index_t linear_to_index(linear_index_t const &i) const {
      int i0 = i / stride0;
      int r0 = i % stride0;
      int i1 = r0 / stride1;
      int i2 = i0 % stride1;
      return {i0, i1, i2};
    }

    // -------------------- Mesh Implemenation & Mesh Point Access -------------------

    /// from the index (n_i) to the cartesian coordinates
    /** for a point M of coordinates n_i in the {a_i} basis, the cartesian coordinates are
    *     $$ OM_i = \sum_j n_j X_{ji} $$
    * @param index_t the (integer) coordinates of the point (in basis a_i)
    * @warning can be made faster by writing this a matrix-vector multiplication
    */
    [[nodiscard]] point_t index_to_point(index_t const &n) const {
      EXPECTS(n == index_modulo(n));
      point_t M(3);
      M() = 0.0;
      for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) M(i) += n[j] * units_(j, i);
      return M;
    }

    /// Accessing a point of the mesh from its index

    [[nodiscard]] mesh_point_t operator[](index_t i) const {
      EXPECTS(i == index_modulo(i)); // TODO: Is this requirement correct?
      return {i, index_to_point(i), index_to_linear(i), mesh_hash_, *this};
    }
    [[nodiscard]] mesh_point_t linear_to_mesh_pt(linear_index_t const &linear_index) const {
      auto i = linear_to_index(linear_index);
      return {i, index_to_point(i), linear_index, mesh_hash_, *this};
    }

    // -------------------- Comparison -------------------

    bool operator==(cluster_mesh const &M) const {
      return ((dims_ == M.dims_) && (units_ == M.units_) && (periodization_matrix_ == M.periodization_matrix_));
    }
    bool operator!=(cluster_mesh const &M) const = default;

    // -------------------- Accessors -------------------

    [[nodiscard]] size_t mesh_hash() const { return mesh_hash_; }

    /// Reduce index modulo to the lattice
    index_t index_modulo(index_t const &r) const { return index_t{_modulo(r[0], 0), _modulo(r[1], 1), _modulo(r[2], 2)}; }

    /// The total number of points in the mesh
    size_t size() const { return size_; }

    int rank() const { return (dims_[2] > 1 ? 3 : (dims_[1] > 1 ? 2 : 1)); }

    /// The extent of each dimension
    std::array<long, 3> dims() const { return dims_; }

    /// Matrix containing the mesh basis vectors as rows
    matrix_const_view<double> units() const { return units_; }

    // The matrix defining the periodization on the mesh
    matrix_const_view<long> periodization_matrix() const { return periodization_matrix_; }

    // -------------------------- Other --------------------------

    /// locate the closest point
    inline index_t closest_index(point_t const &x) const {
      auto idbl = transpose(inverse(units_)) * x;
      return {std::lround(idbl[0]), std::lround(idbl[1]), std::lround(idbl[2])};
    }

    /// Is the point in the mesh ? Always true
    template <typename T> static constexpr bool is_within_boundary(T const &) { return true; }

    // -------------------------- Range & Iteration --------------------------

    auto begin() const {
      r_ = make_mesh_range(*this);
      return r_.begin();
    }
    auto end() const { return r_.end(); }
    auto cbegin() const {
      r_ = make_mesh_range(*this);
      return r_.begin();
    }
    auto cend() const { return r_.end(); }

    // -------------- HDF5  --------------------------

    /// Write into HDF5
    friend void h5_write_impl(h5::group fg, std::string subgroup_name, cluster_mesh const &m, const char *_type) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format_as_string(gr, _type);
      h5_write(gr, "units", m.units_);
      h5_write(gr, "periodization_matrix", m.periodization_matrix_);
    }

    /// Read from HDF5
    friend void h5_read_impl(h5::group fg, std::string subgroup_name, cluster_mesh &m, const char *_type) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format_as_string(gr, _type, true);
      auto units                = h5::h5_read<matrix<double>>(gr, "units");
      auto periodization_matrix = h5::h5_read<matrix<long>>(gr, "periodization_matrix");
      m                         = cluster_mesh(units, periodization_matrix);
    }

    // -------------------- print  -------------------

    friend std::ostream &operator<<(std::ostream &sout, cluster_mesh const &m) {
      return sout << "cluster_mesh of size " << m.dims() << "\n units = " << m.units() << "\n periodization_matrix = " << m.periodization_matrix()
                  << "\n";
    }

    // ------------------------------------------------

    protected:
    long _modulo(long r, int i) const {
      long res = r % dims_[i];
      return (res >= 0 ? res : res + dims_[i]);
    }

    matrix<double> units_;
    matrix<long> periodization_matrix_;
    std::array<long, 3> dims_;
    size_t size_;
    long stride1, stride0;

    size_t mesh_hash_ = 0;
    mutable make_mesh_range_rtype<cluster_mesh> r_;
  };

} // namespace triqs::mesh
