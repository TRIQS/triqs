/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2016 by O. Parcollet
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
#include "../multivar/product.hpp"
namespace triqs {
namespace gfs {

 using triqs::arrays::array_const_view;

 /*------------------------------------------------------------------------------------------------------
 *                      Slicing the matrix_valued/matrix_real_valued into a matrix
 *-----------------------------------------------------------------------------------------------------*/

 template <typename G, typename... Args> auto slice_target(G &&g, Args &&... args) {
  return g.apply_on_data([&args...](auto &&d) { return d(arrays::ellipsis(), args...); },
                         [&args...](auto &&s) { return slice_target_sing(s(), args...); },
                         [&args...](auto &&i) { return slice(i, args...); });
 }

 // FIXME ADD apply_on _dato to TAIL !
 /*
 template <typename G, typename... Args> auto slice_target_sing(G &&g, Args &&... args) {
  return g.apply_on_data([&args...](auto &&d) { return d(arrays::ellipsis(), args...); }, nothing{});
 }
*/

 /*------------------------------------------------------------------------------------------------------
  *                      Slicing the matrix valued into a scalar
  *-----------------------------------------------------------------------------------------------------*/


 template <typename G, typename... Args> auto slice_target_to_scalar(G &&g, Args &&... args) {
  auto r = g.apply_on_data([&args...](auto &&d) { return d(arrays::ellipsis(), args...); },
                           [&args...](auto &&s) { return slice_target_to_scalar_sing(s(), args...); },
                           [&args...](auto &&i) { return slice(i, args...); });
  // The target of the return is scalar_valued or tail_valued<scalar_valued>
  return r;
 }

 /*------------------------------------------------------------------------------------------------------
  *                      Target reinterpretation
  *                      A scalar valued gf can be viewed as a 1x1 matrix
  *-----------------------------------------------------------------------------------------------------*/

 template <typename G, typename... Args> auto reinterpret_scalar_valued_gf_as_matrix_valued(G &&g) {
  static_assert(std::is_same<typename std14::decay_t<G>::target_t, scalar_valued>::value,
                "slice_target_to_scalar : the result is not a scalar valued function");
  return g.apply_on_data([](auto &&d) { return reinterpret_array_add_1x1(d); },
                         [](auto &&s) { return reinterpret_as_matrix_valued_sing(s()); });
 }

 /*------------------------------------------------------------------------------------------------------
  *                      Inversion
  *-----------------------------------------------------------------------------------------------------*/

 // auxiliary function : invert the data : one function for all matrix valued gf (save code).
 template <typename A3> void _gf_invert_data_in_place(A3 &&a) {
  for (int i = 0; i < first_dim(a); ++i) { // Rely on the ordering
   auto v = make_matrix_view(a(i, arrays::range(), arrays::range()));
   v = triqs::arrays::inverse(v);
  }
 }

 template <typename M> void invert_in_place(gf_view<M, matrix_valued> g) {
  _gf_invert_data_in_place(g.data());
  g.singularity() = inverse(g.singularity());
 }

 template <typename M> gf<M, matrix_valued> inverse(gf<M, matrix_valued> const &g) {
  auto res = g;
  gf_view<M, matrix_valued> v = res;
  invert_in_place(v);
  return res;
 }

 // FIXME  : unncessary copies

 template <typename M> gf<M, matrix_valued> inverse(gf_view<M, matrix_valued> g) { return inverse(gf<M, matrix_valued>(g)); }

 template <typename M> gf<M, matrix_valued> inverse(gf_const_view<M, matrix_valued> g) {
  return inverse(gf<M, matrix_valued>(g));
 }

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
 template <typename G> bool is_gf_real(G const &g, double tolerance = 1.e-13) {
  return max_element(abs(imag(g.data()))) <= tolerance;
 }

