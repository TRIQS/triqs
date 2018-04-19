
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
#include <tuple>
#include <vector>
#include <iterator>

namespace triqs::utility {

  template <typename R> auto make_vector_from_range(R &&r) {
    std::vector<std::decay_t<decltype(*(std::begin(r)))>> vec;
    for (auto const &x : r) vec.push_back(x);
    return vec;
  }

  namespace details {

    /********************* Some helpers  ********************/

    // By default, get mutable iterators
    template <typename T> auto _begin(T& range) { return std::begin(range); }
    template <typename T> auto _end(T& range) { return std::end(range); }

    // Always return iterator to const for cbegin/cend
    template <typename T> auto _cbegin(T const& range) { return std::cbegin(range); }
    template <typename T> auto _cend(T const& range) { return std::cend(range); }

    // Sentinel_t, used to denote the end of a range for e.g. product
    template <typename It> struct sentinel_t { It it; };
    template <typename It> sentinel_t<It> make_sentinel(It&& it) { return {it}; }

    /********************* Iterator Facade  ********************/

    template <typename Iter> struct iterator_facade {

      Iter& self() { return static_cast<Iter&>(*this); }
      Iter const& self() const { return static_cast<const Iter&>(*this); }

      Iter& operator++() {
        self().next();
        return self();
      }

      Iter operator++(int) {
        Iter c = *this;
        self().next();
        return c;
      }

      template <typename U> bool operator==(U const& other) const { return self().equal(other); }
      template <typename U> bool operator!=(U const& other) const { return (!operator==(other)); }

      decltype(auto) operator*() { return self().dereference(); }
      decltype(auto) operator*() const { return self().dereference(); }
      decltype(auto) operator-> () { return operator*(); }
      decltype(auto) operator-> () const { return operator*(); }
    };

    /********************* Transform ********************/

    template <typename Iter, typename L> struct transform_iter : iterator_facade<transform_iter<Iter, L>> {

      Iter it;
      mutable L lambda;

      // All usings required by std::iterator_traits<..>
      using difference_type   = std::ptrdiff_t;
      using value_type        = decltype(lambda(*it));
      using pointer           = value_type*;
      using reference         = value_type&;
      using iterator_category = std::forward_iterator_tag;

      transform_iter(Iter it, L lambda) : it(std::move(it)), lambda(std::move(lambda)) {}

      void next() { ++it; }

      bool equal(transform_iter const& other) const { return (it == other.it); }

      decltype(auto) dereference() const { return lambda(*it); }
    };

    // ---------------------------------------------

    template <typename Iter, typename L>
    transform_iter<std::decay_t<Iter>, std::decay_t<L>> make_transform_iter(Iter&& x, L&& lambda) {
      return {std::forward<Iter>(x), std::forward<L>(lambda)};
    }

    template <typename T, typename L> struct transformed {
      T x;
      L lambda;
      auto begin() const { return make_transform_iter(std::cbegin(x), lambda); } // FIXME c++17 remove maker
      auto end() const { return make_transform_iter(std::cend(x), lambda); }
      auto cbegin() const { return make_transform_iter(std::cbegin(x), lambda); }
      auto cend() const { return make_transform_iter(std::cend(x), lambda); }
    };

    /********************* Enumerate ********************/

    template <typename Iter> struct enum_iter : iterator_facade<enum_iter<Iter>> {

      Iter it;
      long i = 0;

      using difference_type   = std::ptrdiff_t;
      using value_type        = std::pair<long, typename std::iterator_traits<Iter>::value_type>;
      using pointer           = value_type*;
      using reference         = value_type&;
      using iterator_category = std::forward_iterator_tag;

      enum_iter(Iter it) : it(std::move(it)) {}

      void next() {
        ++it;
        ++i;
      }

      bool equal(enum_iter const& other) const { return (it == other.it); }

      decltype(auto) dereference() const { return std::tuple<long, decltype(*it)>{i, *it}; }
      decltype(auto) dereference() { return std::tuple<long, decltype(*it)>{i, *it}; }
    };

