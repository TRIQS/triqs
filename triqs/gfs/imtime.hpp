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

 struct imtime {};

 template <typename Opt> struct gf_mesh<imtime, Opt> : matsubara_time_mesh { 
  template <typename... T> gf_mesh(T &&... x) : matsubara_time_mesh(std::forward<T>(x)...) {}
  // using matsubara_time_mesh::matsubara_time_mesh;
 };

 namespace gfs_implementation {

  // singularity. If no_tail is given, then it is the default (nothing)
  template <> struct singularity<imtime, matrix_valued, void> {
   using type = local::tail;
  };
  template <> struct singularity<imtime, scalar_valued, void> {
   using type = local::tail;
  };

  // h5 name
  template <typename Opt> struct h5_name<imtime, matrix_valued, Opt> {
   static std::string invoke() { return "ImTime"; }
  };

  /// ---------------------------  data access  ---------------------------------

  template <typename Opt> struct data_proxy<imtime, matrix_valued, Opt> : data_proxy_array<double, 3> {};
  template <typename Opt> struct data_proxy<imtime, scalar_valued, Opt> : data_proxy_array<double, 1> {};

  /// ---------------------------  closest mesh point on the grid ---------------------------------

  template <typename Opt, typename Target> struct get_closest_point<imtime, Target, Opt> {
   // index_t is int
   template <typename G, typename T> static int invoke(G const *g, closest_pt_wrap<T> const &p) {
    double x = (g->mesh().kind() == half_bins ? double(p.value) : double(p.value) + 0.5 * g->mesh().delta());
    int n = std::floor(x / g->mesh().delta());
    return n;
   }
  };

  /// ---------------------------  evaluator ---------------------------------

  // this one is specific because of the beta-antiperiodicity for fermions
  template <> struct evaluator_fnt_on_mesh<imtime> {
   double w1, w2;
   long n;

   evaluator_fnt_on_mesh() = default;

   evaluator_fnt_on_mesh(gf_mesh<imtime> const &m, double tau) {
    double beta = m.domain().beta;
    int p = std::floor(tau / beta);
    tau -= p * beta;
    double w;
    bool in;
    std::tie(in, n, w) = windowing(m, tau);
    if ((m.domain().statistic == Fermion) && (p % 2 != 0)) {
     w2 = -w;
     w1 = w - 1;
    } else {
     w2 = w;
     w1 = 1 - w;
    }
   }

   template <typename F> auto operator()(F const &f) const DECL_AND_RETURN(w1 *f(n) + w2 *f(n + 1));
  };

  // now evaluator
  template <typename Opt, typename Target> struct evaluator<imtime, Target, Opt> : evaluator_one_var<imtime> {};

 } // gfs_implementation.
}
}

