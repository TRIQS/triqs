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

#ifndef TRIQS_CPP11
  // simple evaluation : take the point on the grid...
  struct evaluator_of_clef_expression_grid_simple {
   template <typename Expr, int N, typename Arg, typename MeshType>
   auto operator()(Expr const &expr, clef::placeholder<N>, MeshType const &m, Arg const &p) {
    long n = p;
    return clef::eval(expr, clef::placeholder<N>() = m[n]);
   }
  };
#endif

  // a little vehicle to forbid cast : internal use only
  template<typename T> struct __no_cast { 
   T value;
   template <typename U> explicit __no_cast(U &&x) : value(std::forward<U>(x)) {}
  };
  template <typename T> __no_cast<T> no_cast(T &&x) {
   
   return __no_cast<T>{std::forward<T>(x)};
  }


  // a linear interpolation
  struct evaluator_grid_linear_interpolation {

   template <typename Expr, int N, typename Arg, typename MeshType>
   auto operator()(Expr const &expr, clef::placeholder<N>, MeshType const &m, Arg const &x) 
#ifdef TRIQS_CPP11
-> decltype(1.0 * clef::eval(expr, clef::placeholder<N>() = no_cast(m[size_t()])) + 1.0 * clef::eval(expr, clef::placeholder<N>() = no_cast(m[size_t()])))
#endif
{
    size_t n1, n2;
    bool in;
    double w;
    std::tie(in, n1, w) = windowing(m, x);
    if (!in) TRIQS_RUNTIME_ERROR << " Evaluation out of bounds";
    double w1 = (1 - w);
    double w2 = w;
    n2 = n1 + 1;
    return std::move(w1) * clef::eval(expr, clef::placeholder<N>() = no_cast(m[n1])) +
           std::move(w2) * clef::eval(expr, clef::placeholder<N>() = no_cast(m[n2]));
   }
  };

  template <typename MeshType> struct evaluator_of_clef_expression;

  //
  template <typename Variable> struct evaluator_one_var {
   mutable evaluator_of_clef_expression<Variable> ev;
   clef::placeholder<0> x_;

   public:
   static constexpr int arity = 1;

   template <typename G> auto operator()(G const *g, double x) const RETURN(ev((*g)(x_), x_, g -> mesh(), x));
   
   //template <typename G> auto operator()(G const *g, typename G::mesh_t::mesh_point_t const & p) const RETURN((*g)[p]);
   template <typename G, typename MP> auto operator()(G const *g, __no_cast<MP> const & p) const RETURN((*g)[p.value]);

   template <typename G> typename G::singularity_t operator()(G const *g, tail_view t) const {
   return compose(g->singularity(), t);
  }
 };
}
}
}
