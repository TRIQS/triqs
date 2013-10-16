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
#ifndef TRIQS_UTILITY_MACROS_H
#define TRIQS_UTILITY_MACROS_H

#include <triqs/utility/first_include.hpp>
#include <boost/utility/enable_if.hpp>

#define TYPE_ENABLE_IF(Type,...)    typename boost::enable_if < __VA_ARGS__ , Type >::type
#define TYPE_ENABLE_IFC(Type,...)   typename boost::enable_if_c < __VA_ARGS__ , Type >::type
#define TYPE_DISABLE_IF(Type,...)   typename boost::disable_if< __VA_ARGS__ , Type >::type
#define TYPE_DISABLE_IFC(Type,...)  typename boost::disable_if_c< __VA_ARGS__ , Type >::type

#define ENABLE_IF(...)    typename boost::enable_if < __VA_ARGS__ , void >::type
#define ENABLE_IFC(...)   typename boost::enable_if_c < __VA_ARGS__ , void >::type
#define DISABLE_IF(...)   typename boost::disable_if< __VA_ARGS__ , void >::type
#define DISABLE_IFC(...)  typename boost::disable_if_c< __VA_ARGS__ , void >::type

#define DECL_AND_RETURN(...)  -> decltype(__VA_ARGS__) { return __VA_ARGS__;}

namespace triqs { 
 template<typename T> struct remove_cv_ref : std::remove_cv< typename std::remove_reference<T>::type> {};
};

#define TRIQS_CATCH_AND_ABORT catch(std::exception const & e) { std::cout  << e.what()<< std::endl; return 1;}

#endif

