/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by H. Hafermann, O. Parcollet
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
#ifndef TRIQS_PYTHON_TOOLS_INSERTER_MAP_H
#define TRIQS_PYTHON_TOOLS_INSERTER_MAP_H
#include <sstream>
#include <string>

namespace triqs { namespace python_tools {

 template<typename T, typename U> struct inserter_in_map {
  T * p;
  inserter_in_map(T & x) : p(&x) {} 
  void operator()(std::string const & key, U const & x) { (*p)[key] = x; }
 };

 template<typename T, typename U> struct inserter_in_map2 {
  T * p;
  inserter_in_map2(T & x) : p(&x) {} 
  void operator()(std::string const & key, PyObject * ob) { (*p)[key] = U(ob); }
 };

 template<typename T> struct printer { 
  std::string operator()(T const & x) { std::stringstream fs; fs<<x; return fs.str();}
 };

}}
#endif
