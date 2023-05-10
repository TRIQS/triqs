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
#include "utils.hpp"
#include <triqs/lattice/bravais_lattice.hpp>

namespace triqs::mesh {

  using lattice::bravais_lattice;

  /// Cyclic Lattice on a Bravais Lattice
  class cyclat {

    public:
    using idx_t    = std::array<long, 3>;
    using datidx_t = long;
    using value_t  = bravais_lattice::point_t;

    // -------------------- Data -------------------
    private:
    bravais_lattice bl_       = {};
    std::array<long, 3> dims_ = {1, 1, 1};
    long size_                = 1;
    long stride1 = 1, stride0 = 1;
    nda::matrix<double> units_     = nda::eye<double>(3);
    nda::matrix<double> units_inv_ = nda::eye<double>(3);
    uint64_t mesh_hash_            = 0;

    // -------------------- Constructors -------------------
    public:
    /**
     * Construct a periodic Mesh on a BravaisLattice
     *
     * @param bl Object representing the underlying Bravais Lattice
     * @param dims The extents for each of the three dimensions
     */
    cyclat(bravais_lattice const &bl, std::array<long, 3> const &dims)
       : bl_(bl),
         dims_(dims),
         size_(nda::stdutil::product(dims)),
         stride1(dims_[2]),
         stride0(dims_[1] * dims_[2]),
         units_(bl.units()),
         units_inv_(inverse(units_)),
         mesh_hash_(hash(sum(bl.units()), dims[0], dims[1], dims[2])) {}

    cyclat(bravais_lattice const &bl, nda::matrix<long> const &pm) : cyclat(bl, {pm(0, 0), pm(1, 1), pm(2, 2)}) {
      // The index_modulo operation currently assumes a diagonal periodization matrix by treating each index element separately.
      // It needs to be generalized to use only the periodicity as specified in the periodization matrix, i.e.
      //   $$ (i, j, k) -> (i, j, k) + (n1, n2, n3) * periodization_matrix $$
      // nda::is_diagonal(mat, precision) --> add to lib
      EXPECTS((pm.shape() == std::array{3l, 3l}));
      if (not is_matrix_diagonal(pm)) throw std::runtime_error{"Non-diagonal periodization matrices are currently not supported."};
    }

    /**
     *  Construct a Mesh with on a BravaisLattice with periodicity L in each spacial direction
     *
     *  @param bl Object representing the underlying Bravais Lattice
     *  @param L Number of mesh-points in each spacial direction
     */
    cyclat(bravais_lattice const &bl, long L) : cyclat{bl, std::array{L, (bl.ndim() >= 2 ? L : 1l), (bl.ndim() >= 3 ? L : 1)}} {}

    ///Construct  from three linear sizes assuming a cubic lattice (backward compatibility)
    cyclat(long L1 = 1, long L2 = 1, long L3 = 1) : cyclat{bravais_lattice{nda::eye<double>(3)}, std::array{L1, L2, L3}} {}

    // ------------------- Comparison -------------------

    bool operator==(cyclat const &M) const { return dims_ == M.dims() and bl_ == M.lattice(); }
    bool operator!=(cyclat const &M) const { return !(operator==(M)); }

    // ------------------- Accessors -------------------

    /// The Hash for the mesh configuration
    [[nodiscard]] size_t mesh_hash() const { return mesh_hash_; }

    /// The total number of points in the mesh
    [[nodiscard]] size_t size() const { return size_; }

    /// The extent of each dimension
    [[nodiscard]] std::array<long, 3> dims() const { return dims_; }

    /// Matrix containing the mesh basis vectors as rows
    [[nodiscard]] nda::matrix_const_view<double> units() const { return units_; }

    bravais_lattice const &lattice() const noexcept { return bl_; }

    // ----------------------------------------

    idx_t idx_modulo(idx_t const &r) const {
      return {positive_modulo(r[0], dims_[0]), positive_modulo(r[1], dims_[1]), positive_modulo(r[2], dims_[2])};
    }

    // -------------------- mesh_point -------------------

    struct mesh_point_t : public value_t {
      using mesh_t                       = cyclat;
      std::array<long, 3> const &idx     = value_t::idx;
      long datidx                        = 0;
      uint64_t mesh_hash                 = 0;
      mutable std::optional<value_t> val = {};

      [[nodiscard]] value_t const &value() const { return *this; }

      friend std::ostream &operator<<(std::ostream &out, mesh_point_t const &x) { return out << x.value(); }

