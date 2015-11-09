/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2015 by O. Parcollet
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

namespace triqs {
namespace gfs {
 /*------------------------------------------------------------------------------------------------------
  *                                     View  assignment
  *-----------------------------------------------------------------------------------------------------*/

 // delegate = so that I can overload it for specific RHS...
 template <typename M, typename T, typename S, typename E, typename RHS>
 std14::enable_if_t<!arrays::is_scalar<RHS>::value> triqs_gf_view_assign_delegation(gf_view<M, T, S, E> g, RHS const &rhs) {
  if (!(g.mesh() == rhs.mesh()))
   TRIQS_RUNTIME_ERROR << "Gf Assignment in View : incompatible mesh" << g.mesh() << " vs " << rhs.mesh();
  for (auto const &w : g.mesh()) g[w] = rhs[w];
  g.singularity() = rhs.singularity();
 }

 template <typename M, typename T, typename S, typename E, typename RHS>
 std14::enable_if_t<arrays::is_scalar<RHS>::value> triqs_gf_view_assign_delegation(gf_view<M, T, S, E> g, RHS const &x) {
  gf_view<M, T, S, E>::data_proxy_t::assign_to_scalar(g.data(), x);
  g.singularity() = x;
 }

 /*------------------------------------------------------------------------------------------------------
  *                                  For lazy transformation   
  *-----------------------------------------------------------------------------------------------------*/

 template <typename Tag, typename G>
 struct tagged_cview {
  typename G::const_view_type g;
 };

 template <typename Tag, typename D, typename Target = matrix_valued, typename Singularity = gf_default_singularity_t<D, Target>,
           typename Evaluator = void>
 struct gf_keeper {
  gf_const_view<D, Target, Singularity, Evaluator> g;
 };

 /*------------------------------------------------------------------------------------------------------
  *             Interaction with the CLEF library : auto assignment implementation
  *-----------------------------------------------------------------------------------------------------*/

 // auto assignment of the gf (gf(om_) << expression fills the functions by evaluation of expression)

 template <typename RHS, typename M, typename T, typename S, typename E>
 void triqs_clef_auto_assign(gf_view<M, T, S, E> g, RHS const &rhs) {
  triqs_clef_auto_assign_impl(g, rhs, typename std::is_base_of<tag::composite, gf_mesh<M>>::type());
  assign_singularity_from_function(g.singularity(), rhs);
  // access to the data . Beware, we view it as a *matrix* NOT an array... (crucial for assignment to scalars !)
  // if f is an expression, replace the placeholder with a simple tail.
 }

 // Seems to be needed only in C++11, but why ?? (both clang and gcc)
 template <typename RHS, typename V> void triqs_clef_auto_assign(view_proxy<V> v, RHS const &rhs) {
  triqs_clef_auto_assign(static_cast<V>(v), rhs);
 }

 template <typename RHS, typename M, typename T, typename S, typename E>
 void triqs_clef_auto_assign(gf<M, T, S, E> &g, RHS const &rhs) {
  triqs_clef_auto_assign(g(),rhs);
 }

 // enable the writing g[om_] << .... also
 template <typename RHS, typename M, typename T, typename S, typename E>
 void triqs_clef_auto_assign_subscript(gf_view<M, T, S, E> g, RHS const &rhs) {
  triqs_clef_auto_assign(g, rhs);
 }

 template <typename RHS, typename M, typename T, typename S, typename E>
 void triqs_clef_auto_assign_subscript(gf<M, T, S, E> &g, RHS const &rhs) {
  triqs_clef_auto_assign(g(),rhs);
 }

 template <typename G, typename RHS> void triqs_gf_clef_auto_assign_impl_aux_assign(G &&g, RHS &&rhs) {
  std::forward<G>(g) = std::forward<RHS>(rhs);
 }

 template <typename G, typename Expr, int... Is>
 void triqs_gf_clef_auto_assign_impl_aux_assign(G &&g, clef::make_fun_impl<Expr, Is...> &&rhs) {
  triqs_clef_auto_assign(std::forward<G>(g), std::forward<clef::make_fun_impl<Expr, Is...>>(rhs));
 }

 template <typename G, typename RHS>
 void triqs_clef_auto_assign_impl(G &g, RHS const &rhs, std::false_type) {
  for (auto const &w : g.mesh()) { triqs_gf_clef_auto_assign_impl_aux_assign(g[w], rhs(w)); }
 }

 template <typename G, typename RHS>
 void triqs_clef_auto_assign_impl(G &g, RHS const &rhs, std::true_type) {
  for (auto const &w : g.mesh()) {
   triqs_gf_clef_auto_assign_impl_aux_assign(g[w], triqs::tuple::apply(rhs, w.components_tuple()));
  }
 }

 /*------------------------------------------------------------------------------------------------------
  *                            default factories
  *-----------------------------------------------------------------------------------------------------*/

