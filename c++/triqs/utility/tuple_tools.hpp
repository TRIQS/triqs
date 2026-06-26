// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
// Copyright (c) 2015 Igor Krivenko
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Igor Krivenko, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Generic tuple manipulation tools.
 */

#pragma once

#include "./macros.hpp"

#include <cstddef>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <utility>

namespace triqs {

  // Lazy adaptor for zipping tuples together.
  template <typename... T> struct _triqs_zipped_tuple {
    std::tuple<T...> _tu;
    template <typename... U> _triqs_zipped_tuple(U &&...u) : _tu(std::forward<U>(u)...) {}

    // Get a tuple containing the I-th element of each tuple.
    template <size_t I, size_t... Is> auto _get(std::index_sequence<Is...>) { return std::tie(std::get<I>(std::get<Is>(_tu))...); }
    template <size_t I, size_t... Is> [[nodiscard]] auto _get(std::index_sequence<Is...>) const {
      return std::tie(std::get<I>(std::get<Is>(_tu))...);
    }
  };

  // Build a zipped tuple of tuples.
  template <typename... T> _triqs_zipped_tuple<T...> zip_tuples(T &&...x) { return {std::forward<T>(x)...}; }

} // namespace triqs

// Specialize std::get / std::tuple_size for _triqs_zipped_tuple and add a reversed lazy tuple to std.
namespace std {

  // Specialization of std::tuple_size for zipped tuples.
  template <typename T0, typename... T> class tuple_size<triqs::_triqs_zipped_tuple<T0, T...>> : public std::tuple_size<std::decay_t<T0>> {};

  // Specialization of std::get for zipped tuples.
  template <size_t I, typename... T> decltype(auto) get(triqs::_triqs_zipped_tuple<T...> const &tu) {
    return tu.template _get<I>(std::make_index_sequence<sizeof...(T)>());
  }

  // Lazy adaptor for reversing a tuple.
  template <typename TU> struct _triqs_reversed_tuple {
    TU _x;
  };

  // Build a reversed view of a tuple.
  template <typename... T> _triqs_reversed_tuple<std::tuple<T...>> reverse(std::tuple<T...> &&x) { return {std::move(x)}; }
  template <typename... T> _triqs_reversed_tuple<std::tuple<T...> &> reverse(std::tuple<T...> &x) { return {x}; }
  template <typename... T> _triqs_reversed_tuple<std::tuple<T...> const &> reverse(std::tuple<T...> const &x) { return {x}; }

  // Specializations of std::get for reversed tuples.
  template <int pos, typename TU> decltype(auto) get(_triqs_reversed_tuple<TU> const &t) {
    return std::get<std::tuple_size_v<std::decay_t<TU>> - 1 - pos>(t._x);
  }

  template <int pos, typename TU> decltype(auto) get(_triqs_reversed_tuple<TU> &t) {
    return std::get<std::tuple_size_v<std::decay_t<TU>> - 1 - pos>(t._x);
  }

  template <int pos, typename TU> decltype(auto) get(_triqs_reversed_tuple<TU> &&t) { // NOLINT
    return std::get<std::tuple_size_v<std::decay_t<TU>> - 1 - pos>(std::move(t._x));
  }

  // Specialization of std::tuple_size for reversed tuples.
  template <typename TU> class tuple_size<_triqs_reversed_tuple<TU>> : public tuple_size<std::decay_t<TU>> {};

} // namespace std

namespace triqs::tuple {

  /**
   * @addtogroup triqs-utility-tuple
   * @{
   */

  // Return the index sequence 0, 1, ..., tuple_size_v<T> - 1 corresponding to a given tuple type.
  template <typename T> std::make_index_sequence<std::tuple_size_v<std::decay_t<T>>> _get_seq() { return {}; }

  // Get the number of elements in a tuple-like type.
  template <typename Tu> constexpr int _get_seq_len() { return std::tuple_size_v<std::decay_t<Tu>>; }

  // Empty tag type carrying a compile-time integer N.
  template <int N> struct _int {};

  // Tag type inheriting from _int<Is> for a pack of indices Is.
  template <int... Is> struct all_indices : _int<Is>... {};

  // Recursively builds the complement of a set of indices, keeping those not in hole_seq and exposing them as ::type.
  template <int N, typename hole_seq, int... Is> struct complement_sequence_impl {
    static complement_sequence_impl<N - 1, hole_seq, Is...> get(_int<N>) {}
    static complement_sequence_impl<N - 1, hole_seq, N, Is...> get(...) {}
    using type = typename decltype(get(hole_seq()))::type;
  };

