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
#ifndef TRIQS_GF_RE_IM_FREQ_H
#define TRIQS_GF_RE_IM_FREQ_H
#include "./tools.hpp"
#include "./gf.hpp"
#include "./refreq.hpp"
#include "./imfreq.hpp"
#include "./meshes/product.hpp"

namespace triqs { namespace gfs { 
  
  struct re_im_freq {};

  // the mesh
  template<typename Opt> struct gf_mesh<re_im_freq,Opt> : mesh_product<gf_mesh<refreq,Opt>,gf_mesh<imfreq,Opt>>  { 
   typedef gf_mesh<refreq,Opt> m1_t; 
   typedef gf_mesh<imfreq,Opt> m2_t; 
   typedef mesh_product<m1_t,m2_t> B;
   gf_mesh () = default;
   gf_mesh (double wmin, double wmax, int n_freq_re, double beta, statistic_enum S, int n_freq_im) :
    B { gf_mesh<refreq,Opt>(wmin,wmax,n_freq_re,full_bins), gf_mesh<imfreq,Opt>(beta, S, n_freq_im)} {}
  };

  namespace gfs_implementation {

   // singularity
   template<typename Opt> struct singularity<re_im_freq,scalar_valued,Opt>  { typedef gf<refreq,scalar_valued> type;};

   // h5 name
   template<typename Opt> struct h5_name<re_im_freq,scalar_valued,Opt> { static std::string invoke(){ return  "GfReImFreq";}};

   /// ---------------------------  data access  ---------------------------------

   template<typename Opt> struct data_proxy<re_im_freq,scalar_valued,Opt> : data_proxy_array<std::complex<double>,1> {};

   /// ---------------------------  evaluator ---------------------------------  

   template<typename Opt>
    struct evaluator<re_im_freq,scalar_valued,Opt> {
     static constexpr int arity = 2;
     template<typename G>
      std::complex<double> operator() (G const * g, double w, long n)  const {
       auto & data = g->data();
       auto & mesh = g->mesh();
       int nr; double wr; bool in;
       std::tie(in, nr, wr) = windowing( std::get<0>(g->mesh().components()), w);
       if (!in) TRIQS_RUNTIME_ERROR <<" Evaluation out of bounds";
       auto gg = [g,data,mesh]( int nr, int n) {return data(mesh.index_to_linear(std::tuple<int,int>{nr,n}));};
       return wr * gg(nr,n) + (1-wr) * gg(nr+1,n) ;
      } 
    };

   // -------------------------------   Factories  --------------------------------------------------

   template<typename Opt> struct factories<re_im_freq, scalar_valued,Opt> {
    typedef gf<re_im_freq, scalar_valued,Opt> gf_t;
    struct target_shape_t {};
    //       typedef typename gf_mesh<re_im_freq, Opt>::type mesh_t;

    static gf_t make_gf(double wmin, double wmax, int nw, double beta, statistic_enum S, int nwn) { 
     auto m =  gf_mesh<re_im_freq,Opt>(wmin, wmax, nw, beta, S, nwn);
     typename gf_t::data_regular_t A(m.size()); 
     A() =0;
     return gf_t (m, std::move(A), gf<refreq,scalar_valued>({wmin, wmax, nw}), nothing() ) ;
    }
   };

  } // gfs_implementation

}}
#endif

