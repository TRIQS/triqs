// Copyright (c) 2025 Simons Foundation
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
// Authors: Nils Wentzell

/**
 * @file
 * @brief Provides a mesh type for Chebyshev collocation points in imaginary time.
 */

#pragma once

#include "./mesh_iterator.hpp"
#include "./utils.hpp"
#include "../utility/chebyshev.hpp"
#include "../utility/macros.hpp"

#include <fmt/format.h>
#include <h5/h5.hpp>
#include <nda/nda.hpp>

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

namespace triqs::mesh {

  /**
   * @addtogroup triqs-meshes-imag
   * @{
   */

  /**
   * @brief Chebyshev imaginary time mesh type.
   *
   * @details A Chebyshev mesh satisfies the triqs::mesh::MeshWithValues concept. It stores function values at
   * Chebyshev collocation points and uses barycentric interpolation for evaluation at arbitrary imaginary times,
   * providing exponential convergence for smooth functions.
   *
   * The mesh is defined by:
   * - \f$ N \f$: number of Chebyshev points (polynomial order)
   * - \f$ \beta \f$: inverse temperature (interval \f$ [0, \beta] \f$)
   * - `stat`: particle statistics (Boson/Fermion)
   *
   * Chebyshev points of the first kind are used:
   * \f[
   *   x_i = \cos\left(\frac{(2i + 1) \pi}{2N}\right) \quad \text{for } i = 0, \ldots, N-1 \text{ on } [-1, 1]
   * \f]
   *
   * These are scaled to \f$ [0, \beta] \f$ as:
   * \f[
   *   \tau_i = \frac{\beta}{2} (x_i + 1)
   * \f]
   *
   * Properties:
   * - Each mesh point has index \f$ n \in \{0, \ldots, N-1\} \f$
   * - Identity mapping: \f$ d(n) = n \f$, \f$ n(d) = d \f$
   * - \f$ \text{value}(n) = \tau_n \f$ (Chebyshev point scaled to \f$ [0, \beta] \f$)
   * - Evaluation uses barycentric interpolation (numerically stable)
   *
   * @code
   * #include <fmt/base.h>
   * #include <triqs/mesh.hpp>
   *
   * int main() {
   *   // initialize a Chebyshev mesh with beta = 10, fermionic statistics and 5 points
   *   triqs::mesh::chebyshev m{10, triqs::mesh::Fermion, 5};
   *
   *   // loop over all mesh points and print their index, data index and value
   *   for (int i = 0; auto mp : m)
   *     fmt::println("mesh point #{}: index = {}, data index = {}, value = {}", i++, mp.index(), mp.data_index(), mp.value());
   * }
   * @endcode
   */
  class C2PY_RENAME(MeshChebyshev) chebyshev {
    public:
    /// Value type (imaginary time tau).
    using value_t = double;

    /// Index type.
    using index_t = long;

    /// Data index type.
    using data_index_t = long;

    /**
     * @brief %Mesh point of a triqs::mesh::chebyshev mesh.
     * @details It stores the index \f$ n \f$, the data index \f$ d \f$, the hash value of the parent mesh,
     * and the value \f$ \tau_n \f$ (Chebyshev point scaled to \f$ [0, \beta] \f$).
     */
    class C2PY_IGNORE mesh_point_t {
      public:
      /// Parent mesh type.
      using mesh_t = chebyshev;

      /// Default constructor leaves the mesh point uninitialized.
      mesh_point_t() = default;

      /**
       * @brief Construct a mesh point with a given index \f$ n \f$, data index \f$ d \f$, hash value of the
       * parent mesh and value \f$ \tau_n \f$.
       *
       * @param n Index \f$ n \f$ of the mesh point.
       * @param d Data index \f$ d \f$ of the mesh point.
       * @param mhash Hash value of the parent mesh.
       * @param tau Value \f$ \tau_n \f$ of the mesh point.
       */
      mesh_point_t(long n, long d, uint64_t mhash, double tau) : index_(n), data_index_(d), mesh_hash_(mhash), value_(tau) {}

