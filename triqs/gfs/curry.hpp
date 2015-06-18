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
#pragma once
#include "./product.hpp"
namespace triqs { namespace gfs { 

 template<typename F> struct lambda_valued {};

 template <typename Var, typename M, typename L> gf_view<Var, lambda_valued<L>> make_gf_view_lambda_valued(M m, L l) {
  return {std::move(m), l, nothing(), nothing(), {}};
 }

 namespace gfs_implementation { 
  
  /// ---------------------------  data access  ---------------------------------

  template <typename F, typename M> struct data_proxy<M, lambda_valued<F>> : data_proxy_lambda<F, false> {};
  template <typename F, typename... Ms>
  struct data_proxy<cartesian_product<Ms...>, lambda_valued<F>> : data_proxy_lambda<F, true> {};

  /// ---------------------------  Factories ---------------------------------

  template<typename F, typename ... Ms>
   struct data_factory<cartesian_product<Ms...>, lambda_valued<F>, nothing> {};

  /// ---------------------------  partial_eval ---------------------------------
  // partial_eval<0> (g, 1) returns :  x -> g(1,x)
  // partial_eval<1> (g, 3) returns :  x -> g(x,3)

  // a technical trait: from a tuple of mesh, return the mesh (either M if it is a tuple of size 1, or the corresponding cartesian_product<M..>).
  template<typename ... Ms> struct cart_prod_impl;
  template<typename ... Ms> struct cart_prod_impl<std::tuple<Ms...>> { using type = cartesian_product<Ms...>;};
  template<typename M> struct cart_prod_impl<std::tuple<M>> { using type = M;};
  template<typename ... Ms> using cart_prod = typename cart_prod_impl<Ms...>::type;

  // The implementation (can be overloaded for some types), so put in a struct to have partial specialization
  template <typename Variable, typename Target, typename Singularity, typename Evaluator, bool IsConst> struct partial_eval_impl;

  // The user function when the indices is already a linear index.
  template <int... pos, typename Variable, typename Target, typename Singularity, typename Evaluator, bool C, typename T>
  auto partial_eval_linear_index(gf_view<Variable, Target, Singularity, Evaluator, C> g, T const& x) {
   return partial_eval_impl<Variable, Target, Singularity, Evaluator, C>::template invoke<pos...>(g(), x);
  }

  template <int... pos, typename Variable, typename Target, typename Singularity, typename Evaluator, typename T>
  auto partial_eval_linear_index(gf<Variable, Target, Singularity, Evaluator>& g, T const& x) {
   return partial_eval_impl<Variable, Target, Singularity, Evaluator, false>::template invoke<pos...>(g(), x);
  }

  template <int... pos, typename Variable, typename Target, typename Singularity, typename Evaluator, typename T>
  auto partial_eval_linear_index(gf<Variable, Target, Singularity, Evaluator> const& g, T const& x) {
   return partial_eval_impl<Variable, Target, Singularity, Evaluator, true>::template invoke<pos...>(g(), x);
  }

  /// ------------------------------------------------------------
  // a little helper function that takes a cartesian product mesh m, a x ... pack and return the tuple of m[i](x[i])...
  // if true_type. If false_type do nothing
  template <typename Ms, typename Xs, size_t... Is> auto _zip_mesh_x(Ms const& ms, Xs const& xs, std14::index_sequence<Is...>) {
   return std::make_tuple(std::get<Is>(ms).index_to_linear(std::get<Is>(xs))...);
  }

  template <int... pos, typename G, typename... T> auto _indices_to_linear(G const& g, T&&... x) {
   auto meshes_tuple_evaluated = triqs::tuple::filter<pos...>(g.mesh().components());
   // x are supposed to be index_t, and are converted to a linear index by their respective mesh
   return _zip_mesh_x(meshes_tuple_evaluated, std::make_tuple(x...), std14::make_index_sequence<sizeof...(T)>{});
  }

  // top level user function 
  template <int... pos, typename G, typename... T> auto partial_eval(G&& g, T&&... x) {
   return partial_eval_linear_index<pos...>(std::forward<G>(g), _indices_to_linear<pos...>(g, std::forward<T>(x)...));
  }

