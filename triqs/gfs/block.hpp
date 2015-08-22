/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2015 by M. Ferrero, O. Parcollet
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
#include "./gf.hpp"
#include "./meshes/discrete.hpp"
#include <iterator>

namespace triqs {
namespace gfs {

 struct block_index {};
 using block_index2 = cartesian_product<block_index, block_index>;

 template <> struct gf_mesh<block_index> : discrete_mesh<discrete_domain> {
  using B = discrete_mesh<discrete_domain>;
  gf_mesh() = default;
  gf_mesh(int s) : B(s) {}
  gf_mesh(discrete_domain const &d) : B(d) {}
  gf_mesh(std::vector<std::string> const &s) : B(s) {}
  gf_mesh(std::initializer_list<std::string> const &s) : B(s) {}
 };

 // -------------------------------  aliases  --------------------------------------------------

 template <typename... T> using block_gf = gf<block_index, gf<T...>>;
 template <typename... T> using block_gf_view = gf_view<block_index, gf<T...>>;
 template <typename... T> using block_gf_const_view = gf_const_view<block_index, gf<T...>>;

 /// ---------------------------  trait to identify BlockGf ---------------------------------

 // Is G a block_gf, block_gf_view, block_gf_const_view
 // is_block_gf_or_view<G> is true iif G is a block_gf or block2_gf
 // is_block_gf_or_view<G,1> is true iff G is a block_gf
 // is_block_gf_or_view<G,2> is true iff G is a block2_gf
 //
 template <typename G, int n = 0> struct is_block_gf_or_view;

 template <typename G> struct is_block_gf_or_view<G, 1> : is_gf_or_view<G, block_index>{};
 template <typename G> struct is_block_gf_or_view<G, 2> : is_gf_or_view<G, block_index2>{};
 template <typename G>
 struct is_block_gf_or_view<G, 0> : std::integral_constant<bool, is_block_gf_or_view<G, 1>::value ||
                                                                     is_block_gf_or_view<G, 2>::value> {};

 /// ---------------------------  hdf5 ---------------------------------

 template <typename Target> struct gf_h5_name<block_index, Target, nothing> {
  static std::string invoke() { return "BlockGf"; }
 };

 template <typename Target> struct gf_h5_rw<block_index, Target, nothing, void> {

  static void write(h5::group gr, gf_const_view<block_index, Target> g) {
   for (size_t i = 0; i < g.mesh().size(); ++i) h5_write(gr, g.mesh().domain().names()[i], g._data[i]);
   h5_write(gr, "block_names", g.mesh().domain().names());
  }

  template <typename G> static void read(h5::group gr, G &g) {
   auto block_names = h5::h5_read<std::vector<std::string>>(gr, "block_names");
   g._mesh = gf_mesh<block_index>(block_names);
   // auto check_names = gr.get_all_subgroup_names();
   // sort both and check ?
   g._data.resize(g._mesh.size());
   for (size_t i = 0; i < g.mesh().size(); ++i) h5_read(gr, g.mesh().domain().names()[i], g._data[i]);
  }
 };

 /// ---------------------------  data access  ---------------------------------

 template <typename Target>
 struct gf_data_proxy<block_index, Target, void> : data_proxy_vector<typename regular_type_if_exists_else_type<Target>::type> {};

 // -------------------------------   Factories  --------------------------------------------------

 template <typename Target> struct gf_data_factory<block_index, Target, nothing> {
  using mesh_t = gf_mesh<block_index>;
  using gf_t = gf<block_index, Target>;
  using gf_view_t = gf_view<block_index, Target>;
  using aux_t = nothing;
  struct target_shape_t {};

  static typename gf_t::data_t make(mesh_t const &m, target_shape_t, aux_t) { return std::vector<Target>(m.size()); }
 };

 // -------------------------------   Free Factories for regular type  --------------------------------------------------