      /// Get the index \f$ n \f$ of the mesh point.
      [[nodiscard]] long index() const { return index_; }

      /// Get the data index \f$ d \f$ of the mesh point.
      [[nodiscard]] long data_index() const { return data_index_; }

      /// Get the value \f$ \tau_n \f$ of the mesh point.
      [[nodiscard]] double value() const { return value_; }

      /// Get the hash value of the parent mesh.
      [[nodiscard]] uint64_t mesh_hash() const noexcept { return mesh_hash_; }

      /// Implicit conversion to the value \f$ \tau_n \f$.
      operator double() const { return value_; } // NOLINT (implicit conversion intended)

      // Arithmetic operations with scalars.
#define IMPL_OP(OP)                                                                                                                                  \
  template <typename U> friend auto operator OP(mesh_point_t const &mp, U &&y) { return mp.value() OP std::forward<U>(y); }                          \
  template <typename U>                                                                                                                              \
    requires(not std::is_same_v<std::decay_t<U>, mesh_point_t>)                                                                                      \
  friend auto operator OP(U &&x, mesh_point_t const &mp) {                                                                                           \
    return std::forward<U>(x) OP mp.value();                                                                                                         \
  }
      IMPL_OP(+)
      IMPL_OP(-)
      IMPL_OP(*)
      IMPL_OP(/)
#undef IMPL_OP

      private:
      long index_         = 0;
      long data_index_    = 0;
      uint64_t mesh_hash_ = 0;
      double value_       = 0.0;
    };

    /// Default constructor constructs an empty mesh.
    chebyshev() = default;

    /**
     * @brief Construct a Chebyshev mesh on \f$ [0, \beta] \f$ with \f$ N \f$ collocation points.
     *
     * @param beta Inverse temperature \f$ \beta > 0 \f$.
     * @param stat Particle statistics (see triqs::mesh::statistic_enum).
     * @param N Number of Chebyshev points \f$ N > 0 \f$.
     */
    chebyshev(double beta, statistic_enum stat, long N) : beta_(beta), stat_(stat), N_(N), mesh_hash_(hash(beta, stat, N)) {
      EXPECTS(beta_ > 0);
      EXPECTS(N_ > 0);
      init_arrays();
    }

    /// Equal-to comparison operator compares \f$ N \f$, \f$ \beta \f$ and the particle statistics.
    bool operator==(chebyshev const &) const = default;

    /**
     * @brief Check if an index \f$ n \f$ is valid.
     *
     * @param n Index \f$ n \f$ to check.
     * @return True if \f$ 0 \leq n < N \f$, false otherwise.
     */
    [[nodiscard]] bool is_index_valid(index_t n) const noexcept { return 0 <= n and n < N_; }

    /**
     * @brief Map an index \f$ n \in \{0, 1, \ldots, N-1\} \f$ to its corresponding data index \f$ d(n) \f$.
     *
     * @param n Index \f$ n \f$ to map.
     * @return Data index \f$ d(n) = n \f$.
     */
    [[nodiscard]] data_index_t to_data_index(index_t n) const noexcept {
      EXPECTS(is_index_valid(n));
      return n;
    }

    /**
     * @brief Map a data index \f$ d \in \{0, 1, \ldots, N-1\} \f$ to the corresponding index \f$ n(d) \f$.
     *
     * @param d Data index \f$ d \f$ to map.
     * @return Index \f$ n(d) = d \f$.
     */
    [[nodiscard]] index_t to_index(data_index_t d) const noexcept {
      EXPECTS(is_index_valid(d));
      return d;
    }

    /**
     * @brief Map an index \f$ n \f$ to its corresponding value \f$ \tau_n \f$.
     *
     * @param n Index \f$ n \f$ to map.
     * @return Value \f$ \tau_n \f$ (Chebyshev point scaled to \f$ [0, \beta] \f$).
     */
    [[nodiscard]] value_t to_value(index_t n) const noexcept {
      EXPECTS(is_index_valid(n));
      return points_scaled_[n];
    }

