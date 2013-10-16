/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#ifndef TRIQS_GF_MATSUBARA_TIME_H
#define TRIQS_GF_MATSUBARA_TIME_H
#include "./tools.hpp"
#include "./gf.hpp"
#include "./local/tail.hpp"
#include "./domains/matsubara.hpp"
#include "./meshes/linear.hpp"
#include "./evaluators.hpp"

namespace triqs { namespace gfs {

 struct imtime {};

 // gf_mesh type and its factories
 template<typename Opt> struct gf_mesh<imtime,Opt> : linear_mesh<matsubara_domain<false>> {
  typedef linear_mesh<matsubara_domain<false>> B;
  gf_mesh() = default;
  gf_mesh (typename B::domain_t d, int n_time_slices, mesh_kind mk=half_bins):
   B( d, 0, d.beta, n_time_slices, mk){}
   gf_mesh (double beta, statistic_enum S, int n_time_slices, mesh_kind mk=half_bins):
   //gf_mesh( {beta,S}, n_time_slices, mk){}
   B( typename B::domain_t(beta,S), 0, beta, n_time_slices, mk){}
 };

 namespace gfs_implementation { 

  // singularity 
  template<typename Opt> struct singularity<imtime,matrix_valued,Opt>  { typedef local::tail type;};
  template<typename Opt> struct singularity<imtime,scalar_valued,Opt>  { typedef local::tail type;};

  // h5 name
  template<typename Opt> struct h5_name<imtime,matrix_valued,Opt>      { static std::string invoke(){ return  "ImTime";}};

  /// ---------------------------  data access  ---------------------------------

  template<typename Opt> struct data_proxy<imtime,matrix_valued,Opt> : data_proxy_array<double,3> {};
  template<typename Opt> struct data_proxy<imtime,scalar_valued,Opt> : data_proxy_array<double,1> {};

  /// ---------------------------  closest mesh point on the grid ---------------------------------

  template<typename Opt, typename Target>
   struct get_closest_point <imtime,Target,Opt> {
    // index_t is int
    template<typename G, typename T>
     static int invoke(G const * g, closest_pt_wrap<T> const & p) {
      double x = (g->mesh().kind()==half_bins ? double(p.value) :  double(p.value)+ 0.5*g->mesh().delta());
      int n = std::floor(x/g->mesh().delta());
      return n;
     }
   };

  /// ---------------------------  evaluator ---------------------------------

  // this one is specific because of the beta-antiperiodicity for fermions
  template<>
   struct  evaluator_fnt_on_mesh<imtime>  { 
    double w1, w2; long n; 

    evaluator_fnt_on_mesh() = default;

    evaluator_fnt_on_mesh (gf_mesh<imtime> const & m, double tau) {
     double beta = m.domain().beta;
     int p = std::floor(tau/beta);
     tau -= p*beta;
     double w; bool in;
     std::tie(in, n, w) = windowing(m,tau);
     if (!in) TRIQS_RUNTIME_ERROR <<" Evaluation out of bounds";
     if ((m.domain().statistic == Fermion) && (p%2==1)) {w2 = -w; w1 = w-1;} else { w2 = w; w1 = 1-w;}
    }

    template<typename F> auto operator()(F const & f) const DECL_AND_RETURN(w1 * f(n) + w2 * f (n+1));
   };

   // now evaluator
   template<typename Opt, typename Target> struct evaluator<imtime,Target,Opt> : evaluator_one_var<imtime>{};

 } // gfs_implementation.
}}
#endif

