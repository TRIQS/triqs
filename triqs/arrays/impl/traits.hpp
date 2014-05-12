/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by O. Parcollet
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
#ifndef TRIQS_ARRAYS_IMPL_TRAITS_H
#define TRIQS_ARRAYS_IMPL_TRAITS_H
#include <triqs/utility/concept_tools.hpp>
#include <triqs/utility/traits.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>

namespace triqs {
namespace arrays {
 namespace mpl = boost::mpl;

 // The ImmutableCuboidArray concept
 TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT(ImmutableCuboidArray);
 TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT_R(MutableCuboidArray, (ImmutableCuboidArray));

 // The ImmutableArray concept
 TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT_R(ImmutableArray, (ImmutableCuboidArray));
 TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT_R(MutableArray, (ImmutableArray)(MutableCuboidArray));

 // The ImmutableMatrix concept
 TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT_R(ImmutableMatrix, (ImmutableCuboidArray));
 TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT_R(MutableMatrix, (ImmutableMatrix)(MutableCuboidArray));

 // The ImmutableVector concept
 TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT_R(ImmutableVector, (ImmutableCuboidArray));
 TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT_R(MutableVector, (ImmutableVector)(MutableCuboidArray));

 namespace Tag {
  struct array {};
  struct array_view {};
  struct vector {};
  struct vector_view {};
  struct matrix_view {};
  struct matrix {};
 }
 template <typename T> struct is_array : std::is_base_of<Tag::array, T> {};
 template <typename T> struct is_array_view : std::is_base_of<Tag::array_view, T> {};
 template <typename T> struct is_array_or_view : _or<is_array<T>, is_array_view<T>> {};

 template <typename T> struct is_vector : std::is_base_of<Tag::vector, T> {};
 template <typename T> struct is_vector_view : std::is_base_of<Tag::vector_view, T> {};
 template <typename T> struct is_vector_or_view : _or<is_vector<T>, is_vector_view<T>> {};

 template <typename T> struct is_matrix : std::is_base_of<Tag::matrix, T> {};
 template <typename T> struct is_matrix_view : std::is_base_of<Tag::matrix_view, T> {};
 template <typename T> struct is_matrix_or_view : _or<is_matrix<T>, is_matrix_view<T>> {};

 template <class T> struct is_amv_value_class : _or<is_array<T>, is_matrix<T>, is_vector<T>> {};
 template <class T> struct is_amv_view_class : _or<is_array_view<T>, is_matrix_view<T>, is_vector_view<T>> {};
 template <class T> struct is_amv_value_or_view_class : _or<is_amv_value_class<T>, is_amv_view_class<T>> {};

 template <class S> struct is_scalar : _or<std::is_arithmetic<S>, triqs::is_complex<S>> {};

 template <class S, class A>
 struct is_scalar_for
     : std::conditional<is_scalar<typename A::value_type>::value, is_scalar<S>, std::is_same<S, typename A::value_type>>::type {
 };
}
} // namespace triqs::arrays
#endif

