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

namespace triqs { namespace gf { 

 struct two_real_times {};

 namespace gf_implementation { 

  // the mesh
  template<typename Opt> struct mesh<two_real_times,Opt>  { 
   typedef typename mesh<retime,Opt>::type m1_t; 
   typedef mesh_product<m1_t,m1_t> type;
   static type make (double tmax, double n_time_slices) { 
#ifndef TRIQS_WORKAROUND_INTEL_COMPILER_BUGS 
    m1_t m1({},0, tmax,n_time_slices, triqs::gf::full_bins);
    return {m1,m1};
#else
    m1_t m1(typename m1_t::domain_t(),0, tmax,n_time_slices, triqs::gf::full_bins);
    type m(m1,m1);
    return m; 
#endif
   }
  };
  
  // h5 name
  template<typename Opt> struct h5_name<two_real_times,matrix_valued,Opt> { static std::string invoke(){ return  "GfTwoRealTime";}};

  /// ---------------------------  closest mesh point on the grid ---------------------------------

  template<typename Opt>
   struct get_closest_point <two_real_times,matrix_valued,Opt> {
   typedef typename mesh<two_real_times, Opt>::type mesh_t;
    
//    // NOT FINISHED, NOT TESTED 
//     template<typename G, typename T>
//      static typename mesh_t::index_t invoke(G const * g, closest_pt_wrap<T,T> const & p) {
//       return std::floor( double(p.value) / g->mesh().delta() + 0.5);
//      }

   };

  /// ---------------------------  evaluator ---------------------------------

  template<typename Opt>
  struct evaluator<two_real_times,matrix_valued,Opt> {
    static constexpr int arity = 2;
    template<typename G>
    arrays::matrix<std::complex<double> > operator() (G const * g, double t0, double t1)  const {
      auto & data = g->data();
      auto & m = std::get<0>(g->mesh().components()); 
      size_t n0,n1; double w0,w1; bool in;
      std::tie(in, n0, w0) = windowing(m,t0);
      if (!in) TRIQS_RUNTIME_ERROR <<" Evaluation out of bounds";
      std::tie(in, n1, w1) = windowing(m,t1);
      if (!in) TRIQS_RUNTIME_ERROR <<" Evaluation out of bounds";
      auto gg = [g,data]( size_t n0, size_t n1) {return data(g->mesh().index_to_linear(std::tuple<size_t,size_t>{n0,n1}), arrays::ellipsis());};
      return  w0 * ( w1*gg(n0,n1) + (1-w1)*gg(n0,n1+1) ) + (1-w0) * ( w1*gg(n0+1,n1) + (1-w1)*gg(n0+1,n1+1)); 
    } 
  };

  /// ---------------------------  data access  ---------------------------------

  template<typename Opt> struct data_proxy<two_real_times,matrix_valued,Opt> : data_proxy_array<std::complex<double>,3> {};

  // -------------------------------   Factories  --------------------------------------------------

  template<typename Opt> struct factories<two_real_times, matrix_valued,Opt> {
   typedef gf<two_real_times, matrix_valued,Opt> gf_t;
   typedef typename mesh<two_real_times, Opt>::type mesh_t;

   static gf_t make_gf(double tmax, double n_time_slices, tqa::mini_vector<size_t,2> shape) {
    auto m =  mesh<two_real_times,Opt>::make(tmax, n_time_slices);
    typename gf_t::data_non_view_t A(shape.front_append(m.size())); A() =0;
    return gf_t (m, std::move(A), nothing(), nothing() ) ;
   }
  };

  // -------------------------------   Path  --------------------------------------------------
  /*
     struct path { 

     typedef typename mesh_t::index_t mesh_pt_t; 
     typedef triqs::arrays::mini_vector<long,2> delta_t;

     delta_t pt, delta;
     size_t L;

     path( mesh_t const & m, pt_t const & start_pt, delta_t const & d_) : pt(start_pt), delta(d_), L(std::get<1>(m.components()).size()){}

     void advance() { pt += delta;}

     bool out_of_mesh () const { return (! ( (pt[1]>=0) && ( pt[0] >= pt[1]) && (pt[0]<= L)));} 

     typedef mesh_pt_generator<path> iterator;
     iterator begin() const { return {this, false};}
     iterator end()   const { return {this, true};}

     };

     path make_path ( mesh_t const & m, typename mesh_t::index_t starting_point, delta) {
     return path(m, starting_point,delta);
     }

  // for (auto & p : make_path(G.mesh(), make_tuple(i,j), make_tuple(di,dj) )) G(p) +=0;
  */

 } // gf_implementation

  // -------------------------------   Additionnal free function for this gf  --------------------------------------------------
  
  // from g(t,t') and t, return g(t-t') for any t'>t 
  // 
 gf<retime> slice (gf_view<two_real_times> const & g, double t) { 
  auto const & m = std::get<0> (g.mesh().components());   //one-time mesh
  long it = get_closest_mesh_pt_index(m, t);              //index of t on this mesh
  long nt = m.size() - it;
  if (it+1 < nt) nt = it+1 ;                              //nt=length of the resulting GF's mesh
  double dt = m.delta();
  auto res = make_gf<retime>(0, 2*(nt-1)*dt, nt, g(t,t).shape());
  res() = 0;
  auto _ = arrays::range();// everyone
  for(long sh=0; sh<nt; sh++){
   res.data()(sh,_,_) = g.data()(g.mesh().index_to_linear(std::make_tuple( it+sh, it-sh) ),_,_);
  }
  return res;
 }

 // Get the 1 time mesh from the 2 times cartesian product (for cython interface mainly)
 template<typename M> 
  auto get_1d_mesh_from_2times_mesh(M const & m) DECL_AND_RETURN(std::get<0>(m.components()));

}}
#endif

