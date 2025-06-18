// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides tail fitting for functions defined on frequency meshes.
 */

#pragma once

#include "../arrays.hpp"

#include <itertools/itertools.hpp>
#include <nda/nda.hpp>
#include <nda/lapack/gelss_worker.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

namespace triqs::mesh {

  /**
   * @addtogroup triqs-meshes-tailfitting
   * @{
   */

  // Forward declaration.
  class imfreq;

  /**
   * @brief Construct a Vandermonde matrix.
   *
   * @details The resulting matrix will be of size \f$ p \times (q+1) \f$, where \f$ p \f$ is the number of points
   * \f$ z_i \f$ in the input vector and \f$ q \f$ is the expansion order.
   *
   * The Vandermonde matrix has the following form:
   * \f[
   *   V = \begin{bmatrix}
   *   1 & z_0 & z_0^2 & \cdots & z_0^q \\
   *   1 & z_1 & z_1^2 & \cdots & z_1^q \\
   *   1 & z_2 & z_2^2 & \cdots & z_2^q \\
   *   \vdots & \vdots & \vdots & \ddots & \vdots \\
   *   1 & z_{p-1} & z_{p-1}^2 & \cdots & z_{p-1}^q
   *   \end{bmatrix}
   * \f]
   *
   * For more details see <a href="https://en.wikipedia.org/wiki/Vandermonde_matrix">Wikipedia</a>.
   *
   * @param z_pts `std::vector` of size \f$ p \f$ containing the points \f$ z_i \f$.
   * @param q Expansion order \f$ q \f$.
   * @return Vandermonde matrix \f$ V \f$.
   */
  inline auto vander(std::vector<std::complex<double>> const &z_pts, int q) {
    nda::matrix<std::complex<double>> V(z_pts.size(), q + 1);
    for (auto [i, z_i] : itertools::enumerate(z_pts)) {
      auto z = std::complex<double>{1};
      for (int n = 0; n <= q; ++n) {
        V(i, n) = z;
        z *= z_i;
      }
    }
    return V;
  }

  /**
   * @brief Evaluate the tail expansion of a function \f$ f(z) \f$ at a given point \f$ z_0 \f$.
   *
   * @details Suppose the function to be evaluated returns an \f$ R - 1 \f$-dimensional array with shape \f$ (d_1,
   * \dots, d_{R-1}) \f$. Then the given coefficient array \f$ A \f$ is an array of rank \f$ R \f$ with shape \f$
   * (q + 1, d_1, \dots, d_{R-1}) \f$, where \f$ q \f$ is the expansion order.
   *
   * This function simply evaluates
   * \f[
   *   f(z_0) \approx \sum_{n=0}^{q} \frac{A_n}{z_0^n} \; ,
   * \f]
   * where \f$ A_n = A(n, \dots) \f$ is the \f$ R - 1 \f$ dimensional array of coefficients for the n<sup>th</sup> term 
   * in the expansion.
   *
   * @tparam R Rank of the coefficient array.
   * @param A Array \f$ A \f$ of rank \f$ R \f$ containing the coefficients of the tail expansion.
   * @param z_0 Point \f$ z_0 \f$ at which to evaluate the tail expansion.
   * @return Evaluated tail expansion of \f$ f(z_0) \f$ which can either be a complex number (if \f$ R = 1 \f$) or an
   * array of complex numbers (if \f$ R > 1 \f$).
   */
  template <int R> auto tail_eval(nda::array_const_view<std::complex<double>, R> A, std::complex<double> z_0) {
    auto compute = [&A, z_0](auto res) {
      auto z       = std::complex<double>{1};
      auto const q = A.extent(0);
      for (int n = 0; n < q; ++n, z /= z_0) res += A(n, nda::ellipsis{}) * z;
      return res;
    };
    if constexpr (R > 1) {
      // return an array of rank R - 1
      return compute(nda::zeros<std::complex<double>>(nda::stdutil::front_pop(A.shape())));
    } else {
      // return a complex scalar
      return compute(std::complex<double>{0});
    }
  }

