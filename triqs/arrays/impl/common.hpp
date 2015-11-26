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
#ifndef TRIQS_ARRAYS_IMPL_COMMON_H
#define TRIQS_ARRAYS_IMPL_COMMON_H
#include <triqs/utility/first_include.hpp>
#include <triqs/clef.hpp>

/// Maximum dimension of the arrays
#define ARRAY_NRANK_MAX 10

#include <exception> 
#include <assert.h>
#include <triqs/utility/exceptions.hpp>
#include <sstream>
#include <type_traits>

template <char C> using char_ = std::integral_constant<char, C>;

#include <triqs/utility/is_complex.hpp>
#include <triqs/utility/macros.hpp>
// LEADS to an error on OS X???
//#include <triqs/utility/c14.hpp>
#include "./traits.hpp"

namespace boost { namespace serialization { class access;}}

#ifdef TRIQS_ARRAYS_DEBUG
#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#endif

namespace triqs { 

 typedef unsigned long long ull_t;
 
 /// Makes a view
 template<typename A> typename A::view_type make_view(A const & x) { return typename A::view_type(x);}
 template<typename A> typename A::const_view_type make_const_view(A const & x) { return typename A::const_view_type(x);}

 /// Makes a clone
 template<typename A> typename A::regular_type make_clone(A const & x) { return typename A::regular_type(x);}

 namespace arrays {
  using triqs::make_clone;

  /// Is the data contiguous
  template<typename A> TYPE_DISABLE_IFC(bool, is_amv_value_or_view_class<A>::value) has_contiguous_data(A const &) {return false;}
  template<typename A> TYPE_ENABLE_IFC(bool, is_amv_value_class<A>::value) has_contiguous_data(A const &) {return true;}
  template<typename A> TYPE_ENABLE_IFC(bool, is_amv_view_class<A>::value) has_contiguous_data(A const & v){return v.indexmap().is_contiguous();}

  template< typename A> 
   ENABLE_IF(is_amv_view_class<A>)
   resize_or_check_if_view ( A & a, typename A::shape_type const & sha) { 
    if (a.shape()!=sha) TRIQS_RUNTIME_ERROR<< "Size mismatch : view class shape = "<<a.shape() << " expected "<<sha;
   }

  template< typename A> 
   ENABLE_IF(is_amv_value_class<A>)
   resize_or_check_if_view ( A & a, typename A::shape_type const & sha) { if (a.shape()!=sha) a.resize(sha); }
 }}//namespace triqs::arrays
#endif

