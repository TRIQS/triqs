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
#pragma once
#include "./tools.hpp"
#include "./gf.hpp"
#include "./local/tail.hpp"
#include "./meshes/discrete.hpp"

namespace triqs {
namespace gfs {

 struct block_index {};

 template <typename Opt> struct gf_mesh<block_index, Opt> : discrete_mesh<discrete_domain> {
  using B = discrete_mesh<discrete_domain>;
  gf_mesh() = default;
  gf_mesh(int s) : B(s) {}
  gf_mesh(discrete_domain const &d) : B(d) {}
  gf_mesh(std::initializer_list<std::string> const &s) : B(s) {}
 };

 namespace gfs_implementation {

  /// ---------------------------  hdf5 ---------------------------------

  template <typename Target> struct h5_name<block_index, Target, nothing, void> {
   static std::string invoke() { return "BlockGf"; }
  };

  template <typename Target> struct h5_rw<block_index, Target, nothing, void> {

   static void write(h5::group gr, gf_const_view<block_index, Target, nothing, void> g) {
    for (size_t i = 0; i < g.mesh().size(); ++i) h5_write(gr, g.mesh().domain().names()[i], g._data[i]);
    // h5_write(gr,"symmetry",g._symmetry);
   }

   template <bool IsView> static void read(h5::group gr, gf_impl<block_index, Target, nothing, void, IsView, false> &g) {
    // does not work : need to read the block name and remake the mesh...
    g._mesh = gf_mesh<block_index>(gr.get_all_subgroup_names());
    g._data.resize(g._mesh.size());
    // if (g._data.size() != g._mesh.size()) TRIQS_RUNTIME_ERROR << "h5 read block gf : number of block mismatch";
    for (size_t i = 0; i < g.mesh().size(); ++i) h5_read(gr, g.mesh().domain().names()[i], g._data[i]);
    // h5_read(gr,"symmetry",g._symmetry);
   }
  };

  /// ---------------------------  data access  ---------------------------------

  template <typename Target>
  struct data_proxy<block_index, Target, void> : data_proxy_vector<typename regular_type_if_exists_else_type<Target>::type> {};

  // -------------------------------   Factories  --------------------------------------------------

  template <typename Target> struct factories<block_index, Target, nothing, void> {
   using mesh_t = gf_mesh<block_index>;
   using gf_t = gf<block_index, Target>;
   using gf_view_t = gf_view<block_index, Target>;
   using aux_t = nothing;

   struct target_shape_t {};

   static typename gf_t::data_t make_data(mesh_t const &m, target_shape_t, aux_t) { return std::vector<Target>(m.size()); }
   static typename gf_t::singularity_t make_singularity(mesh_t const &m, target_shape_t) {
    return {};
   }
  };

 } // gfs_implementation

 // -------------------------------  aliases  --------------------------------------------------

 template <typename... T> using block_gf = gf<block_index, gf<T...>>;
 template <typename... T> using block_gf_view = gf_view<block_index, gf<T...>>;
 template <typename... T> using block_gf_const_view = gf_const_view<block_index, gf<T...>>;

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

 /*
 // from a vector of gf : generalized to have a different type of gf in the vector (e.g. views...)
 template <typename Variable, typename Target, typename Singularity, typename Opt, typename GF2>
 block_gf<Variable, Target, Singularity, Opt> make_block_gf(std::vector<GF2> const &V) {
  auto V2 = std::vector<gf<Variable, Target, Singularity, Opt>>{};
  for (auto const &g : V) V2.push_back(g);
  return {{int(V.size())}, std::move(V2), nothing{}, nothing{}, nothing{}};
 }
*/

 // from a init list of GF with the correct type
 template <typename... A> block_gf<A...> make_block_gf(std::initializer_list<gf<A...>> const &V) {
  return {{int(V.size())}, V, nothing{}, nothing{}, nothing{}};
 }

