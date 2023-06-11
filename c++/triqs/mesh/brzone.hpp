// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
// Copyright (c) 2023 Hugo U.R. Strand
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
// Authors: Philipp Dumitrescu, Dominik Kiese, Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell, Thomas Ayral

#pragma once
#include "utils.hpp"
#include <triqs/lattice/brillouin_zone.hpp>
#include <nda/linalg.hpp>
#include "./k_expr.hpp"

namespace triqs::mesh {

  using lattice::brillouin_zone;

  /// Mesh on Brillouin zone
  class brzone {

    public:
    using index_t      = std::array<long, 3>;
    using data_index_t = long;
    using value_t      = brillouin_zone::value_t;

    // -------------------- Data -------------------
    private:
    brillouin_zone bz_        = {};
    std::array<long, 3> dims_ = {0, 0, 0};
    long size_                = 0;
    long stride1 = 1, stride0 = 1;
    nda::matrix<double> units_     = nda::eye<double>(3);
    nda::matrix<double> units_inv_ = nda::eye<double>(3);
    uint64_t _mesh_hash            = 0;

    // -------------------- Constructors -------------------
    public:
    brzone() = default;

    /**
     * Construct mesh on a Brillouin-zone
     *
     * The unit vectors $U$ of the cluster-mesh are constructed such that
     *
     *   $$K = N * U$$
     *
     * where $K$ is the reciprocal matrix and $N$ the periodization matrix.
     *
     * @param bz Brillouin zone (domain)
     * @param dims The extents for each of the three dimensions
     */
    brzone(brillouin_zone const &bz, std::array<long, 3> const &dims)
       : bz_(bz),
         dims_(dims),
         size_(nda::stdutil::product(dims)),
         stride1(dims_[2]),
         stride0(dims_[1] * dims_[2]),
         units_(inverse(1.0 * nda::diag(dims)) * bz.units()),
         units_inv_(inverse(units_)),
         _mesh_hash(hash(sum(bz.units()), dims[0], dims[1], dims[2])) {}

    ///
    brzone(brillouin_zone const &bz, nda::matrix<long> const &pm) : brzone(bz, std::array{pm(0, 0), pm(1, 1), pm(2, 2)}) {
      // The index_modulo operation currently assumes a diagonal periodization matrix by treating each index element separately.
      // It needs to be generalized to use only the periodicity as specified in the periodization matrix, i.e.
      //   $$ (i, j, k) -> (i, j, k) + (n1, n2, n3) * periodization_matrix $$
      // nda::is_diagonal(mat, precision) --> add to lib
      EXPECTS((pm.shape() == std::array{3l, 3l}));
      if (not is_matrix_diagonal(pm)) throw std::runtime_error{"Non-diagonal periodization matrices are currently not supported."};
    }

    /** 
     * Construct a brzone mesh on a given brillouin zone
     * with n_k mesh-points in each reciprocal direction
     * i.e. using a diagonal periodization matrix
     *
     * @param bz Brillouin zone (domain)
     * @param n_k Number of mesh-points in each reciprocal direction

     */
    brzone(brillouin_zone const &bz_, long n_k)
       : brzone(bz_, std::array{n_k, (bz_.lattice().ndim() >= 2 ? n_k : 1l), (bz_.lattice().ndim() >= 3 ? n_k : 1)}) {}

    // ------------------- Comparison -------------------

    bool operator==(brzone const &M) const { return mesh_hash() == M.mesh_hash(); }
    bool operator!=(brzone const &M) const { return !(operator==(M)); }

    // ------------------- Accessors -------------------

    /// The Hash for the mesh configuration
    [[nodiscard]] size_t mesh_hash() const { return _mesh_hash; }

    /// The total number of points in the mesh
    [[nodiscard]] long size() const { return size_; }

    /// The extent of each dimension
    [[nodiscard]] std::array<long, 3> dims() const { return dims_; }

    /// Matrix containing the mesh basis vectors as rows
    [[nodiscard]] nda::matrix_const_view<double> units() const { return units_; }

    brillouin_zone const &bz() const noexcept { return bz_; }

    // ----------------------------------------

    index_t index_modulo(index_t const &r) const {
      return {positive_modulo(r[0], dims_[0]), positive_modulo(r[1], dims_[1]), positive_modulo(r[2], dims_[2])};
    }

    // -------------------- mesh_point -------------------

    struct mesh_point_t {
      using mesh_t = brzone;

