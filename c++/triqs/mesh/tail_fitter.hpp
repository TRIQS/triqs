// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once
#include <nda/nda.hpp>
#include <itertools/itertools.hpp>
#include <triqs/arrays.hpp>
#include <nda/lapack/gelss_worker.hpp>

namespace triqs::mesh {

  class imfreq;

  using nda::array_const_view;

  //----------------------------------------------------------------------------------------------
  // construct the Vandermonde matrix
  inline nda::matrix<dcomplex> vander(std::vector<dcomplex> const &pts, int expansion_order) {
    nda::matrix<dcomplex> V(pts.size(), expansion_order + 1);
    for (auto [i, p] : itertools::enumerate(pts)) {
      dcomplex z = 1;
      for (int n = 0; n <= expansion_order; ++n) {
        V(i, n) = z;
        z *= p;
      }
    }
    return V;
  }
  //----------------------------------------------------------------------------------------------

  // Computes sum A_n / om^n
  // Return array<dcomplex, R -1 > if R>1 else dcomplex
  template <int R> auto tail_eval(nda::array_const_view<dcomplex, R> A, dcomplex om) {

    // same algo for both cases below
    auto compute = [&A, om](auto res) { // copy, in fact rvalue
      dcomplex z = 1;
      long N     = A.extent(0);
      for (int n = 0; n < N; ++n, z /= om) res += A(n, nda::ellipsis()) * z;
      return std::move(res);
    };

    if constexpr (R > 1) {
      return compute(nda::zeros<dcomplex>(stdutil::front_pop(A.indexmap().lengths())));
    } else {
      return compute(dcomplex{0});
    }
  }
  //----------------------------------------------------------------------------------------------

  class tail_fitter {

    static constexpr int max_order = 9;
    const double _tail_fraction;
    const int _n_tail_max;
    const bool _adjust_order;
    const int _expansion_order;
    const double _rcond = 1e-8;
    std::array<std::unique_ptr<const nda::lapack::gelss_worker<dcomplex>>, max_order + 1> _lss;
    std::array<std::unique_ptr<const nda::lapack::gelss_worker_hermitian>, max_order + 1> _lss_hermitian;
    nda::matrix<dcomplex> _vander;
    std::vector<long> _fit_idx_lst;

    public:
    tail_fitter(double tail_fraction, int n_tail_max, std::optional<int> expansion_order = {})
       : _tail_fraction(tail_fraction),
         _n_tail_max(n_tail_max),
         _adjust_order(not expansion_order.has_value()),
         _expansion_order(_adjust_order ? max_order : *expansion_order) {}
    //----------------------------------------------------------------------------------------------

    // number of the points in the tail for positive omega.
    template <typename M> int n_pts_in_tail(M const &m) const { return std::min(int(std::round(_tail_fraction * m.size() / 2)), _n_tail_max); }

    // The default fraction of frequency points to consider for the tail fit
    static constexpr double default_tail_fraction = 0.2;

    // The default upper limit for the number of frequencies to consider in the tail fit
    static constexpr int default_n_tail_max = 30;

    //----------------------------------------------------------------------------------------------

    // Return the tail_fraction
    double get_tail_fraction() const { return _tail_fraction; }

    //----------------------------------------------------------------------------------------------

    // Return the vector of all indices that are used fit the fitting procedure
    template <typename M> std::vector<long> get_tail_fit_indices(M const &m) {

      // Total number of points in the fitting window
      int n_pts_in_fit_range = int(std::round(_tail_fraction * m.size() / 2));

      // Number of points used for the fit
      int n_tail = n_pts_in_tail(m);

      std::vector<long> idx_vec;
      idx_vec.reserve(2 * n_tail);

      double step = double(n_pts_in_fit_range) / n_tail;
      double idx1 = m.first_index();
      double idx2 = m.last_index() - n_pts_in_fit_range;

      for (int n : range(n_tail)) {
        idx_vec.push_back(long(idx1));
        idx_vec.push_back(long(idx2));
        idx1 += step;
        idx2 += step;
      }

      return std::move(idx_vec);
    }

    //----------------------------------------------------------------------------------------------

    template <bool enforce_hermiticity = false> auto &get_lss() {
      if constexpr (enforce_hermiticity)
        return _lss_hermitian;
      else
        return _lss;
    }

