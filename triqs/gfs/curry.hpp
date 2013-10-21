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

  /// ---------------------------  partial_eval ---------------------------------
  // partial_eval<0> (g, 1) returns :  x -> g(1,x)
  // partial_eval<1> (g, 3) returns :  x -> g(x,3)

  // a technical trait: from a tuple of mesh, return the mesh (either M if it is a tuple of size 1, or the corresponding cartesian_product<M..>).
  template<typename ... Ms> struct cart_prod_impl;
  template<typename ... Ms> using cart_prod = typename cart_prod_impl<Ms...>::type;
  template<typename ... Ms> struct cart_prod_impl<std::tuple<Ms...>> { using type = cartesian_product<Ms...>;};
  template<typename M> struct cart_prod_impl<std::tuple<M>> { typedef M type;}; //using type = M;};

  template<typename M0, typename M1, typename ...M> auto rm_tuple_of_size_one(std::tuple<M0,M1,M...> const & t) DECL_AND_RETURN(t);
  template<typename M> auto rm_tuple_of_size_one(std::tuple<M> const & t) DECL_AND_RETURN(std::get<0>(t));

  // as_tuple leaves a tuple intact and wrap everything else in a tuple...
  template<typename T> std::tuple<T> as_tuple(T && x) { return std::tuple<T> {std::forward<T>(x)};}
  template<typename ... T> std::tuple<T...> as_tuple(std::tuple<T...> && x) { return std::forward<T...>(x);}
  template<typename ... T> std::tuple<T...> const &  as_tuple(std::tuple<T...> const & x) { return x;}
  template<typename ... T> std::tuple<T...> & as_tuple(std::tuple<T...> & x) { return x;}

  template<int ... pos, typename Opt, typename Target, bool B, bool IsConst, typename IT, typename ... Ms>
  gf_view< typename cart_prod_impl< triqs::tuple::filter_out_t<std::tuple<Ms...>, pos...>>::type ,Target, Opt,IsConst>
   partial_eval(gf_impl< cartesian_product<Ms...>, Target,Opt,B,IsConst> const & g, IT index) { 
    // meshes of the returned gf_view : just drop the mesh of the evaluated variables
    auto meshes_tuple_partial = triqs::tuple::filter_out<pos...>(g.mesh().components());
    // a view of the array of g, with the dimension sizeof...(Ms)
    auto arr = reinterpret_linear_array(g.mesh(),g.data()); // NO the second () forces a view
    // now rebuild a tuple of the size sizeof...(Ms), containing the indices and range at the position of evaluated variables.
    auto arr_args = triqs::tuple::inverse_filter<sizeof...(Ms),pos...>(as_tuple(index), arrays::range());
    // from it, we make a slice of the array of g, corresponding to the data of the returned gf_view
    auto arr2 =  triqs::tuple::apply(arr, arr_args);
    // finally, we build the view on this data. 
    using r_t = gf_view< cart_prod< triqs::tuple::filter_out_t<std::tuple<Ms...>, pos...>> ,Target, Opt,IsConst>;
    return r_t{ rm_tuple_of_size_one(meshes_tuple_partial), arr2,  typename r_t::singularity_non_view_t{}, typename r_t::symmetry_t{} };
   }

  /// ---------------------------  curry  ---------------------------------
  // curry<0>(g) returns : x-> y... -> g(x,y...)
  // curry<1>(g) returns : y-> x,z... -> g(x,y,z...)
  
  // to adapt the partial_eval as a polymorphic lambda (replace by a lambda in c++14)
  template<typename Gview, int ... pos> struct curry_polymorphic_lambda { 
   Gview g;
   template<typename ...I> auto operator()(I ... i) const DECL_AND_RETURN(partial_eval<pos...>(g,std::make_tuple(i...)));
   friend int get_shape(curry_polymorphic_lambda const&) { return 0;}// no shape here, but needed for compilation
   //void resize(int){}
  };

  // curry function ...
  template <int... pos, typename Target, typename Opt, bool IsConst, typename... Ms>
  gf_view<cart_prod<triqs::tuple::filter_t<std::tuple<Ms...>, pos...>>,
                lambda_valued<curry_polymorphic_lambda<gf_view<cartesian_product<Ms...>, Target, Opt,IsConst>, pos...>>, Opt, IsConst>
  curry(gf_view<cartesian_product<Ms...>, Target, Opt, IsConst> g) {
    // pick up the meshed corresponding to the curryed variables
    auto meshes_tuple = triqs::tuple::filter<pos...>(g.mesh().components());
    // building the view
    return {rm_tuple_of_size_one(meshes_tuple),curry_polymorphic_lambda<gf_view<cartesian_product<Ms...>, Target,Opt,IsConst>, pos ...>{g}, nothing(), nothing()}; 
    //using m_t = gf_mesh< cart_prod< triqs::tuple::filter_t<std::tuple<Ms...>,pos...>>>; 
    //return {triqs::tuple::apply_construct<m_t>(meshes_tuple),curry_polymorphic_lambda<gf_view<cartesian_product<Ms...>, Target,Opt>, pos ...>{g}, nothing(), nothing()}; 
   };
  
  template <int... pos, typename Target, typename Opt, typename... Ms>
  auto curry(gf<cartesian_product<Ms...>, Target, Opt> & g) DECL_AND_RETURN(curry<pos...>(g()));
  
  template <int... pos, typename Target, typename Opt, typename... Ms>
  auto curry(gf<cartesian_product<Ms...>, Target, Opt> const & g) DECL_AND_RETURN(curry<pos...>(g()));

 } // gf_implementation
 using gfs_implementation::partial_eval;
 using gfs_implementation::curry;
}}
#endif


