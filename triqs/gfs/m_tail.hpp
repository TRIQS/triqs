/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2014 by O. Parcollet
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

 namespace gfs_implementation {

  /*template <typename T> constexpr bool has_a_singularity() {
   return std::is_same<typename singularity<V1, Target>::type, local::tail>::value;
  }

  /// ---------------------------  singularity ---------------------------------

  template <typename V1, typename V2, typename Target, typename Opt> struct singularity<cartesian_product<V1,V2>, Target, Opt> {
   using type = std14::conditional_t < (!has_a_singularity<V1>()) && has_a_singularity<V2>(), gf<V1, local::tail>, nothing > ;
  };
  */
  /// ---------------------------  hdf5 ---------------------------------
 
  template <typename Variable, typename Opt> struct h5_name<Variable, local::tail, nothing, Opt> {
   static std::string invoke() { return "xxxxx"; }
  };

  template <typename Variable, typename Opt> struct h5_rw<Variable, local::tail, nothing, Opt> {

   static void write(h5::group gr, gf_const_view<Variable, local::tail, nothing, Opt> g) {
    for (size_t i = 0; i < g.mesh().size(); ++i) h5_write(gr, std::to_string(i), g._data[i]);
    // h5_write(gr,"symmetry",g._symmetry);
   }

   template <bool IsView> static void read(h5::group gr, gf_impl<Variable, local::tail, nothing, Opt, IsView, false> &g) {
    // does not work : need to read the block name and remake the mesh...
    //g._mesh = gf_mesh<block_index, Opt>(gr.get_all_subgroup_names());
    //g._data.resize(g._mesh.size());
    // if (g._data.size() != g._mesh.size()) TRIQS_RUNTIME_ERROR << "h5 read block gf : number of block mismatch";
    //for (size_t i = 0; i < g.mesh().size(); ++i) h5_read(gr, g.mesh().domain().names()[i], g._data[i]);
    // h5_read(gr,"symmetry",g._symmetry);
   }
  };

  /// ---------------------------  data access  ---------------------------------

  template <typename Variable, typename Opt> struct data_proxy<Variable, local::tail, Opt> : data_proxy_vector<local::tail> {};

  // -------------------------------   Factories  --------------------------------------------------

  template <typename Variable, typename Opt> struct factories<Variable, local::tail, nothing, Opt> {
   using mesh_t=gf_mesh<Variable, Opt> ;
   using gf_t=gf<Variable, local::tail> ;
   //using gf_view_t=gf_view<block_index, Target> ;
   struct target_shape_t {};

   static typename gf_t::data_t make_data(mesh_t const &m, target_shape_t) { return std::vector<local::tail>(m.size()); }
   static nothing make_singularity(mesh_t const &m, target_shape_t) {
    return {};
   }
  };

  // partial_eval
  template <typename Variable, typename Opt, bool IsConst> struct partial_eval_impl<Variable, local::tail, nothing, Opt, IsConst> {

   using gv_t = gf_view<Variable, local::tail, nothing, Opt, IsConst>;
   template <int... pos, typename... T> static auto invoke(gv_t g, T const &... x) {
    return invoke_impl(g, std14::index_sequence<pos...>(), x...);
   }

   template <typename T> static local::tail_view invoke_impl(gv_t g, std14::index_sequence<0>, T const &x) {
    return g[g.mesh().index_to_linear(x)];
   }

   template <typename T> static nothing invoke_impl(gv_t g, std14::index_sequence<1>, T const &x) {
    return {};
   }
  };

 } // gfs_implementation


}}
