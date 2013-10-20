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
#ifndef TRIQS_GF_MATSUBARA_FREQ_H
#define TRIQS_GF_MATSUBARA_FREQ_H
#include "./tools.hpp"
#include "./gf.hpp"
#include "./local/tail.hpp"
#include "./domains/matsubara.hpp"
#include "./meshes/linear.hpp"
#include "./evaluators.hpp"
namespace triqs { namespace gfs {

 struct imfreq {};

 template<typename Opt> struct gf_mesh<imfreq,Opt> : linear_mesh<matsubara_domain<true>> {
  typedef  linear_mesh<matsubara_domain<true>> B;
  static double m1(double beta) { return std::acos(-1)/beta;}
  gf_mesh() = default;
  gf_mesh (typename B::domain_t const & d, int Nmax = 1025) : 
   B(d, d.statistic==Fermion?m1(d.beta):0, d.statistic==Fermion?(2*Nmax+1)*m1(d.beta): 2*Nmax*m1(d.beta), Nmax, without_last){}
  gf_mesh (double beta, statistic_enum S, int Nmax = 1025) : gf_mesh({beta,S}, Nmax){} 
 };

 namespace gfs_implementation { 

  //singularity
  template<typename Opt> struct singularity<imfreq,matrix_valued,Opt>  { typedef local::tail type;};
  template<typename Opt> struct singularity<imfreq,scalar_valued,Opt>  { typedef local::tail type;};

  //h5 name
  template<typename Opt> struct h5_name<imfreq,matrix_valued,Opt>      { static std::string invoke(){ return "ImFreq";}};

  /// ---------------------------  evaluator ---------------------------------

 template<> struct evaluator_fnt_on_mesh<imfreq> TRIQS_INHERIT_AND_FORWARD_CONSTRUCTOR(evaluator_fnt_on_mesh, evaluator_grid_simple);

  template<typename Opt, typename Target>
   struct evaluator<imfreq,Target,Opt> { // factorize and template on the long instead of double ?
    public :
     static constexpr int arity = 1;
     template<typename G> 
      auto operator()(G const * g, int n) const DECL_AND_RETURN((*g)[n]);

     template<typename G>
      auto operator() (G const * g, linear_mesh<matsubara_domain<true>>::mesh_point_t const & p)  
      const DECL_AND_RETURN((*g)[p.index()]);
     
     template<typename G> 
      typename G::singularity_t const & operator()(G const * g,freq_infty const &) const {return g->singularity();}
   };

  /// ---------------------------  data access  ---------------------------------
  template<typename Opt> struct data_proxy<imfreq,matrix_valued,Opt> : data_proxy_array<std::complex<double>,3> {};
  template<typename Opt> struct data_proxy<imfreq,scalar_valued,Opt> : data_proxy_array<std::complex<double>,1> {};

 } // gfs_implementation 

}}
#endif
