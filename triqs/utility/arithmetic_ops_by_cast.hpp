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
#pragma once
namespace triqs {
namespace utility {

 // Derive from this object using CRTP to provide arithmetic operation by casting the final object to C
 // by not forwarding x, I assume the cast is a simple value, not a matrix, but this is ok
 template <typename Derived, typename C> struct arithmetic_ops_by_cast {};

#define IMPL_OP(OP)                                                                                                              \
 template <typename D, typename C, typename Y>                                                                                   \
 auto operator OP(arithmetic_ops_by_cast<D, C> const &x, Y &&y)->decltype(std::declval<C>() OP std::forward<Y>(y)) {             \
  return C(static_cast<D const &>(x)) OP std::forward<Y>(y);                                                                     \
 }                                                                                                                               \
 template <typename D, typename C, typename Y>                                                                                   \
 auto operator OP(Y &&y, arithmetic_ops_by_cast<D, C> const &x)                                                                  \
     ->TYPE_DISABLE_IF(decltype(std::forward<Y>(y) OP std::declval<C>()),                                                        \
                       std::is_base_of<D, typename std::remove_cv<typename std::remove_reference<Y>::type>::type>) {             \
  return std::forward<Y>(y) OP C(static_cast<D const &>(x));                                                                     \
 }

 IMPL_OP(+);
 IMPL_OP(-);
 IMPL_OP(*);
 IMPL_OP(/ );
#undef IMPL_OP

 // Same thing, but disallow the operations with the type itself
 template <typename Derived, typename C> struct arithmetic_ops_by_cast_disable_same_type {};
#define IMPL_OP(OP)                                                                                                              \
 template <typename D, typename C, typename Y>                                                                                   \
 auto operator OP(arithmetic_ops_by_cast_disable_same_type<D, C> const &x, Y &&y)                                                \
     ->TYPE_DISABLE_IF(decltype(std::declval<C>() OP std::forward<Y>(y)),                                                        \
                       std::is_base_of<D, typename std::remove_cv<typename std::remove_reference<Y>::type>::type>) {             \
  return C(static_cast<D const &>(x)) OP std::forward<Y>(y);                                                                     \
 }                                                                                                                               \
 template <typename D, typename C, typename Y>                                                                                   \
 auto operator OP(Y &&y, arithmetic_ops_by_cast_disable_same_type<D, C> const &x)                                                \
     ->TYPE_DISABLE_IF(decltype(std::forward<Y>(y) OP std::declval<C>()),                                                        \
                       std::is_base_of<D, typename std::remove_cv<typename std::remove_reference<Y>::type>::type>) {             \
  return std::forward<Y>(y) OP C(static_cast<D const &>(x));                                                                     \
 }
 IMPL_OP(+);
 IMPL_OP(-);
 IMPL_OP(*);
 IMPL_OP(/ );
#undef IMPL_OP
}
}

