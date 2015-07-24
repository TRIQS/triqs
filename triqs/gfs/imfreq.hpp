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
#include "./gf.hpp"
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

 /// ---------------------------  hdf5 ---------------------------------

 template <typename S> struct gf_h5_name<imfreq, matrix_valued, S> {
  static std::string invoke() { return "ImFreq"; }
 };

 /// ---------------------------  gf_evaluator ---------------------------------

 template <typename Target, typename Sing> struct gf_evaluator<imfreq, Target, Sing> {

  static constexpr int arity = 1;
  template <typename G> gf_evaluator(G *) {};

  // technical details...
  using r_t = std14::conditional_t<std::is_same<Target, scalar_valued>::value, dcomplex, matrix<dcomplex>>;
  using rv_t = std14::conditional_t<std::is_same<Target, scalar_valued>::value, dcomplex, matrix_view<dcomplex>>;

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

 // Specialization of the conjugate for imaginary Green's functions
 template <typename Singularity, typename Evaluator>
 gf<imfreq, matrix_valued, Singularity, Evaluator> conj(gf_view<imfreq, matrix_valued, Singularity, Evaluator> g) {
  return {g.mesh(), conj(g.data()), conj(g.singularity(), true), g.symmetry(), g.indices(), g.name};
 }

}
}