    // Set up the least-squares solver for a given number of known moments.
    template <bool enforce_hermiticity = false, typename M> void setup_lss(M const &m, int n_fixed_moments) {

      using namespace nda::lapack;
      using cache_t = std::conditional_t<enforce_hermiticity, gelss_worker_hermitian, gelss_worker<dcomplex>>;

      // Calculate the indices to fit on
      if (_fit_idx_lst.empty()) _fit_idx_lst = get_tail_fit_indices(m);

      // Set Up full Vandermonde matrix up to order expansion_order if not set
      double om_max = std::abs(m.w_max());
      if (_vander.is_empty()) {
        std::vector<dcomplex> C;
        C.reserve(_fit_idx_lst.size());
        for (long n : _fit_idx_lst) C.push_back(om_max / m.to_value(n));
        _vander = vander(C, _expansion_order);
      }

      if (n_fixed_moments + 1 > _vander.extent(0) / 2) TRIQS_RUNTIME_ERROR << "Insufficient data points for least square procedure";

      auto l = [&](int n) { return std::make_unique<const cache_t>(_vander(range::all, range(n_fixed_moments, n + 1))); };

      auto &lss = get_lss<enforce_hermiticity>();

      if (!_adjust_order)
        lss[n_fixed_moments] = l(_expansion_order);
      else { // Use biggest submatrix of Vandermonde for fitting such that condition boundary fulfilled
        lss[n_fixed_moments].reset();
        // Ensure that |m.w_max()|^(1-N) > 10^{-16}
        long n_max = std::min<long>(size_t{max_order}, 1. + 16. / std::log10(1 + std::abs(m.w_max())));
        // We use at least two times as many data-points as we have moments to fit
        n_max = std::min(n_max, _vander.extent(0) / 2);
        for (int n = n_max; n >= n_fixed_moments; --n) {
          auto ptr = l(n);
          if (ptr->S_vec()[ptr->S_vec().size() - 1] > _rcond) {
            lss[n_fixed_moments] = std::move(ptr);
            break;
          }
        }
      }

      if (!lss[n_fixed_moments]) TRIQS_RUNTIME_ERROR << "Conditioning of tail-fit violates boundary";
    }

    //----------------------------------------------------------------------------------------------

