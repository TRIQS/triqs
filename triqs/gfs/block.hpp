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
 template <typename G> using is_block_gf_or_view = is_gf_or_view<G, block_index>;

 /// ---------------------------  hdf5 ---------------------------------

 template <typename Target> struct gf_h5_name<block_index, Target, nothing> {
  static std::string invoke() { return "BlockGf"; }
 };

 template <typename Target> struct gf_h5_rw<block_index, Target, nothing, void> {

  static void write(h5::group gr, gf_const_view<block_index, Target> g) {
   for (size_t i = 0; i < g.mesh().size(); ++i) h5_write(gr, g.mesh().domain().names()[i], g._data[i]);
   h5_write(gr, "block_names", g.mesh().domain().names());
  }

  template <bool IsView> static void read(h5::group gr, gf_impl<block_index, Target, nothing, void, IsView, false> &g) {
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

 template <typename GF>
 gf_const_view<block_index, typename GF::regular_type> make_block_gf_const_view_from_vector(std::vector<GF> V) {
  int s = V.size();
  return {{s}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 template <typename GF>
 gf_const_view<block_index, typename GF::regular_type> make_block_gf_const_view_from_vector(std::vector<std::string> block_names,
                                                                                            std::vector<GF> V) {
  return {{std::move(block_names)}, std::move(V), nothing{}, nothing{}, nothing{}};
 }

 // -------------------------------   Free functions   --------------------------------------------------

 /// The number of blocks
 template <typename G> TYPE_ENABLE_IF(int, is_block_gf_or_view<G>) n_blocks(G const &g) { return g.mesh().size(); }

 /// The vector of names of the blocks
 template <typename G> TYPE_ENABLE_IF(std::vector<std::string> const &, is_block_gf_or_view<G>) get_block_names(G const &g) {
  return g.mesh().domain().names();
 }

 // -------------------------------   Map --------------------------------------------------
 // map takes a function f, a block_gf or its view g
 // then it computes f(g[i]) for all i
 // If the result of f is :
 //  * a gf             : then map returns a block_gf
 //  * a gf_view        : then map returns a block_gf_view
 //  * a gf_const_view  : then map returns a block_gf_const_view
 //  * otherwise        : then map returns a std::vector<>
 namespace impl {

  template <typename F, typename T> std::vector<std14::result_of_t<F(T)>> _map(F &&f, std::vector<T> const &V) {
   std::vector<std14::result_of_t<F(T)>> res;
   res.reserve(V.size());
   for (auto &x : V) res.emplace_back(f(x));
   return res;
  }

  // implementation is dispatched according to R
  template <typename F, typename G, typename R = std14::decay_t<std14::result_of_t<F(typename std14::decay_t<G>::target_t)>>>
  struct map;

  // general case
  template <typename F, typename G, typename R> struct map {
   static auto invoke(F &&f, G &&g) RETURN(_map(std::forward<F>(f), std::forward<G>(g).data()));
  };

  // now , when R is a gf, gf_view, a gf_const_view
  template <typename F, typename G, typename... T> struct map<F, G, gf<T...>> {
   static auto invoke(F &&f, G &&g)
       RETURN(make_block_gf(get_block_names(g), _map(std::forward<F>(f), std::forward<G>(g).data())));
  };

  template <typename F, typename G, typename M, typename T, typename S, typename E> struct map<F, G, gf_view<M, T, S, E>> {
   static auto invoke(F &&f, G &&g)
       RETURN(make_block_gf_view_from_vector(get_block_names(g), _map(std::forward<F>(f), std::forward<G>(g).data())));
  };

  template <typename F, typename G, typename M, typename T, typename S, typename E> struct map<F, G, gf_const_view<M, T, S, E>> {
   static auto invoke(F &&f, G &&g)
       RETURN(make_block_gf_const_view_from_vector(get_block_names(g), _map(std::forward<F>(f), std::forward<G>(g).data())));
  };
 }

#ifndef TRIQS_CPP11
 template <typename F, typename G> auto map_block_gf(F &&f, G &&g) {
  static_assert(is_block_gf_or_view<G>::value, "map_block_gf requires a block gf");
  return impl::map<F, G>::invoke(std::forward<F>(f), std::forward<G>(g));
 }
#else
 template <typename F, typename G>
 auto map_block_gf(F &&f, G &&g) RETURN(impl::map<F, G>::invoke(std::forward<F>(f), std::forward<G>(g)));
#endif

 // the map function itself...
 template <typename F, typename G>
 auto map(F &&f, G &&g)
     -> std14::enable_if_t<is_block_gf_or_view<G>::value,
                           decltype(impl::map<F, G>::invoke(std::forward<F>(f), std::forward<G>(g)))> {
  return impl::map<F, G>::invoke(std::forward<F>(f), std::forward<G>(g));
 }

// -------------------------------   some functions mapped ... --------------------------------------------------

// A macro to automatically map a function to the block gf
#define TRIQS_PROMOTE_AS_BLOCK_GF_FUNCTION(f)                                                                                    \
 namespace impl {                                                                                                                \
  struct _mapped_##f {                                                                                                           \
   template <typename T> auto operator()(T &&x)RETURN(f(std::forward<T>(x)));                                                    \
  };                                                                                                                             \
 }                                                                                                                               \
 template <typename G> auto f(gf<block_index, G> &g) RETURN(map_block_gf(impl::_mapped_##f{}, g));                               \
 template <typename G> auto f(gf_view<block_index, G> g) RETURN(map_block_gf(impl::_mapped_##f{}, g));                           \
 template <typename G> auto f(gf<block_index, G> const &g) RETURN(map_block_gf(impl::_mapped_##f{}, g));                         \
 template <typename G> auto f(gf_const_view<block_index, G> g) RETURN(map_block_gf(impl::_mapped_##f{}, g));

 TRIQS_PROMOTE_AS_BLOCK_GF_FUNCTION(reinterpret_scalar_valued_gf_as_matrix_valued);
 TRIQS_PROMOTE_AS_BLOCK_GF_FUNCTION(inverse);

 // -------------------------------   an iterator over the blocks --------------------------------------------------

 template <typename G>
 class block_gf_iterator : std::iterator<std::forward_iterator_tag, std14::remove_reference_t<decltype(std::declval<G>()[0])>> {
  G *bgf = NULL;
  long n = 0;

  public:
  block_gf_iterator() = default;
  block_gf_iterator(G &bgf, bool at_end = false) : bgf(&bgf), n(at_end ? bgf.mesh().size() : 0) {}

  using value_type = std14::remove_reference_t<decltype(std::declval<G>()[0])>;

  value_type &operator*() { return (*bgf)[n]; }
  value_type &operator->() { return (*bgf)[n]; }

  block_gf_iterator &operator++() {
   ++n;
   return *this;
  }

  block_gf_iterator operator++(int) {
   auto it = *this;
   ++n;
   return it;
  }

  bool operator==(block_gf_iterator const &other) const { return ((bgf == other.bgf) && (n == other.n)); }
  bool operator!=(block_gf_iterator const &other) const { return (!operator==(other)); }
 };

 //------------

 template <typename G> std14::enable_if_t<is_block_gf_or_view<G>::value, block_gf_iterator<G>> begin(G &bgf) {
  return {bgf, false};
 }

 template <typename G> std14::enable_if_t<is_block_gf_or_view<G>::value, block_gf_iterator<G const>> cbegin(G const &bgf) {
  return {bgf, false};
 }

 template <typename G> std14::enable_if_t<is_block_gf_or_view<G>::value, block_gf_iterator<G>> end(G &bgf) {
  return {bgf, true};
 }

 template <typename G> std14::enable_if_t<is_block_gf_or_view<G>::value, block_gf_iterator<G const>> cend(G const &bgf) {
  return {bgf, true};
 }
}
}