      private:
      std::array<long, 3> _index            = {0, 0, 0};
      brzone const *_m_ptr                  = nullptr;
      long _data_index                      = 0;
      uint64_t _mesh_hash                   = 0;
      mutable std::optional<value_t> _value = {};

      public:
      mesh_point_t() = default;
      mesh_point_t(std::array<long, 3> const &index, brzone const *m_ptr, long data_index)
         : _index(index), _m_ptr(m_ptr), _data_index(data_index), _mesh_hash(m_ptr->mesh_hash()) {}

      /// The index of the mesh point
      [[nodiscard]] mesh_t::index_t index() const { return _index; }

      /// The data index of the mesh point
      [[nodiscard]] long data_index() const { return _data_index; }

      /// The value of the mesh point
      [[nodiscard]] value_t const &value() const {
        if (_value)
          return *_value;
        else
          return *(_value = _m_ptr->to_value(_index));
      }

      /// The Hash for the mesh configuration
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return _mesh_hash; }

      [[nodiscard]] operator value_t() const { return value(); }

      // The mesh point behaves like a vector
      [[deprecated("() is deprecated for a brzone::mesh_point_t. Use [] instead")]] double operator()(int d) const { return value()[d]; }
      double operator[](int d) const { return value()[d]; }

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

    template <typename V> [[nodiscard]] data_index_t to_data_index(closest_mesh_point_t<V> const &cmp) const {
      return to_data_index(closest_index(cmp.value));
    }

    template <char OP, typename L> [[nodiscard]] data_index_t to_data_index(k_expr_unary<OP, L> const &ex) const {
      EXPECTS(_mesh_hash == ex.mesh_hash());
      return this->to_data_index(this->index_modulo(ex.index()));
    }

    template <char OP, typename L, typename R> [[nodiscard]] data_index_t to_data_index(k_expr<OP, L, R> const &ex) const {
      EXPECTS(_mesh_hash == ex.mesh_hash());
      return this->to_data_index(this->index_modulo(ex.index()));
    }

    // -------------------- to_index -------------------

    [[nodiscard]] index_t to_index(data_index_t data_index) const {
      EXPECTS(0 <= data_index and data_index < size());
      long i0 = data_index / stride0;
      long r0 = data_index % stride0;
      long i1 = r0 / stride1;
      long i2 = r0 % stride1;
      return {i0, i1, i2};
    }

    //-------------

    template <typename V>
      requires(is_k_expr<V> or std::ranges::contiguous_range<V> or nda::ArrayOfRank<V, 1>)
    [[nodiscard]] index_t closest_index(V const &k) const {

      if constexpr (is_k_expr<V>)
        return closest_index(k.value());
      else {
        // calculate k in the brzone basis
        auto ks      = nda::stack_vector<double, 3>{k[0], k[1], k[2]};
        auto k_units = transpose(units_inv_) * ks;
        auto n       = nda::floor(k_units);

        // calculate position relative to neighbors in mesh
        auto w = k_units - n;

        // prepare result container and distance measure
        auto dst = std::numeric_limits<double>::infinity();

        // check flatness along mesh dimensions
        long r1 = std::min(dims()[0], 2l);
        long r2 = std::min(dims()[1], 2l);
        long r3 = std::min(dims()[2], 2l);

        // find nearest neighbor by comparing distances
        nda::stack_vector<long, 3> res;
        for (auto const &[i1, i2, i3] : itertools::product_range(r1, r2, r3)) {
          auto iv   = nda::stack_vector<long, 3>{i1, i2, i3};
          auto dstp = nda::norm(transpose(units()) * (w - iv));

          // update result when distance is smaller than current
          if (dstp < dst) {
            dst = dstp;
            res = n + iv;
          }
        }

        // fold back to brzone mesh (nearest neighbor could be out of bounds)
        return index_modulo({res[0], res[1], res[2]});
      }
    }

    template <typename V> [[nodiscard]] index_t to_index(closest_mesh_point_t<V> const &cmp) const {
      static_assert(is_k_expr<V> or std::ranges::contiguous_range<V> or nda::ArrayOfRank<V, 1>);
      return closest_index(cmp.value);
    }

    // -------------------- operator [] () -------------------

    /// Make a mesh point from a linear index
    [[nodiscard]] mesh_point_t operator[](long data_index) const {
      auto index = to_index(data_index);
      EXPECTS(is_index_valid(index));
      return {index, this, data_index};
    }

    [[nodiscard]] mesh_point_t operator[](closest_mesh_point_t<value_t> const &cmp) const { return (*this)[this->to_data_index(cmp)]; }