  /**
   * @brief Fit the high- and low-frequency tail of a function \f$ f \f$ defined on a triqs::mesh::refreq or a 
   * triqs::mesh::imfreq frequency mesh.
   *
   * @details For the moment, we assume that the function \f$ f \f$ to be fitted only depends on a single frequency
   * argument \f$ z \f$ and is matrix-valued, i.e. \f$ f(z) \in \mathbb{C}^{d_1 \times d_2} \f$. Then we can write the
   * high- and low-frequency expansion as
   * \f[
   *   f(z) = \sum_{n=0}^{q} \frac{A_n}{z^n} + \mathcal{O}(z^{-q-1}) \; ,
   * \f]
   * where \f$ A_n \in \mathbb{C}^{d_1 \times d_2} \f$ is a matrix containing the n<sup>th</sup> order expansion 
   * coefficients.
   *
   * Since the function is defined on discrete mesh points, its values can be stored in a data array \f$ D \f$ of rank 
   * \f$ 3 \f$ with shape \f$ (N, d_1, d_2) \f$, where \f$ N \f$ is the number of points (or discrete frequencies).
   *
   * Let \f$ \Omega = \{ z_l, \dots, z_h \} \f$ be the total set of \f$ N \f$ mesh points, with the lowest frequency \f$
   * z_l \f$ and the highest frequency \f$ z_h \f$. The two subranges of mesh points, \f$ \widetilde{\Omega}_l = \{ z_l,
   * \dots, z_{l+p_r-1} \} \f$ and \f$ \widetilde{\Omega}_h = \{ z_{h-p_r+1}, \dots, z_h \} \f$, relevant for the low-
   * and high-frequency tail fit are determined by \f$ p_r = \lfloor r N / 2 + 0.5 \rfloor \f$ with \f$ 0 < r \leq 1
   * \f$. The actual number of frequencies used for the fitting procedure is \f$ p = \min\{ p_r, p_\text{max} \} \f$,
   * where \f$ p_\text{max} > 0\f$ is a given maximum number of points. \f$ r \f$ and \f$ p_\text{max} \f$ are set in
   * the constructor of the tail fitter.
   *
   * Once we have the \f$ p \f$ low-frequencies \f$ \Omega_l = \{ z_{l_0}, \dots, z_{l_{p-1}} \} \subseteq
   * \widetilde{\Omega}_l \f$ and the \f$ p \f$ high-frequencies \f$ \Omega_h = \{ z_{h_0}, \dots, z_{h_{p-1}} \}
   * \subseteq \widetilde{\Omega}_h \f$, we can write the equations for the unknown coefficients \f$ A_n \f$ as a system
   * of linear equations
   * \f[
   *   \begin{bmatrix}
   *   1 & z_{l_0} & z_{l_0}^{-2} & \cdots & z_{l_0}^{-q} \\
   *   1 & z_{h_0} & z_{h_0}^{-2} & \cdots & z_{h_0}^{-q} \\
   *   1 & z_{l_1} & z_{l_1}^{-2} & \cdots & z_{l_1}^{-q} \\
   *   \vdots & \vdots & \vdots & \ddots & \vdots \\
   *   1 & z_{h_{p-1}} & z_{h_{p-1}}^{-2} & \cdots & z_{h_{p-1}}^{-q} \\
   *   \end{bmatrix}
   *   \begin{bmatrix}
   *   [A_0]_{11} & [A_0]_{12} & [A_0]_{21} & [A_0]_{22} \\
   *   [A_1]_{11} & [A_1]_{12} & [A_1]_{21} & [A_1]_{22} \\
   *   [A_2]_{11} & [A_2]_{12} & [A_2]_{21} & [A_2]_{22} \\
   *   \vdots & \vdots & \vdots & \vdots \\
   *   [A_{q-1}]_{11} & [A_{q-1}]_{12} & [A_{q-1}]_{21} & [A_{q-1}]_{22} \\
   *   \end{bmatrix}
   *   =
   *   \begin{bmatrix}
   *   [f(z_{l_0})]_{11} & [f(z_{l_0})]_{12} & [f(z_{l_0})]_{21} & [f(z_{l_0})]_{22} \\
   *   [f(z_{h_0})]_{11} & [f(z_{h_0})]_{12} & [f(z_{h_0})]_{21} & [f(z_{h_0})]_{22} \\
   *   [f(z_{l_1})]_{11} & [f(z_{l_1})]_{12} & [f(z_{l_1})]_{21} & [f(z_{l_1})]_{22} \\
   *   \vdots & \vdots & \vdots & \vdots \\
   *   [f(z_{h_{p-1}})]_{11} & [f(z_{h_{p-1}})]_{12} & [f(z_{h_{p-1}})]_{21} & [f(z_{h_{p-1}})]_{22} \\
   *   \end{bmatrix} \; ,
   * \f]
   * or in a more compact form
   * \f[
   *  V A = B \; .
   * \f]
   * Here, \f$ V \f$ is the \f$ 2p \times (q+1) \f$ Vandermonde matrix (see also triqs::mesh::vander), \f$ A \f$ is the
   * \f$ (q+1) \times (d_1 \times d_2) \f$ matrix of unknown coefficients and \f$ B \f$ is the \f$ 2p \times (d_1
   * \times d_2) \f$ matrix of function values at the frequencies in \f$ \Omega_l \f$ and \f$ \Omega_h \f$.
   * 
   * @note To make the notation easier, we have assumed in the above equations that \f$ d_1 = d_2 = 2 \f$.
   *
   * This system of equations can then be solved with the linear least squares worker classes
   * `nda::lapack::gelss_worker` or `nda::lapack::gelss_worker_hermitian`. Hermitian means that the coefficient matrices
   * \f$ A_n \f$ are required to be hermitian, i,.e. \f$ [A_n]_{ij} = [A_n]_{ji}^* \f$, which enforces the symmetry \f$
   * [f(z)]_{ij} = [f(-z)]_{ji}^* \f$ in the function values.
   */
  class tail_fitter {

