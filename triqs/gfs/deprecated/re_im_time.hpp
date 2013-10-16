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
#ifndef TRIQS_GF_RE_IM_TIMES_H
#define TRIQS_GF_RE_IM_TIMES_H
#include "./tools.hpp"
#include "./gf.hpp"
#include "./retime.hpp"
#include "./imtime.hpp"
#include "./meshes/product.hpp"

namespace triqs { namespace gfs { 

 struct re_im_time {};

 // the gf_mesh
 template<typename Opt> struct gf_mesh<re_im_time,Opt> :   mesh_product<gf_mesh<retime,Opt>,gf_mesh<imtime,Opt>>  { 
  typedef gf_mesh<retime,Opt> m1_t; 
  typedef gf_mesh<imtime,Opt> m2_t; 
  typedef mesh_product<m1_t,m2_t> B;
  gf_mesh () = default;
  gf_mesh (double tmin, double tmax, size_t nt, double beta, statistic_enum S, size_t ntau, mesh_kind mk=full_bins) :
   B {gf_mesh<retime,Opt>(tmin,tmax,nt), gf_mesh<imtime,Opt>(beta,S, ntau, mk)} {}
 };

 namespace gfs_implementation { 

  // singularity
  //template<typename Opt> struct singularity<re_im_time, scalar_valued, Opt>  { typedef gf<retime,scalar_valued> type;};

  // h5 name
  template<typename Opt> struct h5_name<re_im_time,scalar_valued,Opt> { static std::string invoke(){ return  "GfReImTime";}};

  /// ---------------------------  data access  ---------------------------------

  template<typename Opt> struct data_proxy<re_im_time,scalar_valued,Opt> : data_proxy_array<std::complex<double>,1> {};

  /// ---------------------------  evaluator ---------------------------------
  template<typename Opt>
   struct evaluator<re_im_time,scalar_valued,Opt> {
    static constexpr int arity = 2;
    template<typename G>
     std::complex<double> operator() (G const * g, double t, double tau)  const {
      double beta = std::get<1>(g->mesh().components()).domain().beta;
      int p = std::floor(tau/beta);
      tau -= p*beta;
      size_t nr,ni; double wr,wi; bool in;
      std::tie(in, nr, wr) = windowing( std::get<0>(g->mesh().components()),t);
      if (!in) TRIQS_RUNTIME_ERROR <<" Evaluation out of bounds, tmax=" << std::get<0>(g->mesh().components()).x_max() << ", tmin=" << std::get<0>(g->mesh().components()).x_min() << "here, t=" <<t;
      std::tie(in, ni, wi) = windowing( std::get<1>(g->mesh().components()),tau);
      if (!in) TRIQS_RUNTIME_ERROR <<" Evaluation out of bounds, taumax=" << std::get<1>(g->mesh().components()).x_max()<< ", taumin=" << std::get<1>(g->mesh().components()).x_min() << "here, tau=" <<tau;
      auto gg = on_mesh(*g); //[g]( size_t nr, size_t ni) {return g->on_mesh(nr,ni);}; //data( g->mesh().index_to_linear(nr,ni));
      auto res =  (1-wr) * ( (1-wi) * gg(nr,ni) + wi * gg(nr,ni+1)) + wr * ( (1-wi) * gg(nr+1,ni) + wi * gg(nr+1,ni+1)); 
      return ((std::get<1>(g->mesh().components()).domain().statistic == Fermion) && (p%2==1) ? -res : res);
     } 
   };

  // -------------------------------   Factories  --------------------------------------------------

  template<typename Opt> struct factories<re_im_time, scalar_valued,Opt> {
   typedef gf<re_im_time, scalar_valued,Opt> gf_t;
    struct target_shape_t {};

   template<typename MeshType>
    static gf_t make_gf(MeshType && m) {
     typename gf_t::data_regular_t A(m.size());
     A() =0;
     return gf_t (m, std::move(A), nothing(), nothing() ) ;
    }

   static gf_t make_gf(double tmin, double tmax, size_t nt, double beta, statistic_enum S, size_t ntau, mesh_kind mk=full_bins) { 
    auto m =  gf_mesh<re_im_time,Opt>(tmin,tmax, nt, beta, S, ntau, mk);
    typename gf_t::data_regular_t A(m.size()); 
    A() =0;
    return gf_t (m, std::move(A), nothing(), nothing());
    //return gf_t (m, std::move(A), make_gf<retime,scalar_valued>(tmin, tmax, nt), nothing());
   }
  };

 } // gfs_implementation

 // CHANGE THIS NAME !!! 
 template<typename RHS, bool V, typename Variable, typename Target, typename Opt > 
  void assign_from_expression (gf_impl<Variable,Target,Opt,V> const &, RHS) {}

 //slices
 inline gf_view<retime,scalar_valued> slice_mesh_imtime (gf_view<re_im_time,scalar_valued> g, size_t index) { 
  auto arr = reinterpret_linear_array(g.mesh(),g.data()); // view it as a 2d array
  return { std::get<0>(g.mesh().components()), arr(arrays::range(), index), local::tail(1,1), nothing() };
 }

 /* gf_view<imtime,scalar_valued> slice_mesh_retime ( gf_view<re_im_time,scalar_valued> g, size_t index) { 
    auto arr = reinterpret_linear_array(g.mesh(),g.data()); // view it as a 2d array
    return { std::get<1>(g.mesh().components()), arr(index, arrays::range()), g.singularity().singularity(), nothing() };
    }
    */
 //   
 //  gf_view<retime,scalar_valued>  slice_meshes ( gf_view<re_im_time,scalar_valued> g, size_t index) { 
 //    return { std::get<0>(g.mesh().components()), g.data()(arrays::range(), index), tail ( g.singularity(.......) ), g.symmetry()}
 //  }

}}
#endif

