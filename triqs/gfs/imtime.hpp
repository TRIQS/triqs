/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2013 by O. Parcollet
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
#include "./meshes/matsubara_time.hpp"

namespace triqs {
namespace gfs {

 // singularity
 template <> struct gf_default_singularity<imtime, matrix_valued> {
  using type = tail;
 };
 template <> struct gf_default_singularity<imtime, scalar_valued> {
  using type = tail;
 };
 template <> struct gf_default_singularity<imtime, matrix_real_valued> {
  using type = tail;
 };
 template <> struct gf_default_singularity<imtime, scalar_real_valued> {
  using type = tail;
 };

 /// ---------------------------  HDF5 ---------------------------------
 template <typename Singularity> struct gf_h5_name<imtime, matrix_valued, Singularity> {
  static std::string invoke() { return "ImTime"; }
 };
 template <typename S, int R>
 struct gf_h5_name<imtime, tensor_valued<R>, S> : gf_h5_name<imtime, matrix_valued, S> {};


 /// ---------------------------  A few specific functions ---------------------------------

 /*
 template <typename M, typename S, typename E>
 gf<M, matrix_real_valued, S, E> real_or_throw_impl(gf_const_view<M, matrix_valued, S, E> g, double tolerance,
                                                    const char *message) {
  if (max_element(abs(imag(g.data()))) > tolerance) TRIQS_RUNTIME_ERROR << message;
  return {g.mesh(), real(g.data()), g.singularity(), g.symmetry(), {}, {}};
 }

 template <typename G>
 auto real_or_throw(G const &g, double tolerance = 1.e-13,
                    const char *message = "real_or_throw : the imaginary part of G(tau) is not zero") {
  return real_or_throw_impl(make_const_view(g), tolerance, message);
 }

*/

 /// is_real_in_tau(g, tolerance). Returns true iif the function g(tau) is real up to tolerance
 template <typename T, typename S, typename E> bool is_real_in_tau(gf_const_view<imtime, T, S, E> g, double tolerance = 1.e-13) {
  return max_element(abs(imag(g.data()))) <= tolerance;
 }
 template <typename T, typename S, typename E> bool is_real_in_tau(gf_view<imtime, T, S, E> g, double tolerance = 1.e-13) {
  return is_real_in_tau(make_const_view(g), tolerance);
 }
 template <typename T, typename S, typename E> bool is_real_in_tau(gf<imtime, T, S, E> const &g, double tolerance = 1.e-13) {
  return is_real_in_tau(make_const_view(g), tolerance);
 }

 /// Take the real part of g(tau), without check
 template <typename T, typename S, typename E> gf<imtime, real_target_t<T>, S, E> real(gf_const_view<imtime, T, S, E> g) {
  return {g.mesh(), real(g.data()), g.singularity(), g.symmetry(), {}, {}};
 }
 template <typename T, typename S, typename E> auto real(gf_view<imtime, T, S, E> g) { return real(make_const_view(g)); }
 template <typename T, typename S, typename E> auto real(gf<imtime, T, S, E> const &g) { return real(make_const_view(g)); }

 /// Takes a complex G(tau) function and return a real G(tau) if imaginary part is small enough
 /// and throws an exception otherwise.
 template <typename G>
 auto real_or_throw(G const &g, double tolerance = 1.e-13,
                    const char *message = "real_or_throw : the imaginary part of G(tau) is not zero") {
  if (!is_real_in_tau(g, tolerance)) TRIQS_RUNTIME_ERROR<<message;
  return real(g);
 }

 /// ---------------------------  closest mesh point on the grid ---------------------------------

 template <typename Singularity, typename Target> struct gf_closest_point<imtime, Target, Singularity, void> {
  // index_t is int
  template <typename G, typename T> static int invoke(G const *g, closest_pt_wrap<T> const &p) {
   double x = double(p.value) + 0.5 * g->mesh().delta();
   int n = std::floor(x / g->mesh().delta());
   return n;
  }
 };

template <typename T, typename S, typename E> struct gf_h5_rw<imtime, T, S, E> {

  template <typename G> static void write(h5::group gr, G const & g) {
   if (!is_real_in_tau(g))
    h5_write(gr, "data", g._data);
   else
    h5_write(gr, "data", array<double, 3>(real(g._data))); // NOT NICE, CLEAN h5_write for lazy
   h5_write(gr, "singularity", g._singularity);
   h5_write(gr, "mesh", g._mesh);
   h5_write(gr, "symmetry", g._symmetry);
   h5_write(gr, "indices", g._indices);
  }

  template <typename G> static void read(h5::group gr, G&g) {
   h5_read(gr, "data", g._data);
   h5_read(gr, "singularity", g._singularity);
   h5_read(gr, "mesh", g._mesh);
   h5_read(gr, "symmetry", g._symmetry);
   h5_read(gr, "indices", g._indices);
  }
 };

}
}