  // Base case of complement_sequence_impl: emit the accumulated indices.
  template <typename hole_seq, int... Is> struct complement_sequence_impl<-1, hole_seq, Is...> {
    using type = std::index_sequence<Is...>;
  };

  /// Index sequence containing all integers of \f$ [0, N] \f$ that are not in `Is...`.
  template <int N, int... Is> using complement_sequence = typename complement_sequence_impl<N, all_indices<Is...>>::type;

  // Implementation of make_tuple_repeat().
  template <typename T, std::size_t... Is> auto make_tuple_repeat_impl(T const &x, std::index_sequence<Is...>) {
    return std::make_tuple(((void)Is, x)...);
  }

  /**
   * @brief Build a tuple with \f$ N \f$ copies of a value.
   * 
   * @tparam N Number of elements (non-negative).
   * @tparam X Element type.
   * @param x Value to repeat.
   * @return A tuple with \f$ N \f$ elements, each equal to `x`.
   */
  template <int N, typename X> auto make_tuple_repeat(X const &x) { return make_tuple_repeat_impl(x, std::make_index_sequence<N>()); }

  // Implementation of apply().
  template <typename F, typename T, size_t... Is> decltype(auto) apply_impl(F &&f, T &&t, std::index_sequence<Is...>) {
    return std::forward<F>(f)(std::get<Is>(std::forward<T>(t))...);
  }

  /**
   * @brief Call a function with the elements of a tuple as its arguments.
   *
   * @details Equivalent to the C++17 `std::apply`.
   * 
   * @tparam F Callable type.
   * @tparam T Tuple type.
   * @param f Callable to invoke.
   * @param t Tuple of arguments, i.e. `t = (t0, t1, ...)`.
   * @return The result of the function call to `f(t0, t1, ...)`.
   */
  template <typename F, typename T> decltype(auto) apply(F &&f, T &&t) { return apply_impl(std::forward<F>(f), std::forward<T>(t), _get_seq<T>()); }

  // Implementation of apply_construct().
  template <typename C, typename T, size_t... Is> decltype(auto) apply_construct_impl(T &&t, std::index_sequence<Is...>) {
    return C{std::get<Is>(std::forward<T>(t))...};
  }

  /**
   * @brief Brace-construct an object from the elements of a tuple.
   * 
   * @tparam C Type to construct.
   * @tparam T Tuple type.
   * @param t Tuple of constructor arguments, i.e. `t = (t0, t1, ...)`.
   * @return An object of type `C` constructed as `C{t0, t1, ...}`.
   */
  template <typename C, typename T> decltype(auto) apply_construct(T &&t) { return apply_construct_impl<C>(std::forward<T>(t), _get_seq<T>()); }

  // Implementation of apply_construct_parenthesis().
  template <typename C, typename T, size_t... Is> decltype(auto) apply_construct_parenthesis_impl(T &&t, std::index_sequence<Is...>) {
    return C(std::get<Is>(std::forward<T>(t))...);
  }

  /**
   * @brief Parenthesis-construct an object from the elements of a tuple.
   * 
   * @tparam C Type to construct.
   * @tparam T Tuple type.
   * @param t Tuple of constructor arguments, i.e. `t = (t0, t1, ...)`.
   * @return An object of type `C` constructed as `C(t0, t1, ...)`.
   */
  template <typename C, typename T> decltype(auto) apply_construct_parenthesis(T &&t) {
    return apply_construct_parenthesis_impl<C>(std::forward<T>(t), _get_seq<T>());
  }

  // Wrapper turning a callable into one that accepts a single tuple and unpacks it via apply().
  template <typename F> struct _called_on_tuple {
    F _f;
    template <typename Tu> decltype(auto) operator()(Tu &&tu) { return triqs::tuple::apply(_f, std::forward<Tu>(tu)); }
  };

  /**
   * @brief Wrap a callable so that it can be invoked with a single tuple argument.
   *
   * @tparam F Callable type.
   * @param f Callable to wrap.
   * @return A new callable that takes a single tuple argument and unpacks it to call `f`.
   */
  template <typename F> auto called_on_tuple(F &&f) { return _called_on_tuple<F>{std::forward<F>(f)}; }

  // Base case for _for_each_impl().
  template <typename F> void _for_each_impl(F &&) {}

  // Call f(x0); f(x1); ...; f(xn); in this order.
  template <typename F, typename T0, typename... T> void _for_each_impl(F &&f, T0 &&x0, T &&...x) { // NOLINT
    f(std::forward<T0>(x0));
    _for_each_impl(f, std::forward<T>(x)...);
  }

