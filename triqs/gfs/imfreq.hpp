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
namespace triqs { namespace gfs {

 struct imfreq {};

 namespace gfs_implementation { 

  // mesh type and its factories
  template<typename Opt> struct mesh<imfreq,Opt>                { 
   typedef linear_mesh<matsubara_domain<true>> type;
   typedef typename type::domain_t domain_t;

   static type make(double beta, statistic_enum S, size_t Nmax = 1025) {
    double m1 = std::acos(-1)/beta;
    return type( domain_t(beta,S), S==Fermion?m1:0, S==Fermion?(2*Nmax+1)*m1: 2*Nmax*m1, Nmax, without_last);
   }
  };

  //singularity
  template<typename Opt> struct singularity<imfreq,matrix_valued,Opt>  { typedef local::tail type;};
  template<typename Opt> struct singularity<imfreq,scalar_valued,Opt>  { typedef local::tail type;};
  
  //h5 name
  template<typename Opt> struct h5_name<imfreq,matrix_valued,Opt>      { static std::string invoke(){ return "ImFreq";}};

  /// ---------------------------  evaluator ---------------------------------
  template<typename Opt, typename Target>
   struct evaluator<imfreq,Target,Opt> {
    static constexpr int arity = 1;
    typedef typename std::conditional < std::is_same<Target, matrix_valued>::value, arrays::matrix_view<std::complex<double>>, std::complex<double>>::type rtype; 
    template<typename G>
     rtype operator() (G const * g, long n)  const {return g->data()(n, arrays::ellipsis()); }
    // crucial because the mesh_point is cast in a complex, not an int !
    template<typename G>
     rtype operator() (G const * g, linear_mesh<matsubara_domain<true>>::mesh_point_t const & p)  const { return (*this)(g,p.index());}
    template<typename G>
     local::tail_view operator()(G const * g, freq_infty const &) const {return g->singularity();}
   };

  /// ---------------------------  data access  ---------------------------------
  template<typename Opt> struct data_proxy<imfreq,matrix_valued,Opt> : data_proxy_array<std::complex<double>,3> {};
  template<typename Opt> struct data_proxy<imfreq,scalar_valued,Opt> : data_proxy_array<std::complex<double>,1> {};

  // -------------------------------   Factories  --------------------------------------------------
  
  // matrix_valued
  template<typename Opt> struct factories<imfreq,matrix_valued,Opt> { 
    typedef gf<imfreq,matrix_valued,Opt> gf_t;
    
    template<typename MeshType>
    static gf_t make_gf(MeshType && m, tqa::mini_vector<size_t,2> shape, local::tail_view const & t) {
      typename gf_t::data_non_view_t A(shape.front_append(m.size())); A() =0;
      return gf_t ( std::forward<MeshType>(m), std::move(A), t, nothing() ) ;
    }
    static gf_t make_gf(double beta, statistic_enum S, tqa::mini_vector<size_t,2> shape) {
      return make_gf(mesh<imfreq,Opt>::make(beta,S), shape, local::tail(shape));
    }
    static gf_t make_gf(double beta, statistic_enum S,  tqa::mini_vector<size_t,2> shape, size_t Nmax) {
      return make_gf(mesh<imfreq,Opt>::make(beta,S,Nmax), shape, local::tail(shape));
    }
    static gf_t make_gf(double beta, statistic_enum S, tqa::mini_vector<size_t,2> shape, size_t Nmax, local::tail_view const & t) {
      return make_gf(mesh<imfreq,Opt>::make(beta,S,Nmax), shape, t);
    }
  };
  
  // scalar_valued
  template<typename Opt> struct factories<imfreq,scalar_valued,Opt> { 
   typedef gf<imfreq,scalar_valued,Opt> gf_t;

   template<typename MeshType>
   static gf_t make_gf(MeshType && m, local::tail_view const & t) {
    typename gf_t::data_non_view_t A(m.size()); A() =0;
    return gf_t ( std::forward<MeshType>(m), std::move(A), t, nothing() ) ;
   }
   static gf_t make_gf(double beta, statistic_enum S) {
     return make_gf(mesh<imfreq,Opt>::make(beta,S), local::tail(tqa::mini_vector<size_t,2> (1,1)));
   }
   static gf_t make_gf(double beta, statistic_enum S, size_t Nmax) {
    return make_gf(mesh<imfreq,Opt>::make(beta,S,Nmax), local::tail(tqa::mini_vector<size_t,2> (1,1)));
   }
   static gf_t make_gf(double beta, statistic_enum S, size_t Nmax, local::tail_view const & t) {
    return make_gf(mesh<imfreq,Opt>::make(beta,S,Nmax), t);
   }
  };
 } // gfs_implementation 

}}
#endif
