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
#include "../meshes/product.hpp"
namespace triqs::gfs {

  using triqs::arrays::array_const_view;

  /*------------------------------------------------------------------------------------------------------
 *                 Fitting the tail
 *-----------------------------------------------------------------------------------------------------*/

  template <int N, template <typename, typename> typename G, typename T, typename... M> auto fit_tail(G<cartesian_product<M...>, T> const &g) {
    auto const &m = std::get<N>(g.mesh());
    return m.get_tail_fitter()(m, make_const_view(g.data()), N, true, array_const_view<dcomplex, G<cartesian_product<M...>, T>::data_rank>{});
  }

  template <int N, template <typename, typename> typename G, typename T, typename A, typename... M>
  auto fit_tail(G<cartesian_product<M...>, T> const &g, A const &known_moments) {
    auto const &m = std::get<N>(g.mesh());
    return m.get_tail_fitter()(m, make_const_view(g.data()), N, true, make_const_view(known_moments));
  }

  // FIXME : DOC
  template <template <typename, typename> typename G, typename V, typename T, typename A> auto fit_tail(G<V, T> const &g, A const &known_moments) {
    return g.mesh().get_tail_fitter()(g.mesh(), make_const_view(g.data()), 0, true, make_const_view(known_moments));
  }

  // FIXME : DOC
  template <template <typename, typename> typename G, typename V, typename T> auto fit_tail(G<V, T> const &g) {
    return g.mesh().get_tail_fitter()(g.mesh(), make_const_view(g.data()), 0, true, array_const_view<dcomplex, G<V, T>::data_rank>{});
  }

  // Full functionality
  template <template <typename, typename> typename G, typename V, typename T, typename A>
  auto fit_tail(G<V, T> const &g, A const &known_moments, double tail_fraction, int n_tail_max = 30, int expansion_order = -1) {
    return g.mesh().get_tail_fitter(tail_fraction, n_tail_max, expansion_order)(g.mesh(), make_const_view(g.data()), 0, true,
                                                                                make_const_view(known_moments));
  }

  // No normalization is a special need.
  template <template <typename, typename> typename G, typename V, typename T> auto fit_tail_no_normalize(G<V, T> const &g) {
    return g.mesh().get_tail_fitter()(g.mesh(), make_const_view(g.data()), 0, false, array_const_view<dcomplex, G<V, T>::data_rank>{});
  }

  // Create a tail object for a given Green function
  template <int N = 0, template <typename, typename> typename G, typename V, typename T> auto make_zero_tail(G<V, T> const &g, int n_moments = 10) {
    if constexpr (is_gf<G<V, T>>::value) // gf[_const][_view]<V, T>
    {
      auto sh = rotate_index_view(make_const_view(g.data()), N).shape();
      sh[0]   = n_moments;
      return arrays::zeros<dcomplex>(sh);
    } else // block[2]_gf[_const][_view]<V, T>
      return map_block_gf([&](auto const & g_bl){ return make_zero_tail<N>(g_bl, n_moments); }, g);
  }

  // FIXME : merge the slice_target_to_scalar
  /*------------------------------------------------------------------------------------------------------
 *                      Slicing the matrix_valued/matrix_real_valued into a matrix
 *-----------------------------------------------------------------------------------------------------*/

  template <typename G, typename... Args> auto slice_target(G &&g, Args &&... args) {
    return g.apply_on_data([&args...](auto &&d) { return d(triqs::arrays::ellipsis(), args...); },
                           [&args...](auto &&i) { return slice(i, args...); });
  }

  /*------------------------------------------------------------------------------------------------------
  *                      Slicing the matrix valued into a scalar
  *-----------------------------------------------------------------------------------------------------*/

  template <typename G, typename... Args> auto slice_target_to_scalar(G &&g, Args &&... args) {
    auto r =
       g.apply_on_data([&args...](auto &&d) { return d(triqs::arrays::ellipsis(), args...); }, [&args...](auto &&i) { return slice(i, args...); });
    return r;
  }

  /*------------------------------------------------------------------------------------------------------
  *                      Target reinterpretation
  *                      A scalar valued gf can be viewed as a 1x1 matrix
  *-----------------------------------------------------------------------------------------------------*/

  template <typename G, typename... Args> auto reinterpret_scalar_valued_gf_as_matrix_valued(G &&g) {
    static_assert(std::is_same<typename std::decay_t<G>::target_t, scalar_valued>::value,
                  "slice_target_to_scalar : the result is not a scalar valued function");
    return g.apply_on_data([](auto &&d) { return reinterpret_array_add_1x1(d); });
  }

