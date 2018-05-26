/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2013 by O. Parcollet
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
#include <triqs/arrays/array.hpp>
#include <triqs/arrays/vector.hpp>
#include <triqs/h5.hpp>
#include "../cache.hpp"

namespace triqs {
namespace arrays {
 
  // details for generic save/read of arrays.
 namespace h5_impl {
  inline std::string _h5_name() { return ""; }

  template <typename T0, typename... Ts> std::string _h5_name(T0 const& t0, Ts const&... ts) {
   auto r = std::to_string(t0);
   auto r1 = _h5_name(ts...);
   if (r1 != "") r += "_" + r1;
   return r;
  }

#ifndef __cpp_generic_lambdas
  template <typename ArrayType> struct _save_lambda {
   ArrayType const& a;
   h5::group g;
   template <typename... Is> void operator()(Is const&... is) const { h5_write(g, _h5_name(is...), a(is...)); }
  };

  template <typename ArrayType> struct _load_lambda {
   ArrayType& a;
   h5::group g;
   template <typename... Is> void operator()(Is const&... is) { h5_read(g, _h5_name(is...), a(is...)); }
  };
#endif
 } // details
 
 /*
   * Write an array or a view into an hdf5 file when type is not fundamental
   * ArrayType The type of the array/matrix/vector, etc..
   * g The h5 group
   * name The name of the hdf5 array in the file/group where the stack will be stored
   * A The array to be stored
   * The HDF5 exceptions will be caught and rethrown as TRIQS_RUNTIME_ERROR (with a full stackstrace, cf triqs doc).
   */
 template <typename ArrayType>
 std::c14::enable_if_t<is_amv_value_or_view_class<ArrayType>::value && !has_scalar_or_string_value_type<ArrayType>::value>
 h5_write(h5::group gr, std::string name, ArrayType const& a) {
  if (a.is_empty()) TRIQS_RUNTIME_ERROR << " Cannot save an empty array into hdf5";
  auto gr2 = gr.create_group(name);
  gr2.write_hdf5_scheme(a);
  // save the shape
  array<int, 1> sha(ArrayType::rank);
  for (int u = 0; u < ArrayType::rank; ++u) sha(u) = a.shape()[u];
  h5_write(gr2, "shape", sha);
#ifndef __cpp_generic_lambdas
  foreach(a, h5_impl::_save_lambda<ArrayType>{a, gr2});
#else
  foreach(a, [&](auto... is) { h5_write(gr2, h5_impl::_h5_name(is...), a(is...)); });
#endif
 }

 /*
   * Read an array or a view from an hdf5 file when type is not fundamental
   * ArrayType The type of the array/matrix/vector, etc..
   * g The h5 group
   * name The name of the hdf5 array in the file/group where the stack will be stored
   * A The array to be stored
   * The HDF5 exceptions will be caught and rethrown as TRIQS_RUNTIME_ERROR (with a full stackstrace, cf triqs doc).
   */
 template <typename ArrayType>
 std::c14::enable_if_t<is_amv_value_or_view_class<ArrayType>::value && !has_scalar_or_string_value_type<ArrayType>::value>
 h5_read(h5::group gr, std::string name, ArrayType& a) {
  static_assert(!std::is_const<ArrayType>::value, "Cannot read in const object");
  auto gr2 = gr.open_group(name);
  // TODO checking scheme...
  // load the shape
  auto sha2 = a.shape();
  array<int, 1> sha;
  h5_read(gr2, "shape", sha);
  if (first_dim(sha) != sha2.size())
   TRIQS_RUNTIME_ERROR << " array<array<...>> load : rank mismatch. Expected " << sha2.size()<< " Got " << first_dim(sha);
  for (int u = 0; u < sha2.size(); ++u) sha2[u] = sha(u);
  if (a.shape() != sha2) a.resize(sha2);
#ifndef __cpp_generic_lambdas
  foreach(a, h5_impl::_load_lambda<ArrayType>{a, gr2});
#else
  foreach(a, [&](auto... is) { h5_read(gr2, h5_impl::_h5_name(is...), a(is...)); });
#endif
 }
}
}

