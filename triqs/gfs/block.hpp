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
#ifndef TRIQS_GF_BLOCK_H
#define TRIQS_GF_BLOCK_H
#include "./tools.hpp"
#include "./gf.hpp"
#include "./local/tail.hpp"
#include "./meshes/discrete.hpp"

namespace triqs {
namespace gfs {

 struct block_index {};

 template <typename Opt> struct gf_mesh<block_index, Opt> : discrete_mesh<discrete_domain> {
  typedef discrete_mesh<discrete_domain> B;
  gf_mesh() = default;
  gf_mesh(int s) : B(s) {}
  gf_mesh(discrete_domain const &d) : B(d) {}
  gf_mesh(std::initializer_list<std::string> const &s) : B(s) {}
 };

 namespace gfs_implementation {

  /// ---------------------------  hdf5 ---------------------------------

  template <typename Target, typename Opt> struct h5_name<block_index, Target, Opt> {
   static std::string invoke() { return "BlockGf"; }
  };

  template <typename Target, typename Opt> struct h5_rw<block_index, Target, Opt> {

   static void write(h5::group gr, gf_const_view<block_index, Target, Opt> g) {
    for (size_t i = 0; i < g.mesh().size(); ++i) h5_write(gr, g.mesh().domain().names()[i], g._data[i]);
    // h5_write(gr,"symmetry",g._symmetry);
   }

   template <bool IsView> static void read(h5::group gr, gf_impl<block_index, Target, Opt, IsView, false> &g) {
    // does not work : need to read the block name and remake the mesh...
    g._mesh = gf_mesh<block_index, Opt>(gr.get_all_subgroup_names());
    g._data.resize(g._mesh.size());
    // if (g._data.size() != g._mesh.size()) TRIQS_RUNTIME_ERROR << "h5 read block gf : number of block mismatch";
    for (size_t i = 0; i < g.mesh().size(); ++i) h5_read(gr, g.mesh().domain().names()[i], g._data[i]);
    // h5_read(gr,"symmetry",g._symmetry);
   }
  };

  /// ---------------------------  data access  ---------------------------------

  template <typename Target, typename Opt>
  struct data_proxy<block_index, Target, Opt> : data_proxy_vector<typename regular_type_if_exists_else_type<Target>::type> {};

  // -------------------------------   Factories  --------------------------------------------------

