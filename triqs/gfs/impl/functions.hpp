/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2015 by O. Parcollet
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
namespace triqs {
namespace gfs {

 /*------------------------------------------------------------------------------------------------------
 *                      Slicing the matrix_valued/matrix_real_valued into a matrix
 *-----------------------------------------------------------------------------------------------------*/

 template <typename M, typename T, typename S, typename E, typename... Args>
 gf_view<M, T, S, E> slice_target(gf_view<M, T, S, E> g, Args &&... args) {
  return {g.mesh(), g.data()(arrays::range(), std::forward<Args>(args)...),
          slice_target(g.singularity(), std::forward<Args>(args)...), g.symmetry(),
          slice(g.indices(), std::forward<Args>(args)...), g.name};
 }

 template <typename M, typename T, typename S, typename E, typename... Args>
 gf_const_view<M, T, S, E> slice_target(gf_const_view<M, T, S, E> g, Args &&... args) {
  return {g.mesh(), g.data()(arrays::range(), std::forward<Args>(args)...),
          slice_target(g.singularity(), std::forward<Args>(args)...), g.symmetry(),
          slice(g.indices(), std::forward<Args>(args)...), g.name};
 }

 template <typename M, typename T, typename S, typename E, typename... Args>
 gf_view<M, T, S, E> slice_target(gf<M, T, S, E> &g, Args &&... args) {
  return slice_target(g(), std::forward<Args>(args)...);
 }

 template <typename M, typename T, typename S, typename E, typename... Args>
 gf_const_view<M, T, S, E> slice_target(gf<M, T, S, E> const &g, Args &&... args) {
  return slice_target(g(), std::forward<Args>(args)...);
 }

 /*------------------------------------------------------------------------------------------------------
  *                      Slicing the matrix valued into a scalar
  *-----------------------------------------------------------------------------------------------------*/

 template <typename M, typename S, typename E, typename... Args>
 gf_view<M, scalar_valued, S> slice_target_to_scalar(gf_view<M, matrix_valued, S, E> g, Args &&... args) {
  return {g.mesh(),
          g.data()(arrays::range(), std::forward<Args>(args)...),
          slice_target(g.singularity(), range(args, args + 1)...),
          g.symmetry(),
          {},
          g.name};
 }

 template <typename M, typename S, typename E, typename... Args>
 gf_const_view<M, scalar_valued, S> slice_target_to_scalar(gf_const_view<M, matrix_valued, S, E> g, Args &&... args) {
  return {g.mesh(),
          g.data()(arrays::range(), std::forward<Args>(args)...),
          slice_target(g.singularity(), range(args, args + 1)...),
          g.symmetry(),
          {},
          g.name};
 }

 template <typename M, typename S, typename E, typename... Args>
 gf_view<M, scalar_valued, S> slice_target_to_scalar(gf<M, matrix_valued, S, E> &g, Args &&... args) {
  return slice_target_to_scalar(g(), std::forward<Args>(args)...);
 }

 template <typename M, typename S, typename E, typename... Args>
 gf_const_view<M, scalar_valued, S> slice_target_to_scalar(gf<M, matrix_valued, S, E> const &g, Args &&... args) {
  return slice_target_to_scalar(g(), std::forward<Args>(args)...);
 }

 /*------------------------------------------------------------------------------------------------------
  *                      Target reinterpretation
  *                      A scalar valued gf can be viewed as a 1x1 matrix
  *-----------------------------------------------------------------------------------------------------*/

 template <typename M, typename S, typename E>
 gf_view<M, matrix_valued, S> reinterpret_scalar_valued_gf_as_matrix_valued(gf_view<M, scalar_valued, S, E> g) {
  using a_t = typename gf_view<M, matrix_valued, S>::data_view_t;
  auto &imap = g.data().indexmap();
  typename a_t::indexmap_type index_map(join(imap.lengths(), make_shape(1, 1)), join(imap.strides(), make_shape(1, 1)),
                                        imap.start_shift());
  auto a = a_t{index_map, g.data().storage()};
  return {g.mesh(), a, g.singularity(), g.symmetry(), {}, g.name};
 }

