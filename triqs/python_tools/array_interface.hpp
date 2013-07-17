/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
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
#ifndef TRIQS_ARRAY_INTERFACE_H
#define TRIQS_ARRAY_INTERFACE_H

#include <triqs/arrays.hpp>
#include <triqs/python_tools/cython_proxy.hpp>

namespace triqs { namespace arrays {

 template<class T, int R, ull_t Opt=0, ull_t To=0>
 struct array_c: public triqs::python_tools::cython_proxy<array_view<T,R,Opt,To> > {

  typedef triqs::python_tools::cython_proxy<array_view<T,R,Opt,To>> B;
  array_c(): B() {}
  array_c(array_c const &a): B(a) {};
  array_c(array_view<T,R,Opt,To> const &a): B(a) {};
  template<typename ... Args> explicit array_c(Args && ... args): B(std::forward<Args>(args)...) {}

 };

 template<class T, ull_t Opt=0, ull_t To=0>
 struct matrix_c: public triqs::python_tools::cython_proxy<matrix_view<T,Opt,To>> {

  typedef triqs::python_tools::cython_proxy<matrix_view<T,Opt,To>> B;
  matrix_c(): B() {}
  matrix_c(matrix_c const &a): B(a) {};
  matrix_c(matrix_view<T,Opt,To> const &a): B(a) {};
  template<typename ... Args> explicit matrix_c(Args && ... args): B(std::forward<Args>(args)...) {}

 };

 template<class T, ull_t Opt=0>
 struct vector_c: public triqs::python_tools::cython_proxy<vector_view<T,Opt>> {

  typedef triqs::python_tools::cython_proxy<vector_view<T,Opt>> B;
  vector_c(): B() {}
  vector_c(vector_c const &a): B(a) {};
  vector_c(vector_view<T,Opt> const &a): B(a) {};
  template<typename ... Args> explicit vector_c(Args && ... args): B(std::forward<Args>(args)...) {}

 };


}}//namespace triqs
#endif