 // from a number and a gf to be copied
 template <typename... A> block_gf<A...> make_block_gf(int n, gf<A...> const &g) {
  auto V = std::vector<gf<A...>>{};
  for (int i = 0; i < n; ++i) V.push_back(g);
  return {{n}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // from a vector of gf (moving directly)
 template <typename... A> block_gf<A...> make_block_gf(std::vector<gf<A...>> V) {
  int s = V.size(); // DO NOT use V.size in next statement, the V is moved and the order of arg. evaluation is undefined.
  return {{s}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // from a init list of GF with the correct type
 template <typename... A> block_gf<A...> make_block_gf(std::initializer_list<gf<A...>> const &V) {
  return {{int(V.size())}, V, nothing{}, nothing{}, nothing{}};
 }

 // from vector<string>
 //template <typename... A> block_gf<A...> make_block_gf(std::vector<std::string> const & block_names) {
 // return {{block_names}, std::vector<gf<A...>>(block_names.size()), nothing{}, nothing{}, nothing{}};
 //}

 // from vector<string> and a gf to be copied
 template <typename... A> block_gf<A...> make_block_gf(std::vector<std::string> const & block_names, gf<A...> const &g) {
  auto V = std::vector<gf<A...>>{};
  for (int i = 0; i < block_names.size(); ++i) V.push_back(g);
  return {{block_names}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // from mesh, vector<gf>
 template <typename... A> block_gf<A...> make_block_gf(gf_mesh<block_index> m, std::vector<gf<A...>> V) {
  if (m.size() != V.size())
   TRIQS_RUNTIME_ERROR << "size mismatch between the mesh and the data vector !";
  return {std::move(m), std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // from vector<string>, vector<gf>
 template <typename... A> block_gf<A...> make_block_gf(std::vector<std::string> const & block_names, std::vector<gf<A...>> V) {
  if (block_names.size() != V.size())
   TRIQS_RUNTIME_ERROR << "make_block_gf(vector<string>, vector<gf>) : the two vectors do not have the same size !";
  return {{block_names}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // from vector<string>, init_list<GF>
 template <typename... A>
 block_gf<A...> make_block_gf(std::vector<std::string> block_names, std::initializer_list<gf<A...>> const &V) {
  if (block_names.size() != V.size()) TRIQS_RUNTIME_ERROR << "make_block_gf(vector<string>, init_list) : size mismatch !";
  return {{block_names}, V, nothing{}, nothing{}, nothing{}};
 }

 // -------------------------------   Free Factories for view type  --------------------------------------------------

 template <typename G0, typename... G>
 gf_view<block_index, typename std14::decay_t<G0>::regular_type> make_block_gf_view(G0 &&g0, G &&... g) {
  auto V = std::vector<typename std14::decay_t<G0>::view_type>{std::forward<G0>(g0), std::forward<G>(g)...};
  return {{int(V.size())}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 template <typename GF> gf_view<block_index, typename GF::regular_type> make_block_gf_view(std::vector<GF> V) {
  int s = V.size();
  return {{s}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // from mesh and data vector
 template <typename GF>
 gf_view<block_index, typename GF::regular_type> make_block_gf_view(gf_mesh<block_index> m, std::vector<GF> V) {
  return {std::move(m), std::move(V), nothing{}, nothing{}, nothing{}};
 }
 
 // from block_names and data vector
 template <typename GF>
 gf_view<block_index, typename GF::regular_type> make_block_gf_view(std::vector<std::string> block_names,
                                                                                std::vector<GF> V) {
  return {{std::move(block_names)}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // -------------------------------   Free Factories for view type  --------------------------------------------------

 template <typename GF>
 gf_const_view<block_index, typename GF::regular_type> make_block_gf_const_view(std::vector<GF> V) {
  int s = V.size();
  return {{s}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // from mesh and data vector
 template <typename GF>
 gf_const_view<block_index, typename GF::regular_type> make_block_gf_const_view(std::vector<std::string> block_names,
                                                                                            std::vector<GF> V) {
  return {{std::move(block_names)}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // from block_names and data vector
 template <typename GF>
 gf_const_view<block_index, typename GF::regular_type> make_block_gf_const_view(gf_mesh<block_index> m, std::vector<GF> V) {
  return {std::move(m), std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // -------------------------------   Free functions   --------------------------------------------------

 /// The number of blocks
 template <typename G> TYPE_ENABLE_IF(int, is_block_gf_or_view<G,1>) n_blocks(G const &g) { return g.mesh().size(); }

 /// The vector of names of the blocks
 template <typename G> TYPE_ENABLE_IF(std::vector<std::string> const &, is_block_gf_or_view<G,1>) get_block_names(G const &g) {
  return g.mesh().domain().names();
 }

}
}

