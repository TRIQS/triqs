/*******************************************************************************
 * 
 * Copyright (C) 2015-2018, O. Parcollet
 * Copyright (C) 2019, The Simons Foundation
 *   author : N. Wentzell
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#pragma once
#include <tuple>
#include <vector>
#include <iterator>
#include <iostream>
#include <exception>

namespace itertools {

  template <class Iter, class Value, class Tag = std::forward_iterator_tag, class Reference = Value &, class Difference = std::ptrdiff_t>
  struct iterator_facade;

  /*
   * A helper for the implementation of forward iterators using CRTP
   *
   * @tparam Iter
   * The Iterator Class to be implemented
   * `Iter` is required to have the following member functions
   * - bool equal(Iter other)
   * - value_type [const] [&] dereference()
   * - void increment()
   */
  template <typename Iter, typename Value, typename Reference, typename Difference>
  struct iterator_facade<Iter, Value, std::forward_iterator_tag, Reference, Difference> {

    private:
    Iter &self() { return static_cast<Iter &>(*this); }
    Iter const &self() const { return static_cast<const Iter &>(*this); }

    public:
    using value_type        = Value;
    using reference         = Reference;
    using pointer           = Value *;
    using difference_type   = Difference;
    using iterator_category = std::forward_iterator_tag;

    Iter &operator++() {
      self().increment();
      return self();
    }

    Iter operator++(int) {
      Iter c = self();
      self().increment();
      return c;
    }

    template <typename U>
    bool operator==(U const &other) const {
      return self().equal(other);
    }
    template <typename U>
    bool operator!=(U const &other) const {
      return (!operator==(other));
    }

    decltype(auto) operator*() const { return self().dereference(); }
    decltype(auto) operator-> () const { return operator*(); }
  };

  namespace details {

    /********************* Enumerate Iterator ********************/

    template <typename Iter>
    struct enum_iter : iterator_facade<enum_iter<Iter>, std::pair<long, typename std::iterator_traits<Iter>::value_type>> {

      Iter it;
      long i = 0;

      enum_iter(Iter it) : it(std::move(it)) {}

      void increment() {
        ++it;
        ++i;
      }

      bool equal(enum_iter const &other) const { return (it == other.it); }

      decltype(auto) dereference() const { return std::tuple<long, decltype(*it)>{i, *it}; }
    };

    /********************* Transform Iterator ********************/

    template <typename Iter, typename L, typename Value = std::result_of<L(typename std::iterator_traits<Iter>::value_type)>>
    struct transform_iter : iterator_facade<transform_iter<Iter, L>, Value> {

      Iter it;
      mutable L lambda;

      transform_iter(Iter it, L lambda) : it(std::move(it)), lambda(std::move(lambda)) {}

      void increment() { ++it; }

      bool equal(transform_iter const &other) const { return (it == other.it); }

      decltype(auto) dereference() const { return lambda(*it); }
    };

    /********************* Zip Iterator ********************/

    template <typename... It>
    struct zip_iter : iterator_facade<zip_iter<It...>, std::tuple<typename std::iterator_traits<It>::value_type...>> {

      std::tuple<It...> its;
      zip_iter(std::tuple<It...> its) : its(std::move(its)) {}

      private:
      template <size_t... Is>
      [[gnu::always_inline]] void increment_all(std::index_sequence<Is...>) {
        ((void)(++std::get<Is>(its)), ...);
      }

      public:
      void increment() { increment_all(std::index_sequence_for<It...>{}); }

      bool equal(zip_iter const &other) const { return (its == other.its); }

      template <size_t... Is>
      auto tuple_map_impl(std::index_sequence<Is...>) const {
        return std::tuple<decltype(*std::get<Is>(its))...>(*std::get<Is>(its)...);
      }

      decltype(auto) dereference() const { return tuple_map_impl(std::index_sequence_for<It...>{}); }
    };

    /********************* Product Iterator ********************/

    // Sentinel_t, used to denote the end of a product range
    template <typename It>
    struct sentinel_t {
      It it;
    };
    template <typename It>
    sentinel_t<It> make_sentinel(It &&it) {
      return {it};
    }

    template <typename... It>
    struct prod_iter : iterator_facade<prod_iter<It...>, std::tuple<typename std::iterator_traits<It>::value_type...>> {

      std::tuple<It...> its_begin, its_end;
      std::tuple<It...> its = its_begin;

      prod_iter(std::tuple<It...> its_begin, std::tuple<It...> its_end) : its_begin(std::move(its_begin)), its_end(std::move(its_end)) {}

      template <int N>
      void _increment() {
        ++std::get<N>(its);
        if constexpr (N < sizeof...(It) - 1) {
          if (std::get<N>(its) == std::get<N>(its_end)) {
            std::get<N>(its) = std::get<N>(its_begin);
            _increment<N + 1>();
          }
        }
      }
      void increment() { _increment<0>(); }

      bool equal(prod_iter const &other) const { return (its == other.its); }
      template <typename U>
      bool equal(sentinel_t<U> const &s) const {
        return (s.it == std::get<sizeof...(It) - 1>(its));
      }

      private:
      template <size_t... Is>
      [[gnu::always_inline]] auto tuple_map_impl(std::index_sequence<Is...>) const {
        return std::tuple<decltype(*std::get<Is>(its))...>(*std::get<Is>(its)...);
      }

      public:
      decltype(auto) dereference() const { return tuple_map_impl(std::index_sequence_for<It...>{}); }
    };

    /********************* Stride Iterator ********************/

    template <typename Iter>
    struct stride_iter : iterator_facade<stride_iter<Iter>, typename std::iterator_traits<Iter>::value_type> {

      Iter it;
      std::ptrdiff_t stride;

      stride_iter(Iter it, std::ptrdiff_t stride) : it(it), stride(stride) {
        if (stride <= 0) throw std::runtime_error("strided range requires a positive stride");
      }

      void increment() { std::advance(it, stride); }
      bool equal(stride_iter const &other) const { return (it == other.it); }
      decltype(auto) dereference() const { return *it; }
    };

    /********************* The Wrapper Classes representing the adapted ranges ********************/

    template <typename T, typename L>
    struct transformed {
      T x;
      L lambda;

      using const_iterator = transform_iter<decltype(std::cbegin(x)), L>;
      using iterator       = const_iterator;

      const_iterator cbegin() const noexcept { return {std::cbegin(x), lambda}; }
      const_iterator begin() const noexcept { return cbegin(); }

      const_iterator cend() const noexcept { return {std::cend(x), lambda}; }
      const_iterator end() const noexcept { return cend(); }
    };

    // ---------------------------------------------

    template <typename T>
    struct enumerated {
      T x;

      using iterator       = enum_iter<decltype(std::begin(x))>;
      using const_iterator = enum_iter<decltype(std::cbegin(x))>;

      iterator begin() noexcept { return std::begin(x); }
      const_iterator cbegin() const noexcept { return std::cbegin(x); }
      const_iterator begin() const noexcept { return cbegin(); }

      iterator end() noexcept { return std::end(x); }
      const_iterator cend() const noexcept { return std::cend(x); }
      const_iterator end() const noexcept { return cend(); }
    };

    // ---------------------------------------------

    template <typename... T>
    struct zipped {
      std::tuple<T...> tu; // T can be a ref.

      using seq_t          = std::index_sequence_for<T...>;
      using iterator       = zip_iter<decltype(std::begin(std::declval<T &>()))...>;
      using const_iterator = zip_iter<decltype(std::cbegin(std::declval<T &>()))...>;

      template <typename... U>
      zipped(U &&... ranges) : tu{std::forward<U>(ranges)...} {}

      private:
      // Apply function to tuple
      template <typename F, size_t... Is>
      [[gnu::always_inline]] auto tuple_map(F &&f, std::index_sequence<Is...>) {
        return iterator{std::make_tuple(f(std::get<Is>(tu))...)};
      }
      template <typename F, size_t... Is>
      [[gnu::always_inline]] auto tuple_map(F &&f, std::index_sequence<Is...>) const {
        return const_iterator{std::make_tuple(f(std::get<Is>(tu))...)};
      }

      public:
      iterator begin() noexcept {
        return tuple_map([](auto &&x) { return std::begin(x); }, seq_t{});
      }
      const_iterator cbegin() const noexcept {
        return tuple_map([](auto &&x) { return std::cbegin(x); }, seq_t{});
      }
      const_iterator begin() const noexcept { return cbegin(); }

      iterator end() noexcept {
        return tuple_map([](auto &&x) { return std::end(x); }, seq_t{});
      }
      const_iterator cend() const noexcept {
        return tuple_map([](auto &&x) { return std::cend(x); }, seq_t{});
      }
      const_iterator end() const noexcept { return cend(); }
    };

    // ---------------------------------------------

    template <typename... T>
    struct multiplied {
      std::tuple<T...> tu; // T can be a ref.

      using iterator       = prod_iter<decltype(std::begin(std::declval<T &>()))...>;
      using const_iterator = prod_iter<decltype(std::cbegin(std::declval<T &>()))...>;

      template <typename... U>
      multiplied(U &&... ranges) : tu{std::forward<U>(ranges)...} {}

      private:
      template <size_t... Is>
      [[gnu::always_inline]] auto _begin(std::index_sequence<Is...>) {
        return iterator{std::make_tuple(std::begin(std::get<Is>(tu))...), std::make_tuple(std::end(std::get<Is>(tu))...)};
      }

      template <size_t... Is>
      [[gnu::always_inline]] auto _cbegin(std::index_sequence<Is...>) const {
        return const_iterator{std::make_tuple(std::cbegin(std::get<Is>(tu))...), std::make_tuple(std::cend(std::get<Is>(tu))...)};
      }

      public:
      iterator begin() noexcept { return _begin(std::index_sequence_for<T...>{}); }
      const_iterator cbegin() const noexcept { return _cbegin(std::index_sequence_for<T...>{}); }
      const_iterator begin() const noexcept { return cbegin(); }

      auto end() noexcept { return make_sentinel(std::end(std::get<sizeof...(T) - 1>(tu))); }
      auto cend() const noexcept { return make_sentinel(std::cend(std::get<sizeof...(T) - 1>(tu))); }
      auto end() const noexcept { return cend(); }
    };

    template <typename... T>
    multiplied(T &&...)->multiplied<std::decay_t<T>...>;

    // ---------------------------------------------

    template <typename T>
    struct sliced {
      T x;
      std::ptrdiff_t start_idx, end_idx;

      using iterator       = decltype(std::begin(x));
      using const_iterator = decltype(std::cbegin(x));

      iterator begin() noexcept { return std::next(std::begin(x), start_idx); }
      const_iterator cbegin() const noexcept { return std::next(std::cbegin(x), start_idx); }
      const_iterator begin() const noexcept { return cbegin(); }

      iterator end() noexcept {
        std::ptrdiff_t total_size = std::distance(std::cbegin(x), std::cend(x));
        return std::next(begin(), std::min(total_size, end_idx) - start_idx);
      }
      const_iterator cend() const noexcept {
        std::ptrdiff_t total_size = std::distance(std::cbegin(x), std::cend(x));
        return std::next(cbegin(), std::min(total_size, end_idx) - start_idx);
      }
      const_iterator end() const noexcept { return cend(); }
    };

    // ---------------------------------------------

    template <typename T>
    struct strided {
      T x;
      std::ptrdiff_t stride;

      using iterator       = stride_iter<decltype(std::begin(x))>;
      using const_iterator = stride_iter<decltype(std::cbegin(x))>;

      iterator begin() noexcept { return {std::begin(x), stride}; }
      const_iterator cbegin() const noexcept { return {std::cbegin(x), stride}; }
      const_iterator begin() const noexcept { return cbegin(); }

      iterator end() noexcept {
        std::ptrdiff_t end_idx = std::distance(std::cbegin(x), std::cend(x));
        return std::next(std::end(x), stride - end_idx % stride);
      }
      const_iterator cend() const noexcept {
        std::ptrdiff_t end_idx = std::distance(std::cbegin(x), std::cend(x));
        return std::next(std::cend(x), stride - end_idx % stride);
      }
      const_iterator end() const noexcept { return cend(); }
    };

  } // namespace details

  /********************* The range adapting functions ********************/

  /**
   * Transform (lazy)applies a unary lambda function to every
   * element of a range. It returns itself a range.
   *
   * @param range The range that the lambda is applied to
   * @param range The lambda to apply to the range
   */
  template <typename T, typename L>
  auto transform(T &&range, L lambda) {
    return details::transformed<T, L>{std::forward<T>(range), std::move(lambda)};
  }

  /**
   * Lazy-enumerate a range (similar to Python enumerate)
   * 
   * The function returns a iterable lazy object. When iterated upon, 
   * this object yields a pair (n,x) where :
   *   * n is the index (starting at 0)
   *   * x is in the object in the range
   *
   * @tparam R Type of the ranges
   * @param range The range to enumerate
   */
  template <typename R>
  auto enumerate(R &&range) {
    return details::enumerated<R>{std::forward<R>(range)};
  }

  /**
   * Generate a zip of the ranges (similar to Python zip).
   *
   * The function returns a iterable lazy object. When iterated upon, 
   * this object yields a tuple of the objects in the ranges. 
   *
   * @tparam R Type of the ranges
   * @param ranges 
   *     The ranges to zip. 
   *
   *     .. warning::
   *          The ranges have to be equal lengths or behaviour is undefined.
   */
  template <typename... R>
  auto zip(R &&... ranges) {
    return details::zipped<R...>{std::forward<R>(ranges)...};
  }

  /**
   * Generate a zip of the ranges (similar to Python zip).
   *
   *  DOC TO BE WRITTEN.
   *
   * @param ranges 
   * @param lambda 
   * @tparam R Type of the ranges
   * @tparam L Type of the Lambda
   */
  template <typename... T, typename L>
  auto zip_with(T &&... ranges, L &&lambda) {
    return transform(zip(std::forward<T>(ranges)...), [lambda](std::tuple<T...> t) { return std::apply(lambda, t); });
  }

  /**
   * Lazy-product of multiple ranges. This function returns itself a range of tuple<T...>.
   * Iterating over it will yield all combinations of the different range values.
   * Note: The ranges are incremented beginning with the leftmost range.
   *
   * @tparam T The types of the different ranges
   * @param ranges The ranges to zip. Note: They have to be of equal length!
   */
  template <typename... T>
  auto product(T &&... ranges) {
    return details::multiplied<T...>{std::forward<T>(ranges)...};
  }

  /**
   * Lazy-slice a range.
   * This function returns itself a slice of the initial range
   *
   * @param range The range to slice
   * @param start_idx The index to start the slice at
   * @param end_idx The index one past the end of the sliced range
   */
  template <typename T>
  auto slice(T &&range, std::ptrdiff_t start_idx, std::ptrdiff_t end_idx) {
    return details::sliced<T>{std::forward<T>(range), start_idx, std::max(start_idx, end_idx)};
  }

  /**
   * Lazy-stride a range.
   * This function returns itself a subrange of the initial range
   * by considering only every N-th element
   *
   * @param range The range to take the subrange of
   * @param stride The numer of elements to skip
   */
  template <typename T>
  auto stride(T &&range, std::ptrdiff_t stride) {
    return details::strided<T>{std::forward<T>(range), stride};
  }

  /********************* Some factory functions ********************/

  namespace details {
    template <typename A, size_t... Is>
    [[gnu::always_inline]] auto make_product_impl(A &arr, std::index_sequence<Is...>) {
      return product(arr[Is]...);
    }
  } // namespace details

  template <typename T, size_t N>
  auto make_product(std::array<T, N> &arr) {
    return details::make_product_impl(arr, std::make_index_sequence<N>{});
  }

  template <typename T, size_t N>
  auto make_product(std::array<T, N> const &arr) {
    return details::make_product_impl(arr, std::make_index_sequence<N>{});
  }

  template <typename R>
  auto make_vector_from_range(R const &r) {
    std::vector<std::decay_t<decltype(*(std::begin(r)))>> vec; // decltype returns a &
    auto total_size = std::distance(std::cbegin(r), std::cend(r));
    vec.reserve(total_size);
    for (auto const &x : r) vec.emplace_back(x);
    return vec;
  }

  /********************* Functionality related to Integer ranges ********************/

  /**
   * A range of integer indices that mimics the Python `range`.
   */
  class range {

    public:
    using index_t = std::ptrdiff_t;

    /**
     * Default constructor
     *
     * range() stands for the whole set of indices in the dimension (like `:` in python)
     * Example: A(range(), 0) takes the first column of A
     */
    range() = default;

    /**
     * Constructor
     *
     * @param first__
     * The first index of the range
     *
     * @param last__
     * The first index past the end of the range
     *
     * @param step__
     * The step between two indices
     */
    range(index_t first__, index_t last__, index_t step__ = 1) : first_(first__), last_(last__), step_(step__) {}

    /**
     * Constructor
     *
     * Constructs range(0, i, 1)
     */
    explicit range(index_t i) : range(0, i, 1) {}

    /// The first index of the range
    index_t first() const { return first_; }

    /// The first index past the end of the range
    index_t last() const { return last_; }

    /// The step between two indices
    index_t step() const { return step_; }

    /// Number of indices in the range
    size_t size() const {
      index_t r = (last_ - first_) / step_;
      if (r < 0) throw std::runtime_error("range with negative size");
      return size_t(r);
    }

    range operator+(index_t shift) const { return range(first_ + shift, last_ + shift, step_); }

    friend inline std::ostream &operator<<(std::ostream &os, const range &range) {
      os << "range(" << range.first() << "," << range.last() << "," << range.step() << ")";
      return os;
    }

    struct const_iterator : iterator_facade<const_iterator, const index_t> {

      const_iterator(range const *r, bool atEnd) {
        last = r->last();
        step = r->step();
        pos  = (atEnd ? last : r->first());
      }

      void increment() { pos += step; }

      bool equal(const_iterator const &other) const { return (other.pos == pos); }

      index_t dereference() const { return pos; }

      private:
      index_t last, pos, step;
    };

    const_iterator begin() const noexcept { return {this, false}; }
    const_iterator cbegin() const noexcept { return {this, false}; }

    const_iterator end() const noexcept { return {this, true}; }
    const_iterator cend() const noexcept { return {this, true}; }

    private:
    index_t first_ = 0, last_ = -1, step_ = 1;
  };

  /**
   * A product of an arbitrary number of integer ranges
   * given a set of integers or an integer tuple
   *
   * @tparam Integers The integer types
   */
  template <typename... Integers, typename EnableIf = std::enable_if_t<(std::is_integral_v<Integers> and ...), int>>
  auto product_range(Integers... Is) {
    return product(range(Is)...);
  }

  namespace details {
    template <typename Tuple, size_t... Is>
    [[gnu::always_inline]] auto product_range_impl(Tuple const &idx_tpl, std::index_sequence<Is...>) {
      return product_range(std::get<Is>(idx_tpl)...);
    }
  } // namespace details

  template <typename... Integers, typename EnableIf = std::enable_if_t<(std::is_integral_v<Integers> and ...), int>>
  auto product_range(std::tuple<Integers...> const &idx_tpl) {
    return details::product_range_impl(idx_tpl, std::make_index_sequence<sizeof...(Integers)>{});
  }

  /**
   * Given an integer range [start, end), chunk it as equally as possible into n_chunks.
   * If the range is not dividable in n_chunks equal parts, the first chunks have
   * one more element than the last ones.
   */
  inline std::pair<std::ptrdiff_t, std::ptrdiff_t> chunk_range(std::ptrdiff_t start, std::ptrdiff_t end, long n_chunks, long rank) {
    auto total_size    = end - start;
    auto chunk_size    = total_size / n_chunks;
    auto n_large_nodes = total_size - n_chunks * chunk_size;
    if (rank < n_large_nodes) // larger nodes have size chunk_size + 1
      return {start + rank * (chunk_size + 1), start + (rank + 1) * (chunk_size + 1)};
    else // smaller nodes have size chunk_size
      return {start + n_large_nodes + rank * chunk_size, start + n_large_nodes + (rank + 1) * chunk_size};
  }

  /**
   * Apply a function f to every element of an integer range
   *
   * @param r
   * The range to apply the function to
   *
   * @param f
   * The function to apply
   */
  template <typename F>
  void foreach(range const &r, F&& f) {
    auto i = r.first(), last = r.last(), step = r.step();
    for (; i < last; i += step) f(i);
  }

} // namespace itertools
