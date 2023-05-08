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
#include <triqs/lattice/brillouin_zone.hpp>
#include "./k_pt_alg.hpp"

namespace triqs::mesh {

  using lattice::brillouin_zone;

  /// Mesh on Brillouin zone
  class brzone {

    public:
    using idx_t    = std::array<long, 3>;
    using datidx_t = long;
    using value_t  = brillouin_zone::value_t;

    // -------------------- Data -------------------
    private:
    brillouin_zone bz_        = {};
    std::array<long, 3> dims_ = {1, 1, 1};
    long size_                = 1;
    long stride1 = 1, stride0 = 1;
    nda::matrix<double> units_     = nda::eye<double>(3);
    nda::matrix<double> units_inv_ = nda::eye<double>(3);
    uint64_t mesh_hash_            = 0;

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
         mesh_hash_(hash(sum(bz.units()), dims[0], dims[1], dims[2])) {}

    ///
    brzone(brillouin_zone const &bz, nda::matrix<long> const &pm) : brzone(bz, {pm(0, 0), pm(1, 1), pm(2, 2)}) {
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

    bool operator==(brzone const &M) const { return dims_ == M.dims() and bz_ == M.bz(); }
    bool operator!=(brzone const &M) const { return !(operator==(M)); }

    // ------------------- Accessors -------------------

    /// The Hash for the mesh configuration
    [[nodiscard]] size_t mesh_hash() const { return mesh_hash_; }

    /// The total number of points in the mesh
    [[nodiscard]] size_t size() const { return size_; }

    /// The extent of each dimension
    [[nodiscard]] std::array<long, 3> dims() const { return dims_; }

    /// Matrix containing the mesh basis vectors as rows
    [[nodiscard]] nda::matrix_const_view<double> units() const { return units_; }

    brillouin_zone const &bz() const noexcept { return bz_; }

    // ----------------------------------------

    idx_t idx_modulo(idx_t const &r) const {
      return {positive_modulo(r[0], dims_[0]), positive_modulo(r[1], dims_[1]), positive_modulo(r[2], dims_[2])};
    }

    // -------------------- mesh_point -------------------

    struct mesh_point_t {
      using mesh_t            = brzone;
      std::array<long, 3> idx = {0, 0, 0};
      brzone const *m_ptr;
      long datidx                        = 0;
      uint64_t mesh_hash                 = 0;
      mutable std::optional<value_t> val = {};

      [[nodiscard]] value_t const &value() const {
        if (val)
          return *val;
        else
          return *(val = m_ptr->to_value(idx));
      }

      auto const &get_idx() const { return idx; } // for lazy expr of mesh_point

      [[nodiscard]] operator value_t() const { return value(); }

      // The mesh point behaves like a vector
      double operator()(int d) const { return value()[d]; }
      double operator[](int d) const { return value()[d]; }

      friend std::ostream &operator<<(std::ostream &out, mesh_point_t const &x) { return out << x.value(); }
    };

    // -------------------- index checks and conversions -------------------

    [[nodiscard]] bool is_idx_valid(idx_t const &idx) const noexcept {
      for (auto i : range(3))
        if (idx[i] < 0 or idx[i] >= dims_[i]) return false;
      return true;
    }

    // FIXME Do we really want to terminate on the EXPECTS ? or rather throw (e.g. in Python terminate is irrecoverable)
    [[nodiscard]] datidx_t to_datidx(idx_t const &idx) const {
      EXPECTS(is_idx_valid(idx));
      return idx[0] * stride0 + idx[1] * stride1 + idx[2];
    }

    [[nodiscard]] datidx_t to_datidx(mesh_point_t const &mp) const {
      EXPECTS(mesh_hash_ == mp.mesh_hash);
      return mp.datidx;
    }

    template <typename V> [[nodiscard]] datidx_t to_datidx(closest_mesh_point_t<V> const &cmp) const { return to_datidx(closest_idx(cmp.value)); }

    template <char OP, typename L> [[nodiscard]] datidx_t to_datidx(k_expr_unary<OP, L> const &ex) const {
      EXPECTS(mesh_hash_ == ex.mesh_hash);
      return this->to_datidx(this->idx_modulo(ex.get_idx()));
    }

    template <char OP, typename L, typename R> [[nodiscard]] datidx_t to_datidx(k_expr<OP, L, R> const &ex) const {
      EXPECTS(mesh_hash_ == ex.mesh_hash);
      return this->to_datidx(this->idx_modulo(ex.get_idx()));
    }

    [[nodiscard]] idx_t to_idx(datidx_t datidx) const {
      EXPECTS(0 <= datidx and datidx < size());
      long i0 = datidx / stride0;
      long r0 = datidx % stride0;
      long i1 = r0 / stride1;
      long i2 = i0 % stride1;
      return {i0, i1, i2};
    }

    template <typename V>
      requires(std::ranges::contiguous_range<V> or nda::ArrayOfRank<V, 1>)
    [[nodiscard]] idx_t closest_idx(V const &v) const {
      auto idbl = transpose(units_inv_) * nda::basic_array_view{v};
      return {std::lround(idbl[0]), std::lround(idbl[1]), std::lround(idbl[2])};
    }

    template <typename V>
      requires(is_bz_k_expr<V>)
    [[nodiscard]] idx_t closest_idx(V const &v) const {
      auto idbl = transpose(units_inv_) * nda::basic_array{v.value()};
      return {std::lround(idbl[0]), std::lround(idbl[1]), std::lround(idbl[2])};
    }

    template <typename V>
      requires(std::ranges::contiguous_range<V> or nda::ArrayOfRank<V, 1>)
    [[nodiscard]] idx_t to_idx(closest_mesh_point_t<V> const &cmp) const {
      return closest_idx(cmp.value);
    }

    /// Make a mesh point from a linear index
    [[nodiscard]] mesh_point_t operator[](long datidx) const {
      auto idx = to_idx(datidx);
      EXPECTS(is_idx_valid(idx));
      return {idx, this, datidx, mesh_hash_};
    }

    [[nodiscard]] mesh_point_t operator[](closest_mesh_point_t<value_t> const &cmp) const { return (*this)[this->to_datidx(cmp)]; }

    [[nodiscard]] mesh_point_t operator()(idx_t const &idx) const {
      EXPECTS(is_idx_valid(idx));
      auto datidx = to_datidx(idx);
      return {idx, this, datidx, mesh_hash_};
    }

    /// Convert an index to the domain value
    [[nodiscard]] virtual value_t to_value(idx_t const &idx) const {
      EXPECTS(is_idx_valid(idx));
      return transpose(units_)(range::all, range(bz_.lattice().ndim())) * nda::basic_array_view{idx}(range(bz_.lattice().ndim()));
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

      std::array<long, 3> dims;
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

    private:
    // Evaluation helpers
    struct brzone1d {
      long dim;
    };

    friend auto evaluate(brzone1d const &m, auto const &f, double vi) {
      long i   = std::floor(vi);
      double w = vi - i;
      return (1 - w) * f(positive_modulo(i, m.dim)) + w * f(m.dim == 1 ? 0 : positive_modulo(i + 1, m.dim));
    }

    // Use the cartesian product evaluation to evaluate on domain pts
    template <typename V> friend auto evaluate(brzone const &m, auto const &f, V const &v) {
      // NB : this is a static_assert, not a requires.
      // If m is brzone, there is no other possible overload of evaluate, so we capture every V and check.
      // If V does not satisfy the condition, we will have a clear error message here,
      // rather than a long list of other evaluate with other meshes which are irrelevant anyway.
      static_assert(std::ranges::contiguous_range<V> or nda::ArrayOfRank<V, 1> or is_bz_k_expr<V>, "Incorrect type in evaluate on a brzone");
      auto _get_val = [](auto &&vv) {
        if constexpr (is_bz_k_expr<V>)
          return nda::basic_array{vv.value()};
        else
          return nda::basic_array_view{vv};
      };
      auto v_idx        = make_regular(transpose(m.units_inv_) * _get_val(v));
      auto g            = [&f](long x, long y, long z) { return f(typename brzone::idx_t{x, y, z}); };
      auto [d0, d1, d2] = m.dims();
      return evaluate(std::tuple{brzone1d{d0}, brzone1d{d1}, brzone1d{d2}}, g, v_idx[0], v_idx[1], v_idx[2]);
    }
  };

  static_assert(MeshWithValues<brzone>);

  // Mark the mesh point to include it in the expression grammar
  template <> inline constexpr bool is_bz_k_expr<brzone::mesh_point_t> = true;

} // namespace triqs::mesh
