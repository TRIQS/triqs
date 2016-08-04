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
#pragma once
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>

#define TRIQS_CONCEPT_TAG_NAME(MyBeautifulConcept) MyBeautifulConcept##__concept_tag


#define TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT(MyBeautifulConcept) \
 struct TRIQS_CONCEPT_TAG_NAME(MyBeautifulConcept) {};\
 template<typename T> struct MyBeautifulConcept : std::is_base_of<TRIQS_CONCEPT_TAG_NAME(MyBeautifulConcept) , T> {};\
 template<typename T> struct MyBeautifulConcept<const T> : MyBeautifulConcept<T>{};\
 template<typename T> struct MyBeautifulConcept<T&> : MyBeautifulConcept<T>{};\
 template<typename T> struct MyBeautifulConcept<T&&> : MyBeautifulConcept<T>{};\
 template<typename T> constexpr bool MyBeautifulConcept##_v = std::is_base_of<TRIQS_CONCEPT_TAG_NAME(MyBeautifulConcept) , typename std::decay<T>::type>::value;

#define TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT_R_AUX(r, data, i, elem) BOOST_PP_COMMA_IF(i) TRIQS_CONCEPT_TAG_NAME(elem) 

#define TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT_R(MyBeautifulConcept,Rs) \
 struct TRIQS_CONCEPT_TAG_NAME(MyBeautifulConcept) : BOOST_PP_SEQ_FOR_EACH_I (TRIQS_DEFINE_CONCEPT_AND_ASSOCIATED_TRAIT_R_AUX,nil,Rs) {};\
 template<typename T> struct MyBeautifulConcept : std::is_base_of<TRIQS_CONCEPT_TAG_NAME(MyBeautifulConcept), T> {};\
 template<typename T> struct MyBeautifulConcept<const T>  : MyBeautifulConcept<T>{};\
 template<typename T> struct MyBeautifulConcept<T&>  : MyBeautifulConcept<T>{};\
 template<typename T> struct MyBeautifulConcept<T&&>  : MyBeautifulConcept<T>{};\
 template<typename T> constexpr bool MyBeautifulConcept##_v = std::is_base_of<TRIQS_CONCEPT_TAG_NAME(MyBeautifulConcept), typename std::decay<T>::type>::value;