      mesh_point_t() = default;
      mesh_point_t(bravais_lattice const &bl, idx_t idx, long datidx_, uint64_t mesh_hash_)
         : value_t(&bl, idx), datidx(datidx_), mesh_hash(mesh_hash_) {}
    };

    // -------------------- index checks and conversions -------------------

    [[nodiscard]] bool is_idx_valid(idx_t const &idx) const noexcept {
      for (auto i : range(3))
        if (idx[i] < 0 or idx[i] >= dims_[i]) return false;
      return true;
    }

    [[nodiscard]] datidx_t to_datidx(idx_t const &idx) const {
      EXPECTS(is_idx_valid(idx));
      return idx[0] * stride0 + idx[1] * stride1 + idx[2];
    }

    [[nodiscard]] datidx_t to_datidx(mesh_point_t const &mp) const {
      EXPECTS(mesh_hash_ == mp.mesh_hash);
      return mp.datidx;
    }

    template <typename V> [[nodiscard]] datidx_t to_datidx(closest_mesh_point_t<V> const &cmp) const { return to_datidx(to_idx(cmp)); }

    [[nodiscard]] idx_t to_idx(datidx_t datidx) const {
      EXPECTS(0 <= datidx and datidx < size());
      long i0 = datidx / stride0;
      long r0 = datidx % stride0;
      long i1 = r0 / stride1;
      long i2 = i0 % stride1;
      return {i0, i1, i2};
    }

    [[nodiscard]] idx_t to_idx(closest_mesh_point_t<value_t> const &cmp) const { return cmp.value.idx; }

    /// Make a mesh point from a linear index
    [[nodiscard]] mesh_point_t operator[](long datidx) const {
      auto idx = to_idx(datidx);
      EXPECTS(is_idx_valid(idx));
      return {bl_, idx, datidx, mesh_hash_};
    }

    [[nodiscard]] mesh_point_t operator[](closest_mesh_point_t<value_t> const &cmp) const { return (*this)[this->to_datidx(cmp)]; }

    [[nodiscard]] mesh_point_t operator()(idx_t const &idx) const {
      EXPECTS(is_idx_valid(idx));
      auto datidx = to_datidx(idx);
      return {bl_, idx, datidx, mesh_hash_};
    }

    /// Convert an index to a lattice value
    [[nodiscard]] value_t to_value(idx_t const &idx) const {
      EXPECTS(is_idx_valid(idx));
      return {&bl_, idx};
    }

    // -------------------- print -------------------

    friend std::ostream &operator<<(std::ostream &sout, cyclat const &m) {
      return sout << "Cyclic Lattice Mesh with linear dimensions " << m.dims() << "\n -- units = " << m.units() << "\n -- lattice: " << m.lattice();
    }

    // -------------------------- Range & Iteration --------------------------

    private:
    [[nodiscard]] auto r_() const {
      return itertools::transform(range(size()), [this](long i) { return (*this)[i]; });
    }

    public:
    [[nodiscard]] auto begin() const { return r_().begin(); }
    [[nodiscard]] auto cbegin() const { return r_().cbegin(); }
    [[nodiscard]] auto end() const { return r_().end(); }
    [[nodiscard]] auto cend() const { return r_().cend(); }

    // -------------- HDF5  --------------------------

    [[nodiscard]] static std::string hdf5_format() { return "MeshCyclicLattice"; }

    friend void h5_write(h5::group fg, std::string const &subgroup_name, cyclat const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m);

      h5::write(gr, "dims", m.dims_);
      h5::write(gr, "bravais_lattice", m.bl_);
    }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, cyclat &m) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, m, true);

      std::array<long, 3> dims;
      if (gr.has_key("dims")) {
        h5::read(gr, "dims", dims);
      } else { // Backward Compat periodization_matrix
        auto pm = h5::read<matrix<long>>(gr, "periodization_matrix");
        dims    = {pm(0, 0), pm(1, 1), pm(2, 2)};
      }

      bravais_lattice bl{};
      if (gr.has_key("bravais_lattice")) {
        h5::read(gr, "bravais_lattice", bl);
      } else {
        h5::read(gr, "bl", bl);
      }

      m = cyclat(bl, dims);
    }

    // -------------- Evaluation --------------------------

    friend auto evaluate(cyclat const &m, auto const &f, value_t const &v) { return evaluate(m, f, m.idx_modulo(v.idx)); }
  };

  static_assert(MeshWithValues<cyclat>);

} // namespace triqs::mesh
