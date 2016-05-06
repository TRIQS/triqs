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
#include "./gf_classes.hpp"
#include "./meshes/product.hpp"
#include "./singularity/tail_zero.hpp"

namespace triqs {
namespace gfs {

 // special case : we need to pop for all the variables
 template <typename... Ms, typename T, typename S, typename E>
 auto get_target_shape(gf_const_view<cartesian_product<Ms...>, T, S, E> const &g) {
  return g.data().shape().template front_mpop<sizeof...(Ms)>();
 }
 template <typename... Ms, typename T, typename S, typename E>
 auto get_target_shape(gf_view<cartesian_product<Ms...>, T, S, E> const &g) {
  return get_target_shape(g());
 }
 template <typename... Ms, typename T, typename S, typename E>
 auto get_target_shape(gf<cartesian_product<Ms...>, T, S, E> const &g) {
  return get_target_shape(g());
 }

 // The default singularity, for each Mesh.
 template <typename... Ms> struct gf_default_singularity<cartesian_product<Ms...>, scalar_valued> {
  using type = tail_zero<dcomplex>;
 };
 template <typename... Ms> struct gf_default_singularity<cartesian_product<Ms...>, matrix_valued> {
  using type = tail_zero<matrix<dcomplex>>;
 };
 template <typename... Ms, int R> struct gf_default_singularity<cartesian_product<Ms...>, tensor_valued<R>> {
  using type = tail_zero<array<dcomplex, R>>;
 };

 // forward declaration, Cf m_tail
 template <typename Mesh, typename... Args> auto evaluate(gf_const_view<Mesh, tail> const &g, Args const &... args);
 template <typename Mesh, typename... Args> auto evaluate(gf<Mesh, tail> const &g, Args const &... args);
 template <typename Mesh, typename... Args> auto evaluate(gf_view<Mesh, tail> const &g, Args const &... args);

 /// ---------------------------  data access  ---------------------------------

 template <typename... Ms>
 struct gf_data_proxy<cartesian_product<Ms...>, scalar_valued> : data_proxy_array_multivar<std::complex<double>, sizeof...(Ms)> {
 };

 template <typename... Ms>
 struct gf_data_proxy<cartesian_product<Ms...>, matrix_valued> : data_proxy_array_multivar_matrix_valued<std::complex<double>,
                                                                                                         2 + sizeof...(Ms)> {};

 template <int R, typename... Ms>
 struct gf_data_proxy<cartesian_product<Ms...>, tensor_valued<R>> : data_proxy_array_multivar<std::complex<double>,
                                                                                              R + sizeof...(Ms)> {};

 /// ---------------------------  hdf5 ---------------------------------

 // h5 name : name1_x_name2_.....
 template <typename S, typename... Ms> struct gf_h5_name<cartesian_product<Ms...>, matrix_valued, S> {
  static std::string invoke() {
   return triqs::tuple::fold([](std::string a, std::string b) { return a + std::string(b.empty() ? "" : "_x_") + b; },
                             reverse(std::make_tuple(gf_h5_name<Ms, matrix_valued, nothing>::invoke()...)), std::string());
  }
 };
 template <typename S, int R, typename... Ms>
 struct gf_h5_name<cartesian_product<Ms...>, tensor_valued<R>, S> {
  static std::string invoke() {
   std::string name = triqs::tuple::fold([](std::string a, std::string b) { return a + std::string(b.empty() ? "" : "_x_") + b; }, reverse(std::make_tuple(gf_h5_name<Ms, matrix_valued, nothing>::invoke()...)), std::string());
   name += "Tv"+std::to_string(R); 
   return name;
  }
 
 };

 /// ---------------------------  gf_evaluator ---------------------------------

 using triqs::make_const_view;
 inline dcomplex make_const_view(dcomplex z) { return z; }

 // now the multi d gf_evaluator itself.
 template <typename Target, typename Sing, typename... Ms> struct gf_evaluator<cartesian_product<Ms...>, Target, Sing> {

  static constexpr int arity = sizeof...(Ms); // METTRE ARITY DANS LA MESH !
  template <typename G> gf_evaluator(G *) {};

  template <typename G, typename... Args> const auto operator()(G const &g, Args &&... args) const {
   static_assert(sizeof...(Args) == arity, "Wrong number of arguments in gf evaluation");
   if (g.mesh().is_within_boundary(args...))
    return make_const_view(g.mesh().evaluate(typename G::mesh_t::default_interpol_policy{}, g, std::forward<Args>(args)...));
   using rt = std14::decay_t<decltype(
       make_const_view(g.mesh().evaluate(typename G::mesh_t::default_interpol_policy{}, g, std::forward<Args>(args)...)))>;
   return rt{evaluate(g.singularity(), args...)};
  }
 };

 // special case when the tail is nothing
 template <typename Target, typename... Ms> struct gf_evaluator<cartesian_product<Ms...>, Target, nothing> {

  static constexpr int arity = sizeof...(Ms); // METTRE ARITY DANS LA MESH !
  template <typename G> gf_evaluator(G *) {};

  template <typename G, typename... Args> auto operator()(G const &g, Args &&... args) const {
   static_assert(sizeof...(Args) == arity, "Wrong number of arguments in gf evaluation");
   if (!g.mesh().is_within_boundary(args...)) TRIQS_RUNTIME_ERROR << "Evaluation out of the mesh";
   return g.mesh().evaluate(typename G::mesh_t::default_interpol_policy{}, g, std::forward<Args>(args)...);
  }
 };
}
}

