/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2013 by M. Ferrero, O. Parcollet
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
#ifndef TRIQS_TOOLS_MC_IMPL_TOOLS_H
#define TRIQS_TOOLS_MC_IMPL_TOOLS_H
#include <triqs/utility/first_include.hpp>
#include <triqs/h5.hpp>
#include <string>

namespace triqs { namespace mc_tools {

 template<typename T, typename Enable=void> struct has_h5_read                                                              : std::false_type {};
 template<typename T> struct has_h5_read<T, decltype(h5_read(std::declval<h5::group>(), std::string(), std::declval<T>()))> : std::true_type {};

 template<typename T, typename Enable=void> struct has_h5_write                                                              : std::false_type {};
 template<typename T> struct has_h5_write<T, decltype(h5_read(std::declval<h5::group>(), std::string(), std::declval<T>()))> : std::true_type {};

 typedef std::function<void(h5::group, std::string const &)> h5_rw_lambda_t;

 // given a pointer to an object, synthesize h5_read/write function for this object
 template<typename T> h5_rw_lambda_t make_h5_write_impl(T * p, std::true_type) {
  return [p](h5::group F, std::string const &Name) {h5_write(F,Name, *p);};
 }
 template<typename T> h5_rw_lambda_t make_h5_read_impl(T * p, std::true_type) {
  return [p](h5::group F, std::string const &Name) {h5_read(F,Name, *p);};
 }
 template<typename T> h5_rw_lambda_t make_h5_read_impl (T * p, std::false_type) { return h5_rw_lambda_t();}
 template<typename T> h5_rw_lambda_t make_h5_write_impl(T * p, std::false_type) { return h5_rw_lambda_t();}

 template<typename T> h5_rw_lambda_t make_h5_write(T * p) { return make_h5_write_impl(p,std::integral_constant<bool,has_h5_write<T>::value>());}
 template<typename T> h5_rw_lambda_t make_h5_read (T * p) { return make_h5_read_impl (p,std::integral_constant<bool,has_h5_read <T>::value>());}

 // move_construtible is not in gcc 4.6 std lib
 template<class T>
  struct is_move_constructible : std::is_constructible<T, typename std::add_rvalue_reference<T>::type> {};

}}// end namespace
#endif