    static constexpr int max_order = 9;
    const double _tail_fraction;
    const int _n_tail_max;
    const bool _adjust_order;
    const int _expansion_order;
    const double _rcond = 1e-8;
    std::array<std::unique_ptr<const nda::lapack::gelss_worker<std::complex<double>>>, max_order + 1> _lss;
    std::array<std::unique_ptr<const nda::lapack::gelss_worker_hermitian>, max_order + 1> _lss_hermitian;
    nda::matrix<std::complex<double>> _vander;
    std::vector<long> _fit_idx_lst;

    public:
    /**
     * @brief Construct a tail fitter for a given fraction \f$ r \f$ of the mesh, the maximum number of mesh points 
     * \f$ p_{\text{max}} \f$ to use in the fit and an optional expansion order \f$ q \f$.
     *
     * @param r Fraction of the mesh to consider in the tail fit (\f$ 0 < r \leq 1 \f$).
     * @param p_max Maximum number of points to use in the tail fit (\f$ p_\text{max} > 0 \f$).
     * @param q Optional expansion order \f$ q \leq q_{\text{max}} = 9 \f$. If not set, it will be adjusted 
     * automatically.
     */
    tail_fitter(double r, int p_max, std::optional<int> q = {})
       : _tail_fraction(r), _n_tail_max(p_max), _adjust_order(not q.has_value()), _expansion_order(_adjust_order ? max_order : *q) {}

    /**
     * @brief Get the number of mesh points used in the tail fit for the given mesh.
     *
     * @details The number of points \f$ p \f$ depends on the size \f$ N \f$ of the underlying mesh, the fraction \f$ r
     * \f$ of the mesh that should be considered and the maximum number of points \f$ p_\text{max} \f$ we want to use:
     * \f[
     *   p = \min \left\{ \left\lfloor \frac{N}{2} r + 0.5 \right\rfloor, p_\text{max} \right\} \; .
     * \f]
     *
     * @tparam M Frequency mesh type (either triqs::mesh::imfreq or triqs::mesh::refreq).
     * @param m Frequency mesh.
     * @return Number of mesh points \f$ p \f$ to use in the fit.
     */
    template <typename M> int n_pts_in_tail(M const &m) const {
      return std::min(static_cast<int>(std::round(_tail_fraction * m.size() / 2)), _n_tail_max);
    }

    /// Default fraction \f$ r \f$ of the mesh to consider in the tail fit.
    static constexpr double default_tail_fraction = 0.2;

    /// Default maximum number of points \f$ p_\text{max} \f$ to use in the tail fit.
    static constexpr int default_n_tail_max = 30;

    /// Get the fraction \f$ r \f$ of the mesh to be considered for the tail fit.
    double get_tail_fraction() const { return _tail_fraction; }

