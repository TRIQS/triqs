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
#include "./tools.hpp"
#include "./gf.hpp"
#include "./meshes/product.hpp"
#include "./evaluators.hpp"

namespace triqs {
namespace gfs {

 template <typename... Ms> struct cartesian_product {
  using type = std::tuple<Ms...>;
  static constexpr size_t size = sizeof...(Ms);
 };

 //template <typename... Ms>  constexpr int get_n_variables(cartesian_product<Ms...>) { return sizeof...(Ms);} 
 template <typename... Ms>  struct get_n_variables<cartesian_product<Ms...>> { static const int value = sizeof...(Ms);}; 
 
 // use alias
 template <typename... Ms> struct cartesian_product<std::tuple<Ms...>> : cartesian_product<Ms...> {};

 /// TODO : Put inheriting constructor, simpler...
 // the mesh is simply a cartesian product
 template <typename Opt, typename... Ms> struct gf_mesh<cartesian_product<Ms...>, Opt> : mesh_product<gf_mesh<Ms, Opt>...> {
  // using mesh_product< gf_mesh<Ms,Opt> ... >::mesh_product< gf_mesh<Ms,Opt> ... > ;
  using B = mesh_product<gf_mesh<Ms, Opt>...>;
  gf_mesh() = default;
  gf_mesh(gf_mesh<Ms, Opt>... ms) : B{std::move(ms)...} {}
 };

 namespace gfs_implementation {

  /// ---------------------------  data access  ---------------------------------

  template <typename Opt, typename... Ms>
  struct data_proxy<cartesian_product<Ms...>, scalar_valued, Opt> : data_proxy_array_multivar<std::complex<double>,
                                                                                              sizeof...(Ms)> {};

  template <typename Opt, typename... Ms>
  struct data_proxy<cartesian_product<Ms...>, matrix_valued, Opt> : data_proxy_array_multivar_matrix_valued<std::complex<double>,
                                                                                                            2 + sizeof...(Ms)> {};

  template <int R, typename Opt, typename... Ms>
  struct data_proxy<cartesian_product<Ms...>, tensor_valued<R>, Opt> : data_proxy_array_multivar<std::complex<double>,
                                                                                                 R + sizeof...(Ms)> {};

  // special case ? Or make a specific container....
  template <typename Opt, typename M0>
  struct data_proxy<cartesian_product<M0, imtime>, matrix_valued, Opt> : data_proxy_array_multivar_matrix_valued<double, 2 + 2> {
  };

  /// ---------------------------  hdf5 ---------------------------------

  // h5 name : name1_x_name2_.....
  template <typename Opt, typename S, typename... Ms> struct h5_name<cartesian_product<Ms...>, matrix_valued, S, Opt> {
   static std::string invoke() {
    return triqs::tuple::fold([](std::string a, std::string b) { return a + std::string(b.empty() ? "" : "_x_") + b; },
                              reverse(std::make_tuple(h5_name<Ms, matrix_valued, nothing, Opt>::invoke()...)), std::string());
   }
  };
  template <typename Opt, typename S, int R, typename... Ms>
  struct h5_name<cartesian_product<Ms...>, tensor_valued<R>, S, Opt> : h5_name<cartesian_product<Ms...>, matrix_valued, S, Opt> {};

  /// ---------------------------  evaluator ---------------------------------

