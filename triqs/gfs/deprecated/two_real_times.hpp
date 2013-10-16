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
#ifndef TRIQS_GF_TWO_TIMES_H
#define TRIQS_GF_TWO_TIMES_H
#include "./tools.hpp"
#include "./gf.hpp"
#include "./retime.hpp"
#include "./meshes/product.hpp"

namespace triqs { namespace gfs { 
 
 struct two_real_times {};

 // the mesh
 template<typename Opt> struct gf_mesh<two_real_times,Opt> :mesh_product<gf_mesh<retime,Opt> ,gf_mesh<retime,Opt> >  { 
  typedef mesh_product<gf_mesh<retime,Opt> ,gf_mesh<retime,Opt> > B;
  gf_mesh() = default;
  gf_mesh (double tmax, double n_time_slices) :
   B(gf_mesh<retime,Opt> ( 0, tmax,n_time_slices, full_bins), 
     gf_mesh<retime,Opt> ( 0, tmax,n_time_slices, full_bins) ) {}
 };

 namespace gfs_implementation { 

  /// ---------------------------  closest mesh point on the grid ---------------------------------

  template<typename Opt>
   struct get_closest_point <two_real_times,matrix_valued,Opt> {
    typedef typename gf_mesh<two_real_times, Opt>::type mesh_t;

    //    // NOT FINISHED, NOT TESTED 
    //     template<typename G, typename T>
    //      static typename mesh_t::index_t invoke(G const * g, closest_pt_wrap<T,T> const & p) {
    //       return std::floor( double(p.value) / g->mesh().delta() + 0.5);
    //      }

   };

   // h5 name
  template<typename Opt> struct h5_name<two_real_times,matrix_valued,Opt> { static std::string invoke(){ return  "GfTwoRealTime";}};
  template<typename Opt> struct h5_name<two_real_times,scalar_valued,Opt> { static std::string invoke(){ return  "GfTwoRealTime_s";}};
  
  /// ---------------------------  evaluator ---------------------------------
  
  template<typename Opt, typename Target>
  struct evaluator<two_real_times,Target,Opt> {
   static constexpr int arity = 2;
   typedef typename std::conditional < std::is_same<Target, matrix_valued>::value, arrays::matrix<std::complex<double>>, std::complex<double>>::type rtype; 
   template<typename G>
   rtype operator() (G const * g, double t0, double t1)  const {
    int n0,n1; double w0,w1; bool in;
    std::tie(in, n0, w0) = windowing(std::get<0>(g->mesh().components()),t0);
    if (!in) TRIQS_RUNTIME_ERROR <<" Evaluation out of bounds";
    std::tie(in, n1, w1) = windowing(std::get<1>(g->mesh().components()),t1);
    if (!in) TRIQS_RUNTIME_ERROR <<" Evaluation out of bounds";
    auto gg = on_mesh(*g);
    return  (1-w0) * ( (1-w1) * gg(n0, n1) + w1 * gg(n0, n1+1) ) + w0 * ( (1-w1) * gg(n0+1, n1) + w1 * gg(n0+1, n1+1)); 
   } 
  };
  
  /// ---------------------------  data access  ---------------------------------
  
  template<typename Opt> struct data_proxy<two_real_times,matrix_valued,Opt> : data_proxy_array<std::complex<double>,3> {};
  template<typename Opt> struct data_proxy<two_real_times,scalar_valued,Opt> : data_proxy_array<std::complex<double>,1> {};
  
  // -------------------------------   Factories  --------------------------------------------------
  
  //matrix_valued
  template<typename Opt> struct factories<two_real_times, matrix_valued,Opt> {
   typedef gf<two_real_times, matrix_valued,Opt> gf_t;
   typedef gf_mesh<two_real_times, Opt> mesh_t;
   typedef tqa::mini_vector<int,2> target_shape_t;
   
   static gf_t make_gf(double tmax, double n_time_slices, tqa::mini_vector<int,2> shape) {
    auto m =  gf_mesh<two_real_times,Opt>(tmax, n_time_slices);
    typename gf_t::data_regular_t A(shape.front_append(m.size())); A() =0;
    return gf_t (m, std::move(A), nothing(), nothing() ) ;
   }
  };
  
  //scalar_valued
  template<typename Opt> struct factories<two_real_times, scalar_valued,Opt> {
   typedef gf<two_real_times, scalar_valued,Opt> gf_t;
   typedef gf_mesh<two_real_times, Opt> mesh_t;
   struct target_shape_t {};
   
   static gf_t make_gf(double tmax, double n_time_slices) {
    auto m =  gf_mesh<two_real_times,Opt>(tmax, n_time_slices);
    typename gf_t::data_regular_t A(m.size()); A() =0;
    return gf_t (m, std::move(A), nothing(), nothing() ) ;
   }
  };
  
 } // gfs_implementation
 
 // -------------------------------   Additionnal free function for this gf  --------------------------------------------------
 
 // from g(t,t') and t, return g(t-t') for any t'>t 
 // 
 inline gf<retime> slice (gf_view<two_real_times> const & g, double t) { 
  auto const & m = std::get<0> (g.mesh().components());   //one-time mesh
  int it = get_closest_mesh_pt_index(m, t);              //index of t on this mesh
  int nt = m.size() - it;
  if (it+1 < nt) nt = it+1 ;                              //nt=length of the resulting GF's mesh
    double dt = m.delta();
  auto res = gf<retime>{{0, 2*(nt-1)*dt, nt}, g(t,t).shape()};
  res() = 0;
  auto _ = arrays::range();// everyone
  for(int sh=0; sh<nt; sh++){
   res.data()(sh,_,_) = g.data()(g.mesh().index_to_linear(std::make_tuple( it+sh, it-sh) ),_,_);
  }
  return res;
 }
 
 // Get the 1 time mesh from the 2 times cartesian product (for cython interface mainly)
 template<typename M> 
 auto get_1d_mesh_from_2times_mesh(M const & m) DECL_AND_RETURN(std::get<0>(m.components()));
 
}}
#endif