    /**
     * @param m mesh
     * @param data
     * @param n position of the omega in the data array
     * @param normalize Finish the normalization of the tail coefficient (normally true)
     * @param known_moments  Array of the known_moments
     * */
    template <int N, bool enforce_hermiticity = false, typename M, int R, int R2 = R>
    std::pair<nda::array<dcomplex, R>, double> fit(M const &m, array_const_view<dcomplex, R> g_data, bool normalize,
                                                   array_const_view<dcomplex, R2> known_moments, std::optional<long> inner_matrix_dim = {}) {

      if (enforce_hermiticity and not inner_matrix_dim.has_value())
        TRIQS_RUNTIME_ERROR << "Enforcing the hermiticity in tail_fit requires inner matrix dimension";
      if constexpr (enforce_hermiticity) static_assert(std::is_same_v<M, imfreq>, "Enforcing the hermiticity in tail_fit requires Matsubara mesh");
      static_assert((R == R2), "The rank of the moment array is not equal to the data to fit !!!");
      if (m.positive_only()) TRIQS_RUNTIME_ERROR << "Can not fit on a positive_only mesh";

      // If not set, build least square solver for for given number of known moments
      int n_fixed_moments = known_moments.extent(0);
      if (n_fixed_moments > _expansion_order) return {known_moments, 0.0};

      auto &lss = get_lss<enforce_hermiticity>();
      if (!bool(lss[n_fixed_moments])) setup_lss<enforce_hermiticity>(m, n_fixed_moments);

      // Total number of moments
      int n_moments = lss[n_fixed_moments]->n_var() + n_fixed_moments;

      using itertools::enumerate;
      using nda::ellipsis;

      // The values of the Green function. Swap relevant mesh to front
      auto g_data_swap_idx = nda::rotate_index_view<N>(g_data);
      auto const &imp      = g_data_swap_idx.indexmap();
      long ncols           = imp.size() / imp.lengths()[0];

      // We flatten the data in the target space and remaining mesh into the second dim
      nda::matrix<dcomplex> g_mat(_vander.extent(0), ncols);

      // Copy g_data into new matrix (necessary because g_data might have fancy strides/lengths)
      for (auto [i, n] : enumerate(_fit_idx_lst)) {
        if constexpr (R == 1)
          g_mat(i, 0) = g_data_swap_idx(m.to_datidx(n));
        else
          for (auto [j, x] : enumerate(g_data_swap_idx(m.to_datidx(n), ellipsis()))) { g_mat(i, j) = x; }
      }

      // If an array with known_moments was passed, flatten the array into a matrix
      // just like g_data. Then account for the proper shift in g_mat
      if (n_fixed_moments > 0) {
        auto imp_km   = known_moments.indexmap();
        long ncols_km = imp_km.size() / imp_km.lengths()[0];

        if (ncols != ncols_km) TRIQS_RUNTIME_ERROR << "known_moments shape incompatible with shape of data";
        nda::matrix<dcomplex> km_mat(n_fixed_moments, ncols);

        // We have to scale the known_moments by 1/Omega_max^n
        double z      = 1.0;
        double om_max = std::abs(m.w_max());

        for (int order : range(n_fixed_moments)) {
          if constexpr (R == 1)
            km_mat(order, 0) = z * known_moments(order, ellipsis());
          else
            for (auto [n, x] : enumerate(known_moments(order, ellipsis()))) km_mat(order, n) = z * x;
          z /= om_max;
        }

        // Shift g_mat to account for known moment correction
        g_mat -= _vander(range::all, range(n_fixed_moments)) * km_mat;
      }
      // Call least square solver
      auto [a_mat, epsilon] = (*lss[n_fixed_moments])(g_mat, inner_matrix_dim); // coef + error

      // === The result a_mat contains the fitted moments divided by w_max()^n
      // Here we extract the real moments
      if (normalize) {
        double z      = 1.0;
        double om_max = std::abs(m.w_max());
        for (int i : range(n_fixed_moments)) z *= om_max;
        for (int i : range(a_mat.extent(0))) {
          a_mat(i, range::all) *= z;
          z *= om_max;
        }
      }
      // === Reinterpret the result as an R-dimensional array according to initial shape and return together with the error

      using r_t = nda::array<dcomplex, R>; // return type
      auto lg   = g_data_swap_idx.indexmap().lengths();

      // Index map for the view on the a_mat result
      lg[0]     = n_moments - n_fixed_moments;
      auto imp1 = typename r_t::layout_t{lg};
      //auto imp1 = typename r_t::indexmap_type{typename r_t::indexmap_type::domain_type{lg}};

      // Index map for the full result
      lg[0]    = n_moments;
      auto res = r_t(lg);

      if (n_fixed_moments) res(range(n_fixed_moments), ellipsis()) = known_moments;
      res(range(n_fixed_moments, n_moments), ellipsis()) = nda::array_view<dcomplex, R>{imp1, a_mat.storage()};
      //res(range(n_fixed_moments, n_moments), ellipsis()) = typename r_t::view_type{imp1, a_mat.storage()};

      return {std::move(res), epsilon};
    }

    //--------------------

    template <int N, typename M, int R, int R2 = R>
    std::pair<nda::array<dcomplex, R>, double> fit_hermitian(M const &m, array_const_view<dcomplex, R> g_data, bool normalize,
                                                             array_const_view<dcomplex, R2> known_moments,
                                                             std::optional<long> inner_matrix_dim = {}) {
      return fit<N, true, M, R, R2>(m, g_data, normalize, known_moments, inner_matrix_dim);
    }
  };

  //----------------------------------------------------------------------------------------------

  struct tail_fitter_handle {

    // Adjust the parameters for the tail-fitting
    void set_tail_fit_parameters(double tail_fraction, int n_tail_max = tail_fitter::default_n_tail_max,
                                 std::optional<int> expansion_order = {}) const {
      _tail_fitter = std::make_shared<tail_fitter>(tail_fitter{tail_fraction, n_tail_max, expansion_order});
    }

    // The tail fitter is mutable, even if the mesh is immutable to cache some data
    tail_fitter &get_tail_fitter() const {
      if (!_tail_fitter) _tail_fitter = std::make_shared<tail_fitter>(tail_fitter::default_tail_fraction, tail_fitter::default_n_tail_max);
      return *_tail_fitter;
    }

    // Adjust the parameters for the tail-fitting and return the fitter
    tail_fitter &get_tail_fitter(double tail_fraction, int n_tail_max = tail_fitter::default_n_tail_max,
                                 std::optional<int> expansion_order = {}) const {
      set_tail_fit_parameters(tail_fraction, n_tail_max, expansion_order);
      return *_tail_fitter;
    }

    private:
    mutable std::shared_ptr<tail_fitter> _tail_fitter;
  };

} // namespace triqs::mesh
