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

namespace triqs {
namespace gfs {

 template <typename Var, typename RHS, bool IsView>
 void assign_singularity_from_function(gf_impl<Var, tail, nothing, void, IsView, false> &s, RHS const &rhs) {
  auto t = tail_omega(s.get_from_linear_index(0));
  // a bit faster to first replace (some part of expression are precomputed).
  clef::placeholder<5> x_;
  auto expr = rhs(x_, t);
  // need to quality the eval on gcc... why ?
  for (auto & y : s.mesh()) { s[y] = clef::eval(expr, x_ = y); }
  //for (auto & x : s.mesh()) s[x] = rhs(x, t);
 }

 /// ---------------------------  singularity ---------------------------------

 template <typename V1, typename V2, typename Target> struct gf_default_singularity<cartesian_product<V1, V2>, Target> {
  using S1 = typename gf_default_singularity<V1, Target>::type;
  using S2 = typename gf_default_singularity<V2, Target>::type;
  // type is nothing unless S1 is nothing and S2 is not, or 1 <-> 2
  using type = std14::conditional_t < is_nothing<S1>() && (!is_nothing<S2>()), gf<V1, S2>,
        std14::conditional_t < is_nothing<S2>() && (!is_nothing<S1>()), gf<V2, S1>, nothing >>
      ;
 };

 namespace gfs_implementation {

  /// ---------------------------  hdf5 ---------------------------------

  template <typename Variable, typename Opt> struct h5_name<Variable, tail, nothing, Opt> {
   static std::string invoke() { return "V_TailGf"; }
  };

  template <typename Variable, typename Opt> struct h5_rw<Variable, tail, nothing, Opt> {

   static void write(h5::group gr, gf_const_view<Variable, tail, nothing, Opt> g) {
    h5_write(gr, "omin", g._data.omin);
    h5_write(gr, "mask", g._data.mask);
    h5_write(gr, "data", g._data.data);
   }

   template <bool IsView> static void read(h5::group gr, gf_impl<Variable, tail, nothing, Opt, IsView, false> &g) {
    h5_read(gr, "omin", g._data.omin);
    h5_read(gr, "mask", g._data.mask);
    h5_read(gr, "data", g._data.data);
   }
  };

  // ---------------------------  data access  ---------------------------------

  template <typename Variable, typename Opt> struct data_proxy<Variable, tail, Opt> {

   struct storage_t {
    arrays::array<dcomplex, 4> data;
    arrays::array<int, 2> mask;
    int omin;
   };

   struct storage_view_t {
    arrays::array_view<dcomplex, 4> data;
    arrays::array_view<int, 2> mask;
    int omin;
    template<typename S> storage_view_t(S &s): data(s.data), mask(s.mask), omin(s.omin){}
   };

   struct storage_const_view_t {
    arrays::array_const_view<dcomplex, 4> data;
    arrays::array_const_view<int, 2> mask;
    int omin;
    template<typename S> storage_const_view_t(S &s): data(s.data), mask(s.mask), omin(s.omin){}
   };

   // from the shape of the mesh and the target, make the shape of the array. default is to glue them
   // template <typename S1, typename S2> static auto join_shape(S1 const &s1, S2 const &s2) { return make_shape(join(s1, s2); }

   template <typename S, typename RHS> static void assign_to_scalar(S &data, RHS &&rhs) { data() = std::forward<RHS>(rhs); }
   template <typename ST, typename RHS> static void rebind(ST &data, RHS &&rhs) { data.rebind(rhs.data()); }

   template <typename S> tail_view operator()(S &data, int i) const {
    return {data.data(i, arrays::ellipsis()), data.mask, data.omin};
   }

   template <typename S> tail_const_view operator()(S const &data, int i) const {
    return {data.data(i, arrays::ellipsis()), data.mask, data.omin};
   }
  };

  // -------------------------------   Factory for data  --------------------------------------------------

  template <typename Variable, typename Opt> struct data_factory<Variable, tail, nothing, Opt> {
   using mesh_t = gf_mesh<Variable, Opt>;
   using gf_t = gf<Variable, tail>;
   using target_shape_t = arrays::mini_vector<int, 2>;
   // struct target_shape_t {};
   using aux_t = nothing;

   static typename gf_t::data_t make(mesh_t const &m, target_shape_t sh, aux_t) {
    auto t = tail(sh); // build a defaut tail
    // and duplicate it over the mesh size
    return {arrays::array<dcomplex, 4>{t.data().shape().front_append(m.size())}, arrays::array<int, 2>{t.shape()},
            t.order_min()};
   }
  };

  // -------------------------------   Factory for singularity  --------------------------------------------------
  
  template <typename V1, typename V2, typename Target, typename Opt>
  struct singularity_factory<cartesian_product<V1, V2>, Target, gf<V1, tail, nothing, void>, Opt> {
   template <typename TargetShape>
   static gf<V1, tail> make(gf_mesh<cartesian_product<V1, V2>, Opt> const &m, TargetShape shape) {
    return {std::get<0>(m.components()), shape};
   }
  };

  // -------------------------------   partial_eval  --------------------------------------------------

  template <typename Variable, typename Opt, bool IsConst>
  struct partial_eval_impl<Variable, tail, nothing, Opt, IsConst> {

   using gv_t = gf_view<Variable, tail, nothing, Opt, IsConst>;
   template <int... pos, typename... T> static auto invoke(gv_t g, T const &... x) {
    return invoke_impl(g, std14::index_sequence<pos...>(), x...);
   }

   template <typename T> static tail_view invoke_impl(gv_t g, std14::index_sequence<0>, T const &x) {
    return g.get_from_linear_index(x); 
   }

   template <typename T> static gv_t invoke_impl(gv_t g, std14::index_sequence<1>, T const &x) {
    return g;
   }
  };

 } // gfs_implementation

}}