    /**
     * @brief Subscript operator to access a mesh point by its data index \f$ d \in \{0, 1, \ldots, N-1\} \f$.
     *
     * @param d Data index \f$ d \f$ of the mesh point.
     * @return mesh_point_t with the index \f$ n(d) = d \f$, data index \f$ d \f$, hash value and value \f$ \tau_d \f$.
     */
    [[nodiscard]] mesh_point_t operator[](long d) const { return {to_index(d), d, mesh_hash_, to_value(d)}; }

    /**
     * @brief Function call operator to access a mesh point by its index \f$ n \in \{0, 1, \ldots, N-1\} \f$.
     *
     * @param n Index \f$ n \f$ of the mesh point.
     * @return mesh_point_t with the index \f$ n \f$, data index \f$ d(n) = n \f$, hash value and value \f$ \tau_n \f$.
     */
    [[nodiscard]] mesh_point_t operator()(long n) const { return {n, to_data_index(n), mesh_hash_, to_value(n)}; }

    /// Get the inverse temperature \f$ \beta \f$.
    [[nodiscard]] C2PY_PROPERTY_GET(beta) double beta() const noexcept { return beta_; }

    /// Get the particle statistics.
    [[nodiscard]] C2PY_PROPERTY_GET(statistic) statistic_enum statistic() const noexcept { return stat_; }

    /// Get the size \f$ N \f$ of the mesh, i.e. the number of mesh points.
    [[nodiscard]] long size() const { return N_; }

    /// Get the hash value of the mesh.
    [[nodiscard]] C2PY_PROPERTY_GET(mesh_hash) uint64_t mesh_hash() const { return mesh_hash_; }

    /// Access to Chebyshev points on [-1, 1].
    [[nodiscard]] nda::vector_const_view<double> points_standard() const { return points_standard_; }

    /// Access to Chebyshev points scaled to [0, beta].
    [[nodiscard]] C2PY_PROPERTY_GET(points) nda::vector_const_view<double> points() const { return points_scaled_; }

    /// Access to barycentric weights.
    [[nodiscard]] C2PY_PROPERTY_GET(weights) nda::vector_const_view<double> weights() const { return weights_; }

    /// Get an iterator to the beginning of the mesh.
    [[nodiscard]] auto begin() const { return mesh_iterator<chebyshev>{.mesh_ptr = this, .data_index = 0}; }

    /// Get a const iterator to the beginning of the mesh.
    [[nodiscard]] auto cbegin() const { return begin(); }

    /// Get an iterator to the end of the mesh.
    [[nodiscard]] auto end() const { return mesh_iterator<chebyshev>{.mesh_ptr = this, .data_index = size()}; }

    /// Get a const iterator to the end of the mesh.
    [[nodiscard]] auto cend() const { return end(); }

