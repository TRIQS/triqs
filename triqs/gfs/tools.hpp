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
#include <triqs/arrays.hpp>
#include "triqs/utility/complex_ops.hpp"
#include <triqs/utility/view_tools.hpp>
#include <triqs/utility/expression_template_tools.hpp>
#include <utility>
#include <boost/iterator/iterator_facade.hpp>

namespace triqs {
namespace gfs {
 namespace mpl = boost::mpl;

 namespace tag {
  struct composite {};
  struct mesh_point {};
 }

 // scalar_valued, matrix_valued, tensor_valued
 struct scalar_valued {};

 template <int R> struct tensor_valued {
  static_assert(R > 0, "tensor_valued only for rank >0");
 };

 struct matrix_valued {};

 //------------------------------------------------------

 using dcomplex = std::complex<double>;

 /** The statistics : Boson or Fermion
  */
 enum statistic_enum {
  Boson,
  Fermion
 };

 struct freq_infty {}; // the point at infinity

 //------------------------------------------------------

 template <typename... T> struct closest_pt_wrap;

 template <typename T> struct closest_pt_wrap<T> : tag::mesh_point {
  T value;
  template <typename U> explicit closest_pt_wrap(U &&x) : value(std::forward<U>(x)) {}
 };

 template <typename T1, typename T2, typename... Ts> struct closest_pt_wrap<T1, T2, Ts...> : tag::mesh_point {
  std::tuple<T1, T2, Ts...> value_tuple;
  template <typename... U> explicit closest_pt_wrap(U &&... x) : value_tuple(std::forward<U>(x)...) {}
 };

 template <typename... T> closest_pt_wrap<T...> closest_mesh_pt(T &&... x) {
  return closest_pt_wrap<T...>{std::forward<T>(x)...};
 }

 //------------------------------------------------------

 // A simple replacement of tail when there is none to maintain generic code simple...
 struct nothing {
  template <typename... Args> explicit nothing(Args &&...) {} // takes anything, do nothing..
  nothing() {}
  using view_type = nothing;
  using regular_type = nothing;
  void rebind(nothing) {}
  template <typename RHS> void operator=(RHS &&) {}
  friend void h5_write(h5::group, std::string subgroup_name, nothing) {}
  friend void h5_read(h5::group, std::string subgroup_name, nothing) {}
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive &ar, const unsigned int version) {}
  friend nothing operator+(nothing, nothing) { return nothing(); }
  template <typename RHS> friend void assign_from_expression(nothing &, RHS) {}
 };

 template <typename... T> nothing slice_target(nothing, T...) { return nothing(); }
 template <typename T> nothing operator+(nothing, T const &) { return nothing(); }
 template <typename T> nothing operator-(nothing, T const &) { return nothing(); }
 template <typename T> nothing operator*(nothing, T const &) { return nothing(); }
 template <typename T> nothing operator/(nothing, T const &) { return nothing(); }
 template <typename T> TYPE_DISABLE_IF(nothing, std::is_same<T, nothing>) operator+(T const &, nothing) { return nothing(); }
 template <typename T> TYPE_DISABLE_IF(nothing, std::is_same<T, nothing>) operator-(T const &, nothing) { return nothing(); }
 template <typename T> TYPE_DISABLE_IF(nothing, std::is_same<T, nothing>) operator*(T const &, nothing) { return nothing(); }
 template <typename T> TYPE_DISABLE_IF(nothing, std::is_same<T, nothing>) operator/(T const &, nothing) { return nothing(); }
}
}
