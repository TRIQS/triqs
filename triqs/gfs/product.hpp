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
#include "./tail_zero.hpp"

namespace triqs {
namespace gfs {

 template <typename... Ms> struct cartesian_product {
  using type = std::tuple<Ms...>;
  static constexpr size_t size = sizeof...(Ms);
 };

 // template <typename... Ms>  constexpr int get_n_variables(cartesian_product<Ms...>) { return sizeof...(Ms);}
 template <typename... Ms> struct get_n_variables<cartesian_product<Ms...>> {
  static const int value = sizeof...(Ms);
 };

 // use alias
 template <typename... Ms> struct cartesian_product<std::tuple<Ms...>> : cartesian_product<Ms...> {};

 // the mesh is simply a cartesian product
 template <typename... Ms> struct gf_mesh<cartesian_product<Ms...>> : mesh_product<gf_mesh<Ms>...> {
  using B = mesh_product<gf_mesh<Ms>...>;
  gf_mesh() = default;
  gf_mesh(gf_mesh<Ms>... ms) : B{std::move(ms)...} {}
 };

 // special case : we need to pop for all the variables
 template <typename... Ms, typename Target, typename Singularity, typename Evaluator, bool IsView, bool IsConst>
 auto get_target_shape(gf_impl<cartesian_product<Ms...>, Target, Singularity, Evaluator, IsView, IsConst> const &g) {
  return g.data().shape().template front_mpop<sizeof...(Ms)>();
 }

 // The default singularity, for each Variable.
 template <typename... Ms> struct gf_default_singularity<cartesian_product<Ms...>, scalar_valued> {
  using type = tail_zero;
 };
 template <typename... Ms> struct gf_default_singularity<cartesian_product<Ms...>, matrix_valued> {
  using type = tail_zero;
 };
 template <typename... Ms, int R> struct gf_default_singularity<cartesian_product<Ms...>, tensor_valued<R>> {
  using type = tail_zero;
 };

 // forward declaration, Cf m_tail
 template <typename Variable, typename... Args> auto evaluate(gf_const_view<Variable, tail> const &g, Args const &... args);
 template <typename Variable, typename... Args> auto evaluate(gf<Variable, tail> const &g, Args const &... args);
 template <typename Variable, typename... Args> auto evaluate(gf_view<Variable, tail> const &g, Args const &... args);

 namespace gfs_implementation {

  /// ---------------------------  data access  ---------------------------------

  template <typename... Ms>
  struct data_proxy<cartesian_product<Ms...>, scalar_valued> : data_proxy_array_multivar<std::complex<double>, sizeof...(Ms)> {};

  template <typename... Ms>
  struct data_proxy<cartesian_product<Ms...>, matrix_valued> : data_proxy_array_multivar_matrix_valued<std::complex<double>,
                                                                                                       2 + sizeof...(Ms)> {};

  template <int R, typename... Ms>
  struct data_proxy<cartesian_product<Ms...>, tensor_valued<R>> : data_proxy_array_multivar<std::complex<double>,
                                                                                            R + sizeof...(Ms)> {};

  // special case ? Or make a specific container....
  template <typename M0>
  struct data_proxy<cartesian_product<M0, imtime>, matrix_valued> : data_proxy_array_multivar_matrix_valued<double, 2 + 2> {};

  /// ---------------------------  hdf5 ---------------------------------

  // h5 name : name1_x_name2_.....
  template <typename S, typename... Ms> struct h5_name<cartesian_product<Ms...>, matrix_valued, S> {
   static std::string invoke() {
    return triqs::tuple::fold([](std::string a, std::string b) { return a + std::string(b.empty() ? "" : "_x_") + b; },
                              reverse(std::make_tuple(h5_name<Ms, matrix_valued, nothing>::invoke()...)), std::string());
   }
  };
  template <typename S, int R, typename... Ms>
  struct h5_name<cartesian_product<Ms...>, tensor_valued<R>, S> : h5_name<cartesian_product<Ms...>, matrix_valued, S> {
  };

  /// ---------------------------  evaluator ---------------------------------

  // forward declaration, cf curry
  template <int... pos, typename Variable, typename Target, typename Singularity, typename Evaluator, bool IsView, bool IsConst>
  auto curry(gf_impl<Variable, Target, Singularity, Evaluator, IsView, IsConst> const &g);

  // helper function : from a callable object F, return the expression of calling it with placeholders.
  template <typename F, size_t... Is> auto __make_lazy_call(F &f, std14::index_sequence<Is...>) {
   return f(clef::placeholder<Is>()...);
  }

  // ... to move in array lib ?
  inline dcomplex _make_from_shape(mini_vector<size_t, 0> const &) { return 0; }
  template <int R> arrays::array<dcomplex, R> _make_from_shape(mini_vector<size_t, R> const &sh) {
   auto r = arrays::array<dcomplex, R>{sh};
   r() = 0;
   return r;
  }