    // ---------------------------------------------

    template <typename Iter> enum_iter<std::decay_t<Iter>> make_enum_iter(Iter&& x) { return {std::forward<Iter>(x)}; }

    template <typename T> struct enumerated {

      T x;
      auto begin() { return make_enum_iter(_begin(x)); }
      auto end() { return make_enum_iter(_end(x)); }
      auto begin() const { return make_enum_iter(_cbegin(x)); }
      auto end() const { return make_enum_iter(_cend(x)); }
      auto cbegin() const { return make_enum_iter(_cbegin(x)); }
      auto cend() const { return make_enum_iter(_cend(x)); }
    };

    /********************* Zip ********************/

    template <typename... It> struct zip_iter : iterator_facade<zip_iter<It...>> {

      std::tuple<It...> its;
      zip_iter(std::tuple<It...> its) : its(std::move(its)) {}

      using difference_type   = std::ptrdiff_t;
      using value_type        = std::tuple<typename std::iterator_traits<It>::value_type...>;
      using pointer           = value_type*;
      using reference         = value_type&;
      using iterator_category = std::forward_iterator_tag;

      template <typename... U> void nop(U&&... u) {} // do nothing...
      template <size_t... Is> void increment_all(std::index_sequence<Is...>) { nop(++std::get<Is>(its)...); }
      void next() { increment_all(std::index_sequence_for<It...>{}); }

      bool equal(zip_iter const& other) const { return (its == other.its); }

      template <size_t... Is> auto tuple_map_impl(std::index_sequence<Is...>) {
        return std::tuple<decltype(*std::get<Is>(its))...>(*std::get<Is>(its)...);
	// return std::forward_as_tuple(*std::get<Is>(its)...); // ?
      }
      decltype(auto) dereference() { return tuple_map_impl(std::index_sequence_for<It...>{}); }
    };

    // ---------------------------------------------

    template <typename... It> zip_iter<std::decay_t<It>...> make_zip_iter(std::tuple<It...>&& x) { return {std::move(x)}; }

    template <typename... T> struct zipped {

      std::tuple<T...> tu; // T can be a ref.
      using seq_t = std::index_sequence_for<T...>;

      template <typename... U> zipped(U&&... ranges) : tu{std::forward<U>(ranges)...} {}

      // Apply function to tuple
      template <typename F, size_t... Is> auto tuple_map(F&& f, std::index_sequence<Is...>) {
        return make_zip_iter(std::make_tuple(f(std::get<Is>(tu))...));
      }

      auto begin() {
        return tuple_map([](auto&& x) { return _begin(x); }, seq_t{});
      }
      auto end() {
        return tuple_map([](auto&& x) { return _end(x); }, seq_t{});
      }
      auto begin() const {
        return tuple_map([](auto&& x) { return _cbegin(x); }, seq_t{});
      }
      auto end() const {
        return tuple_map([](auto&& x) { return _cend(x); }, seq_t{});
      }
      auto cbegin() const { return begin(); }
      auto cend() const { return end(); }
    };

    /********************* Product ********************/

    template <typename... It> struct prod_iter : iterator_facade<prod_iter<It...>> {

      std::tuple<It...> its_begin, its_end, its;
      prod_iter(std::tuple<It...> its_begin, std::tuple<It...> its_end)
         : its_begin(std::move(its_begin)), its_end(std::move(its_end)), its(its_begin) {}

      using difference_type   = std::ptrdiff_t;
      using value_type        = std::tuple<typename std::iterator_traits<It>::value_type...>;
      using pointer           = value_type*;
      using reference         = value_type&;
      using iterator_category = std::forward_iterator_tag;

      template <int N> void _next() {
        ++std::get<N>(its);
        if
          constexpr(N < sizeof...(It) - 1) {
            if (std::get<N>(its) == std::get<N>(its_end)) {
              std::get<N>(its) = std::get<N>(its_begin);
              _next<N + 1>();
            }
          }
      }
      void next() { _next<0>(); }

      bool equal(prod_iter const& other) const { return (its == other.its); }
      template <typename U> bool equal(sentinel_t<U> const& s) const { return (s.it == std::get<sizeof...(It) - 1>(its)); }

      template <size_t... Is> auto tuple_map_impl(std::index_sequence<Is...>) const {
        return std::tuple<decltype(*std::get<Is>(its))...>(*std::get<Is>(its)...);
      }
      decltype(auto) dereference() const { return tuple_map_impl(std::index_sequence_for<It...>{}); }
    };