    /**
     * @brief Get a vector containing the indices of all mesh points to use in the tail fit, i.e. \f$ (l_0, h_0, l_1, 
     * h_1, \dots, l_{p-1}, h_{p-1}) \f$.
     *
     * @tparam M Frequency mesh type (either triqs::mesh::imfreq or triqs::mesh::refreq).
     * @param m Frequency mesh.
     * @return `std::vector<long>` containing the indices of the mesh points to use in the fit.
     */
    template <typename M> auto get_tail_fit_indices(M const &m) {
      // total number of points in the fitting window
      auto const p_r = static_cast<int>(std::round(_tail_fraction * m.size() / 2));

      // number of points actually used for the fit
      auto const p = n_pts_in_tail(m);

      // reserve space for the indices
      std::vector<long> idx_vec;
      idx_vec.reserve(2ul * p);

      // initialize the left most and right most indices for both fitting windows
      double const step = static_cast<double>(p_r) / p;
      double left_idx   = m.first_index();
      double right_idx  = m.last_index() - p_r;

      for ([[maybe_unused]] auto i : nda::range(p)) {
        idx_vec.push_back(long(left_idx));
        idx_vec.push_back(long(right_idx));
        left_idx += step;
        right_idx += step;
      }

      return idx_vec;
    }

    /**
     * @brief Get linear least squares workers.
     *
     * @tparam enforce_hermiticity Enforce hermiticity in the coefficient matrices \f$ A_n \f$.
     * @return `std::array` of `std::unique_ptr` objects to `nda::lapack::gelss_worker` or
     * `nda::lapack::gelss_worker_hermitian` objects.
     */
    template <bool enforce_hermiticity = false> auto &get_lss() {
      if constexpr (enforce_hermiticity)
        return _lss_hermitian;
      else
        return _lss;
    }

    /**
     * @brief Set up the linear least squares workers for a given mesh and a given number \f$ n_A \f$ of known 
     * coefficient arrays \f$ A_n \f$.
     *
     * @details To set up the workers, we first determine the frequencies for the tail fit with get_tail_fit_indices()
     * and then build the Vandermonde matrix with triqs::mesh::vander. For numerical reasons, we scale the frequencies
     * by the absolute value of the maximum frequency in the mesh such that \f$ V_{ij} = \left(|z_{\text{max}}| /
     * z_i \right)^{j} \f$. Note that this will also scale the coefficients \f$ \tilde{A}_n = A_n / |z_{\text{max}}|^{n}
     * \f$.
     *
     * The least squares workers are then initialized with the Vandermonde matrix. If the given number \f$ n_A \f$ of 
     * known coefficient arrays is \f$ > 0 \f$, then only the \f$ 2p \times (q - n_A + 1) \f$ submatrix of \f$ V \f$ is 
     * used. That means that only \f$ q - n_A + 1 \f$ coefficients \f$ A_n \f$ with \f$ n \geq n_A \f$ will be 
     * calculated with the fitting procedure.
     *
     * Furthermore, if no expansion order \f$ q \f$ was specified during construction, this function tries to find the 
     * largest \f$ q \f$ such that \f$ n_A \leq q \leq q_{\text{max}} = 9 \f$ and for which the smallest singular value 
     * of the Vandermonde matrix is larger than some threshold \f$ r_{\text{cond}} = 10^{-8} \f$. If no suitable \f$ q 
     * \f$ is found, an exception is thrown.
     *
     * @tparam enforce_hermiticity Enforce hermiticity in the coefficient matrices \f$ A_n \f$.
     * @tparam M Frequency mesh type (either triqs::mesh::imfreq or triqs::mesh::refreq).
     * @param m Frequency mesh.
     * @param n_A Number of known coefficient arrays.
     */
    template <bool enforce_hermiticity = false, typename M> void setup_lss(M const &m, int n_A) {
      // least square worker type
      using worker_t = std::conditional_t<enforce_hermiticity, nda::lapack::gelss_worker_hermitian, nda::lapack::gelss_worker<std::complex<double>>>;

      // indices of the mesh points to use in the tail fit
      if (_fit_idx_lst.empty()) _fit_idx_lst = get_tail_fit_indices(m);

      // set up Vandermonde matrix (the points are given by z_i = |m.w_max()| / m.to_value(n))
      double const z_max = std::abs(m.w_max());
      if (_vander.is_empty()) {
        std::vector<std::complex<double>> z_pts;
        z_pts.reserve(_fit_idx_lst.size());
        for (long n : _fit_idx_lst) z_pts.push_back(z_max / m.to_value(n));
        _vander = vander(z_pts, _expansion_order);
      }

      // check if we have enough data points for the least square procedure (p > n_A + 1)
      if (n_A + 1 > _vander.extent(0) / 2)
        TRIQS_RUNTIME_ERROR << "Error in triqs::mesh::tail_fitter::setup_lss: Insufficient data points for least square procedure";

      // factory function for least square workers
      auto worker_factory = [&](int n) { return std::make_unique<const worker_t>(_vander(nda::range::all, nda::range(n_A, n + 1))); };

      // get the correct (hermitian vs. non-hermitian) array of least square workers
      auto &lss = get_lss<enforce_hermiticity>();

      // set up the least square workers
      if (!_adjust_order) {
        // use the expansion order given in the constructor
        lss[n_A] = worker_factory(_expansion_order);
      } else {
        // find the maximum expansion order such that the smallest singular value of the Vandermonde matrix is > rcond_
        lss[n_A].reset();
        // ensure that |z_max|^{1-q} > 10^{-16}
        long q_max = std::min(static_cast<long>(max_order), static_cast<long>(1. + 16. / std::log10(1 + std::abs(m.w_max()))));
        // we try to use at least two times as many data points as we have unknown coefficients
        q_max = std::min(q_max, _vander.extent(0) / 2);
        for (long q = q_max; q >= n_A; --q) {
          auto ptr = worker_factory(q);
          if (ptr->S_vec()[ptr->S_vec().size() - 1] > _rcond) {
            lss[n_A] = std::move(ptr);
            break;
          }
        }
      }

      // throw an exception if the Vandermonde matrix is ill-conditioned
      if (!lss[n_A]) TRIQS_RUNTIME_ERROR << "Error in triqs::mesh::tail_fitter::setup_lss: Ill-conditioned Vandermonde matrix";
    }