 // Factory for the data
 template <typename M, typename T, typename S> struct gf_data_factory {
  using gf_t = gf<M, T, S>;
  using target_shape_t = arrays::mini_vector<int, T::dim>;
  using mesh_t = gf_mesh<M>;
  using aux_t = arrays::memory_layout<get_n_variables<M>::value + T::dim>;
  using data_t = typename gf_data_proxy<M, T>::storage_t;

  //
  static data_t make(mesh_t const &m, target_shape_t shape, aux_t ml) {
   data_t A(join(m.size_of_components(), shape), ml);
   A() = 0;
   return A;
  }
 };

 // Factory for the singularity
 template <typename M, typename T, typename S> struct gf_singularity_factory {
  template <typename TargetShape> static S make(gf_mesh<M> const &m, TargetShape shape) { return S{shape}; }
 };

 /*------------------------------------------------------------------------------------------------------
  *                              HDF5
  *-----------------------------------------------------------------------------------------------------*/

 // scalar function : just add a _s
 template <typename M, typename S> struct gf_h5_name<M, scalar_valued, S> {
  static std::string invoke() { return gf_h5_name<M, matrix_valued, S>::invoke() + "_s"; }
 };

 // Some transformation of the data may be needed before writing
 template <typename M, typename T, typename S, typename E> struct gf_h5_write_data {
  template <typename G> static void invoke(h5::group gr, G const &g) { h5_write(gr, "data", g.data()); }
 };

 // A special case used for imtime and legendre.
 // If the data is real, write a real array, otherwise a complex array
 struct gf_h5_write_data_real_or_complex_runtime {
  template <typename G> static void invoke(h5::group gr, G const &g) {
   if (!is_gf_real(g))
    h5_write(gr, "data", g.data());
   else
    h5_write(gr, "data", array<double, 3>(real(g.data())));
  }
 };

 // the h5 write and read of gf members, so that we can specialize it e.g. for block gf
 template <typename M, typename T, typename S, typename E> struct gf_h5_rw {

  static void write(h5::group gr, gf_const_view<M, T, S, E> g) {
   gf_h5_write_data<M, T, S, E>::invoke(gr, g); // h5_write(gr, "data", g._data);
   h5_write(gr, "singularity", g._singularity);
   h5_write(gr, "mesh", g._mesh);
   h5_write(gr, "symmetry", g._symmetry);
   h5_write(gr, "indices", g._indices);
  }

  template <typename G> static void read(h5::group gr, G &g) {
   h5_read(gr, "data", g._data);
   h5_read(gr, "singularity", g._singularity);
   h5_read(gr, "mesh", g._mesh);
   h5_read(gr, "symmetry", g._symmetry);
   h5_read(gr, "indices", g._indices);
  }
 };

 // Some work that may be necessary before writing (some compression, see imfreq)
 // Default : do nothing
 template <typename M, typename T, typename S, typename E> struct gf_h5_before_write {
  template <typename G> static G const &invoke(h5::group gr, G const &g) { return g; }
 };

 // Some work that may be necessary after the read (for backward compatibility e.g.)
 // Default : do nothing
 template <typename M, typename T, typename S, typename E> struct gf_h5_after_read {
  template <typename G> static void invoke(h5::group gr, G&g) {}
 };

 /// ---------------------------  real for gf ---------------------------------

 /// is_gf_real(g, tolerance). Returns true iif the function g is real up to tolerance
 template <typename G> bool is_gf_real(G const &g, double tolerance = 1.e-13) {
  return max_element(abs(imag(g.data()))) <= tolerance;
 }

 /// Takes the real part of g without check, and returns a new gf with a real target
 template <typename M, typename T, typename S, typename E> gf<M, real_target_t<T>, S> real(gf_const_view<M, T, S, E> g) {
  return {g.mesh(), real(g.data()), g.singularity(), g.symmetry(), {}, {}}; // no indices for real_valued, internal C++ use only
 }
 template <typename M, typename T, typename S, typename E> gf<M, real_target_t<T>, S> real(gf_view<M, T, S, E> g) {
  return {g.mesh(), real(g.data()), g.singularity(), g.symmetry(), {}, {}};
 }
 template <typename M, typename T, typename S, typename E> gf<M, real_target_t<T>, S> real(gf<M, T, S, E> const &g) {
  return {g.mesh(), real(g.data()), g.singularity(), g.symmetry(), {}, {}};
 }

}// triqs::gfs
}

/*------------------------------------------------------------------------------------------------------
 *             Delete std::swap for views, as for arrays
 *-----------------------------------------------------------------------------------------------------*/
namespace std {
template <typename Mesh, typename Target, typename Singularity, typename Evaluator>
void swap(triqs::gfs::gf_view<Mesh, Target, Singularity, Evaluator> &a,
          triqs::gfs::gf_view<Mesh, Target, Singularity, Evaluator> &b) = delete;
}