    // ---------------------------------------------

    template <typename... It>
    prod_iter<std::decay_t<It>...> make_prod_iter(std::tuple<It...>&& its, std::tuple<It...>&& its_end) {
      return {std::move(its), std::move(its_end)};
    }

    template <typename... T> struct multiplied {
      std::tuple<T...> tu; // T can be a ref.

      template <typename... U> multiplied(U&&... ranges) : tu{std::forward<U>(ranges)...} {}

      template <size_t... Is> auto _begin(std::index_sequence<Is...>) const {
        return make_prod_iter(std::make_tuple(std::cbegin(std::get<Is>(tu))...), std::make_tuple(std::cend(std::get<Is>(tu))...));
      }

      auto begin() const { return _begin(std::index_sequence_for<T...>{}); }
      auto end() const { return make_sentinel(std::cend(std::get<sizeof...(T) - 1>(tu))); }
      auto cbegin() const { return _begin(std::index_sequence_for<T...>{}); }
      auto cend() const { return make_sentinel(std::cend(std::get<sizeof...(T) - 1>(tu))); }
    };

    template <typename... T> multiplied(T &&...) -> multiplied<std::decay_t<T>...>;

  } // namespace details

  /***************************************************/

  /**
   * Transform (lazy)applies a unary lambda function to every
   * element of a range. It returns itself a range.
   *
   * @param range The range that the lambda is applied to
   * @param range The lambda to apply to the range
   */
  template <typename T, typename L> auto transform(T&& range, L&& lambda) {
    return details::transformed<T, L>{std::forward<T>(range), std::forward<L>(lambda)};
  }

  /**
   * Lazy-enumerate a range.
   * This function returns itself a range of tuple<int, T>
   *
   * @param range The range to enumerate
   */
  template <typename T> auto enumerate(T&& range) { return details::enumerated<T>{std::forward<T>(range)}; }

  /**
   * Lazy-zip a range.
   * This function returns itself a range of tuple<T...>
   *
   * @param ranges The ranges to zip. Note: They have to be of equal length!
   */
  template <typename... T> auto zip(T&&... ranges) { return details::zipped<T...>{std::forward<T>(ranges)...}; }
  template <typename... T, typename L> auto zip_with(T&&... ranges, L&& lambda) {
    return transform(
        zip(std::forward<T>(ranges)...), [lambda](std::tuple<T...> t) { return std::apply(lambda, t); });  // FIXME Check
  }

  /**
   * Lazy-product of multiple ranges. This function returns itself a range of tuple<T...>.
   * Iterating over it will yield all combinations of the different range values.
   * Note: The ranges are incremented beginning with the leftmost range.
   *
   * @tparam T The types of the different ranges
   * @param ranges The ranges to zip. Note: They have to be of equal length!
   */
  template <typename... T> auto product(T &&... ranges) { return details::multiplied<T...>{std::forward<T>(ranges)...}; }

  template <typename T, size_t N, size_t... Is> auto _make_product_impl(std::array<T, N> & arr, std::index_sequence<Is...>) {
    static_assert(N == sizeof...(Is));
    return details::multiplied{arr[Is]...};
  }
  template <typename T, size_t N> auto make_product(std::array<T, N> & arr) {
    return _make_product_impl(arr, std::make_index_sequence<N>{});
  }

} // namespace triqs::utility
