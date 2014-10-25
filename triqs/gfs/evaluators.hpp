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
#include "./tools.hpp"
#include "./gf.hpp"

namespace triqs {
namespace gfs {
 namespace gfs_implementation {

  // simple evaluation : take the point on the grid...
  struct evaluator_grid_simple {
   long n;
   evaluator_grid_simple() = default;
   template <typename MeshType, typename PointType> evaluator_grid_simple(MeshType const &m, PointType const &p) { n = p; }
   template <typename F> auto operator()(F const &f) const DECL_AND_RETURN(f(n));
  };

  // a linear interpolation
  struct evaluator_grid_linear_interpolation {
   double w1, w2;
   size_t n1, n2;

   evaluator_grid_linear_interpolation() = default;

   template <typename MeshType, typename PointType>
   void reset(MeshType const &m, PointType const &p, double prefactor = 1) {
    bool in;
    double w;
    std::tie(in, n1, w) = windowing(m, p);
    if (!in) TRIQS_RUNTIME_ERROR << " Evaluation out of bounds";
    w1 = prefactor * (1 - w);
    w2 = prefactor * w;
    n2 = n1 + 1;
   }

   template <typename F> auto operator()(F const &f) const RETURN(w1 *f(n1) + w2 *f(n2));
  };

  // the evaluator for various types.
  template <typename MeshType> struct evaluator_fnt_on_mesh;

// cannot use inherited constructors, too recent...
#define TRIQS_INHERIT_AND_FORWARD_CONSTRUCTOR(NEWCLASS, CLASS) : CLASS {                                                          \
  template <typename... T> NEWCLASS(T &&... t) : CLASS(std::forward<T>(t)...) {};                                                \
 };

  //
  template <typename Variable> struct evaluator_one_var {
   mutable evaluator_fnt_on_mesh<Variable> ev;

   public:
   static constexpr int arity = 1;
   evaluator_one_var() = default;

#ifndef TRIQS_C11
   template <typename G> auto operator()(G const *g, double x) const {
#else
   template <typename G> auto operator()(G const *g, double x) const -> decltype(ev(on_mesh(*g))){
#endif
    ev.reset(g->mesh(), x);
    return ev(on_mesh(*g));
   }

   template <typename G> typename G::singularity_t operator()(G const *g, tail_view t) const {
    return compose(g->singularity(), t);
   }
  };
 }
}
}