  // Base case for _for_each_apply_impl().
  template <typename F> void _for_each_apply_impl(F &&) {}

  // Call apply(f, t0); apply(f, t1); ...; apply(f, tn); in this order.
  template <typename F, typename T0, typename... T> void _for_each_apply_impl(F &&f, T0 &&t0, T &&...t) { // NOLINT
    triqs::tuple::apply(f, std::forward<T0>(t0));
    _for_each_apply_impl(f, std::forward<T>(t)...);
  }

  // Implementation of for_each().
  template <typename F, typename T, size_t... Is> void for_each_impl(F &&f, T &&t, std::index_sequence<Is...>) { // NOLINT
    _for_each_impl(f, std::get<Is>(t)...);
  }

  /**
   * @brief Apply a callable to every element of a tuple, in order.
   * 
   * @tparam T Tuple type.
   * @tparam F Callable type.
   * @param t Tuple to iterate over.
   * @param f Callable invoked as `f(x)` for each element `x` of the tuple `t`.
   */
  template <typename T, typename F> void for_each(T &&t, F &&f) { for_each_impl(std::forward<F>(f), std::forward<T>(t), _get_seq<T>()); }

  // Call f(n, x) for each element x at position n (Python's enumerate).
  template <typename F, typename T, size_t... Is> void _for_each_enum_impl(F &&f, T &&t, std::index_sequence<Is...>) { // NOLINT
    _for_each_apply_impl(f, std::tuple<int, decltype(std::get<Is>(t))>(Is, std::get<Is>(t))...);
  }

  /**
   * @brief Apply a callable to every (index, element) pair of a tuple.
   *
   * @details Mimics Python's `for n, x in enumerate(t): f(n, x)`.
   * 
   * @tparam T Tuple type.
   * @tparam F Callable type.
   * @param t Tuple to iterate over.
   * @param f Callable invoked as `f(n, x)` for each element `x` at index `n` of the tuple `t`.
   */
  template <typename T, typename F> void for_each_enumerate(T &&t, F &&f) {
    _for_each_enum_impl(std::forward<F>(f), std::forward<T>(t), _get_seq<T>());
  }

  /**
   * @brief Apply an N-ary callable across \f$ N \f$ tuples zipped together.
   *
   * @details Mimics Python's `for i1, ..., iN in zip(t1, ..., tN): f(i1, ..., iN)`. The tuples must have equal size.
   * 
   * @tparam F Callable type taking \f$ N \f$ arguments.
   * @tparam Ts Tuple types.
   * @param f Callable invoked on each zipped tuple of elements.
   * @param ts The \f$ N \f$ tuples to zip and iterate over.
   */
  template <typename F, typename... Ts> void for_each_zip(F &&f, Ts &&...ts) {
    for_each(zip_tuples(std::forward<Ts>(ts)...), called_on_tuple(std::forward<F>(f)));
  }

  // Build std::make_tuple(f(get<Is>(t))...) for map().
  template <typename F, typename T, size_t... Is> decltype(auto) _map_impl(F &&f, T &&t, std::index_sequence<Is...>) {
    return std::make_tuple(std::forward<F>(f)(std::get<Is>(std::forward<T>(t)))...);
  }

  /**
   * @brief Map a callable over the elements of a tuple.
   * 
   * @tparam F Callable type.
   * @tparam T Tuple type.
   * @param f Callable applied to each element.
   * @param t Tuple to map over.
   * @return A new tuple containing the result of applying the callable to each element of the input tuple.
   */
  template <typename F, typename T> decltype(auto) map(F &&f, T &&t) { return _map_impl(std::forward<F>(f), std::forward<T>(t), _get_seq<T>()); }

  /**
   * @brief Map an N-ary callable across N tuples zipped together.
   * 
   * @tparam Ts Tuple types.
   * @tparam F Callable type.
   * @param f Callable applied to each zipped tuple of elements.
   * @param ts The \f$ N \f$ tuples to zip and map over.
   * @return A new tuple containing the result of applying the callable to each zipped tuple of elements.
   */
  template <typename... Ts, typename F> auto map_on_zip(F &&f, Ts &&...ts) {
    return map(called_on_tuple(std::forward<F>(f)), zip_tuples(std::forward<Ts>(ts)...));
  }

