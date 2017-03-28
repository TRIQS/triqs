/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2017 by O. Parcollet
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
#include "./first_include.hpp"
#include <iterator>

namespace triqs {
 namespace utility {

  // enumerate
  namespace details {

   template <typename Iter> struct enum_iter : std::iterator_traits<Iter> {

    Iter it;
    long i = 0;
    // using _iter = std::iterator_traits<Iter>;

    public:
    // FIXME !!
    // using difference_type   = typename _iter::Difference;
    using value_type    = std::pair<long, typename std::iterator_traits<Iter>::value_type>;
    using pointer       = value_type*;
    using reference     = value_type&;
    // using iterator_category = typename _iter::iterator_category;

    enum_iter(Iter it) : it(it) {}

    enum_iter& operator++() {
     ++it;
     ++i;
     return *this;
    }

    enum_iter operator++(int) {
     enum_iter c = *this;
     ++it;
     ++i;
     return c;
    }

    bool operator==(enum_iter const& other) const { return (it == other.it); }
    bool operator!=(enum_iter const& other) const { return (!operator==(other)); }

    //auto operator*() const { return std::tuple < long, std::decay_t<decltype(*it)>> {i, *it}; }
    //auto operator*() const { return std::tuple < long, decltype(*it)> {i, *it}; }
    auto operator*() const { return std::tie(i, *it); }
    auto operator-> () const { return operator*(); }
   };

   // RM THIS
   template <typename Iter> enum_iter<std::decay_t<Iter>> make_enum_iter(Iter&& x) { return {std::forward<Iter>(x)}; }

   // enumerating a forward iterator
   template <typename T> struct enumerated {
    T x;
    auto begin() { return make_enum_iter(std::cbegin(x)); }
    auto end() { return make_enum_iter(std::cend(x)); }
    auto cbegin() { return make_enum_iter(std::cbegin(x)); }
    auto cend() { return make_enum_iter(std::cend(x)); }
    };

   // enumerating a forward iterator
   template <typename T> struct enumerated<std::reference_wrapper<T>> {
    std::reference_wrapper<T> x;
    auto begin() { return make_enum_iter(std::begin(x.get())); }
    auto end() { return make_enum_iter(std::end(x.get())); }
    auto cbegin() { return make_enum_iter(std::cbegin(x.get())); }
    auto cend() { return make_enum_iter(std::cend(x.get())); }
    };

  } // namespace details

  // RM : template deduction
  template <typename T> details::enumerated<T> enumerate(T&& x) { return {std::forward<T>(x)}; }
 }
}
