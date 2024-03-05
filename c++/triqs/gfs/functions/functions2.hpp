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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once
#include <itertools/itertools.hpp>

namespace triqs::gfs {

  template <typename Gf>
    requires(is_gf_v<Gf>)
  auto make_const_view(Gf const &g) {
    return gf_const_view{g};
  }

  using nda::array_const_view;

  /*------------------------------------------------------------------------------------------------------
   *                 Fitting the tail
   *-----------------------------------------------------------------------------------------------------*/

  /**
   * Fit the tail of a Green function using a least-squares fitting procedure
   *
   * @tparam N The position of the frequency mesh in case of a product mesh [default: 0]
   * @tparam G The type of the Green function (gf, gf_view, gf_const_view)
   * @tparam A The type of the high-frequency moment array (array, array_view, array_const_view)
   *
   * @param g The Green function object to fit the tail for 
   * @param known_moments The object containing the known high-frequency moments
   *
   * @return A pair of the tail object and the fitting error
   * @example triqs/gfs/fit_tail.cpp
   */
  template <int N = 0, typename G, typename A = typename G::const_view_type::data_t>
  std::pair<typename A::regular_type, double> fit_tail(G const &g, A const &known_moments = {})
    requires(is_gf_v<G>)
  {
    auto const &m = get_mesh<N>(g);
    return m.get_tail_fitter().template fit<N>(m, make_array_const_view(g.data()), true, make_array_const_view(known_moments));
  }

  /**
   * Fit the tail of a Block Green function using a least-squares fitting procedure
   *
   * @tparam BG The type of the Block Green function (block_gf, block_gf_view, block_gf_const_view)
   * @tparam AG The type of the high-frequecy moments for Block Green functions (e.g. std::vector<array>)
   *
   * @param bg The Block Green function object to fit the tail for 
   */
  template <int N = 0, typename BG, typename BA = std::vector<typename BG::g_t::data_t::regular_type>>
  std::pair<std::vector<typename BG::g_t::data_t::regular_type>, double> fit_tail(BG const &bg, BA const &known_moments = {})
    requires(is_block_gf_v<BG, 1>)
  {
    double max_err = 0.0;
    std::vector<typename BG::g_t::data_t::regular_type> tail_vec;
    for (auto [i, g_bl] : itertools::enumerate(bg)) {
      auto [tail, err] = known_moments.empty() ? fit_tail<N, typename BG::g_t>(g_bl) : fit_tail<N, typename BG::g_t>(g_bl, known_moments[i]);
      tail_vec.emplace_back(std::move(tail));
      max_err = std::max(err, max_err);
    }
    return std::make_pair(tail_vec, max_err);
  }

  /**
   * Fit the tail of a Green function using a least-squares fitting procedure
   * imposing the symmetry :math:`G[i\omega](i,j) = G[-i\omega](j,i)^*`
   *
   * @param g The Green function object to fit the tail for 
   * @param known_moments The object containing the known high-frequency moments
   *
   * @tparam N The position of the frequency mesh in case of a product mesh [default: 0]
   * @tparam G The type of the Green function object
   * @tparam A The type of the high-frequency moments
   *
   * @return A pair of the tail object and the fitting error
   * @example triqs/gfs/fit_hermitian_tail.cpp
   */
  template <int N = 0, typename G, typename A = typename G::data_t>
  std::pair<typename A::regular_type, double> fit_hermitian_tail(G const &g, A const &known_moments = {})
    requires(is_gf_v<G>)
  {
    std::optional<long> inner_matrix_dim;
    constexpr int rank = G::target_t::rank;
    if (rank == 0)
      inner_matrix_dim = 1;
    else if (rank == 2 && g.target_shape()[0] == g.target_shape()[1]) {
      inner_matrix_dim = g.target_shape()[0];
    } else
      TRIQS_RUNTIME_ERROR << "Incompatible target_shape for fit_hermitian_tail\n";

    auto const &m = get_mesh<N>(g);
    return m.get_tail_fitter().template fit_hermitian<N>(m, make_array_const_view(g.data()), true, make_array_const_view(known_moments),
                                                         inner_matrix_dim);
  }

