#pragma once

#include "./mesh_iterator.hpp"
#include "./utils.hpp"

#include <fmt/format.h>
#include <h5/h5.hpp>
#include <mpi/mpi.hpp>
#include <nda/nda.hpp>

#include <array>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace triqs::mesh {

  namespace detail {

    // Holds the R-vectors of a fourier_poly mesh, both as integer triples and
    // as a packed [nR, 3] matrix of doubles for BLAS-accelerated evaluation.
    struct fourier_poly_ops {
      std::vector<std::array<long, 3>> R_list;
      nda::matrix<double> R_mat;
    };

  } // namespace detail

  /**
   * @brief Mesh type storing a finite set of real-space lattice vectors (R-vectors).
   *
   * @details A `gf<fourier_poly, matrix_valued>` stores matrix-valued Fourier coefficients
   * for each R-vector. Evaluation at k-points is delegated to free Fourier-evaluation
   * kernels in `triqs::tb`.
   */
  class C2PY_RENAME(MeshFourierPoly) fourier_poly {
    public:
    using value_t      = std::array<long, 3>;
    using index_t      = long;
    using data_index_t = long;

    class C2PY_IGNORE mesh_point_t {
      public:
      using mesh_t = fourier_poly;

      mesh_point_t() = default;

      mesh_point_t(long n, long d, uint64_t mhash, std::array<long, 3> val) : index_(n), data_index_(d), mesh_hash_(mhash), value_(val) {}

      [[nodiscard]] long index() const noexcept { return index_; }
      [[nodiscard]] long data_index() const noexcept { return data_index_; }
      [[nodiscard]] std::array<long, 3> value() const noexcept { return value_; }
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return mesh_hash_; }

      operator std::array<long, 3>() const { return value_; } // NOLINT

      private:
      long index_                = 0;
      long data_index_           = 0;
      uint64_t mesh_hash_        = 0;
      std::array<long, 3> value_ = {0, 0, 0};
    };

    fourier_poly() : ops_(std::make_shared<const detail::fourier_poly_ops>()) {}

    fourier_poly(std::vector<std::array<long, 3>> R_list) : mesh_hash_(compute_hash(R_list)), ops_(make_ops(std::move(R_list))) {}

    bool operator==(fourier_poly const &other) const {
      if (ops_ == other.ops_) return true; // cheap shared-ptr identity check
      return ops_->R_list == other.ops_->R_list;
    }

    [[nodiscard]] bool is_index_valid(index_t n) const noexcept { return 0 <= n and n < size(); }

    [[nodiscard]] data_index_t to_data_index(index_t n) const noexcept {
      EXPECTS(is_index_valid(n));
      return n;
    }

    [[nodiscard]] index_t to_index(data_index_t d) const noexcept {
      EXPECTS(is_index_valid(d));
      return d;
    }

    [[nodiscard]] mesh_point_t operator[](long d) const noexcept { return (*this)(d); }

    [[nodiscard]] mesh_point_t operator()(index_t n) const noexcept {
      EXPECTS(is_index_valid(n));
      return {n, n, mesh_hash_, ops_->R_list[n]};
    }

    [[nodiscard]] index_t to_index(value_t const &R) const {
      auto const &rl = ops_->R_list;
      auto it        = std::ranges::find(rl, R);
      if (it == rl.end()) throw std::out_of_range("R-vector not found in fourier_poly mesh");
      return static_cast<index_t>(std::distance(rl.begin(), it));
    }

    [[nodiscard]] std::array<long, 3> to_value(index_t n) const noexcept {
      EXPECTS(is_index_valid(n));
      return ops_->R_list[n];
    }

    [[nodiscard]] uint64_t mesh_hash() const noexcept { return mesh_hash_; }
    [[nodiscard]] long size() const noexcept { return static_cast<long>(ops_->R_list.size()); }

    [[nodiscard]] std::span<const std::array<long, 3>> R_list() const { return ops_->R_list; }
    [[nodiscard]] nda::matrix_const_view<double> R_mat() const { return ops_->R_mat; }

    [[nodiscard]] auto begin() const { return mesh_iterator<fourier_poly>{.mesh_ptr = this, .data_index = 0}; }
    [[nodiscard]] auto cbegin() const { return begin(); }
    [[nodiscard]] auto end() const { return mesh_iterator<fourier_poly>{.mesh_ptr = this, .data_index = size()}; }
    [[nodiscard]] auto cend() const { return end(); }

    friend std::ostream &operator<<(std::ostream &sout, fourier_poly const &m) {
      return sout << fmt::format("Fourier polynomial mesh with {} R-vectors", m.size());
    }

    C2PY_IGNORE friend void mpi_broadcast(fourier_poly &m, mpi::communicator c = {}, int root = 0) {
      auto r_list = m.ops_->R_list;
      mpi::broadcast(r_list, c, root);
      mpi::broadcast(m.mesh_hash_, c, root);
      m.ops_ = make_ops(std::move(r_list));
    }

    [[nodiscard]] static std::string hdf5_format() { return "MeshFourierPoly"; }

    friend void h5_write(h5::group g, std::string const &name, fourier_poly const &m) {
      h5::group gr = g.create_group(name);
      h5::write_hdf5_format(gr, m);
      h5::write(gr, "R_list", m.ops_->R_list);
    }

    friend void h5_read(h5::group g, std::string const &name, fourier_poly &m) {
      h5::group gr = g.open_group(name);
      h5::assert_hdf5_format(gr, m, true);
      std::vector<std::array<long, 3>> r_list;
      h5::read(gr, "R_list", r_list);
      m = fourier_poly(std::move(r_list));
    }

    private:
    static uint64_t compute_hash(std::vector<std::array<long, 3>> const &r_list) {
      long sum = 0;
      for (auto const &R : r_list)
        for (auto v : R) sum += v;
      return hash(sum, static_cast<long>(r_list.size()));
    }

    static std::shared_ptr<const detail::fourier_poly_ops> make_ops(std::vector<std::array<long, 3>> r_list) {
      auto const n = static_cast<long>(r_list.size());
      nda::matrix<double> r_mat(n, 3);
      for (long i = 0; i < n; ++i)
        for (int d = 0; d < 3; ++d) r_mat(i, d) = static_cast<double>(r_list[i][d]);
      return std::make_shared<detail::fourier_poly_ops>(detail::fourier_poly_ops{.R_list = std::move(r_list), .R_mat = std::move(r_mat)});
    }

    uint64_t mesh_hash_                                  = 0;
    std::shared_ptr<const detail::fourier_poly_ops> ops_ = {};
  };

} // namespace triqs::mesh
