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

namespace triqs {
  namespace arrays {

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
      struct array_view : is_view_tag {};
      struct vector {};
      struct vector_view : is_view_tag {};
      struct matrix_view : is_view_tag {};
      struct matrix {};
    } // namespace Tag
    template <typename T> struct is_array : std::is_base_of<Tag::array, T> {};
    template <typename T> struct is_array_view : std::is_base_of<Tag::array_view, T> {};
    template <typename T> struct is_array_or_view : _or<is_array<T>, is_array_view<T>> {};

    template <typename T> constexpr bool is_array_v = is_array<T>::value;
    template <typename T> constexpr bool is_array_view_v = is_array_view<T>::value;
    template <typename T> constexpr bool is_array_or_view_v = is_array_or_view<T>::value;

    template <typename T> struct is_vector : std::is_base_of<Tag::vector, T> {};
    template <typename T> struct is_vector_view : std::is_base_of<Tag::vector_view, T> {};
    template <typename T> struct is_vector_or_view : _or<is_vector<T>, is_vector_view<T>> {};

    template <typename T> constexpr bool is_vector_v = is_vector<T>::value;
    template <typename T> constexpr bool is_vector_view_v = is_vector_view<T>::value;
    template <typename T> constexpr bool is_vector_or_view_v = is_vector_or_view<T>::value;

    template <typename T> struct is_matrix : std::is_base_of<Tag::matrix, T> {};
    template <typename T> struct is_matrix_view : std::is_base_of<Tag::matrix_view, T> {};
    template <typename T> struct is_matrix_or_view : _or<is_matrix<T>, is_matrix_view<T>> {};

    template <typename T> constexpr bool is_matrix_v = is_matrix<T>::value;
    template <typename T> constexpr bool is_matrix_view_v = is_matrix_view<T>::value;
    template <typename T> constexpr bool is_matrix_or_view_v = is_matrix_or_view<T>::value;

    template <typename T> struct is_amv_value_class : _or<is_array<T>, is_matrix<T>, is_vector<T>> {};
    template <typename T> struct is_amv_view_class : _or<is_array_view<T>, is_matrix_view<T>, is_vector_view<T>> {};
    template <typename T> struct is_amv_value_or_view_class : _or<is_amv_value_class<T>, is_amv_view_class<T>> {};

    template <typename T> constexpr bool is_regular_v = is_amv_value_class<T>::value;
    template <typename T> constexpr bool is_view_v = is_amv_view_class<T>::value;
    template <typename T> constexpr bool is_regular_or_view_v = is_amv_value_or_view_class<T>::value;

    template <class S> struct is_scalar : _or<std::is_arithmetic<S>, triqs::is_complex<S>> {};
    template <class S>
    struct is_scalar_or_convertible : std::integral_constant<bool, is_scalar<S>::value || std::is_constructible<std::complex<double>, S>::value> {};
    template <class S> constexpr bool is_scalar_v = is_scalar<S>::value;

    template <class S, class A>
    struct is_scalar_for
       : std::conditional<is_scalar<typename A::value_type>::value, is_scalar_or_convertible<S>, std::is_same<S, typename A::value_type>>::type {};
  } // namespace arrays
} // namespace triqs
#endif
