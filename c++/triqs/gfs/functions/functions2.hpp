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

/**
 * @file
 * @brief Provides tail fitting, slicing, inversion, reality and matrix-multiplication functions for Green's functions.
 */

#pragma once

#include "../gf/gf.hpp"
#include "../gf/gf_view.hpp"
#include "../block/block_gf.hpp"
#include "../block/map.hpp"
#include "../../utility/exceptions.hpp"

#include <itertools/itertools.hpp>

#include <algorithm>
#include <optional>
#include <utility>
#include <type_traits>
#include <vector>

namespace triqs::gfs {

  /**
   * @ingroup triqs-gfs-reshape
   * @brief Make a const view of a Green's function.
   * 
   * @tparam Gf The type of the Green's function.
   * @param g The Green's function.
   * @return A triqs::gfs::gf_const_view of `g`.
   */
  template <typename Gf>
    requires(is_gf_v<Gf>)
  auto make_const_view(Gf const &g) {
    return gf_const_view{g};
  }

  // Name elevated from nda for the declarations below.
  using nda::array_const_view;

  /*------------------------------------------------------------------------------------------------------
   *                 Fitting the tail
   *-----------------------------------------------------------------------------------------------------*/

  /**
   * @ingroup triqs-gfs-tailfitting
   * @brief Fit the high-frequency tail of a Green's function using a least-squares procedure.
   *
   * @details The result is the set of expansion moments that best reproduces the high-frequency behavior of \f$ G \f$ 
   * on the configured tail-fit window. Known moments, when provided, are treated as exact constraints on the fit. 
   *
   * @tparam N Position of the frequency mesh in a product mesh (default \f$ 0 \f$).
   * @tparam G The type of the Green's function.
   * @tparam A The type of the array of known high-frequency moments.
   * @param g The Green's function whose tail is to be fitted.
   * @param known_moments Array of known high-frequency moments to constrain the fit.
   * @return A pair containing the fitted tail moments and the fitting error.
   */
  template <int N = 0, typename G, typename A = typename G::const_view_type::data_t>
  std::pair<typename A::regular_type, double> fit_tail(G const &g, A const &known_moments = {})
    requires(is_gf_v<G>)
  {
    auto const &m = get_mesh<N>(g);
    return m.get_tail_fitter().template fit<N>(m, make_array_const_view(g.data()), true, make_array_const_view(known_moments));
  }

  /**
   * @ingroup triqs-gfs-tailfitting
   * @brief Fit the high-frequency tail of a block Green's function using a least-squares procedure.
   *
   * @details Each block is fitted independently using ``fit_tail``. The returned error is the maximum across blocks.
   *
   * @tparam N Position of the frequency mesh in a product mesh (default \f$ 0 \f$).
   * @tparam BG The type of the block Green's function.
   * @tparam BA The type of the per-block known-moment array.
   * @param bg The block Green's function whose tail is to be fitted.
   * @param known_moments Per-block array of known high-frequency moments.
   * @return A pair containing the per-block fitted tail moments and the worst-block fitting error.
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
   * @ingroup triqs-gfs-tailfitting
   * @brief Fit the high-frequency tail of a Green's function, imposing hermitian symmetry on the fitted moments.
   *
   * @details The symmetry constraint is \f$ G_{i,j}(i\omega) = G_{j,i}^*(-i\omega) \f$.
   *
   * @tparam N Position of the frequency mesh in a product mesh (default \f$ 0 \f$).
   * @tparam G The type of the Green's function.
   * @tparam A The type of the array of known high-frequency moments.
   * @param g The Green's function whose tail is to be fitted.
   * @param known_moments Array of known high-frequency moments to constrain the fit.
   * @return A pair containing the fitted tail moments and the fitting error.
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
   * @ingroup triqs-gfs-tailfitting
   * @brief Fit the high-frequency tail of a block Green's function, imposing hermitian symmetry block by block.
   *
   * @details The symmetry constraint is \f$ G_{i,j}(i\omega) = G_{j,i}^*(-i\omega) \f$.
   * 
   * Each block is fitted independently with the same symmetry constraint. The returned error is the maximum across 
   * blocks.
   *
   * @tparam N Position of the frequency mesh in a product mesh (default \f$ 0 \f$).
   * @tparam BG The type of the block Green's function.
   * @tparam A The type of the per-block known-moment array.
   * @param bg The block Green's function whose tail is to be fitted.
   * @param known_moments Per-block array of known high-frequency moments.
   * @return A pair containing the per-block fitted tail moments and the worst-block fitting error.
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
   * @ingroup triqs-gfs-tailfitting
   * @brief Create a zero-initialized tail object for a given Green function object.
   *
   * @tparam N The mesh position of the frequency or time mesh [default: 0].
   * @tparam G The type of the Green function (gf, gf_view, block_gf, ...).
   * @param g The Green function object to create the tail object for.
   * @param n_moments The number of high-frequency moments to provide (including the zeroth moment).
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

  /**
   * @ingroup triqs-gfs-reshape
   * @fn auto slice_target(G &&g, Args &&...args)
   * @brief Slice the target of a Green's function, keeping the result matrix- (or tensor-) valued.
   *
   * @tparam G The type of the Green's function.
   * @tparam Args Types of the target slice arguments.
   * @param g The Green's function.
   * @param args Slice arguments applied to the target indices.
   * @return A view of `g` sliced over its target space.
   */
  template <typename G, typename... Args> auto slice_target(G &&g, Args &&...args) {
    return std::forward<G>(g).apply_on_data([&args...](auto &&d) { return d(nda::ellipsis(), std::forward<Args>(args)...); });
  }