    /**
     * @brief Write a triqs::mesh::chebyshev mesh to a `std::ostream`.
     *
     * @param sout `std::ostream` object.
     * @param m %Mesh to be written.
     * @return Reference to `std::ostream` object.
     */
    friend std::ostream &operator<<(std::ostream &sout, chebyshev const &m) {
      auto stat_cstr = (m.stat_ == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("Chebyshev mesh with beta = {}, statistics = {}, N = {}", m.beta_, stat_cstr, m.N_);
    }

    /**
     * @brief Serialize the mesh to a generic archive.
     * @param ar Archive to serialize to.
     */
    void serialize(auto &ar) const { ar & beta_ & stat_ & N_ & mesh_hash_; }

    /**
     * @brief Deserialize the mesh from a generic archive.
     * @param ar Archive to deserialize from.
     */
    void deserialize(auto &ar) {
      ar & beta_ & stat_ & N_ & mesh_hash_;
      if (N_ > 0) { init_arrays(); }
    }

    /// Get the HDF5 format tag.
    [[nodiscard]] static std::string hdf5_format() { return "MeshChebyshev"; }

    /**
     * @brief Write a triqs::mesh::chebyshev mesh to HDF5.
     *
     * @param g `h5::group` to be written to.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be written.
     */
    friend void h5_write(h5::group g, std::string const &name, chebyshev const &m) {
      h5::group gr = g.create_group(name);
      h5::write_hdf5_format(gr, m); // NOLINT (downcasting to base class)
      h5::write(gr, "beta", m.beta_);
      h5::write(gr, "statistic", (m.stat_ == Fermion ? "F" : "B"));
      h5::write(gr, "n_chebyshev", m.N_);
    }

    /**
     * @brief Read a triqs::mesh::chebyshev mesh from HDF5.
     *
     * @param g `h5::group` to be read from.
     * @param name Name of the subgroup.
     * @param m %Mesh object to be read into.
     */
    friend void h5_read(h5::group g, std::string const &name, chebyshev &m) {
      h5::group gr = g.open_group(name);
      h5::assert_hdf5_format(gr, m, true); // NOLINT (downcasting to base class)

      auto beta      = h5::read<double>(gr, "beta");
      auto statistic = (h5::read<std::string>(gr, "statistic") == "F" ? Fermion : Boson);
      auto N         = h5::read<long>(gr, "n_chebyshev");

      m = chebyshev(beta, statistic, N);
    }

    private:
    // Initialize precomputed arrays from beta_ and N_
    void init_arrays() {
      points_standard_ = utility::chebyshev_points(N_);
      weights_         = utility::chebyshev_barycentric_weights(N_);
      points_scaled_   = nda::vector<double>(N_);
      for (long i = 0; i < N_; ++i) { points_scaled_[i] = utility::from_standard_interval(points_standard_[i], 0.0, beta_); }
    }

    double beta_         = 1.0;
    statistic_enum stat_ = Fermion;
    long N_              = 0;
    uint64_t mesh_hash_  = 0;

    // Precomputed arrays.
    nda::vector<double> points_standard_{}; // Chebyshev points on [-1, 1]
    nda::vector<double> points_scaled_{};   // Chebyshev points on [0, beta]
    nda::vector<double> weights_{};         // Barycentric weights
  };

  namespace detail {

    // Type-correct return for exact mesh point match
    template <typename F> auto make_exact_result(F const &f, long i) {
      using R = std::decay_t<decltype(f(0))>;
      if constexpr (nda::is_scalar_v<R>) {
        return f(i);
      } else if constexpr (requires { f(0).mesh(); }) {
        return typename R::regular_type{f(i)};
      } else {
        return nda::make_regular(f(i));
      }
    }

    // Scalar barycentric interpolation
    template <typename R, typename F, typename Points, typename Weights>
    R barycentric_scalar(double x, Points const &points, Weights const &weights, F const &f, long N) {
      // For complex callables (e.g. product mesh curry), precompute values to avoid repeated lambda overhead
      constexpr bool f_is_complex = sizeof(f) > 2 * sizeof(void *);

      if constexpr (f_is_complex) {
        for (long i = 0; i < N; ++i) {
          if (x == points[i]) { return f(i); }
        }

        constexpr long stack_threshold = 128;
        R fval_stack[stack_threshold];
        std::unique_ptr<R[]> fval_heap;
        R *fval = (N <= stack_threshold) ? fval_stack : (fval_heap = std::make_unique<R[]>(N)).get();
        for (long i = 0; i < N; ++i) { fval[i] = f(i); }

        R num{0};
        double den = 0.0;
        for (long i = 0; i < N; ++i) {
          double q = weights[i] / (x - points[i]);
          num += q * fval[i];
          den += q;
        }
        return num / den;
      } else {
        // Simple accessor: single-pass barycentric formula
        R num{0};
        double den = 0.0;
        for (long i = 0; i < N; ++i) {
          double diff = x - points[i];
          if (diff == 0.0) { return f(i); }
          double q = weights[i] / diff;
          num += q * f(i);
          den += q;
        }
        return num / den;
      }
    }