  /// ---------------------------  curry  ---------------------------------
  // curry<0>(g) returns : x-> y... -> g(x,y...)
  // curry<1>(g) returns : y-> x,z... -> g(x,y,z...)

  // The implementation (can be overloaded for some types)
  template <int... pos, typename Target, typename Singularity, typename Evaluator, bool IsConst, typename... Ms>
  auto curry_impl(gf_view<cartesian_product<Ms...>, Target, Singularity, Evaluator, IsConst> g) {
   // pick up the meshed corresponding to the curryed variables
   auto meshes_tuple = triqs::tuple::filter<pos...>(g.mesh().components());
   using var_t = cart_prod<triqs::tuple::filter_t<std::tuple<Ms...>, pos...>>;
   auto m = triqs::tuple::apply_construct<gf_mesh<var_t>>(meshes_tuple);
   auto l = [g](auto&&... x) { return partial_eval_linear_index<pos...>(g, std::make_tuple(x...)); };
   return make_gf_view_lambda_valued<var_t>(m, l);
  };

  // The user function
  template <int... pos, typename Variable, typename Target, typename Singularity, typename Evaluator, bool IsView, bool IsConst>
  auto curry(gf_impl<Variable, Target, Singularity, Evaluator, IsView, IsConst> const &g) {
   return curry_impl<pos...>(g());
  }
  template <int... pos, typename Variable, typename Target, typename Singularity, typename Evaluator, bool IsConst>
  auto curry(gf_view<Variable, Target, Singularity, Evaluator, IsConst> g) {
   return curry_impl<pos...>(g());
  }
  template <int... pos, typename Variable, typename Target, typename Singularity, typename Evaluator>
  auto curry(gf<Variable, Target, Singularity, Evaluator>& g) {
   return curry_impl<pos...>(g());
  }
  template <int... pos, typename Variable, typename Target, typename Singularity, typename Evaluator>
  auto curry(gf<Variable, Target, Singularity, Evaluator> const& g) {
   return curry_impl<pos...>(g());
  }

  //---------------------------------------------

  // A generic impl. for cartesian product
  template <typename Target, typename Singularity, typename Evaluator, bool IsConst, typename... Ms>
  struct partial_eval_impl<cartesian_product<Ms...>, Target, Singularity, Evaluator, IsConst> {

   template <int... pos, typename XTuple>
   static auto invoke(gf_view<cartesian_product<Ms...>, Target, Singularity, Evaluator, IsConst> g, XTuple const& x_tuple) {
    using var_t = cart_prod<triqs::tuple::filter_out_t<std::tuple<Ms...>, pos...>>;
    // meshes of the returned gf_view : just drop the mesh of the evaluated variables
    auto meshes_tuple_partial = triqs::tuple::filter_out<pos...>(g.mesh().components());
    // The mesh of the resulting function
    auto m = triqs::tuple::apply_construct<gf_mesh<var_t>>(meshes_tuple_partial);
    // rebuild a tuple of the size sizeof...(Ms), containing the linear indices and range at the position of evaluated variables.
    auto arr_args = triqs::tuple::inverse_filter<sizeof...(Ms), pos...>(x_tuple, arrays::range());
    // from it, we make a slice of the array of g, corresponding to the data of the returned gf_view
    auto arr2 = triqs::tuple::apply(g.data(), std::tuple_cat(arr_args, std::make_tuple(arrays::ellipsis{})));
    // We now also partial_eval the singularity
    auto singv = partial_eval_linear_index<pos...>(g.singularity(), x_tuple);
    using r_sing_t = typename decltype(singv)::regular_type;
    // finally, we build the view on this data.
    using r_t = gf_view<var_t, Target, r_sing_t, void, IsConst>;
    return r_t{m, arr2, singv, {}, {}};
   }
  };

 } // gf_implementation
 using gfs_implementation::partial_eval;
 using gfs_implementation::partial_eval_linear_index;
 using gfs_implementation::curry;
}}


