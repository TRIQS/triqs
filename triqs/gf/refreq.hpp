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
#ifndef TRIQS_GF_FREQ_H
#define TRIQS_GF_FREQ_H
#include "./tools.hpp"
#include "./gf.hpp"
#include "./local/tail.hpp"
#include "./domains/R.hpp"
#include "./meshes/linear.hpp"

namespace triqs { namespace gf {

 struct refreq {};

 namespace gf_implementation { 
  
  template<typename Opt> struct mesh<refreq,Opt>                { 
   typedef linear_mesh<R_domain> type;
   typedef typename type::domain_t domain_t;
   static type make(double wmin, double wmax, size_t n_freq, mesh_kind mk=full_bins) {
    return type(domain_t(), wmin, wmax, n_freq, mk);
   }
  };
  
  // singularity 
  template<typename Opt> struct singularity<refreq,matrix_valued,Opt>  { typedef local::tail type;};
  template<typename Opt> struct singularity<refreq,scalar_valued,Opt>  { typedef local::tail type;};
  
  // h5 name
  template<typename Opt> struct h5_name<refreq,matrix_valued,Opt>      { static std::string invoke(){ return "GfReFreq";}};
  template<typename Opt> struct h5_name<refreq,scalar_valued,Opt>      { static std::string invoke(){ return "GfReFreq_s";}};

  /// ---------------------------  evaluator ---------------------------------
  template<typename Opt, typename Target>
   struct evaluator<refreq,Target,Opt> {
    static constexpr int arity = 1;
    typedef typename std::conditional < std::is_same<Target, matrix_valued>::value, arrays::matrix_view<std::complex<double>>, std::complex<double>>::type rtype; 
    template<typename G>
     rtype operator() (G const * g,double w0)  const {
      auto & data = g->data();
      auto & mesh = g->mesh();
      size_t index; double w; bool in;
      std::tie(in, index, w) = windowing(mesh,w0);
      if (!in) TRIQS_RUNTIME_ERROR <<" Evaluation out of bounds";
      return w*data(mesh.index_to_linear(index), arrays::ellipsis()) + (1-w)*data(mesh.index_to_linear(index+1), arrays::ellipsis());
     }
    template<typename G>
     local::tail_view operator()(G const * g,freq_infty const &) const {return g->singularity();}
   };

  /// ---------------------------  data access  ---------------------------------
  template<typename Opt> struct data_proxy<refreq,matrix_valued,Opt> : data_proxy_array<std::complex<double>,3> {};
  template<typename Opt> struct data_proxy<refreq,scalar_valued,Opt> : data_proxy_array<std::complex<double>,1> {};
   
  // -------------------------------   Factories  --------------------------------------------------
  
  //matrix_valued
  template<typename Opt> struct factories<refreq, matrix_valued,Opt> {
   typedef gf<refreq,matrix_valued> gf_t;

   template<typename MeshType>
    static gf_t make_gf(MeshType && m, tqa::mini_vector<size_t,2> shape, local::tail_view const & t) {
     typename gf_t::data_non_view_t A(shape.front_append(m.size())); A() =0;
     return gf_t ( std::forward<MeshType>(m), std::move(A), t, nothing() ) ;
    }

   static gf_t make_gf(double wmin, double wmax, size_t n_freq, tqa::mini_vector<size_t,2> shape) {
    typename gf_t::data_non_view_t A(shape.front_append(n_freq)); A() =0;
    return gf_t(mesh<refreq,Opt>::make(wmin, wmax, n_freq, full_bins), std::move(A), local::tail(shape), nothing());
   }

   static gf_t make_gf(double wmin, double wmax, size_t n_freq, tqa::mini_vector<size_t,2> shape, mesh_kind mk) {
    typename gf_t::data_non_view_t A(shape.front_append(n_freq)); A() =0;
    return gf_t(mesh<refreq,Opt>::make(wmin, wmax, n_freq, mk), std::move(A), local::tail(shape), nothing());
   }
  };
   
   //scalar_valued
  template<typename Opt> struct factories<refreq,scalar_valued,Opt> {
   typedef gf<refreq,scalar_valued> gf_t;

   template<typename MeshType>
    static gf_t make_gf(MeshType && m, local::tail_view const & t) {
     typename gf_t::data_non_view_t A(m.size()); A() =0;
     return gf_t ( std::forward<MeshType>(m), std::move(A), t, nothing() ) ;
    }
    
    static gf_t make_gf(double wmin, double wmax, size_t n_freq) {
     typename gf_t::data_non_view_t A(n_freq); A() =0;
     return gf_t(mesh<refreq,Opt>::make(wmin, wmax, n_freq), std::move(A), local::tail(tqa::mini_vector<size_t,2>(1,1)), nothing());
    }

   static gf_t make_gf(double wmin, double wmax, size_t n_freq, mesh_kind mk) {
    typename gf_t::data_non_view_t A(n_freq); A() =0;
    return gf_t(mesh<refreq,Opt>::make(wmin, wmax, n_freq, mk), std::move(A), local::tail(tqa::mini_vector<size_t,2>(1,1)), nothing());
   }

  };
 } // gf_implementation

}}
#endif