  /*------------------------------------------------------------------------------------------------------
   *                      Slicing the matrix valued into a scalar
   *-----------------------------------------------------------------------------------------------------*/

  /**
   * @ingroup triqs-gfs-reshape
   * @fn auto slice_target_to_scalar(G &&g, Args &&...args)
   * @brief Slice the target of a matrix-valued Green's function down to a scalar-valued one.
   *
   * @tparam G The type of the Green's function.
   * @tparam Args Types of the target slice arguments.
   * @param g The Green's function.
   * @param args Slice arguments selecting a single target element.
   * @return A scalar-valued view of `g`.
   */
  template <typename G, typename... Args> auto slice_target_to_scalar(G &&g, Args &&...args) {
    auto r = std::forward<G>(g).apply_on_data([&args...](auto &&d) { return d(nda::ellipsis(), std::forward<Args>(args)...); });
    return r;
  }

  /*------------------------------------------------------------------------------------------------------
  *                      Target reinterpretation
  *                      A scalar valued gf can be viewed as a 1x1 matrix
  *-----------------------------------------------------------------------------------------------------*/

  /**
   * @ingroup triqs-gfs-reshape
   * @brief Reinterpret a scalar-valued Green's function as a 1x1 matrix-valued one.
   *
   * @tparam G The type of the (scalar-valued) Green's function.
   * @tparam Args Unused.
   * @param g The scalar-valued Green's function.
   * @return A 1x1 matrix-valued view of `g`.
   */
  template <typename G, typename... Args> auto reinterpret_scalar_valued_gf_as_matrix_valued(G &&g) {
    static_assert(std::is_same_v<typename std::decay_t<G>::target_t, scalar_valued>,
                  "slice_target_to_scalar : the result is not a scalar valued function");
    return std::forward<G>(g).apply_on_data([](auto &&d) { return nda::reinterpret_add_fast_dims_of_size_one<2>(d); });
  }

  /*------------------------------------------------------------------------------------------------------
  *                      Inversion
  *-----------------------------------------------------------------------------------------------------*/

  /**
   * @ingroup triqs-gfs-algebra
   * @brief Invert, in place, the target matrix at each mesh point of a matrix-valued Green's function.
   * 
   * @tparam M Mesh type.
   * @param g The matrix-valued Green's function view, inverted in place.
   */
  template <typename M> void invert_in_place(gf_view<M, matrix_valued> g) {
    auto &a           = g.data();
    auto mesh_lengths = stdutil::mpop<2>(a.indexmap().lengths());
    nda::for_each(mesh_lengths, [&a](auto &&...i) { nda::linalg::inv_in_place(make_matrix_view(a(i..., range::all, range::all))); });
  }

  /**
   * @ingroup triqs-gfs-algebra
   * @brief Return the matrix inverse of a matrix-valued Green's function (inverts the target matrix at each mesh point).
   * 
   * @tparam M Mesh type.
   * @param g The matrix-valued Green's function.
   * @return A new Green's function holding the per-mesh-point matrix inverse.
   */
  template <typename M> gf<M, matrix_valued> inverse(gf<M, matrix_valued> g) {
    invert_in_place(g());
    return g;
  }

  /// @ingroup triqs-gfs-algebra
  /// View overload of triqs::gfs::inverse (makes a regular copy first).
  template <typename M> gf<M, matrix_valued> inverse(gf_view<M, matrix_valued> g) { return inverse(gf{g}); }

  /// @ingroup triqs-gfs-algebra
  /// Const-view overload of triqs::gfs::inverse (makes a regular copy first).
  template <typename M> gf<M, matrix_valued> inverse(gf_const_view<M, matrix_valued> g) { return inverse(gf{g}); }

  /*------------------------------------------------------------------------------------------------------
  *                     is_gf_real : true iif the gf is real
  *-----------------------------------------------------------------------------------------------------*/

  /**
   * @ingroup triqs-gfs-reality
   * @brief Test whether a Green's function is real up to a tolerance.
   * 
   * @tparam G Any Green's function type.
   * @param g The Green's function.
   * @param tolerance Tolerance threshold.
   * @return True if `g` is real up to `tolerance`.
   */
  template <typename G>
  bool is_gf_real(G const &g, double tolerance = 1.e-13)
    requires(is_gf_v<G>)
  {
    return max_element(abs(imag(g.data()))) <= tolerance;
  }

  /// @ingroup triqs-gfs-reality
  /// Block overload of triqs::gfs::is_gf_real (true iff every block is real up to tolerance).
  template <typename G>
  bool is_gf_real(G const &bg, double tolerance = 1.e-13)
    requires(is_block_gf_v<G>)
  {
    return std::all_of(bg.begin(), bg.end(), [&](auto &g) { return is_gf_real(g, tolerance); });
  }

