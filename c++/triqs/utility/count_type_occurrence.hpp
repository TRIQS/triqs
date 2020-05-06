/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#ifndef TRIQS_UTILITY_COUNT_OCCUR_H
#define TRIQS_UTILITY_COUNT_OCCUR_H
namespace triqs {

  // count_type_occurrence<T, Args...>::value is the number of Args deriving or equal to T

  template <typename T, typename... A> struct count_type_occurrence;

  template <typename T, typename A0, typename... A>
  struct count_type_occurrence<T, A0, A...> : std::integral_constant<int, std::is_base_of<T, A0>::value + count_type_occurrence<T, A...>::value> {};

  template <typename T> struct count_type_occurrence<T> : std::integral_constant<int, 0> {};

  template <typename T, typename... A> struct count_type_occurrence_not;

  template <typename T, typename A0, typename... A>
  struct count_type_occurrence_not<T, A0, A...>
     : std::integral_constant<int, !(std::is_base_of<T, A0>::value) + count_type_occurrence_not<T, A...>::value> {};

  template <typename T> struct count_type_occurrence_not<T> : std::integral_constant<int, 0> {};

} // namespace triqs
#endif