  // Recursive step of the single-tuple fold().
  template <int pos, typename F, typename T, typename R> decltype(auto) fold_impl(_int<pos>, F &&f, T &&t, R &&r) {
    return fold_impl(_int<pos - 1>(), std::forward<F>(f), std::forward<T>(t), f(std::get<_get_seq_len<T>() - 1 - pos>(t), std::forward<R>(r)));
  }

  // Base case of the single-tuple fold(): return the accumulator.
  template <typename F, typename T, typename R> R fold_impl(_int<-1>, F &&, T &&, R &&r) { return std::forward<R>(r); }

  /**
   * @brief Left-fold a callable over the elements of a tuple.
   *
   * @details Computes `f(x_N, f(x_{N-1}, ... f(x_1, r)))` for a tuple `t = (x_1, ..., x_N)`.
   * 
   * @tparam F Callable type.
   * @tparam T Tuple type.
   * @tparam R Accumulator type.
   * @param f Folding callable with signature `f(x, r) -> r'`.
   * @param t Tuple to fold.
   * @param r Initial accumulator value.
   * @return The folded result.
   */
  template <typename F, typename T, typename R> decltype(auto) fold(F &&f, T &&t, R &&r) {
    return fold_impl(_int<_get_seq_len<T>() - 1>(), std::forward<F>(f), std::forward<T>(t), std::forward<R>(r));
  }

  // Recursive step of the two-tuple fold().
  template <int pos, typename F, typename T0, typename T1, typename R> decltype(auto) fold_impl(_int<pos>, F &&f, T0 &&t0, T1 &&t1, R &&r) {
    constexpr int n = _get_seq_len<T0>() - 1 - pos;
    return fold_impl(_int<pos - 1>(), std::forward<F>(f), std::forward<T0>(t0), std::forward<T1>(t1),
                     f(std::get<n>(t0), std::get<n>(t1), std::forward<R>(r)));
  }

  // Base case of the two-tuple fold(): return the accumulator.
  template <typename F, typename T0, typename T1, typename R> R fold_impl(_int<-1>, F &&, T0 &&, T1 &&, R &&r) { return std::forward<R>(r); }

  /**
   * @brief Left-fold a callable over the elements of two equally-sized tuples.
   *
   * @details Computes `f(x_N, y_N, f(x_{N-1}, y_{N-1}, ... f(x_0, y_0, r)))` for tuples `t1 = (x_1, ..., x_N)` and 
   * `t2 = (y_1, ..., y_N)`.
   * 
   * @tparam F Callable type.
   * @tparam T0 First tuple type.
   * @tparam T1 Second tuple type.
   * @tparam R Accumulator type.
   * @param f Folding callable with signature `f(x, y, r) -> r'`.
   * @param t1 First tuple to fold.
   * @param t2 Second tuple to fold.
   * @param r Initial accumulator value.
   * @return The folded result.
   */
  template <typename F, typename T0, typename T1, typename R> decltype(auto) fold(F &&f, T0 &&t1, T1 &&t2, R &&r) {
    return fold_impl(_int<_get_seq_len<T0>() - 1>(), std::forward<F>(f), std::forward<T0>(t1), std::forward<T1>(t2), std::forward<R>(r));
  }

  // Return r (selected when index I is one of the replaced positions).
  template <int I, typename T, typename R> R _get_rpl(T &&, R &&r, _int<I>) { return std::forward<R>(r); }

  // Return the original element x (fallback for non-replaced positions).
  // NOLINTNEXTLINE(modernize-avoid-variadic-functions) -- ellipsis is used as the lowest-priority overload.
  template <int I, typename T, typename R> T _get_rpl(T &&x, R &&, ...) { return std::forward<T>(x); }

  // Build the replaced tuple for replace(), choosing r at the positions listed in AllIndices.
  template <size_t... Is, typename Tu, typename R, typename AllIndices>
  auto _replace_impl(Tu &&tu, R &&r, AllIndices _, // NOLINT(cppcoreguidelines-missing-std-forward): r is reused per position, tu read element-wise
                     std::index_sequence<Is...>) {
    return std::make_tuple(_get_rpl<Is>(std::get<Is>(tu), r, _)...);
  }

  /**
   * @brief Return a copy of a tuple with the elements at the given positions replaced by a given value.
   * 
   * @tparam Is Positions to replace.
   * @tparam T Tuple type.
   * @tparam R Replacement type.
   * @param t Source tuple.
   * @param r Replacement value.
   * @return A tuple with the selected positions replaced.
   */
  template <int... Is, typename T, typename R> auto replace(T &&t, R &&r) {
    return _replace_impl(std::forward<T>(t), std::forward<R>(r), all_indices<Is...>(), _get_seq<T>());
  }

