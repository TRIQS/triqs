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
#include "./local/tail.hpp"
#include "./local/no_tail.hpp"
#include "./domains/matsubara.hpp"
#include "./meshes/matsubara_time.hpp"
#include "./evaluators.hpp"

namespace triqs {
namespace gfs {

 // singularity
 template <> struct gf_default_singularity<imtime, matrix_valued> {
  using type = tail;
 };
 template <> struct gf_default_singularity<imtime, scalar_valued> {
  using type = tail;
 };

 namespace gfs_implementation {

  // h5 name
  template <typename Singularity> struct h5_name<imtime, matrix_valued, Singularity> {
   static std::string invoke() { return "ImTime"; }
  };

  /// ---------------------------  data access  ---------------------------------

  template <> struct data_proxy<imtime, matrix_valued> : data_proxy_array<double, 3> {};
  template <> struct data_proxy<imtime, scalar_valued> : data_proxy_array<double, 1> {};

  /// ---------------------------  closest mesh point on the grid ---------------------------------

  template <typename Singularity, typename Target> struct get_closest_point<imtime, Target, Singularity, void> {
   // index_t is int
   template <typename G, typename T> static int invoke(G const *g, closest_pt_wrap<T> const &p) {
    double x = (g->mesh().kind() == half_bins ? double(p.value) : double(p.value) + 0.5 * g->mesh().delta());
    int n = std::floor(x / g->mesh().delta());
    return n;
   }
  };

  /// ---------------------------  evaluator ---------------------------------

  // this one is specific because of the beta-antiperiodicity for fermions
  template <> struct evaluator_of_clef_expression<imtime> {
   template <typename Expr, int N> auto operator()(Expr const &expr, clef::placeholder<N>, gf_mesh<imtime> const &m, double tau) 
#ifdef TRIQS_CPP11
-> decltype(1.0 * clef::eval(expr, clef::placeholder<N>() = no_cast(m[1l])) + 1.0 * clef::eval(expr, clef::placeholder<N>() = no_cast(m[0l])))
#endif
   {
    double beta = m.domain().beta;
    int p = std::floor(tau / beta);
    tau -= p * beta;
    double w, w1, w2;
    bool in;
    long n;
    std::tie(in, n, w) = windowing(m, tau);
    if (!in) TRIQS_RUNTIME_ERROR << " Evaluation out of bounds";
    if ((m.domain().statistic == Fermion) && (p % 2 != 0)) {
     w2 = -w;
     w1 = w - 1;
    } else {
     w2 = w;
     w1 = 1 - w;
    }
    return w1 * clef::eval(expr, clef::placeholder<N>() = no_cast(m[n])) + w2 * clef::eval(expr, clef::placeholder<N>() = no_cast(m[n + 1]));
   }
  };

  // now evaluator
  template <typename Singularity, typename Target> struct evaluator<imtime, Target, Singularity> : evaluator_one_var<imtime> {
   template <typename G> evaluator(G *) {};
  };

 } // gfs_implementation.
}
}