  using triqs::make_const_view;
  inline dcomplex make_const_view(dcomplex z) { return z; }

  // shortcut
  template <int n> using __int = std::integral_constant<int, n>;
  // template <bool B> using __bool = std::integral_constant<bool, B>;

  // now the multi d evaluator itself.
  template <typename Target, typename Sing, typename... Ms> struct evaluator<cartesian_product<Ms...>, Target, Sing> {

   template <typename G> evaluator(G *) {};

   static constexpr int arity = sizeof...(Ms);

   // dispatch the call :
   // Case 1 : if all points are mesh_point_t, we call directly the memory point
   // Case 2 : call evaluator
   template <typename G, typename... Args> decltype(auto) operator()(G const *g, Args &&... args) const {
    static_assert(sizeof...(Args) == arity, "Wrong number of arguments in gf evaluation");
    static constexpr int allmp =
        std::is_same<std::tuple<std14::decay_t<Args>...>, std::tuple<__no_cast<typename gf_mesh<Ms>::mesh_point_t>...>>::value;
    static constexpr int _t = std::is_same<Sing, nothing>::value ? 2 : (std::is_same<Sing, tail_zero>::value ? 3 : 1);
    // _int : 0 -> mp, >0 -> no mp, 1 : tail, 2 : nothing, 3 : tail_zero
    return __call(__int<(1 - allmp) * _t>(), g, std::forward<Args>(args)...);
   }

   private:
   template <int N> using _get_Ms = std14::tuple_element_t<N, std::tuple<Ms...>>;
   using mesh_t = gf_mesh<cartesian_product<Ms...>>;

   // Case 1 : if all points are mesh_point_t, we call directly the memory point
   template <typename G, typename... MP> decltype(auto) __call(__int<0>, G const *g, __no_cast<MP> const &... p) const {
    return g->get_from_linear_index(p.value.linear_index()...);
   }

   // Case 2 : the real evaluator
   // We evaluate the argument recursively, when done we get an expression
   // made of g( mesh_points...), which is then back to case 1
   // This case is subdivided into 3, depending on the type of the singularity.
   
   // First : a normal singularity, we just evaluate it
   // The issue is to get the same return type for both return 
   template <typename G, typename... Args> auto __call(__int<1>, G const *g, Args &&... args) const {
    using rt = std14::decay_t<decltype(make_const_view(__call1(g, std::forward<Args>(args)...)))>;
    if (g->mesh().is_within_boundary(args...))
     return make_const_view(__call1(g, std::forward<Args>(args)...));
    else
     return rt{evaluate(g->singularity(), args...)};
   }

   // In this case, the tail is 0, we need to build the proper tensor, init to 0 and correct size
   // and return the same view type in both. 
   template <typename G, typename... Args> auto __call(__int<3>, G const *g, Args &&... args) const {
    using rt = std14::decay_t<decltype(make_const_view(__call1(g, std::forward<Args>(args)...)))>;
    if (g->mesh().is_within_boundary(args...))
     return make_const_view(__call1(g, std::forward<Args>(args)...));
    else
     return rt{_make_from_shape(get_target_shape(*g))};
   }

   // No singularity : if we are not in the mesh, we throw.
   template <typename G, typename... Args> auto __call(__int<2>, G const *g, Args &&... args) const {
    if (!g->mesh().is_within_boundary(args...)) TRIQS_RUNTIME_ERROR << "Trying to evaluate out the mesh, and Singularity is None";
    return __call1(g, std::forward<Args>(args)...);
   }

   // to save code, impl details.
   template <typename G, typename... Args> auto __call1(G const *g, Args &&... args) const {
    return __partial_eval(__int<arity - 1>(), __make_lazy_call(*g, std14::make_index_sequence<arity>()),
                                          g->mesh(), std::tie(args...));
   }
   // implementation
   template <int N, typename Expr, typename ArgTuple>
   auto __partial_eval(__int<N>, Expr &&expr, mesh_t const &mesh, ArgTuple const &tu) const {
    return __partial_eval(__int<N - 1>(),
                          evaluator_of_clef_expression<_get_Ms<N>>()(std::forward<Expr>(expr), clef::placeholder<N>(),
                                                                     std::get<N>(mesh), std::get<N>(tu)),
                          mesh, tu);
   }
   // end recursion : -1 : do nothing
   template <typename Expr, typename ArgTuple>
   auto __partial_eval(__int<-1>, Expr &&expr, mesh_t const &mesh, ArgTuple const &tu) const {
    return std::forward<Expr>(expr);
   }
  };
 } // gf_implementation
}
}