    /**
     * @brief Perform a linear least squares fit and return the coefficients \f$ A_n \f$ of the tail expansion together 
     * with the error of the fit.
     *
     * @details If the function \f$ f \f$ depends on more arguments, we first permute the indices of the given data
     * array \f$ D \f$ such that the frequency mesh we are fitting on corresponds to the first dimension. Then we 
     * combine all other dimensions to form a matrix of size \f$ N \times M \f$, where \f$ N \f$ is the size of the 
     * frequency mesh and \f$ M \f$ is the product of the remaining dimensions. The same is done with the array \f$ C 
     * \f$ containing the known coefficient arrays \f$ A_n \f$ for \f$ n < n_A \f$.
     *
     * Before the fit is performed, the expansion terms corresponding to the known coefficients (contained in \f$ C \f$) 
     * are subtracted from the function values (contained in \f$ D \f$), i.e.
     * \f[
     *   \tilde{f}(z_i) = f(z_i) - \sum_{n=0}^{n_A-1} \tilde{A}_n \left( \frac{|z_{\text{max}}|}{z_i} \right)^n \; ,
     * \f]
     * where \f$ n_A \f$ is the number of known \f$ A_n \f$ and \f$ \tilde{A}_n = A_n / |z_{\text{max}}|^n \f$ (see 
     * setup_lss() for more information).
     *
     * After the least squares procedure, the original coefficients \f$ A_n \f$ can be recovered from \f$ \tilde{A}_n 
     * \f$ by setting `rescale` to true. Otherwise, the scaled coefficients \f$ \tilde{A}_n \f$ are returned.
     *
     * Furthermore, if `enforce_hermiticity` is set to true, the calculated coefficient matrices \f$ A_n \f$ are forced 
     * to be hermitian. This setting only makes sense for triqs::mesh::imfreq Matsubara meshes and requires an inner 
     * matrix dimension \f$ d \f$ to be specified, i.e. the number of rows/columns of the square matrix that \f$ f(z) 
     * \f$ returns.
     *
     * @tparam P Position of the frequency mesh in case of a product mesh.
     * @tparam enforce_hermiticity Enforce hermiticity in the coefficient matrices \f$ A_n \f$.
     * @tparam M Frequency mesh type (either triqs::mesh::imfreq or triqs::mesh::refreq).
     * @tparam R Rank of the data array and the array containing the known moments.
     * @param m Frequency mesh.
     * @param D Data array \f$ D \f$ containing the function values on the (product) mesh points.
     * @param rescale Should we rescale the calculated coefficients \f$ \tilde{A}_n \f$ by \f$ |z_{\text{max}}|^q \f$?
     * @param C Data array \f$ C \f$ containing the known coefficient arrays.
     * @param d Inner matrix dimensions \f$ d \f$ (only needed if `enforce_hermiticity` is true).
     * @return `std::pair` containing the expansion coefficients \f$ A_n/\tilde{A}_n \f$ and the error of the fit.
     */
    template <int P, bool enforce_hermiticity = false, typename M, int R>
    auto fit(M const &m, nda::array_const_view<std::complex<double>, R> D, bool rescale, nda::array_const_view<std::complex<double>, R> C,
             std::optional<long> d = {}) {
      // compile-time and run-time checks
      static_assert(!enforce_hermiticity || std::is_same_v<M, imfreq>);
      if (enforce_hermiticity and not d.has_value())
        TRIQS_RUNTIME_ERROR << "Error in triqs::mesh::tail_fitter::fit: Enforcing hermiticity requires an inner matrix dimension";
      if (m.positive_only()) TRIQS_RUNTIME_ERROR << "Error in triqs::mesh::tail_fitter::fit: Cannot fit on a positive_only mesh";

      // early return if the number of known coefficients is larger than the expansion order
      int const n_A = C.extent(0);
      if (n_A > _expansion_order) return std::pair<nda::array<std::complex<double>, R>, double>{C, 0.0};

      // set up the least squares worker for the given number of known coefficients if it has not been done already
      auto &lss = get_lss<enforce_hermiticity>();
      if (!lss[n_A]) setup_lss<enforce_hermiticity>(m, n_A);

      // permute the indices of D such that the relevant frequency mesh corresponds to the first dimension
      auto D_rot = nda::rotate_index_view<P>(D);

      // flatten D in the target space and the remaining meshes into the second dimension of a new matrix
      long const ncols = D_rot.size() / D_rot.shape()[0];
      auto D_mat       = nda::matrix<std::complex<double>>(_vander.extent(0), ncols);
      for (auto [i, n] : itertools::enumerate(_fit_idx_lst)) {
        if constexpr (R == 1) {
          D_mat(i, 0) = D_rot(m.to_data_index(n));
        } else {
          for (auto [j, x] : itertools::enumerate(D_rot(m.to_data_index(n), nda::ellipsis{}))) { D_mat(i, j) = x; }
        }
      }

      // flatten and prepare the array of known coefficients C
      double const z_max = std::abs(m.w_max());
      if (n_A > 0) {
        // check the shape of C
        if (ncols != C.size() / C.shape()[0])
          TRIQS_RUNTIME_ERROR << "Error in triqs::mesh::tail_fitter::fit: Shape of C array incompatible with the shape of the D array";

        // flatten C and scale its values by |z_max|^{-q}
        double z   = 1.0;
        auto C_mat = nda::matrix<std::complex<double>>(n_A, ncols);
        for (long i : nda::range(n_A)) {
          if constexpr (R == 1) {
            C_mat(i, 0) = z * C(i, nda::ellipsis{});
          } else {
            for (auto [j, x] : itertools::enumerate(C(i, nda::ellipsis{}))) C_mat(i, j) = z * x;
          }
          z /= z_max;
        }

        // subtract the expansion terms corresponding to the known moments from the function values
        D_mat -= _vander(nda::range::all, nda::range(n_A)) * C_mat;
      }

      // perform the least squares procedure
      auto [A_mat, err] = (*lss[n_A])(D_mat, d);

      // rescale the coefficients if requested
      if (rescale) {
        double z = 1.0;
        for ([[maybe_unused]] long i : nda::range(n_A)) z *= z_max;
        for (long i : nda::range(A_mat.extent(0))) {
          A_mat(i, nda::range::all) *= z;
          z *= z_max;
        }
      }

      // reinterpret the result as an R-dimensional array according to the initial shape
      auto shape = D_rot.shape();
      shape[0]   = lss[n_A]->n_var() + n_A;
      auto A     = nda::array<std::complex<double>, R>{shape};

      // add the known moments to the result
      if (n_A) A(nda::range(n_A), nda::ellipsis{}) = C;

      // add the calculated moments to the result
      shape[0]    = lss[n_A]->n_var();
      auto idxmap = typename nda::array<std::complex<double>, R>::layout_t{shape};
      auto A_view = A(nda::range(n_A, A.shape()[0]), nda::ellipsis{});
      A_view      = nda::array_view<std::complex<double>, R>{idxmap, A_mat.storage()};

      return std::pair<nda::array<std::complex<double>, R>, double>{std::move(A), err};
    }

