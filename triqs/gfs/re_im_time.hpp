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

 namespace gfs_implementation { 

  // the mesh
  template<typename Opt> struct mesh<re_im_time,Opt>  { 
   typedef typename mesh<retime,Opt>::type m1_t; 
   typedef typename mesh<imtime,Opt>::type m2_t; 
   typedef mesh_product<m1_t,m2_t> type;
   static type make (double tmin, double tmax, size_t nt, double beta, statistic_enum S, size_t ntau, mesh_kind mk=full_bins) { 
    return {make_gf_mesh<retime,Opt>(tmin,tmax,nt), make_gf_mesh<imtime,Opt>(beta,S, ntau, mk)};
   }
  };

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
      //auto & data = g->data();
      //auto & mesh = g->mesh();
      double beta = std::get<1>(g->mesh().components()).domain().beta;
      int p = std::floor(tau/beta);
      tau -= p*beta;
      size_t nr,ni; double wr,wi; bool in;
      std::tie(in, nr, wr) = windowing( std::get<0>(g->mesh().components()),t);
      if (!in) TRIQS_RUNTIME_ERROR <<" Evaluation out of bounds";
      std::tie(in, ni, wi) = windowing( std::get<1>(g->mesh().components()),tau);
      if (!in) TRIQS_RUNTIME_ERROR <<" Evaluation out of bounds";
      auto gg = on_mesh(*g); //[g]( size_t nr, size_t ni) {return g->on_mesh(nr,ni);}; //data( g->mesh().index_to_linear(nr,ni));
      auto res =  wr *( wi*gg(nr,ni) + (1-wi)*gg(nr,ni+1)) + (1-wr) * ( wi*gg(nr+1,ni) + (1-wi)*gg(nr+1,ni+1)); 
      return ((std::get<1>(g->mesh().components()).domain().statistic == Fermion) && (p%2==1) ? -res : res);
     } 
   };

  // -------------------------------   Factories  --------------------------------------------------

  template<typename Opt> struct factories<re_im_time, scalar_valued,Opt> {
   typedef gf<re_im_time, scalar_valued,Opt> gf_t;
   
   template<typename MeshType>
   static gf_t make_gf(MeshType && m) {
    typename gf_t::data_regular_t A(m.size());
    A() =0;
    return gf_t (m, std::move(A), nothing(), nothing() ) ;
   }

   static gf_t make_gf(double tmin, double tmax, size_t nt, double beta, statistic_enum S, size_t ntau, mesh_kind mk=full_bins) { 
    auto m =  make_gf_mesh<re_im_time,Opt>(tmin,tmax, nt, beta, S, ntau, mk);
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

