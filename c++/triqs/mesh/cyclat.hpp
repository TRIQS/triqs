// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Philipp Dumitrescu, Michel Ferrero, Olivier Parcollet, Nils Wentzell, Thomas Ayral

#pragma once
#include "utils.hpp"
#include <triqs/lattice/bravais_lattice.hpp>

namespace triqs::mesh {

  using lattice::bravais_lattice;

  /// Cyclic Lattice on a Bravais Lattice
  class cyclat {

    public:
    using index_t      = std::array<long, 3>;
    using data_index_t = long;
    using value_t      = bravais_lattice::point_t;

    // -------------------- Data -------------------
    private:
    bravais_lattice bl_       = {};
    std::array<long, 3> dims_ = {0, 0, 0};
    long size_                = 0;
    long stride1 = 1, stride0 = 1;
    nda::matrix<double> units_     = nda::eye<double>(3);
    nda::matrix<double> units_inv_ = nda::eye<double>(3);
    uint64_t _mesh_hash            = 0;

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
         _mesh_hash(hash(sum(bl.units()), dims[0], dims[1], dims[2])) {}

    cyclat(bravais_lattice const &bl, nda::matrix<long> const &pm) : cyclat(bl, std::array{pm(0, 0), pm(1, 1), pm(2, 2)}) {
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

    bool operator==(cyclat const &M) const { return mesh_hash() == M.mesh_hash(); }
    bool operator!=(cyclat const &M) const { return !(operator==(M)); }

    // ------------------- Accessors -------------------

    /// The Hash for the mesh configuration
    [[nodiscard]] size_t mesh_hash() const { return _mesh_hash; }

    /// The total number of points in the mesh
    [[nodiscard]] long size() const { return size_; }

    /// The extent of each dimension
    [[nodiscard]] std::array<long, 3> dims() const { return dims_; }

    /// Matrix containing the mesh basis vectors as rows
    [[nodiscard]] nda::matrix_const_view<double> units() const { return units_; }

    bravais_lattice const &lattice() const noexcept { return bl_; }

    // ----------------------------------------

    index_t index_modulo(index_t const &r) const {
      return {positive_modulo(r[0], dims_[0]), positive_modulo(r[1], dims_[1]), positive_modulo(r[2], dims_[2])};
    }

    // -------------------- mesh_point -------------------

    struct mesh_point_t : public value_t {
      using mesh_t = cyclat;

      private:
      long _data_index    = 0;
      uint64_t _mesh_hash = 0;

      public:
      mesh_point_t() = default;
      mesh_point_t(std::array<long, 3> const &index, long data_index, uint64_t mesh_hash, bravais_lattice const *bl_ptr)
         : value_t(index, bl_ptr), _data_index(data_index), _mesh_hash(mesh_hash) {}

      /// The data index of the mesh point
      [[nodiscard]] long data_index() const { return _data_index; }

      /// The value of the mesh point
      [[nodiscard]] value_t const &value() const { return *this; }

      /// The Hash for the mesh configuration
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }

      friend std::ostream &operator<<(std::ostream &out, mesh_point_t const &x) { return out << x.value(); }
    };

    // -------------------- checks -------------------

    [[nodiscard]] bool is_index_valid(index_t const &index) const noexcept {
      for (auto i : range(3))
        if (index[i] < 0 or index[i] >= dims_[i]) return false;
      return true;
    }

    // -------------------- to_data_index -------------------

    [[nodiscard]] data_index_t to_data_index(index_t const &index) const {
      EXPECTS(is_index_valid(index));
      return index[0] * stride0 + index[1] * stride1 + index[2];
    }

    template <typename V> [[nodiscard]] data_index_t to_data_index(closest_mesh_point_t<V> const &cmp) const { return to_data_index(to_index(cmp)); }

    // -------------------- to_index -------------------

    [[nodiscard]] index_t to_index(data_index_t data_index) const {
      EXPECTS(0 <= data_index and data_index < size());
      long i0 = data_index / stride0;
      long r0 = data_index % stride0;
      long i1 = r0 / stride1;
      long i2 = r0 % stride1;
      return {i0, i1, i2};
    }

    [[nodiscard]] index_t to_index(closest_mesh_point_t<value_t> const &cmp) const { return cmp.value.index(); }

    // -------------------- operator [] () -------------------

    /// Make a mesh point from a linear index
    [[nodiscard]] mesh_point_t operator[](long data_index) const {
      auto index = to_index(data_index);
      EXPECTS(is_index_valid(index));
      return {index, data_index, _mesh_hash, &bl_};
    }

    [[nodiscard]] mesh_point_t operator[](closest_mesh_point_t<value_t> const &cmp) const { return (*this)[this->to_data_index(cmp)]; }

    [[nodiscard]] mesh_point_t operator()(index_t const &index) const {
      EXPECTS(is_index_valid(index));
      auto data_index = to_data_index(index);
      return {index, data_index, _mesh_hash, &bl_};
    }

    // -------------------- to_value -------------------

    /// Convert an index to a lattice value
    [[nodiscard]] value_t to_value(index_t const &index) const {
      EXPECTS(is_index_valid(index));
      return {index, &bl_};
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
      write_hdf5_format(gr, m); // NOLINT

      h5::write(gr, "dims", m.dims_);
      h5::write(gr, "bravais_lattice", m.bl_);
    }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, cyclat &m) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, m, true);

      std::array<long, 3> dims; //NOLINT
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

    friend auto evaluate(cyclat const &m, auto const &f, index_t const &index) { return f(m.index_modulo(index)); }
    friend auto evaluate(cyclat const &m, auto const &f, value_t const &v) { return evaluate(m, f, v.index()); }
  };

  static_assert(MeshWithValues<cyclat>);

} // namespace triqs::mesh