  /*------------------------------------------------------------------------------------------------------
  *                      Inversion
  *-----------------------------------------------------------------------------------------------------*/

  // auxiliary function : invert the data : one function for all matrix valued gf (save code).
  template <typename A3> void _gf_invert_data_in_place(A3 &&a) {
    auto dom = a(ellipsis(), 0, 0).indexmap().domain();
    auto f   = [&a, _ = arrays::range()](auto... x) { return a(x..., _, _); };
    for (auto ind : dom) {
      auto v = make_matrix_view(triqs::tuple::apply(f, ind));
      v      = triqs::arrays::inverse(v);
    }
  }

  template <typename M> void invert_in_place(gf_view<M, matrix_valued> g) { _gf_invert_data_in_place(g.data()); }

  template <typename M> gf<M, matrix_valued> inverse(gf<M, matrix_valued> const &g) {
    auto res                    = g;
    gf_view<M, matrix_valued> v = res;
    invert_in_place(v);
    return res;
  }

  // FIXME  : unncessary copies

  template <typename M> gf<M, matrix_valued> inverse(gf_view<M, matrix_valued> g) { return inverse(gf<M, matrix_valued>(g)); }

  template <typename M> gf<M, matrix_valued> inverse(gf_const_view<M, matrix_valued> g) { return inverse(gf<M, matrix_valued>(g)); }

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
  template <typename G> std::enable_if_t<is_gf<G>::value, bool> is_gf_real(G const &g, double tolerance = 1.e-13) {
    return max_element(abs(imag(g.data()))) <= tolerance;
  }
  template <typename G> std::enable_if_t<is_block_gf_or_view<G>::value, bool> is_gf_real(G const &g, double tolerance = 1.e-13) {
    return std::all_of(g.begin(), g.end(), [&](auto &g) { return is_gf_real(g, tolerance); });
  }

  /// Takes the real part of g without check, and returns a new gf with a real target
  /// real(g).
  /**
   @tparam G any Gf, BlockGf or Block2Gf type
   @param g a gf
   */
  template <typename G> std::enable_if_t<is_gf<G>::value, typename G::regular_type::real_t> real(G const &g) {
    return {g.mesh(), real(g.data()), g.indices()};
  }
  template <typename G> std::enable_if_t<is_block_gf_or_view<G>::value, typename G::regular_type::real_t> real(G const &g) {
    return map_block_gf(real<typename G::g_t>, g);
  }

  /*------------------------------------------------------------------------------------------------------
  *                      Transpose. Create a NEW gf
  *-----------------------------------------------------------------------------------------------------*/

  template <typename M> gf<M, matrix_valued> transpose(gf_view<M, matrix_valued> g) {
    return {g.mesh(), transposed_view(g.data(), 0, 2, 1), g.indices().transpose()};
  }

  /*------------------------------------------------------------------------------------------------------
  *                      Conjugate
  *-----------------------------------------------------------------------------------------------------*/

  template <typename G> std::enable_if_t<is_gf<G>::value, typename G::regular_type> conj(G const &g) {
    using M = typename G::variable_t;
    return {g.mesh(), conj(g.data()), g.indices()};
  }

  /*------------------------------------------------------------------------------------------------------
  *                      Multiply by matrices left or right
  *-----------------------------------------------------------------------------------------------------*/

  // FIXME : speed issue ? direct gemm
  //
  template <typename A3, typename T> void _gf_data_mul_R(A3 &&a, matrix<T> const &r) {
    for (int i = 0; i < first_dim(a); ++i) { // Rely on the ordering
      matrix_view<T> v = a(i, arrays::range(), arrays::range());
      v                = v * r;
    }
  }

  template <typename A3, typename T> void _gf_data_mul_L(matrix<T> const &l, A3 &&a) {
    for (int i = 0; i < first_dim(a); ++i) { // Rely on the ordering
      matrix_view<T> v = a(i, arrays::range(), arrays::range());
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
    auto _   = arrays::range{};
    for (int i = 0; i < first_dim(a); ++i) { // Rely on the ordering
      auto rhs_v = make_matrix_view(b(i, _, _));
      auto lhs_v = make_matrix_view(a(i, _, _));
      arrays::blas::gemm(1, rhs_v, r, 0, tmp);
      arrays::blas::gemm(1, l, tmp, 0, lhs_v);
    }
  }

  template <typename G1, typename G2, typename M> void set_from_L_G_R(G1 &g1, M const &l, G2 const &g2, M const &r) {
    set_from_gf_data_mul_LR(g1.data(), l, g2.data(), r);
  }
} // namespace triqs::gfs
