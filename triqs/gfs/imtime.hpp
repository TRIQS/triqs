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

namespace triqs { namespace gfs {

 struct imtime {};

 namespace gfs_implementation { 

  // mesh type and its factories
  template<typename Opt> struct mesh<imtime,Opt> {
   typedef linear_mesh<matsubara_domain<false>> type;
   typedef typename type::domain_t domain_t;
   static type make(double beta, statistic_enum S, size_t n_time_slices, mesh_kind mk=half_bins) {
    if(beta<0)TRIQS_RUNTIME_ERROR<<" fabrication of the Matsubara mesh not possible: beta="<< beta <<"\n";
    return type(domain_t(beta,S), 0, beta, n_time_slices, mk);
   }
  };
 
  // singularity 
  template<typename Opt> struct singularity<imtime,matrix_valued,Opt>  { typedef local::tail type;};
  template<typename Opt> struct singularity<imtime,scalar_valued,Opt>  { typedef local::tail type;};
  
  // h5 name
  template<typename Opt> struct h5_name<imtime,matrix_valued,Opt>      { static std::string invoke(){ return  "GfImTime";}};
  template<typename Opt> struct h5_name<imtime,scalar_valued,Opt>      { static std::string invoke(){ return  "GfImTime_s";}};

  /// ---------------------------  data access  ---------------------------------

  template<typename Opt> struct data_proxy<imtime,matrix_valued,Opt> : data_proxy_array<double,3> {};
  template<typename Opt> struct data_proxy<imtime,scalar_valued,Opt> : data_proxy_array<double,1> {};

   /// ---------------------------  closest mesh point on the grid ---------------------------------

  template<typename Opt, typename Target>
   struct get_closest_point <imtime,Target,Opt> {
    // index_t is size_t
    template<typename G, typename T>
     static size_t invoke(G const * g, closest_pt_wrap<T> const & p) {
      double x = (g->mesh().kind()==half_bins ? double(p.value) :  double(p.value)+ 0.5*g->mesh().delta());
      size_t n = std::floor(x/g->mesh().delta());
      return n;
     }
   };


  /// ---------------------------  evaluator ---------------------------------

  // NOT TESTED
  // TEST THE SPPED when q_view are incorporated...
  // true evaluator with interpolation ...
  template<typename G, typename ReturnType>
   ReturnType evaluator_imtime_impl (G const * g, double tau, ReturnType && _tmp) {
    // interpolate between n and n+1, with weight
    double beta = g->mesh().domain().beta;
    int p = std::floor(tau/beta);
    tau -= p*beta;
    double a = tau/g->mesh().delta();
    long n = std::floor(a);
    double w = a-n;
    assert(n < g->mesh().size()-1);
    auto _ = arrays::ellipsis();
    if ((g->mesh().domain().statistic == Fermion) && (p%2==1))
     _tmp = - w*g->data()(n, _) - (1-w)*g->data()(n+1, _);
    else
     _tmp =   w*g->data()(n, _) + (1-w)*g->data()(n+1, _);
    //else { // Speed test to redo when incoparated qview in main branch
    // _tmp(0,0) =   w*g->data()(n, 0,0) + (1-w)*g->data()(n+1, 0,0);
    // _tmp(0,1) =   w*g->data()(n, 0,1) + (1-w)*g->data()(n+1, 0,1);
    // _tmp(1,0) =   w*g->data()(n, 1,0) + (1-w)*g->data()(n+1, 1,0);
    // _tmp(1,1) =   w*g->data()(n, 1,1) + (1-w)*g->data()(n+1, 1,1);
    // }
    return _tmp;
   }

  template<typename Opt>
   struct evaluator<imtime,matrix_valued,Opt> {
    private:
     mutable arrays::matrix<double> _tmp;
    public :
     static constexpr int arity = 1;
     evaluator() = default;
     evaluator(size_t n1, size_t n2) : _tmp(n1,n2) {} // WHAT happen in resize ??

     template<typename G>
      arrays::matrix<double> const & operator()(G const * g, double tau) const { return evaluator_imtime_impl(g, tau, _tmp);}

     template<typename G>
      typename G::singularity_t const & operator()(G const * g,freq_infty const &) const {return g->singularity();}
   };

  template<typename Opt>
   struct evaluator<imtime,scalar_valued,Opt> {
    public :
     static constexpr int arity = 1;

     template<typename G> double operator()(G const * g, double tau) const { return evaluator_imtime_impl(g, tau, 0.0);}

     template<typename G>
      typename G::singularity_t const & operator()(G const * g,freq_infty const &) const {return g->singularity();}
   };

 // -------------------------------   Factories  --------------------------------------------------

  // matrix_valued
  template<typename Opt> struct factories<imtime,matrix_valued,Opt> { 
   typedef gf<imtime,matrix_valued,Opt> gf_t;
   template<typename MeshType>
    static gf_t make_gf(MeshType && m, tqa::mini_vector<size_t,2> shape, local::tail_view const & t) {
     typename gf_t::data_non_view_t A(shape.front_append(m.size())); A() =0;
     //return gf_t ( m, std::move(A), t, nothing() ) ;
     return gf_t (std::forward<MeshType>(m), std::move(A), t, nothing(), evaluator<imtime,matrix_valued,Opt>(shape[0],shape[1]) ) ;
    }
   static gf_t make_gf(double beta, statistic_enum S,  tqa::mini_vector<size_t,2> shape, size_t Nmax=1025, mesh_kind mk= half_bins) {
    return make_gf(mesh<imtime,Opt>::make(beta,S,Nmax,mk), shape, local::tail(shape));
   }
   static gf_t make_gf(double beta, statistic_enum S, tqa::mini_vector<size_t,2> shape, size_t Nmax, mesh_kind mk, local::tail_view const & t) {
    return make_gf(mesh<imtime,Opt>::make(beta,S,Nmax,mk), shape, t);
   }
  };

  // scalar_valued 
  template<typename Opt> struct factories<imtime,scalar_valued,Opt> { 
   typedef gf<imtime,scalar_valued,Opt> gf_t;
   template<typename MeshType>
    static gf_t make_gf(MeshType && m, local::tail_view const & t) {
     typename gf_t::data_non_view_t A(m.size()); A() =0;
     return gf_t (std::forward<MeshType>(m), std::move(A), t, nothing());
    }
   static gf_t make_gf(double beta, statistic_enum S, size_t Nmax=1025, mesh_kind mk= half_bins) {
    return make_gf(mesh<imtime,Opt>::make(beta,S,Nmax,mk), local::tail(tqa::mini_vector<size_t,2> (1,1)));
   }
   static gf_t make_gf(double beta, statistic_enum S, size_t Nmax, mesh_kind mk, local::tail_view const & t) {
    return make_gf(mesh<imtime,Opt>::make(beta,S,Nmax,mk), t);
   }
  };
 } // gfs_implementation.
 
}}
#endif

