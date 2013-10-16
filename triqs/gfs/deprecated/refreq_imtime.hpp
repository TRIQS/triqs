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
#ifndef TRIQS_GF_IMTIME_REFREQ_TIMES_H
#define TRIQS_GF_IMTIME_REFREQ_TIMES_H
#include "./tools.hpp"
#include "./gf.hpp"
#include "./imtime.hpp"
#include "./refreq.hpp"
#include "./meshes/product.hpp"

namespace triqs { namespace gfs {

 struct refreq_imtime {};

 // the mesh
 template<typename Opt> struct gf_mesh<refreq_imtime,Opt> :mesh_product<gf_mesh<refreq,Opt>,gf_mesh<imtime,Opt>>  {
  typedef gf_mesh<refreq,Opt> m1_t; 
  typedef gf_mesh<imtime,Opt> m2_t; 
  typedef mesh_product<m1_t,m2_t> B;
  gf_mesh () = default;
  gf_mesh(B const & x) : B(x) {}
  gf_mesh (double wmin, double wmax, size_t n_freq, double beta, statistic_enum S, size_t nt, mesh_kind mk=full_bins ) :
   B {gf_mesh<refreq,Opt>(wmin,wmax,n_freq), gf_mesh<imtime,Opt>(beta,S,nt,mk)} {}
 };

 namespace gfs_implementation { 
  // singularity
  //template<typename Opt> struct singularity<refreq_imtime,scalar_valued,Opt>  { typedef gf<retime,scalar_valued> type;};

  // h5 name
  template<typename Opt> struct h5_name<refreq_imtime,scalar_valued,Opt> { static std::string invoke(){ return  "GfReFreqImTime";}};

  /// ---------------------------  data access  ---------------------------------

  template<typename Opt> struct data_proxy<refreq_imtime,scalar_valued,Opt> : data_proxy_array<std::complex<double>,1> {};

  /// ---------------------------  evaluator ---------------------------------

  template<typename Opt>
   struct evaluator<refreq_imtime,scalar_valued,Opt> {
    static constexpr int arity = 2;
    template<typename G>
     std::complex<double> operator() (G const * g, double omega, double tau)  const {
      double beta = std::get<1>(g->mesh().components()).domain().beta;
      int p = std::floor(tau/beta);
      tau -= p*beta;
      size_t n1,n2; double w1,w2; bool in;
      std::tie(in, n1, w1) = windowing( std::get<0>(g->mesh().components()),omega);
      if (!in) TRIQS_RUNTIME_ERROR <<" Evaluation out of bounds";
      std::tie(in, n2, w2) = windowing( std::get<1>(g->mesh().components()),tau);
      if (!in) TRIQS_RUNTIME_ERROR <<" Evaluation out of bounds";
      auto gg = on_mesh(*g); //[g]( size_t n1, size_t n2) {return g->on_mesh(n1,n2);};
      auto res =  w1 *( w2*gg(n1,n2) + (1-w2)*gg(n1,n2+1)) + (1-w1) * ( w2*gg(n1+1,n2) + (1-w2)*gg(n1+1,n2+1));

      //std::cout  << "eval reref imtim"<< n1 << " "<< n2 << " "<< w1 << " " << w2 << " "<< omega << " "<< tau<<  std::endl;

      return ((std::get<1>(g->mesh().components()).domain().statistic == Fermion) && (p%2==1) ? -res : res);
     } 
   };

  // -------------------------------   Factories  --------------------------------------------------

  template<typename Opt> struct factories<refreq_imtime, scalar_valued,Opt> {
   typedef gf<refreq_imtime, scalar_valued,Opt> gf_t;
   struct target_shape_t {};

   template<typename MeshType>
    static gf_t make_gf(MeshType && m) {
     typename gf_t::data_regular_t A(m.size());
     A() =0;
     return gf_t (m, std::move(A), nothing(), nothing() ) ;
    }

   static gf_t make_gf(double wmin, double wmax, size_t n_freq, double beta, statistic_enum S, size_t nt, mesh_kind mk=full_bins) { 
    auto m =  gf_mesh<refreq_imtime,Opt>(wmin, wmax, n_freq, beta,S, nt, mk);
    typename gf_t::data_regular_t A(m.size()); 
    A() =0;
    return gf_t (m, std::move(A), nothing(), nothing() ) ;
   }
  };

 } // gfs_implementation

 //slices
 inline gf_view<refreq,scalar_valued> slice_mesh_imtime (gf_view<refreq_imtime,scalar_valued> g, size_t index) { 
  auto arr = reinterpret_linear_array(g.mesh(),g.data()); // view it as a 2d array
  return { std::get<0>(g.mesh().components()), arr(arrays::range(),index), local::tail(1,1), nothing() };
 }

}}
#endif

