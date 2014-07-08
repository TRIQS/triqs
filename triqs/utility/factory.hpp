/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#ifndef TRIQS_UTILITY_FACTORY_H
#define TRIQS_UTILITY_FACTORY_H
#include <type_traits>
#include <vector>
#include "./macros.hpp"
#include "./std_vector_expr_template.hpp"

namespace triqs { namespace utility {

 template <typename T>
  struct factories {
   template < typename U>  static T invoke( U && x) { return T(std::forward<U>(x));}
  };

 template <typename T>
  struct factories <std::vector<T>>{
   typedef std::vector<T> R;

   static R invoke(R && x)      { return R(std::move(x));}

   static R invoke(R const & x) { return R(x);}

   static R invoke(R & x) { return R(x);}

   template <typename U> static R invoke( std::vector<U> && v) {
    static_assert(std::is_constructible<T,U>::value, "Cannot make std::vector<T> from std::vector<U>");
    R r; r.reserve(v.size());
    for (auto & x : v) r.push_back(std::move(x));
    return r;
   }

   template <typename VectorType>
    static TYPE_ENABLE_IF(R,ImmutableStdVector<VectorType>) invoke( VectorType & v) {
     static_assert(std::is_constructible<T,decltype(v[0])>::value, "Cannot make std::vector<T> from the proposed type");
     R r; r.reserve(v.size());
     for(size_t i=0; i<v.size(); ++i) r.push_back(T(v[i]));
     return r;
    }
   /*
      template <typename U> static R invoke( std::vector<U> const & v) {
      static_assert(std::is_constructible<T,U>::value, "Cannot make std::vector<T> from std::vector<U>");
      R r; r.reserve(v.size());
      for (auto & x : v) r.push_back(T(x));
      return r;
      }
      */
  };

 template <typename T, typename ... U> T factory(U && ... x) { return factories<T>::invoke(std::forward<U>(x)...);}

 // redondant : done with x =factory<T>(x)
 //
 /* template <typename T>
    struct gal_assign {
    template < typename U> static void invoke(T & x, U && y) { x = std::forward<U>(y);}
    };

    template <typename T>
    struct gal_assign<std::vector<T>> {
    typedef std::vector<T> R;
    static void invoke(R & x, R && y) { x=std::move(y);}
    static void invoke(R & x, R const & y) { x=y;}
    template <typename U> static R invoke( R &r, std::vector<U> && v) {
    static_assert(std::is_constructible<T,U>::value, "Cannot assign std::vector<U> && -> std::vector<T>");
    r.clear(); r.reserve(v.size());
    for (auto & x : v) r.push_back(std::move(x));
    }
    template <typename U> static R invoke( R &r, std::vector<U> const & v) {
    static_assert(std::is_constructible<T,U>::value, "Cannot assign std::vector<U> const & -> std::vector<T>");
    r.clear(); r.reserve(v.size());
    for (auto & x : v) r.push_back(x);
    }
    };
    */

}}//namespace triqs
#endif