  /**
   * Fit the tail of a Block Green function using a least-squares fitting procedure
   * imposing the symmetry :math:`G[i\omega](i,j) = G[-i\omega](j,i)^*` for each block
   *
   * @tparam BG The type of the Block Green function (block_gf, block_gf_view, block_gf_const_view)
   * @tparam AG The type of the high-frequecy moments for Block Green functions (e.g. std::vector<array>)
   *
   * @param bg The Block Green function object to fit the tail for 
   */
  template <int N = 0, typename BG, typename A = std::vector<typename BG::g_t::data_t::regular_type>>
  std::pair<std::vector<typename BG::g_t::data_t::regular_type>, double> fit_hermitian_tail(BG const &bg, A const &known_moments = {})
    requires(is_block_gf_v<BG, 1>)
  {
    double max_err = 0.0;
    std::vector<typename BG::g_t::data_t::regular_type> tail_vec;
    for (auto [i, g_bl] : itertools::enumerate(bg)) {
      auto [tail, err] =
         known_moments.empty() ? fit_hermitian_tail<N, typename BG::g_t>(g_bl) : fit_hermitian_tail<N, typename BG::g_t>(g_bl, known_moments[i]);
      tail_vec.emplace_back(std::move(tail));
      max_err = std::max(err, max_err);
    }
    return std::make_pair(tail_vec, max_err);
  }

  // Tail-fit without normalization, returns moments rescaled by maximum frequency:  a_n * omega_max^n
  template <template <typename, typename, typename...> typename G, typename V, typename T, typename... U>
  auto fit_tail_no_normalize(G<V, T, U...> const &g) {
    return g.mesh().get_tail_fitter().template fit<0>(g.mesh(), make_array_const_view(g.data()), false,
                                                      array_const_view<dcomplex, G<V, T, U...>::data_rank>{});
  }

  /**
   * Create a zero-initialized tail object for a given Green function object
   *
   * @param g The Green function object to create the tail object for
   * @param n_moments The number of high-frequency moments to provide (including the zeroth moment)
   *
   * @tparam N The mesh position of the frequency or time mesh [default: 0]
   * @tparam G The type of the Green function (gf, gf_view, block_gf, ...)
   *
   * @example triqs/gfs/make_zero_tail.cpp
   */
  template <int N = 0, typename G> auto make_zero_tail(G const &g, int n_moments = 10) {
    if constexpr (is_gf_v<G>) { // gf[_const][_view]<V, T>
      auto sh = nda::rotate_index_view<N>(make_const_view(g.data())).shape();
      sh[0]   = n_moments;
      return nda::zeros<dcomplex>(sh);
    } else if constexpr (is_block_gf_v<G>) { // block[2]_gf[_const][_view]<V, T>
      return map_block_gf([&](auto const &g_bl) { return make_zero_tail<N>(g_bl, n_moments); }, g);
    }
  }

  /*------------------------------------------------------------------------------------------------------
   *                      Slicing the matrix_valued/matrix_real_valued into a matrix
   *-----------------------------------------------------------------------------------------------------*/

  template <typename G, typename... Args> auto slice_target(G &&g, Args &&...args) {
    return g.apply_on_data([&args...](auto &&d) { return d(nda::ellipsis(), args...); });
  }

  /*------------------------------------------------------------------------------------------------------
   *                      Slicing the matrix valued into a scalar
   *-----------------------------------------------------------------------------------------------------*/

  template <typename G, typename... Args> auto slice_target_to_scalar(G &&g, Args &&...args) {
    auto r = g.apply_on_data([&args...](auto &&d) { return d(nda::ellipsis(), args...); });
    return r;
  }

  /*------------------------------------------------------------------------------------------------------
  *                      Target reinterpretation
  *                      A scalar valued gf can be viewed as a 1x1 matrix
  *-----------------------------------------------------------------------------------------------------*/

  template <typename G, typename... Args> auto reinterpret_scalar_valued_gf_as_matrix_valued(G &&g) {
    static_assert(std::is_same_v<typename std::decay_t<G>::target_t, scalar_valued>,
                  "slice_target_to_scalar : the result is not a scalar valued function");
    return g.apply_on_data([](auto &&d) { return nda::reinterpret_add_fast_dims_of_size_one<2>(d); });
  }

  /*------------------------------------------------------------------------------------------------------
  *                      Inversion
  *-----------------------------------------------------------------------------------------------------*/

  // auxiliary function : invert the data : one function for all matrix valued gf (save code).
  template <typename M> void invert_in_place(gf_view<M, matrix_valued> g) {
    auto &a           = g.data();
    auto mesh_lengths = stdutil::mpop<2>(a.indexmap().lengths());
    nda::for_each(mesh_lengths, [&a, _ = nda::range::all](auto &&...i) { nda::inverse_in_place(make_matrix_view(a(i..., _, _))); });
  }

  template <typename M> gf<M, matrix_valued> inverse(gf<M, matrix_valued> g) {
    invert_in_place(g());
    return g;
  }

  template <typename M> gf<M, matrix_valued> inverse(gf_view<M, matrix_valued> g) { return inverse(gf{g}); }
  template <typename M> gf<M, matrix_valued> inverse(gf_const_view<M, matrix_valued> g) { return inverse(gf{g}); }