  /**
   * This the multi-dimensional evaluator.
   * It combine the evaluator of each components, as long as they are  a linear form
   * eval(g, x) = \sum_i w_i g( n_i(x)) , with w some weight and n_i some points on the grid.
   * Mathematically, it is written as (example of evaluating g(x1,x2,x3,x4)).
   * Notation : eval(X)  : g -> g(X)
   * eval(x1,x2,x3,x4) (g) = eval (x1) ( binder ( g, (),  (x2,x3,x4)) )
   * binder( g, (), (x2,x3,x4)) (p1) = eval(x2)(binder (g,(p1),(x3,x4)))
   * binder( g, (p1), (x3,x4))  (p2) = eval(x3)(binder (g,(p1,p2),(x4)))
   * binder( g, (p1,p2), (x4))  (p3) = eval(x4)(binder (g,(p1,p2,p3),()))
   * binder( g, (p1,p2,p3),())  (p4) = g[p1,p2,p3,p4]
   *
   * p_i are points on the grids, x_i points in the domain.
   *
   * Unrolling the formula gives (for 2 variables, with 2 points interpolation)
   * eval(xa,xb) (g) = eval (xa) ( binder ( g, (),  (xb)) ) =
   *    w_1(xa)  binder ( g, (),  (xb))( n_1(xa)) + w_2(xa)  binder ( g, (), (xb))( n_2(xa))
   *  =  w_1(xa) ( eval(xb)(  binder ( g, (n_1(xa) ),  ())))  + 1 <-> 2
   *  =  w_1(xa) ( W_1(xb) *  binder ( g, (n_1(xa) ),  ())(N_1(xb)) + 1<->2 )  + 1 <-> 2
   *  =  w_1(xa) ( W_1(xb) *  g[n_1(xa), N_1(xb)] + 1<->2 )  + 1 <-> 2
   *  =  w_1(xa) ( W_1(xb) *  g[n_1(xa), N_1(xb)] + W_2(xb) *  g[n_1(xa), N_2(xb)] )  + 1 <-> 2
   *  which is the expected formula
   */
  // implementation : G = gf, Tn : tuple of n points, Ev : tuple of evaluators (the evals functions),
  // pos = counter from #args-1 =>0
  // NB : the tuple is build in reverse with respect to the previous comment.
  template <typename G, typename Tn, typename Ev, int pos> struct binder;

  template <int pos, typename G, typename Tn, typename Ev> binder<G, Tn, Ev, pos> make_binder(G const *g, Tn tn, Ev const &ev) {
   return {g, std::move(tn), ev};
  }

  template <typename G, typename Tn, typename Ev, int pos> struct binder {
   G const *g;
   Tn tn;
   Ev const &evals;
   template <size_t... Is> decltype(auto) impl(size_t p, std14::index_sequence<Is...>) const {
    return std::get<pos>(evals)(make_binder<pos - 1>(g, std::make_tuple(p, std::get<Is>(tn)...), evals));
   }
   decltype(auto) operator()(size_t p) const { return impl(p, std14::make_index_sequence<std::tuple_size<Tn>::value>()); }
  };

  template <typename G, typename Tn, typename Ev> struct binder<G, Tn, Ev, -1> {
   G const *g;
   Tn tn;
   Ev const &evals;
   template <size_t... Is> decltype(auto) impl(size_t p, std14::index_sequence<Is...>) const {
    return g->get_from_linear_index(p, std::get<Is>(tn)...);
   }
   decltype(auto) operator()(size_t p) const { return impl(p, std14::make_index_sequence<std::tuple_size<Tn>::value>()); }
  };

  // now the multi d evaluator itself.
  template <typename Target, typename Opt, typename... Ms> struct evaluator<cartesian_product<Ms...>, Target, nothing, Opt> {
   static constexpr int arity = sizeof...(Ms);
   mutable std::tuple<evaluator_fnt_on_mesh<Ms>...> evals;

   template <typename G, typename... Args> decltype(auto) operator()(G const *g, Args &&... args) const {
    static constexpr int R = sizeof...(Args);
    // build the evaluators, as a tuple of ( evaluator<Ms> ( mesh_component, args))
    auto l = [](auto &a, auto &b, auto &c) { a = std14::decay_t<decltype(a)>{b, c}; };
    triqs::tuple::for_each_zip(l, evals, g->mesh().components(), std::make_tuple(args...));
    return std::get<R - 1>(evals)(make_binder<R - 2>(g, std::make_tuple(), evals));
   }
  };

 } // gf_implementation
}
}