 template <typename M, typename S, typename E>
 gf_const_view<M, matrix_valued, S> reinterpret_scalar_valued_gf_as_matrix_valued(gf_const_view<M, scalar_valued, S, E> g) {
  using a_t = typename gf_view<M, matrix_valued, S>::data_view_t;
  auto &imap = g.data().indexmap();
  typename a_t::indexmap_type index_map(join(imap.lengths(), make_shape(1, 1)), join(imap.strides(), make_shape(1, 1)),
                                        imap.start_shift());
  auto a = a_t{index_map, g.data().storage()};
  return {g.mesh(), a, g.singularity(), g.symmetry(), {}, g.name};
 }

 template <typename M, typename S, typename E>
 gf_view<M, matrix_valued, S> reinterpret_scalar_valued_gf_as_matrix_valued(gf<M, scalar_valued, S, E> &g) {
  return reinterpret_scalar_valued_gf_as_matrix_valued(g());
 }

 template <typename M, typename S, typename E>
 gf_const_view<M, matrix_valued, S> reinterpret_scalar_valued_gf_as_matrix_valued(gf<M, scalar_valued, S, E> const &g) {
  return reinterpret_scalar_valued_gf_as_matrix_valued(g());
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

 template <typename M, typename S, typename E> void invert_in_place(gf_view<M, matrix_valued, S, E> g) {
  _gf_invert_data_in_place(g.data());
  g.singularity() = inverse(g.singularity());
 }

 template <typename M, typename S, typename E> gf<M, matrix_valued, S, E> inverse(gf<M, matrix_valued, S, E> const &g) {
  auto res = g;
  gf_view<M, matrix_valued, S, E> v = res;
  invert_in_place(v);
  return res;
 }

 template <typename M, typename S, typename E> gf<M, matrix_valued, S, E> inverse(gf_view<M, matrix_valued, S, E> g) {
  return inverse(gf<M, matrix_valued, S, E>(g));
 }

 template <typename M, typename S, typename E> gf<M, matrix_valued, S, E> inverse(gf_const_view<M, matrix_valued, S, E> g) {
  return inverse(gf<M, matrix_valued, S, E>(g));
 }

 /*------------------------------------------------------------------------------------------------------
  *                      Transpose. Create a NEW gf
  *-----------------------------------------------------------------------------------------------------*/

 template <typename M, typename S, typename E> gf<M, matrix_valued, S, E> transpose(gf_view<M, matrix_valued, S, E> g) {
  return {g.mesh(), transposed_view(g.data(), 0, 2, 1), transpose(g.singularity()), g.symmetry(), transpose(g.indices()), g.name};
 }

 /*------------------------------------------------------------------------------------------------------
  *                      Conjugate
  *-----------------------------------------------------------------------------------------------------*/

 struct imfreq; // forward
 struct imtime; // forward

 template <typename G> std14::enable_if_t<is_gf_or_view<G>::value, typename G::regular_type> conj(G const &g) {
  using M = typename G::variable_t;
  bool is_matsubara = std::is_same<M, imfreq>::value || std::is_same<M, imtime>::value;
  return {g.mesh(), conj(g.data()), conj(g.singularity(), is_matsubara), g.symmetry(), g.indices(), g.name};
 }

 /*------------------------------------------------------------------------------------------------------
  *                      Multiply by matrices left or right 
  *-----------------------------------------------------------------------------------------------------*/

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

 template <typename M, typename S, typename E, typename T>
 gf<M, matrix_valued, S, E> operator*(gf<M, matrix_valued, S, E> g, matrix<T> r) {
  _gf_data_mul_R(g.data(), r);
  g.singularity() = g.singularity() * r;
  return g;
 }

 template <typename M, typename S, typename E, typename T>
 gf<M, matrix_valued, S, E> operator*(matrix<T> l, gf<M, matrix_valued, S, E> g) {
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

}}
