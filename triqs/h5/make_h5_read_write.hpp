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
#ifndef TRIQS_TOOLS_H5_MAKE_READ_WRITE_H
#define TRIQS_TOOLS_H5_MAKE_READ_WRITE_H
#include <triqs/utility/first_include.hpp>
#include <triqs/h5.hpp>
#include <triqs/utility/serialization.hpp>
#include <string>

namespace triqs { namespace h5 {

 template<typename T, typename Enable=void> struct has_h5_read                                                                    : std::false_type {};
 template<typename T> struct has_h5_read<T, decltype(h5_read(std::declval<h5::group>(), std::string(), (T&)(std::declval<T>())))> : std::true_type {};

 template<typename T, typename Enable=void> struct has_h5_write                                                                    : std::false_type {};
 template<typename T> struct has_h5_write<T, decltype(h5_write(std::declval<h5::group>(), std::string(), std::declval<const T>()))> : std::true_type {};

 typedef std::function<void(h5::group, std::string const &)> h5_rw_lambda_t;

 namespace details { 
  // given a pointer to an object, synthesize h5_read/write function for this object
  template<typename T> h5_rw_lambda_t make_h5_write_impl(T * p, std::integral_constant<int,0>) {
   return [p](h5::group F, std::string const &Name) {h5_write(F,Name, *p);};
  }
  template<typename T> h5_rw_lambda_t make_h5_read_impl(T * p, std::integral_constant<int,0>) {
   return [p](h5::group F, std::string const &Name) {h5_read(F,Name, *p);};
  }

  // use boost serialization as a generic case
  template<typename T> h5_rw_lambda_t make_h5_read_impl(T * p, std::integral_constant<int,1>) {
   return [p](h5::group F, std::string const &Name) { std::string s; h5_read(F,Name, s); *p = triqs::deserialize<T>(s); };
  }
  template<typename T> h5_rw_lambda_t make_h5_write_impl(const T * p, std::integral_constant<int,1>) { 
   return 
    [p](h5::group F, std::string const &Name) {h5_write(F,Name, triqs::serialize(*p)); 
    F.write_string_attribute(Name,"TRIQS_HDF5_data_scheme",get_triqs_hdf5_data_scheme(*p));
   };
  }

  // do_nothing 
  template<typename T> h5_rw_lambda_t make_h5_read_impl (T * p, std::integral_constant<int,2>) { return h5_rw_lambda_t();}
  template<typename T> h5_rw_lambda_t make_h5_write_impl(T * p, std::integral_constant<int,2>) { return h5_rw_lambda_t();}

 }//details

 // Generate a lambda (h5::group, Name) -> void 
 // that calls h5_read/write if it exists, and reverts to boost serialization into a string otherwise
 template<typename T> h5_rw_lambda_t make_h5_write(T * p) { return details::make_h5_write_impl(p,std::integral_constant<int,has_h5_write<T>::value?0:1>());}
 template<typename T> h5_rw_lambda_t make_h5_read (T * p) { return details::make_h5_read_impl (p,std::integral_constant<int,has_h5_read <T>::value?0:1>());}

 // Generate a lambda (h5::group, Name) -> void 
 // that calls h5_read/write if it exists, and does nothing otherwise
 template<typename T> h5_rw_lambda_t make_h5_write_or_nothing(T * p) { return details::make_h5_write_impl(p,std::integral_constant<int,has_h5_write<T>::value?0:2>());}
 template<typename T> h5_rw_lambda_t make_h5_read_or_nothing (T * p) { return details::make_h5_read_impl (p,std::integral_constant<int,has_h5_read <T>::value?0:2>());}

}}// end namespace
#endif