  template <typename Target, typename Opt> struct factories<block_index, Target, Opt> {
   typedef gf_mesh<block_index, Opt> mesh_t;
   typedef gf<block_index, Target> gf_t;
   typedef gf_view<block_index, Target> gf_view_t;
   struct target_shape_t {};

   static typename gf_t::data_t make_data(mesh_t const &m, target_shape_t) { return std::vector<Target>(m.size()); }
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
 template <typename Variable, typename Target, typename Opt>
 block_gf<Variable, Target, Opt> make_block_gf(int n, gf<Variable, Target, Opt> const &g) {
  auto V = std::vector<gf<Variable, Target, Opt>>{};
  for (int i = 0; i < n; ++i) V.push_back(g);
  return {{n}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // from a vector of gf (moving directly)
 template <typename Variable, typename Target, typename Opt>
 block_gf<Variable, Target, Opt> make_block_gf(std::vector<gf<Variable, Target, Opt>> V) {
  return {{int(V.size())}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // from a vector of gf : generalized to have a different type of gf in the vector (e.g. views...)
 template <typename Variable, typename Target, typename Opt, typename GF2>
 block_gf<Variable, Target, Opt> make_block_gf(std::vector<GF2> const &V) {
  auto V2 = std::vector<gf<Variable, Target, Opt>>{};
  for (auto const &g : V) V2.push_back(g);
  return {{int(V.size())}, std::move(V2), nothing{}, nothing{}, nothing{}};
 }

 // from a init list of GF with the correct type
 template <typename Variable, typename Target, typename Opt>
 block_gf<Variable, Target, Opt> make_block_gf(std::initializer_list<gf<Variable, Target, Opt>> const &V) {
  return {{int(V.size())}, V, nothing{}, nothing{}, nothing{}};
 }

 // from vector<string> and a gf to be copied
 template <typename Variable, typename Target, typename Opt>
 block_gf<Variable, Target, Opt> make_block_gf(std::vector<std::string> block_names, gf<Variable, Target, Opt> const &g) {
  auto V = std::vector<gf<Variable, Target, Opt>>{};
  for (int i = 0; i < block_names.size(); ++i) V.push_back(g);
  return {{block_names}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // from vector<string>, vector<gf>
 template <typename Variable, typename Target, typename Opt>
 block_gf<Variable, Target, Opt> make_block_gf(std::vector<std::string> block_names, std::vector<gf<Variable, Target, Opt>> V) {
  if (block_names.size() != V.size())
   TRIQS_RUNTIME_ERROR << "make_block_gf(vector<string>, vector<gf>) : the two vectors do not have the same size !";
  return {{block_names}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // from vector<string>, init_list<GF>
 template <typename Variable, typename Target, typename Opt>
 block_gf<Variable, Target, Opt> make_block_gf(std::vector<std::string> block_names,
                                               std::initializer_list<gf<Variable, Target, Opt>> const &V) {
  if (block_names.size() != V.size()) TRIQS_RUNTIME_ERROR << "make_block_gf(vector<string>, init_list) : size mismatch !";
  return {{block_names}, V, nothing{}, nothing{}, nothing{}};
 }

 // -------------------------------   Free Factories for view type  --------------------------------------------------

 template <typename G0, typename... G>
 gf_view<block_index, typename std::remove_reference<G0>::type::view_type> make_block_gf_view(G0 &&g0, G &&... g) {
  auto V = std::vector<typename std::remove_reference<G0>::type::view_type>{std::forward<G0>(g0), std::forward<G>(g)...};
  return {{int(V.size())}, std::move(V), nothing{}, nothing{}, nothing{}};
  // return { gf_mesh<block_index, Opt> {int(V.size())}, std::move(V), nothing{}, nothing{} } ;
 }

 template <typename GF> gf_view<block_index, typename GF::regular_type> make_block_gf_view_from_vector(std::vector<GF> V) {
  return {{int(V.size())}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 template <typename GF>
 gf_view<block_index, typename GF::regular_type> make_block_gf_view_from_vector(std::vector<std::string> block_names,
                                                                                std::vector<GF> V) {
  return {{std::move(block_names)}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // ------------------------------- Extend reinterpret_scalar_valued_gf_as_matrix_valued for block gf   ------

 template <typename Variable, typename Opt, typename Opt2, bool IsConst>
 gf_view<block_index, gf<Variable, matrix_valued, Opt>, Opt2, IsConst>
 reinterpret_scalar_valued_gf_as_matrix_valued(gf_view<block_index, gf<Variable, scalar_valued, Opt>, Opt2, IsConst> bg) {
  std::vector<gf_view<Variable, matrix_valued, Opt>> V;
  for (auto &g : bg) V.push_back(reinterpret_scalar_valued_gf_as_matrix_valued(g));
  return make_block_gf_view_from_vector(std::move(V));
 }

 template <typename Variable, typename Opt, typename Opt2>
 gf_const_view<block_index, gf<Variable, matrix_valued, Opt>, Opt2>
 reinterpret_scalar_valued_gf_as_matrix_valued(gf<block_index, gf<Variable, scalar_valued, Opt>, Opt2> const &bg) {
  return reinterpret_scalar_valued_gf_as_matrix_valued(bg());
 }

 template <typename Variable, typename Opt, typename Opt2>
 gf_view<block_index, gf<Variable, matrix_valued, Opt>, Opt2>
 reinterpret_scalar_valued_gf_as_matrix_valued(gf<block_index, gf<Variable, scalar_valued, Opt>, Opt2> &bg) {
  return reinterpret_scalar_valued_gf_as_matrix_valued(bg());
 }

 // -------------------------------   Free functions   --------------------------------------------------

 // a simple function to get the number of blocks
 template <typename T> size_t n_blocks(gf<block_index, T> const &g) { return g.mesh().size(); }
 template <typename T> size_t n_blocks(gf_view<block_index, T> const &g) { return g.mesh().size(); }

 // -------------------------------   an iterator over the blocks --------------------------------------------------

 template<typename T> using __get_target = typename std::remove_reference<decltype(std::declval<T>()[0])>::type;

 // iterator
 template <typename G>
 class block_gf_iterator
     : public boost::iterator_facade<block_gf_iterator<G>, __get_target<G>, boost::forward_traversal_tag, __get_target<G> &> {
  friend class boost::iterator_core_access;
  typedef typename std::remove_reference<G>::type big_gf_t;
  big_gf_t &big_gf;
  typedef typename big_gf_t::mesh_t::const_iterator mesh_iterator_t;
  mesh_iterator_t mesh_it;

  __get_target<G> &dereference() const { return big_gf[*mesh_it]; }
  bool equal(block_gf_iterator const &other) const { return ((mesh_it == other.mesh_it)); }

  public:
  block_gf_iterator(big_gf_t &bgf, bool at_end = false) : big_gf(bgf), mesh_it(&big_gf.mesh(), at_end) {}
  void increment() { ++mesh_it; }
  bool at_end() const { return mesh_it.at_end(); }
 };

 //------------
 template <typename Target, typename Opt, bool B, bool C>
 block_gf_iterator<gf_impl<block_index, Target, Opt, B, C>> begin(gf_impl<block_index, Target, Opt, B, C> &bgf) {
  return {bgf, false};
 }

 //------------
 template <typename Target, typename Opt, bool B, bool C>
 block_gf_iterator<gf_impl<block_index, Target, Opt, B, C>> end(gf_impl<block_index, Target, Opt, B, C> &bgf) {
  return {bgf, true};
 }

 //----- const iterator
 template <typename G>
 class block_gf_const_iterator
     : public boost::iterator_facade<block_gf_const_iterator<G>, __get_target<G>, boost::forward_traversal_tag, __get_target<G> const &> {
  friend class boost::iterator_core_access;
  typedef typename std::remove_reference<G>::type big_gf_t;
  big_gf_t const &big_gf;
  typedef typename big_gf_t::mesh_t::const_iterator mesh_iterator_t;
  mesh_iterator_t mesh_it;

  __get_target<G> const &dereference() const { return big_gf[*mesh_it]; }
  bool equal(block_gf_const_iterator const &other) const { return ((mesh_it == other.mesh_it)); }

  public:
  block_gf_const_iterator(big_gf_t const &bgf, bool at_end = false) : big_gf(bgf), mesh_it(&big_gf.mesh(), at_end) {}
  void increment() { ++mesh_it; }
  bool at_end() const { return mesh_it.at_end(); }
 };

 template <typename Target, typename Opt, bool B, bool C>
 block_gf_const_iterator<gf_impl<block_index, Target, Opt, B, C>> begin(gf_impl<block_index, Target, Opt, B, C> const &bgf) {
  return {bgf, false};
 }

 template <typename Target, typename Opt, bool B, bool C>
 block_gf_const_iterator<gf_impl<block_index, Target, Opt, B, C>> end(gf_impl<block_index, Target, Opt, B, C> const &bgf) {
  return {bgf, true};
 }

 template <typename Target, typename Opt, bool B, bool C>
 block_gf_const_iterator<gf_impl<block_index, Target, Opt, B, C>> cbegin(gf_impl<block_index, Target, Opt, B, C> const &bgf) {
  return {bgf, false};
 }

 template <typename Target, typename Opt, bool B, bool C>
 block_gf_const_iterator<gf_impl<block_index, Target, Opt, B, C>> cend(gf_impl<block_index, Target, Opt, B, C> const &bgf) {
  return {bgf, true};
 }
}
}
#endif

