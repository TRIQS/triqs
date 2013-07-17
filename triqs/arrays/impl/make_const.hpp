
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

#ifndef TRIQS_ARRAYS_MAKE_CONST_H
#define TRIQS_ARRAYS_MAKE_CONST_H
#include <boost/mpl/if.hpp>
#include <boost/type_traits/add_const.hpp>

namespace triqs { namespace arrays {

 namespace details { 
  /*
   * make_const_type : make the type constant.
   * For a storage, it transforms it into a storage<const Valuetype>
   */
  template<bool Const, typename T> struct make_const_type : boost::mpl::if_c<Const, typename boost::add_const<T>::type, T> {};
 }
}}//namespace triqs::arrays 
#endif

