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
 
 using dcomplex = std::complex<double>;

 namespace tag {
  struct composite {};
  struct mesh_point {};
 }

 struct scalar_valued {
  static constexpr int dim = 0;
  using value_type = dcomplex;
 };
 struct matrix_valued {
  static constexpr int dim = 2;
  using value_type = arrays::matrix<dcomplex>;
 };
 template <int R> struct tensor_valued {
  static_assert(R > 0, "tensor_valued only for rank >0");
  static constexpr int dim = R;
  using value_type = arrays::array<dcomplex, R>;
 };

 struct scalar_real_valued {
  static constexpr int dim = 0;
  using value_type = double;
 };
 struct matrix_real_valued {
  static constexpr int dim = 2;
  using value_type = arrays::matrix<double>;
 };
 template <int R> struct tensor_real_valued {
  static constexpr int dim = R;
  static_assert(R > 0, "tensor_valued only for rank >0");
  using value_type = arrays::array<double, R>;
 };

 struct scalar_real_valued { static constexpr int dim =0;};
 struct matrix_real_valued { static constexpr int dim =2;};
 template <int R> struct tensor_real_valued {  
  static constexpr int dim = R; static_assert(R > 0, "tensor_valued only for rank >0"); 
 };

 template<typename T> struct _real_target_t_impl;
 template<> struct _real_target_t_impl<scalar_valued> { using type = scalar_real_valued;};
 template<> struct _real_target_t_impl<matrix_valued> { using type = matrix_real_valued;};
 template<int R> struct _real_target_t_impl<tensor_valued<R>> { using type = tensor_real_valued<R>;};
 template<typename T> using real_target_t = typename _real_target_t_impl<T>::type;
 
 //------------------------------------------------------

 using dcomplex = std::complex<double>;

 /** The statistics : Boson or Fermion
  */
 enum statistic_enum {
  Boson,
  Fermion
 };

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
}
}