  /**
   * @brief Tuple type made of the elements at the given positions of given tuple type `T`.
   * 
   * @tparam T Tuple type.
   * @tparam Is Positions to keep.
   */
  template <typename T, size_t... Is> using filter_t = std::tuple<std::tuple_element_t<Is, std::decay_t<T>>...>;

  /**
   * @brief Keep only the elements of a tuple at the given positions.
   * 
   * @tparam Is Positions to keep.
   * @tparam T Tuple type.
   * @param t Source tuple.
   * @return A new tuple holding the elements of the source tuple whose positions are in `Is...`.
   */
  template <size_t... Is, typename T> filter_t<T, Is...> filter(T &&t) { return filter_t<T, Is...>(std::get<Is>(std::forward<T>(t))...); }

  /**
   * @brief Keep only the elements of a tuple at the given positions (as an index-sequence).
   * 
   * @tparam Is Positions to keep, supplied through the index-sequence argument.
   * @tparam T Tuple type.
   * @param t Source tuple.
   * @return A new tuple holding the elements of the source tuple whose positions are in `Is...`.
   */
  template <size_t... Is, typename T> filter_t<T, Is...> filter(T &&t, std::index_sequence<Is...>) {
    return filter_t<T, Is...>(std::get<Is>(std::forward<T>(t))...);
  }

  /**
   * @brief Drop the elements of a tuple at the given positions.
   * 
   * @tparam Is Positions to drop.
   * @tparam T Tuple type.
   * @param t Source tuple.
   * @return A new tuple holding the elements of the source tuple whose positions are not in `Is...`.
   */
  template <int... Is, typename T> decltype(auto) filter_out(T &&t) {
    return filter(std::forward<T>(t), complement_sequence<std::tuple_size_v<std::decay_t<T>> - 1, Is...>());
  }

  /**
   * @brief Result type of filter_out() for tuple type `T` dropping positions `Is...`.
   * 
   * @tparam T Tuple type.
   * @tparam Is Positions to drop.
   */
  template <typename T, int... Is> using filter_out_t = std::decay_t<decltype(filter_out<Is...>(std::declval<T>()))>;

  /**
   * @brief Append an element to the end of a tuple.
   * 
   * @tparam T Tuple type.
   * @tparam X Element type.
   * @param t Source tuple.
   * @param x Element to append.
   * @return A new tuple equal to the source tuple with `x` added at the end.
   */
  template <typename T, typename X> auto push_back(T &&t, X &&x) { return std::tuple_cat(std::forward<T>(t), std::make_tuple(std::forward<X>(x))); }

  /**
   * @brief Prepend an element to the front of a tuple.
   * 
   * @tparam T Tuple type.
   * @tparam X Element type.
   * @param t Source tuple.
   * @param x Element to prepend.
   * @return A new tuple equal to the source tuple with `x` added at the front.
   */
  template <typename T, typename X> auto push_front(T &&t, X &&x) { return std::tuple_cat(std::make_tuple(std::forward<X>(x)), std::forward<T>(t)); }

  /**
   * @brief Remove the first element of a tuple.
   * 
   * @tparam T Tuple type.
   * @param t Source tuple.
   * @return A new tuple equal to the source tuple with its first element removed.
   */
  template <typename T> auto pop_front(T &&t) { return filter_out<0>(std::forward<T>(t)); }

  /** @} */

} // namespace triqs::tuple

namespace std {

  /**
   * @addtogroup triqs-utility-tuple
   * @{
   */

  /**
   * @brief Write a `std::tuple` to an output stream.
   * 
   * @tparam T Types of the tuple elements.
   * @param os Output stream.
   * @param t Tuple to write.
   * @return Reference to the output stream.
   */
  template <typename... T> std::ostream &operator<<(std::ostream &os, std::tuple<T...> const &t) {
    os << "(";
    triqs::tuple::for_each(t, [&os, c = 0](auto &x) mutable {
      if (c++) os << ',';
      os << x;
    });
    return os << ")";
  }

  /**
    * @brief Write a `std::pair` to an output stream.
    * 
    * @tparam T1 Type of the first element.
    * @tparam T2 Type of the second element.
    * @param os Output stream.
    * @param x Pair to write.
    * @return Reference to the output stream.
    */
  template <typename T1, typename T2> std::ostream &operator<<(std::ostream &os, std::pair<T1, T2> const &x) {
    return os << '(' << x.first << ", " << x.second << ')';
  }

  /** @} */

} // namespace std
