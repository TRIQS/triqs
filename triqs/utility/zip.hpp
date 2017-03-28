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

  namespace details {

   template <typename... It> struct zip_iter {

    std::tuple<It...> its;
    template <typename... U> void nop(U&&... u) {} // do nothing...
    using seq_t = std::make_index_sequence<sizeof...(It)>;
   
    template <size_t... Is> void increment_all(std::index_sequence<Is...>) { nop(++std::get<Is>(its)...); }

    template <typename F, size_t... Is> auto tie_map_impl(F const& f, std::index_sequence<Is...>) const {
     return std::tie(f(std::get<Is>(its))...);
    }

    public:
    // using difference_type   = typename _iter::Difference;
    using value_type        = std::tuple<typename std::iterator_traits<It>::value_type...>;
    using pointer           = value_type*;
    using reference         = value_type&;
    using iterator_category = std::forward_iterator_tag; 

    zip_iter& operator++() {
     increment_all(seq_t{});
     return *this;
    }

    zip_iter operator++(int) {
     zip_iter c = *this;
     increment_all(seq_t{});
     return c;
    }

    bool operator==(zip_iter const& other) const { return (its == other.its); }
    bool operator!=(zip_iter const& other) const { return (!operator==(other)); }

    std::tuple<typename It::reference...> operator*() const {
     return tie_map_impl([](auto&& x) -> decltype(auto) { return *x; }, seq_t{});
    }
    auto operator-> () const { return operator*(); }
   };

   // RM THIS
   template <typename... It> zip_iter<std::decay_t<It>...> make_zip_iter(std::tuple<It...>&& x) { return {std::move(x)}; }

   template <typename... T> struct zipped {
    std::tuple<T...> tu; // T can be a ref.
    using seq_t = std::make_index_sequence<sizeof...(T)>;

    template <typename F, size_t... Is> auto tuple_map(F&& f, std::index_sequence<Is...>) {
     return make_zip_iter(std::make_tuple(f(std::get<Is>(tu))...));
    }

    auto begin() {
     return tuple_map([](auto&& x) { return std::begin(x); }, seq_t{});
    }
    auto end() {
     return tuple_map([](auto&& x) { return std::end(x); }, seq_t{});
    }
   };

  } // namespace details

  // RM : template deduction
  template <typename... T> details::zipped<T...> zip(T&&... x) { return {std::tie(std::forward<T>(x)...)}; }
 }
}