    // Array/gf barycentric interpolation
    template <typename F, typename Points, typename Weights>
    auto barycentric_array(double x, Points const &points, Weights const &weights, F const &f, long N) {
      using f_ret_t = std::decay_t<decltype(f(0))>;

      // Compute normalized barycentric weights
      constexpr long stack_threshold = 512;
      double q_stack[stack_threshold];
      std::unique_ptr<double[]> q_heap;
      double *q = (N <= stack_threshold) ? q_stack : (q_heap = std::make_unique<double[]>(N)).get();

      double denominator = 0.0;
      for (long i = 0; i < N; ++i) {
        double diff = x - points[i];
        if (diff == 0.0) { return make_exact_result(f, i); }
        q[i] = weights[i] / diff;
        denominator += q[i];
      }
      double inv_denom = 1.0 / denominator;
      for (long i = 0; i < N; ++i) { q[i] *= inv_denom; }

      // Accumulate weighted sum
      if constexpr (requires { f(0).mesh(); }) {
        using gf_t = typename f_ret_t::regular_type;
        gf_t result{q[0] * f(0)};
        for (long i = 1; i < N; ++i) { result() += q[i] * f(i); }
        return result;
      } else {
        constexpr bool f_returns_expression = !requires { f(0).data(); };

        auto result    = nda::make_regular(q[0] * f(0));
        using scalar_t = typename decltype(result)::value_type;
        scalar_t *r    = result.data();
        long const sz  = result.size();

        for (long i = 1; i < N; ++i) {
          auto const fi = [&] {
            if constexpr (f_returns_expression)
              return nda::make_regular(f(i));
            else
              return f(i);
          }();
          scalar_t const *fi_data = fi.data();
          for (long j = 0; j < sz; ++j) { r[j] += q[i] * fi_data[j]; }
        }
        return result;
      }
    }

  } // namespace detail

  /**
   * @brief Barycentric interpolation of a function \f$ f \f$ at an imaginary time point \f$ \tau \in [0, \beta] \f$.
   *
   * @details Uses the barycentric interpolation formula:
   * \f[
   *   f(\tau) = \frac{\sum_{i=0}^{N-1} \frac{w_i}{x - x_i} f_i}{\sum_{i=0}^{N-1} \frac{w_i}{x - x_i}}
   * \f]
   * where \f$ x = 2\tau/\beta - 1 \f$ is the scaled coordinate, \f$ x_i \f$ are the Chebyshev points,
   * \f$ w_i \f$ are the barycentric weights, and \f$ f_i = f(i) \f$ are the function values.
   *
   * @param m triqs::mesh::chebyshev mesh.
   * @param f Callable object \f$ f \f$ containing the function values \f$ f_n = f(\tau_n) \f$ at the mesh points.
   * @param tau Imaginary time point \f$ \tau \in [0, \beta] \f$ at which to interpolate the function.
   * @return Barycentric interpolation of \f$ f(\tau) \f$.
   */
  inline auto evaluate(chebyshev const &m, auto const &f, double tau) {
    EXPECTS(m.size() > 0 and tau >= 0 and tau <= m.beta());

    double x            = utility::to_standard_interval(tau, 0.0, m.beta());
    auto const &points  = m.points_standard();
    auto const &weights = m.weights();
    long N              = m.size();

    using f_ret_t = std::decay_t<decltype(f(0))>;

    if constexpr (nda::is_scalar_v<f_ret_t>)
      return detail::barycentric_scalar<f_ret_t>(x, points, weights, f, N);
    else
      return detail::barycentric_array(x, points, weights, f, N);
  }

  /** @} */

} // namespace triqs::mesh