    /**
     * @brief Perform a linear least squares fit and return the coefficients \f$ A_n \f$ of the tail expansion together 
     * with the error of the fit.
     *
     * @details It simply calls fit() with `enforce_hermiticity` set to true.
     * 
     * @tparam P Position of the frequency mesh in case of a product mesh.
     * @tparam M Frequency mesh type (either triqs::mesh::imfreq or triqs::mesh::refreq).
     * @tparam R Rank of the data array and the array containing the known moments.
     * @param m Frequency mesh.
     * @param D Data array \f$ D \f$ containing the function values on the (product) mesh points.
     * @param rescale Should we rescale the calculated coefficients \f$ \tilde{A}_n \f$ by \f$ |z_{\text{max}}|^q \f$?
     * @param C Data array \f$ C \f$ containing the known coefficient arrays.
     * @param d Inner matrix dimensions \f$ d \f$.
     * @return `std::pair` containing the expansion coefficients \f$ A_n/\tilde{A}_n \f$ and the error of the fit.
     */
    template <int P, typename M, int R>
    auto fit_hermitian(M const &m, nda::array_const_view<std::complex<double>, R> D, bool rescale, nda::array_const_view<std::complex<double>, R> C,
                       std::optional<long> d = {}) {
      return fit<P, true, M, R>(m, D, rescale, C, d);
    }
  };

