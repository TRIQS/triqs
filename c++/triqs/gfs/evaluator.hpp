// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
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

namespace triqs::gfs {

  // evaluator by default forwards evarything to evaluate
  // specialization for tail and out-of-mesh treatment

  /*----------------------------------------------------------
  *  Default
  *--------------------------------------------------------*/

  // gf_evaluator regroup functions to evaluate the function.
  // default : one variable. Will be specialized in more complex cases.
  template <typename Var, typename Target> struct gf_evaluator {
    static constexpr int arity = 1;

    template <typename G, typename X> auto operator()(G const &g, X x) const { return evaluate(g, x); }
  };

  /*----------------------------------------------------------
   *  mesh::imfreq
   *--------------------------------------------------------*/

  template <typename Target> struct gf_evaluator<mesh::imfreq, Target> {

    static constexpr int arity = 1;

    // FIXME : la tail est unique : tail -> valeur tensorielle
    // evaluate(s,f) --> retourne aussi un proxy !!!

    // technical details...
    using r_t = typename Target::value_t;

    // gf_evaluator
    // FIXME We can instead return a view and write the result of the tail calculation
    // to a member
    template <typename G> r_t operator()(G const &g, matsubara_freq const &f) const {
      if (g.mesh().is_within_boundary(f.n)) return g[f.n];
      if (g.mesh().positive_only()) {
        int sh = (g.mesh().domain().statistic == Fermion ? 1 : 0);
        if (g.mesh().is_within_boundary(-f.n - sh)) return r_t{conj(g[-f.n - sh])};
        TRIQS_RUNTIME_ERROR << " ERROR: Cannot evaluate Green function with positive only mesh outside grid ";
      }

      auto [tail, err] = fit_tail_no_normalize(g);

      dcomplex x = std::abs(g.mesh().omega_max()) / f;
      auto res   = nda::zeros<dcomplex>(g.target_shape()); // a new array

      dcomplex z = 1.0;
      for (int n : range(tail.extent(0))) {
        res += tail(n, ellipsis()) * z;
        z = z * x;
      }
      return res;
    }

    // int -> replace by matsubara_freq
    template <typename G> decltype(auto) operator()(G const &g, int n) const {
      return g(matsubara_freq(n, g.mesh().domain().beta, g.mesh().domain().statistic));
    }
  };

  /*----------------------------------------------------------
   *  cartesian product
   *--------------------------------------------------------*/

  template <typename Target, typename... Ms> struct gf_evaluator<mesh::prod<Ms...>, Target> {

    static constexpr int arity = sizeof...(Ms);

    template <typename G, typename... Args> auto operator()(G const &g, Args &&... args) const {
      static_assert(sizeof...(Args) == arity, "Wrong number of arguments in gf evaluation");

      using r1_t = decltype(evaluate(g, std::forward<Args>(args)...));
 
      if constexpr (is_gf_expr<r1_t>::value or is_gf_v<r1_t>) {
        // -> Shouldn't there be an is_within_boundary check here?
        return evaluate(g, std::forward<Args>(args)...);
      } else { // -> Case of a single number / matrix as return value
        if (g.mesh().is_within_boundary(args...)) return make_regular(evaluate(g, std::forward<Args>(args)...));
        using rt = std::decay_t<decltype(make_regular(evaluate(g, std::forward<Args>(args)...)))>;
        return rt{nda::zeros<dcomplex>(g.target_shape())};
      }
    }
  };

  /*----------------------------------------------------------
   * Legendre
   *--------------------------------------------------------*/

  //// Not finished, not tested
  //template <> struct gf_evaluator<mesh::legendre, matrix_valued> {
  //static constexpr int arity = 1;
  //template <typename G> nda::matrix_view<dcomplex> operator()(G const &g, long n) const {
  //return g.data()(n, itertools::range(), itertools::range());
  //}
  //};
} // namespace triqs::gfs