 // from vector<string> and a gf to be copied
 template <typename... A> block_gf<A...> make_block_gf(std::vector<std::string> block_names, gf<A...> const &g) {
  auto V = std::vector<gf<A...>>{};
  for (int i = 0; i < block_names.size(); ++i) V.push_back(g);
  return {{block_names}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // from vector<string>, vector<gf>
 template <typename... A> block_gf<A...> make_block_gf(std::vector<std::string> block_names, std::vector<gf<A...>> V) {
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
 gf_view<block_index, typename std14::decay_t<G0>::view_type> make_block_gf_view(G0 &&g0, G &&... g) {
  auto V = std::vector<typename std14::decay_t<G0>::view_type>{std::forward<G0>(g0), std::forward<G>(g)...};
  return {{int(V.size())}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 template <typename GF> gf_view<block_index, typename GF::regular_type> make_block_gf_view_from_vector(std::vector<GF> V) {
  int s = V.size();
  return {{s}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 template <typename GF>
 gf_view<block_index, typename GF::regular_type> make_block_gf_view_from_vector(std::vector<std::string> block_names,
                                                                                std::vector<GF> V) {
  return {{std::move(block_names)}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // ------------------------------- Extend reinterpret_scalar_valued_gf_as_matrix_valued for block gf   ------

 // TODO simplify ?
 template <typename Variable, typename Singularity, typename Opt, bool IsConst>
 gf_view<block_index, gf<Variable, matrix_valued, Singularity, Opt>, nothing, void, IsConst>
 reinterpret_scalar_valued_gf_as_matrix_valued(
     gf_view<block_index, gf<Variable, scalar_valued, Singularity, Opt>, nothing, void, IsConst> bg) {
  std::vector<gf_view<Variable, matrix_valued, Singularity, Opt>> V;
  for (auto &g : bg) V.push_back(reinterpret_scalar_valued_gf_as_matrix_valued(g));
  return make_block_gf_view_from_vector(std::move(V));
 }

 template <typename Variable, typename Singularity, typename Opt>
 block_gf_const_view<Variable, matrix_valued, Singularity, Opt>
 reinterpret_scalar_valued_gf_as_matrix_valued(block_gf<Variable, scalar_valued, Singularity, Opt> const &bg) {
  return reinterpret_scalar_valued_gf_as_matrix_valued(bg());
 }

 template <typename Variable, typename Singularity, typename Opt>
 block_gf_view<Variable, matrix_valued, Singularity, Opt>
 reinterpret_scalar_valued_gf_as_matrix_valued(block_gf<Variable, scalar_valued, Singularity, Opt> &bg) {
  return reinterpret_scalar_valued_gf_as_matrix_valued(bg());
 }

 // -------------------------------   Free functions   --------------------------------------------------

 // a simple function to get the number of blocks
 template <typename... T> size_t n_blocks(gf<block_index, T...> const &g) { return g.mesh().size(); }
 template <typename... T> size_t n_blocks(gf_view<block_index, T...> const &g) { return g.mesh().size(); }

 // -------------------------------   an iterator over the blocks --------------------------------------------------

 template <typename T> using __get_target = std14::remove_reference_t<decltype(std::declval<T>()[0])>;

 // iterator
 template <typename G>
 class block_gf_iterator
     : public boost::iterator_facade<block_gf_iterator<G>, __get_target<G>, boost::forward_traversal_tag, __get_target<G> &> {
  friend class boost::iterator_core_access;
  using big_gf_t = typename std::remove_reference<G>::type;
  big_gf_t &big_gf;
  using mesh_iterator_t = typename big_gf_t::mesh_t::const_iterator;
  mesh_iterator_t mesh_it;

  __get_target<G> &dereference() const { return big_gf[*mesh_it]; }
  bool equal(block_gf_iterator const &other) const { return ((mesh_it == other.mesh_it)); }

  public:
  block_gf_iterator(big_gf_t &bgf, bool at_end = false) : big_gf(bgf), mesh_it(&big_gf.mesh(), at_end) {}
  void increment() { ++mesh_it; }
  bool at_end() const { return mesh_it.at_end(); }
 };

 //------------
 template <typename Target, typename Singularity, typename Opt, bool B, bool C>
 block_gf_iterator<gf_impl<block_index, Target, Singularity, Opt, B, C>>
 begin(gf_impl<block_index, Target, Singularity, Opt, B, C> &bgf) {
  return {bgf, false};
 }

 //------------
 template <typename Target, typename Singularity, typename Opt, bool B, bool C>
 block_gf_iterator<gf_impl<block_index, Target, Singularity, Opt, B, C>>
 end(gf_impl<block_index, Target, Singularity, Opt, B, C> &bgf) {
  return {bgf, true};
 }

 //----- const iterator
 template <typename G>
 class block_gf_const_iterator : public boost::iterator_facade<block_gf_const_iterator<G>, __get_target<G>,
                                                               boost::forward_traversal_tag, __get_target<G> const &> {
  friend class boost::iterator_core_access;
  using big_gf_t = std14::remove_reference_t<G>;
  big_gf_t const &big_gf;
  using mesh_iterator_t = typename big_gf_t::mesh_t::const_iterator;
  mesh_iterator_t mesh_it;

  __get_target<G> const &dereference() const { return big_gf[*mesh_it]; }
  bool equal(block_gf_const_iterator const &other) const { return ((mesh_it == other.mesh_it)); }

  public:
  block_gf_const_iterator(big_gf_t const &bgf, bool at_end = false) : big_gf(bgf), mesh_it(&big_gf.mesh(), at_end) {}
  void increment() { ++mesh_it; }
  bool at_end() const { return mesh_it.at_end(); }
 };

 template <typename Target, typename Singularity, typename Opt, bool B, bool C>
 block_gf_const_iterator<gf_impl<block_index, Target, Singularity, Opt, B, C>>
 begin(gf_impl<block_index, Target, Singularity, Opt, B, C> const &bgf) {
  return {bgf, false};
 }

 template <typename Target, typename Singularity, typename Opt, bool B, bool C>
 block_gf_const_iterator<gf_impl<block_index, Target, Singularity, Opt, B, C>>
 end(gf_impl<block_index, Target, Singularity, Opt, B, C> const &bgf) {
  return {bgf, true};
 }

 template <typename Target, typename Singularity, typename Opt, bool B, bool C>
 block_gf_const_iterator<gf_impl<block_index, Target, Singularity, Opt, B, C>>
 cbegin(gf_impl<block_index, Target, Singularity, Opt, B, C> const &bgf) {
  return {bgf, false};
 }

 template <typename Target, typename Singularity, typename Opt, bool B, bool C>
 block_gf_const_iterator<gf_impl<block_index, Target, Singularity, Opt, B, C>>
 cend(gf_impl<block_index, Target, Singularity, Opt, B, C> const &bgf) {
  return {bgf, true};
 }
}
}