  /**
   * @ingroup triqs-gfs-reality
   * @brief Take the real part of a Green's function (no check), returning a new Green's function with a real target.
   * 
   * @tparam G Any Gf, BlockGf or Block2Gf type.
   * @param g The Green's function.
   * @return A Green's function holding the real part of `g`.
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
   * @ingroup triqs-gfs-reality
   * @brief Take the imaginary part of a Green's function (no check), returning a new Green's function with a real 
   * target.
   * 
   * @tparam G Any Gf, BlockGf or Block2Gf type.
   * @param g The Green's function.
   * @return A Green's function holding the imaginary part of `g`.
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

  /**
   * @ingroup triqs-gfs-reshape
   * @brief Transpose the target matrix of a matrix-valued Green's function, returning a new Green's function.
   * 
   * @tparam M Mesh type.
   * @param g The matrix-valued Green's function.
   * @return A new Green's function whose target matrix is transposed at every mesh point.
   */
  template <typename M> gf<M, matrix_valued> transpose(gf_view<M, matrix_valued> g) { return {g.mesh(), transposed_view(g.data(), 0, 2, 1)}; }

  /*------------------------------------------------------------------------------------------------------
  *                      Conjugate
  *-----------------------------------------------------------------------------------------------------*/

  /**
   * @ingroup triqs-gfs-reality
   * @brief Complex-conjugate a Green's function, returning a new Green's function.
   * 
   * @tparam G The type of the Green's function.
   * @param g The Green's function.
   * @return A new Green's function holding the complex conjugate of `g`.
   */
  template <typename G>
  typename G::regular_type conj(G const &g)
    requires(is_gf_v<G>)
  {
    return {g.mesh(), conj(g.data())};
  }

  /*------------------------------------------------------------------------------------------------------
  *                      Multiply by matrices left or right
  *-----------------------------------------------------------------------------------------------------*/

  // Right-multiply the target matrix at each mesh point of the data array by r.
  // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): a is indexed per mesh point in the loop, not forwarded
  template <typename A3, typename T> void _gf_data_mul_R(A3 &&a, matrix<T> const &r) {
    for (int i = 0; i < first_dim(a); ++i) { // Rely on the ordering
      matrix_view<T> v = a(i, nda::range::all, nda::range::all);
      v                = v * r;
    }
  }

  // Left-multiply the target matrix at each mesh point of the data array by l.
  // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): a is indexed per mesh point in the loop, not forwarded
  template <typename A3, typename T> void _gf_data_mul_L(matrix<T> const &l, A3 &&a) {
    for (int i = 0; i < first_dim(a); ++i) { // Rely on the ordering
      matrix_view<T> v = a(i, nda::range::all, nda::range::all);
      v                = l * v;
    }
  }

  /**
   * @ingroup triqs-gfs-algebra
   * @brief Right-multiply a matrix-valued Green's function by a matrix at every mesh point.
   * 
   * @tparam M Mesh type. 
   * @tparam T Scalar type of the matrix.
   * @param g The matrix-valued Green's function.
   * @param r The matrix multiplied from the right.
   * @return The resulting Green's function.
   */
  template <typename M, typename T> gf<M, matrix_valued> operator*(gf<M, matrix_valued> g, matrix<T> r) {
    _gf_data_mul_R(g.data(), r);
    return g;
  }

  /**
   * @ingroup triqs-gfs-algebra
   * @brief Left-multiply a matrix-valued Green's function by a matrix at every mesh point.
   * 
   * @tparam M Mesh type. 
   * @tparam T Scalar type of the matrix.
   * @param l The matrix multiplied from the left.
   * @param g The matrix-valued Green's function.
   * @return The resulting Green's function.
   */
  template <typename M, typename T> gf<M, matrix_valued> operator*(matrix<T> l, gf<M, matrix_valued> g) {
    _gf_data_mul_L(l, g.data());
    return g;
  }

  /*------------------------------------------------------------------------------------------------------
  *                      Multiply by matrices left and right, in place.
  *                      Optimized for speed.
  *-----------------------------------------------------------------------------------------------------*/

  // Set a = l * b * r for the target matrix at each mesh point of the data arrays (optimized via gemm).
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

  /**
   * @ingroup triqs-gfs-algebra
   * @brief Set `g1 = l * g2 * r` at every mesh point (in place, optimized for speed).
   * 
   * @tparam G1 Type of the output Green's function. 
   * @tparam G2 Type of the input Green's function. 
   * @tparam M Matrix type.
   * @param g1 The output Green's function (overwritten).
   * @param l The left matrix factor.
   * @param g2 The input Green's function.
   * @param r The right matrix factor.
   */
  template <typename G1, typename G2, typename M> void set_from_L_G_R(G1 &g1, M const &l, G2 const &g2, M const &r) {
    set_from_gf_data_mul_LR(g1.data(), l, g2.data(), r);
  }
} // namespace triqs::gfs
