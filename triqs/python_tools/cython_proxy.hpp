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
#ifndef TRIQS_PYTHON_CYTHON_PROXY_H
#define TRIQS_PYTHON_CYTHON_PROXY_H
#include <utility>

namespace triqs { namespace python_tools {  

 // Wrap a C++ view into an object which has *value* semantics
 // operator = remapped to <<=
 // This class must be friend of V, to use the private default constructor.
 template<typename V> class cython_proxy : public V {
  public:
  cython_proxy() : V(typename V::non_view_type()) {}
  cython_proxy(V const &v) : V(v){};
  cython_proxy(cython_proxy const & cp) : V(cp) {};
  template<typename ... Args> explicit cython_proxy(Args && ... args) : V (std::forward<Args>(args)...){}
  cython_proxy & operator = ( cython_proxy const & cp ) { this->rebind(cp); return *this;}
  cython_proxy & operator = ( V            const & v  ) { this->rebind(v);  return *this;}
  template<typename V2> void operator << ( V2 && v) { V::operator=( std::forward<V2>(v) );} 
  };


}}//namespace triqs
#endif

