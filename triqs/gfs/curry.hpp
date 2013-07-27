/*******************************************************************************
 * 
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#ifndef TRIQS_GF_CURRY_H
#define TRIQS_GF_CURRY_H
#include "./product.hpp"

namespace triqs { namespace gfs { 

 template<typename F> struct lambda_valued {};

 namespace gfs_implementation { 

  /// ---------------------------  data access  ---------------------------------

  template<typename Opt, typename F, typename M> struct data_proxy<M,lambda_valued<F>,Opt> : data_proxy_lambda<F> {};

  /// ---------------------------  Factories ---------------------------------

  template<typename F, typename Opt, typename ... Ms>
   struct factories<cartesian_product<Ms...>, lambda_valued<F>, Opt> {};

  // detail
  template<typename M0, typename CP> struct cartesian_product_add_front;
  template<typename M0, typename ... Ms> struct cartesian_product_add_front<M0,cartesian_product<Ms...>>{ typedef cartesian_product<M0,Ms...> type; };

  // -------------------------------------------------
  //   Partial evaluation of the gf
  // -------------------------------------------------
  //
  // Given a cartesian_product of meshes (CP), and a compile time list of int (I) 
  // - metacompute the list of Ms without position those at position 0,2 (type)
  // - provide 2 runtimes functions : 
  //   - sl : given empty tuple () and a tuple (it) of indices
  //     return the tuple of indices and range, where range are at the position defined by I,
  //     and the indices in other places, in order.
  //   - m : returns from a CP object the corresponding tuple of meshes of the remaining meshes
  //     after partial eval (of the type computed by "type").
  // - auxiliary data : 
  //    pos : position in the CP tuple (  CP::size-1 ->0)
  //    ip : position in the tuple of indices (for sl) 
  //    MP : accumulation of the final type metacomputed.
  //
  template<int pos, int ip, typename CP, typename MP, int ... I> struct pv_impl; 
  template<typename CP, int ... I> struct pv_ : pv_impl<CP::size-1,0,CP,cartesian_product<>,I...>{}; 
 
  template<typename CP, int ip, typename MP, int ... I>  struct pv_impl<-1, ip, CP, MP, I... > { 
   // the final type is a cartesian_product<...> if there is more than one mess
   // and otherwise the only mesh remaining... (avoiding cartesian_product<imfreq> e.g. which makes little sense).
   typedef typename std::conditional<MP::size==1, typename std::tuple_element<0,typename MP::type>::type, MP>::type type; 
   template<typename T, typename IT> static T sl(T t, IT const & it) {return t;}
   template<typename T, typename MT> static T m (T t, MT const & mt) {return t;}
  }; 

  template<int pos, int ip, typename CP, typename MP, int ... I> struct pv_impl {
   typedef pv_impl<pos-1, ip, CP,MP, I...> B;
   typedef typename B::type type;
   template<typename T, typename IT> static auto sl (T t, IT const & it) DECL_AND_RETURN( B::sl(triqs::tuple::push_front(t,arrays::range()),it));
   template<typename T, typename MT> static auto m  (T t, MT const & mt) DECL_AND_RETURN( B::m(t,mt));
  };

  template<int pos, int ip, typename CP, typename MP, int ... I> struct pv_impl<pos, ip, CP, MP, pos ,I...> {
   typedef typename cartesian_product_add_front<typename  std::tuple_element<pos,typename CP::type>::type, MP>::type MP2; 
   typedef pv_impl<pos-1,ip+1, CP,MP2,I...> B;
   typedef typename B::type type;
   template<typename T, typename IT> static auto sl (T t, IT const & it) DECL_AND_RETURN( B::sl(triqs::tuple::push_front(t,std::get<ip >(it)),it));
   template<typename T, typename MT> static auto m  (T t, MT const & mt) DECL_AND_RETURN( B::m (triqs::tuple::push_front(t,std::get<pos>(mt)),mt));
  };

  // partial_eval<0> (g, 1) : returns :  x -> g(1,x)
  // partial_eval<1> (g, 3) : returns :  x -> g(x,3)
  //
  template<int ... pos, typename Opt, typename Target, bool B, typename IT, typename ... Ms>
   gf_view<typename pv_<cartesian_product<Ms...>,pos...>::type ,Target, Opt> 
   partial_eval(gf_impl< cartesian_product<Ms...>, Target,Opt,B> const & g, IT index) { 
    auto arr = reinterpret_linear_array(g.mesh(),g.data());
    typedef pv_<cartesian_product<Ms...>,pos...> pv_t;
    typedef gf_view< typename pv_t::type,Target, Opt> r_t; 
    auto comp = pv_t::m(std::make_tuple(),g.mesh().components());
    auto arr_args = pv_t::sl(std::make_tuple(),index);
    // generalize this get<0> ---> flatten the tuple (construct from a tuple of args...)
    return r_t{ std::get<0>(comp), triqs::tuple::apply(arr, arr_args), typename r_t::singularity_non_view_t{}, typename r_t::symmetry_t{} };
   }

  // to adapt the partial_eval as a polymorphic lambda (replace by a lambda in c++14)
  template<typename Gview, int ... pos> struct curry_polymorphic_lambda { 
   Gview g;
   template<typename ...I>
    auto operator()(I ... i) const DECL_AND_RETURN( partial_eval<pos...>(g,std::make_tuple(i...)));
  };

  // curry<0>(g) returns : x-> y... -> g(x,y...)
  // curry<1>(g) returns : x-> y,z... -> g(y,x,z...)
  // and so on
  template<int ... pos, typename Target, typename Opt, bool B, typename ... Ms>
   gf_view< typename pv_<cartesian_product<Ms...>,pos...>::type,
	   lambda_valued<curry_polymorphic_lambda<gf_view<cartesian_product<Ms...>, Target,Opt>,pos...>>, 
	   Opt>
    curry (gf_impl<cartesian_product<Ms...>, Target,Opt,B> const & g) { 
    auto comp =  pv_<cartesian_product<Ms...>,pos...>::m(std::make_tuple(),g.mesh().components());
    typedef typename mesh< typename pv_<cartesian_product<Ms...>,pos...>::type,Opt>::type m_t;
    return {triqs::tuple::apply_construct<m_t>(comp),curry_polymorphic_lambda<gf_view<cartesian_product<Ms...>, Target,Opt>, pos ...>{g}, nothing(), nothing()}; 
   };

 } // gf_implementation

 using gfs_implementation::partial_eval;
 using gfs_implementation::curry;

 /// ----- first implementation
 /*
 // slicing on first arg
 template<typename Opt, typename M0,  typename M1>
 gf_view<M1,scalar_valued, Opt> slice_mesh0 (gf_view< cartesian_product<M0,M1>, scalar_valued,Opt> g, size_t index) { 
 auto arr = reinterpret_linear_array(g.mesh(),g.data());
 typedef gf_view<M1,scalar_valued, Opt> r_t; 
 return { std::get<1>(g.mesh().components()), arr(index,arrays::range()), typename r_t::singularity_non_view_t{}, typename r_t::symmetry_t{} };
 }

 // slicing on first arg
 template<typename Opt, typename M0, typename M1>
 gf_view<M0,scalar_valued, Opt> slice_mesh1 (gf_view< cartesian_product<M0,M1>, scalar_valued,Opt> g, size_t index) { 
 auto arr = reinterpret_linear_array(g.mesh(),g.data()); 
 typedef gf_view<M0,scalar_valued, Opt> r_t; 
 return { std::get<0>(g.mesh().components()), arr(arrays::range(), index), typename r_t::singularity_non_view_t{}, typename r_t::symmetry_t{} };
 }


 template<typename Gview> struct curry_lambda0 { 
 Gview g;
 auto operator()(size_t i) const DECL_AND_RETURN( slice_mesh0(g,i));
 };

 template<typename Opt, bool B, typename M0, typename M1>
 gf_view<M0,lambda_valued<curry_lambda0<gf_view<cartesian_product<M0,M1>, scalar_valued,Opt>>>, Opt> 
 curry0 (gf_impl<cartesian_product<M0,M1>, scalar_valued,Opt,B> const & g) { 
 return {std::get<0>(g.mesh().components()),curry_lambda0<gf_view<cartesian_product<M0,M1>, scalar_valued,Opt>>{g}, nothing(), nothing()}; 
 };

*/
}}
#endif