    [[nodiscard]] mesh_point_t operator()(index_t const &index) const {
      EXPECTS(is_index_valid(index));
      auto data_index = to_data_index(index);
      return {index, this, data_index};
    }

    // -------------------- to_value -------------------

    /// Convert an index to the domain value
    [[nodiscard]] value_t to_value(index_t const &index) const {
      EXPECTS(is_index_valid(index));
      return transpose(units_)(range::all, range(bz_.lattice().ndim())) * nda::basic_array_view{index}(range(bz_.lattice().ndim()));
    }

    // -------------------- print -------------------

    friend std::ostream &operator<<(std::ostream &sout, brzone const &m) {
      return sout << "Brillouin Zone Mesh with linear dimensions " << m.dims() << "\n -- units = " << m.units() << "\n -- brillouin_zone: " << m.bz();
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

    [[nodiscard]] static std::string hdf5_format() { return "MeshBrillouinZone"; }

    friend void h5_write(h5::group fg, std::string const &subgroup_name, brzone const &m) {
      h5::group gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, m);

      h5::write(gr, "dims", m.dims_);
      h5::write(gr, "brillouin_zone", m.bz_);
    }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, brzone &m) {
      h5::group gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, m, true);

      std::array<long, 3> dims; // NOLINT
      if (gr.has_key("dims")) {
        h5::read(gr, "dims", dims);
      } else { // Backward Compat periodization_matrix
        auto pm = h5::read<matrix<long>>(gr, "periodization_matrix");
        dims    = {pm(0, 0), pm(1, 1), pm(2, 2)};
      }

      brillouin_zone bz{};
      if (gr.has_key("brillouin_zone")) {
        h5::read(gr, "brillouin_zone", bz);
      } else if (gr.has_key("bz")) { // Backward Compatibility
        h5::read(gr, "bz", bz);
      } else {
        std::cout << "WARNING: Reading old MeshBrillouinZone without BrillouinZone\n";
      }

      m = brzone(bz, dims);
    }

    // -------------- Evaluation --------------------------

    friend auto evaluate(brzone const &m, auto const &f, index_t const &index) { return f(m.index_modulo(index)); }

    private:
    // Evaluation helpers
    struct brzone1d {
      long dim;
    };

    friend auto evaluate(brzone1d const &m, auto const &f, double vi) {
      long i   = std::floor(vi);
      double w = vi - double(i);
      return (1 - w) * f(positive_modulo(i, m.dim)) + w * f(m.dim == 1 ? 0 : positive_modulo(i + 1, m.dim));
    }

    template <typename V>
    friend auto evaluate(brzone const &m, auto const &f, V const &v)
      requires(std::ranges::contiguous_range<V> or nda::ArrayOfRank<V, 1> or is_k_expr<V>)
    {
      if constexpr (is_k_expr<V>)
        return evaluate(m, f, v.value());
      else {
        auto v_index      = make_regular(transpose(m.units_inv_) * nda::basic_array_view{v});
        auto g            = [&f](long x, long y, long z) { return f(typename brzone::index_t{x, y, z}); };
        auto [d0, d1, d2] = m.dims();
        return evaluate(std::tuple{brzone1d{d0}, brzone1d{d1}, brzone1d{d2}}, g, v_index[0], v_index[1], v_index[2]);
      }
    }
  };

  static_assert(MeshWithValues<brzone>);

  // -------------------------------------------------------------------------------------------
  //                                 Lazy Mesh Point Algebra
  // -------------------------------------------------------------------------------------------

  template <typename T>
  concept BzMeshPoint = is_k_expr<std::decay_t<T>> or std::is_same_v<std::decay_t<T>, brzone::mesh_point_t>;
  template <BzMeshPoint A> k_expr_unary<'-', A> operator-(A &&a) { return {std::forward<A>(a)}; }
  template <BzMeshPoint L, BzMeshPoint R> k_expr<'+', L, R> operator+(L &&l, R &&r) { return {std::forward<L>(l), std::forward<R>(r)}; }
  template <BzMeshPoint L, BzMeshPoint R> k_expr<'-', L, R> operator-(L &&l, R &&r) { return {std::forward<L>(l), std::forward<R>(r)}; }
  template <std::integral Int, BzMeshPoint R> k_expr<'*', Int, R> operator*(Int l, R &&r) { return {l, std::forward<R>(r)}; }

} // namespace triqs::mesh
