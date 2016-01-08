/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2013 by O. Parcollet
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
#include <triqs/arrays.hpp>
#include "./gf_classes.hpp"
#include "./meshes/matsubara_freq.hpp"
namespace triqs {
namespace gfs {

 // singularity
 template <> struct gf_default_singularity<imfreq, matrix_valued> {
  using type = tail;
 };
 template <> struct gf_default_singularity<imfreq, scalar_valued> {
  using type = tail;
 };

 /// ---------------------------  A few specific functions ---------------------------------

 // This function takes a g(i omega_n) on half mesh (positive omega_n) and returns a gf on the whole mesh
 // using G(-i omega_n) = G(i omega_n)^* for real G(tau) functions.
 template <typename T, typename S, typename E> gf<imfreq, T, S, E> make_gf_from_real_gf(gf_const_view<imfreq, T, S, E> g) {
  if (!g.mesh().positive_only()) TRIQS_RUNTIME_ERROR << "gf imfreq is not for omega_n >0, real_to_complex does not apply";
  auto const &dat = g.data();
  auto sh = dat.shape();
  int is_boson = (g.mesh().domain().statistic == Boson);
  long L = sh[0];
  sh[0] = 2 * sh[0] - is_boson;
  array<dcomplex, std14::decay_t<decltype(dat)>::rank> new_data(sh);
  auto _ = arrays::ellipsis{};
  if (is_boson) new_data(L - 1, _) = dat(0, _);
  int L1 = (is_boson ? L - 1 : L);
  for (int u = is_boson; u < L; ++u) {
   new_data(L1 + u, _) = dat(u, _);
   new_data(L - 1 - u, _) = conj(dat(u, _));
  }
  return {gf_mesh<imfreq>{g.mesh().domain(), L}, std::move(new_data), g.singularity(), g.symmetry(), g.indices(), g.name};
 }

 /// is_real_in_tau(g, tolerance). Returns true iif the FOURIER transform g(tau) is real up to tolerance
 template <typename T, typename S, typename E>
 bool is_gf_real_in_tau(gf_const_view<imfreq, T, S, E> g, double tolerance = 1.e-13) {
  if (g.mesh().positive_only()) return true;
  using triqs::arrays::max_element; // the case real, complex is not found by ADL
  for (auto const &w : g.mesh().get_positive_freq())
   if (max_element(abs(conj(g(-w)) - g(w))) > tolerance) return false;
  return true;
 }
 template <typename T, typename S, typename E> bool is_gf_real_in_tau(gf_view<imfreq, T, S, E> g, double tolerance = 1.e-13) {
  return is_gf_real_in_tau(make_const_view(g), tolerance);
 }
 template <typename T, typename S, typename E> bool is_gf_real_in_tau(gf<imfreq, T, S, E> const &g, double tolerance = 1.e-13) {
  return is_gf_real_in_tau(make_const_view(g), tolerance);
 }

 /// Make a const view of the positive frequency part of the function
 template <typename G> view_type_t<G> positive_freq_view(G &&g) {
  static_assert(std::is_same<typename std14::decay_t<G>::variable_t, imfreq>::value, "positive_freq_view only makes senses for imfreq gf");
  if (g.mesh().positive_only()) return g;
  long L = g.mesh().size();
  long L1 = (L + 1) / 2; // fermion : L is even. boson, L = 2p+1 --> p+1
  int is_boson = (g.mesh().domain().statistic == Boson);
  return {g.mesh().get_positive_freq(),
          g.data()(range(L1 - is_boson, L), arrays::ellipsis()),
          g.singularity(),
          g.symmetry(),
          g.indices(),
          {}};
 }

 /// Make_real_in_tau symmetrize the function in freq, to ensure its FT is real.
 template <typename T, typename S, typename E> gf<imfreq, T, S> make_real_in_tau(gf_const_view<imfreq, T, S, E> g) {
  return make_gf_from_real_gf(positive_freq_view(g));
 }
 template <typename T, typename S, typename E> gf<imfreq, T, S> make_real_in_tau(gf_view<imfreq, T, S, E> g) {
  return make_real_in_tau(g);
 }
 template <typename T, typename S, typename E> gf<imfreq, T, S> make_real_in_tau(gf<imfreq, T, S, E> const &g) {
  return make_real_in_tau(g);
 }

 /// ---------------------------  hdf5 ---------------------------------

 template <typename S> struct gf_h5_name<imfreq, matrix_valued, S> {
  static std::string invoke() { return "ImFreq"; }
 };
 template <typename S, int R>
 struct gf_h5_name<imfreq, tensor_valued<R>, S> : gf_h5_name<imfreq, matrix_valued, S> {};

 // Before writing to h5, check if I can save the positive freq only
 template <typename T, typename S, typename E> struct gf_h5_before_write<imfreq, T, S, E> {
  template <typename G> static gf_const_view<imfreq, T, S, E> invoke(h5::group gr, G const &g) {
   if (is_gf_real_in_tau(g, 1.e-13)) return positive_freq_view(g);
   return g;
  }
 };

 // After reading from h5, is the function is for freq >0, unfold it to the full mesh
 template <typename T, typename S, typename E> struct gf_h5_after_read<imfreq, T, S, E> {
  template <typename G> static void invoke(h5::group gr, G &g) {
   if (g.mesh().positive_only()) g = make_gf_from_real_gf(make_const_view(g));
  }
 };

 /// ---------------------------  gf_evaluator ---------------------------------

  template <typename Target, typename Sing> struct gf_evaluator<imfreq, Target, Sing> {

  static constexpr int arity = 1;
  template <typename G> gf_evaluator(G *) {};

  // technical details...
  using r_t = typename Target::value_type;
  using rv_t = typename const_view_type_if_exists_else_type< typename Target::value_type>::type;

  template <typename S> auto _evaluate_sing(matrix_valued, S const &s, matsubara_freq const &f) const RETURN(evaluate(s, f));
  template <typename S>
  auto _evaluate_sing(scalar_valued, S const &s, matsubara_freq const &f) const RETURN(evaluate(s, f)(0, 0));
  rv_t _evaluate_sing(Target, nothing, matsubara_freq const &f) const {
   TRIQS_RUNTIME_ERROR << "Evaluation out of mesh";
   return r_t{};
  }

  // gf_evaluator
  template <typename G> rv_t operator()(G const &g, matsubara_freq const &f) const {
   if (g.mesh().is_within_boundary(f.n)) return g[f.n];
   if (g.mesh().positive_only()) { 
    int sh = (g.mesh().domain().statistic == Fermion ? 1 : 0);
    if (g.mesh().is_within_boundary(-f.n - sh)) return r_t{conj(g[-f.n - sh])};
   }
   return _evaluate_sing(Target{}, g.singularity(), f);
  }

  // int -> replace by matsubara_freq
  template <typename G>
  AUTO_DECL operator()(G const &g, int n) const RETURN(g(matsubara_freq(n, g.mesh().domain().beta, g.mesh().domain().statistic)));

  // Evaluate on the tail : compose the tails
  template <typename G> typename G::singularity_t operator()(G const &g, tail_view t) const {
   return compose(g.singularity(), t);
  }
 };

  // no evaluator for lambda_valued
 template <typename F, typename Sing> struct gf_evaluator<imfreq, lambda_valued<F>, Sing> {
  static constexpr int arity = 1;//needed to compile
  template <typename G> gf_evaluator(G *) {};
 };

}
}