  /**
   * @brief Shared handle for tail fitting.
   * @details It simply stores a `std::shared_ptr` to a triqs::mesh::tail_fitter object.
   */
  struct tail_fitter_handle {
    /**
     * @brief Set the pointer to a new triqs::mesh::tail_fitter object constructed with the given parameters.
     *
     * @param r Fraction of the mesh to consider in the tail fit (\f$ 0 < r \leq 1 \f$).
     * @param p_max Maximum number of points to use in the tail fit (\f$ p_\text{max} > 0 \f$).
     * @param q Optional expansion order \f$ q \leq q_{\text{max}} = 9 \f$. If not set, it will be adjusted 
     * automatically.
     */
    void set_tail_fit_parameters(double r, int p_max = tail_fitter::default_n_tail_max, std::optional<int> q = {}) const {
      _tail_fitter = std::make_shared<tail_fitter>(tail_fitter{r, p_max, q});
    }

    /**
     * @brief Get the triqs::mesh::tail_fitter object.
     * 
     * @details If the tail fitter object has not been created yet, it will be constructed with the default parameters,
     * i.e. tail_fitter::default_tail_fraction and tail_fitter::default_n_tail_max.
     * 
     * @return Tail fitter object.
     */
    tail_fitter &get_tail_fitter() const {
      if (!_tail_fitter) _tail_fitter = std::make_shared<tail_fitter>(tail_fitter::default_tail_fraction, tail_fitter::default_n_tail_max);
      return *_tail_fitter;
    }

    /**
     * @brief Construct a new triqs::mesh::tail_fitter object with the given parameters and return it.
     *
     * @param r Fraction of the mesh to consider in the tail fit (\f$ 0 < r \leq 1 \f$).
     * @param p_max Maximum number of points to use in the tail fit (\f$ p_\text{max} > 0 \f$).
     * @param q Optional expansion order \f$ q \leq q_{\text{max}} = 9 \f$. If not set, it will be adjusted 
     * automatically.
     * @return Tail fitter object.
     */
    tail_fitter &get_tail_fitter(double r, int p_max = tail_fitter::default_n_tail_max, std::optional<int> q = {}) const {
      set_tail_fit_parameters(r, p_max, q);
      return *_tail_fitter;
    }

    private:
    mutable std::shared_ptr<tail_fitter> _tail_fitter;
  };

  /** @} */

} // namespace triqs::mesh