 /// Takes the real part of g without check, and returns a new gf with a real target
 template <typename M, typename T> gf<M, typename T::real_t> real(gf_const_view<M, T> g) {
  return {g.mesh(), real(g.data()), g.singularity(), {}}; // no indices for real_valued, internal C++ use only
 }
 template <typename M, typename T> gf<M, typename T::real_t> real(gf_view<M, T> g) {
  return {g.mesh(), real(g.data()), g.singularity(), {}};
 }
 template <typename M, typename T> gf<M, typename T::real_t> real(gf<M, T> const &g) {
  return {g.mesh(), real(g.data()), g.singularity(), {}};
 }

 /*------------------------------------------------------------------------------------------------------
  *                      Transpose. Create a NEW gf
  *-----------------------------------------------------------------------------------------------------*/

 template <typename M> gf<M, matrix_valued> transpose(gf_view<M, matrix_valued> g) {
  return {g.mesh(), transposed_view(g.data(), 0, 2, 1), transpose(g.singularity()), g.indices().transpose()};
 }

 /*------------------------------------------------------------------------------------------------------
  *                      Conjugate
  *-----------------------------------------------------------------------------------------------------*/

 template <typename G> std14::enable_if_t<is_gf<G>::value, typename G::regular_type> conj(G const &g) {
  using M = typename G::variable_t;
  bool is_matsubara = std::is_same<M, imfreq>::value || std::is_same<M, imtime>::value;
  return {g.mesh(), conj(g.data()), conj(g.singularity(), is_matsubara), g.indices()};
 }

 /*------------------------------------------------------------------------------------------------------
  *                      Multiply by matrices left or right
  *-----------------------------------------------------------------------------------------------------*/

 // FIXME : speed issue ? direct gemm
 //
 template <typename A3, typename T> void _gf_data_mul_R(A3 &&a, matrix<T> const &r) {
  for (int i = 0; i < first_dim(a); ++i) { // Rely on the ordering
   matrix_view<T> v = a(i, arrays::range(), arrays::range());
   v = v * r;
  }
 }

 template <typename A3, typename T> void _gf_data_mul_L(matrix<T> const &l, A3 &&a) {
  for (int i = 0; i < first_dim(a); ++i) { // Rely on the ordering
   matrix_view<T> v = a(i, arrays::range(), arrays::range());
   v = l * v;
  }
 }

 template <typename M, typename T> gf<M, matrix_valued> operator*(gf<M, matrix_valued> g, matrix<T> r) {
  _gf_data_mul_R(g.data(), r);
  g.singularity() = g.singularity() * r;
  return g;
 }

 template <typename M, typename T> gf<M, matrix_valued> operator*(matrix<T> l, gf<M, matrix_valued> g) {
  _gf_data_mul_L(l, g.data());
  g.singularity() = l * g.singularity();
  return g;
 }

 /*------------------------------------------------------------------------------------------------------
  *                      Multiply by matrices left and right, in place.
  *                      Optimized for speed.
  *-----------------------------------------------------------------------------------------------------*/

 template <typename A, typename B, typename M> void set_from_gf_data_mul_LR(A &a, M const &l, B const &b, M const &r) {
  auto tmp = matrix<typename M::value_type>(second_dim(b), second_dim(r));
  auto _ = arrays::range{};
  for (int i = 0; i < first_dim(a); ++i) { // Rely on the ordering
   auto rhs_v = make_matrix_view(b(i, _, _));
   auto lhs_v = make_matrix_view(a(i, _, _));
   arrays::blas::gemm(1, rhs_v, r, 0, tmp);
   arrays::blas::gemm(1, l, tmp, 0, lhs_v);
  }
 }

 template <typename G1, typename G2, typename M> void set_from_L_G_R(G1 &g1, M const &l, G2 const &g2, M const &r) {
  set_from_gf_data_mul_LR(g1.data(), l, g2.data(), r);
  g1.singularity() = l * g2.singularity() * r;
 }
}
}