  /*------------------------------------------------------------------------------------------------------
  *                     is_gf_real : true iif the gf is real
  *-----------------------------------------------------------------------------------------------------*/

  /// is_gf_real(g, tolerance).
  /**
   @tparam G any Gf type
   @param g a gf
   @param tolerance tolerance threshold
   @return true iif the function g is real up to tolerance
   */
  template <typename G>
  bool is_gf_real(G const &g, double tolerance = 1.e-13)
    requires(is_gf_v<G>)
  {
    return max_element(abs(imag(g.data()))) <= tolerance;
  }
  template <typename G>
  bool is_gf_real(G const &bg, double tolerance = 1.e-13)
    requires(is_block_gf_v<G>)
  {
    return std::all_of(bg.begin(), bg.end(), [&](auto &g) { return is_gf_real(g, tolerance); });
  }

  /// Takes the real part of g without check, and returns a new gf with a real target
  /// real(g).
  /**
   @tparam G any Gf, BlockGf or Block2Gf type
   @param g a gf
   */
  template <typename G>
  typename G::regular_type::real_t real(G const &g)
    requires(is_gf_v<G> or is_block_gf_v<G>)
  {
    if constexpr (is_gf_v<G>)
      return {g.mesh(), real(g.data())};
    else
      return map_block_gf([](auto &&g_bl) { return real(g_bl); }, g);
  }

  /**
   * Takes the imag part of g without check, and returns a new gf with a real target.
   *
   * @tparam G any Gf, BlockGf or Block2Gf type
   * @param g a gf
   */
  template <typename G>
  typename G::regular_type::real_t imag(G const &g)
    requires(is_gf_v<G> or is_block_gf_v<G>)
  {
    if constexpr (is_gf_v<G>)
      return {g.mesh(), imag(g.data())};
    else
      return map_block_gf([](auto &&g_bl) { return imag(g_bl); }, g);
  }

  /*------------------------------------------------------------------------------------------------------
  *                      Transpose. Create a NEW gf
  *-----------------------------------------------------------------------------------------------------*/

  template <typename M> gf<M, matrix_valued> transpose(gf_view<M, matrix_valued> g) { return {g.mesh(), transposed_view(g.data(), 0, 2, 1)}; }

  /*------------------------------------------------------------------------------------------------------
  *                      Conjugate
  *-----------------------------------------------------------------------------------------------------*/

  template <typename G>
  typename G::regular_type conj(G const &g)
    requires(is_gf_v<G>)
  {
    return {g.mesh(), conj(g.data())};
  }

  /*------------------------------------------------------------------------------------------------------
  *                      Multiply by matrices left or right
  *-----------------------------------------------------------------------------------------------------*/

  template <typename A3, typename T> void _gf_data_mul_R(A3 &&a, matrix<T> const &r) {
    for (int i = 0; i < first_dim(a); ++i) { // Rely on the ordering
      matrix_view<T> v = a(i, nda::range::all, nda::range::all);
      v                = v * r;
    }
  }

  template <typename A3, typename T> void _gf_data_mul_L(matrix<T> const &l, A3 &&a) {
    for (int i = 0; i < first_dim(a); ++i) { // Rely on the ordering
      matrix_view<T> v = a(i, nda::range::all, nda::range::all);
      v                = l * v;
    }
  }

  template <typename M, typename T> gf<M, matrix_valued> operator*(gf<M, matrix_valued> g, matrix<T> r) {
    _gf_data_mul_R(g.data(), r);
    return g;
  }

  template <typename M, typename T> gf<M, matrix_valued> operator*(matrix<T> l, gf<M, matrix_valued> g) {
    _gf_data_mul_L(l, g.data());
    return g;
  }

  /*------------------------------------------------------------------------------------------------------
  *                      Multiply by matrices left and right, in place.
  *                      Optimized for speed.
  *-----------------------------------------------------------------------------------------------------*/

  template <typename A, typename B, typename M> void set_from_gf_data_mul_LR(A &a, M const &l, B const &b, M const &r) {
    auto tmp = matrix<typename M::value_type>(second_dim(b), second_dim(r));
    auto _   = nda::range::all;
    for (int i = 0; i < first_dim(a); ++i) { // Rely on the ordering
      auto rhs_v = make_matrix_view(b(i, _, _));
      auto lhs_v = make_matrix_view(a(i, _, _));
      nda::blas::gemm(1, rhs_v, r, 0, tmp);
      nda::blas::gemm(1, l, tmp, 0, lhs_v);
    }
  }

  template <typename G1, typename G2, typename M> void set_from_L_G_R(G1 &g1, M const &l, G2 const &g2, M const &r) {
    set_from_gf_data_mul_LR(g1.data(), l, g2.data(), r);
  }
} // namespace triqs::gfs
