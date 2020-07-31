// Copyright (c) 2020 Simons Foundation
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
// Authors: Nils Wentzell

#ifndef STD_ADDONS_ARRAY_H
#define STD_ADDONS_ARRAY_H
#include <array>
#include <utility>
#include <vector>

// missing part of std ...
namespace nda {

  template <typename T, size_t... Is>
  constexpr std::array<T, sizeof...(Is)> make_initialized_array_impl(T v, std::index_sequence<Is...>) {
    return {(Is ? v : v)...};
  } // always v, just a trick to have the pack

  /**
   * @tparam T  T must be constructible from U
   * @tparam U
   * @tparam R
   * make a std::array<T, R> initialized to v
   */
  template <typename T, typename U, size_t R>
  constexpr std::array<T, R> make_std_array(std::array<U, R> const &a) {
    static_assert(std::is_constructible_v<T, U>, "make_std_array : T must be constructible from U, Cf doc");
    std::array<T, R> result; // NOLINT Ok, not init indeex.
    for (int u = 0; u < R; ++u) result[u] = a[u];
    return result;
  }

  /**
   * @tparam R
   * @tparam T
   * make a std::array<T, R> initialized to v
   */
  template <size_t R, typename T>
  constexpr std::array<T, R> make_initialized_array(T v) {
    return make_initialized_array_impl(v, std::make_index_sequence<R>{});
  }

  /**
  * Convert a std::array to a
  * @tparam T
  * @param a std::array to convert
  */
  template <typename T, size_t R>
  constexpr std::vector<T> to_vector(std::array<T, R> const &a) {
    std::vector<T> V(R);
    for (int i = 0; i < R; ++i) V[i] = a[i];
    return V;
  }

  /**
   * Make a new std::array by appending one element at the end
   * @tparam T
   * @tparam U Must be convertible to T
   * @param a The array
   * @param x Element to append
   * @return A new std::array with the element appended at the end
   */
  template <typename T, typename U, size_t R>
  constexpr std::array<T, R + 1> append(std::array<T, R> const &a, U const &x) {
    std::array<T, R + 1> res;
    for (int i = 0; i < R; ++i) res[i] = a[i];
    res[R] = x;
    return res;
  }

  /**
   * Make a new std::array by appending one element at the front
   * @tparam T
   * @tparam U Must be convertible to T
   * @param a The array
   * @param x Element to append
   * @return A new std::array with the element appended at the front
   */
  template <typename T, typename U, size_t R>
  constexpr std::array<T, R + 1> front_append(std::array<T, R> const &a, U const &x) {
    std::array<T, R + 1> res;
    res[0] = x;
    for (int i = 0; i < R; ++i) res[i + 1] = a[i];
    return res;
  }

  /**
   * Make a new std::array by removing one element at the end
   * @tparam T
   * @param a The array
   * @return A new std::array with the element less at the end
   */
  template <typename T, size_t R>
  constexpr std::array<T, R - 1> pop(std::array<T, R> const &a) {
    std::array<T, R - 1> res;
    for (int i = 0; i < R - 1; ++i) res[i] = a[i];
    return res;
  }

  /**
   * Make a new std::array by removing one element at the front
   * @tparam T
   * @param a The array
   * @return A new std::array with the element less at the front
   */
  template <typename T, size_t R>
  constexpr std::array<T, R - 1> front_pop(std::array<T, R> const &a) {
    std::array<T, R - 1> res;
    for (int i = 1; i < R; ++i) res[i - 1] = a[i];
    return res;
  }

  /**
   * Make a new std::array by removing one element at the front
   * @tparam T
   * @param a The array
   * @return A new std::array with the element less at the front
   */
  template <typename T, size_t R, int N>
  constexpr std::array<T, R - N> front_mpop(std::array<T, R> const &a) {
    std::array<T, R - N> res;
    for (int i = N; i < R; ++i) res[i - N] = a[i];
    return res;
  }

  /**
   * Join two arrays
   * @tparam T
   * @param a1
   * @param a2
   * @return the concatenation of [a1, a2]
   */
  template <typename T, size_t R1, size_t R2>
  constexpr std::array<T, R1 + R2> join(std::array<T, R1> const &a1, std::array<T, R2> const &a2) {
    std::array<T, R1 + R2> res;
    for (int i = 0; i < R1; ++i) res[i] = a1[i];
    for (int i = 0; i < R2; ++i) res[R1 + i] = a2[i];
    return res;
  }

  /**
   * Dot product of two arrays.
   * @tparam T
   * @tparam U
   * @param a1
   * @param a2
   * @return The dot product to whatever type T*U is promoted to. If R = 0, return T{}
   */
  // ------------- dot --------------------------------------
  template <typename T, typename U, size_t R>
  constexpr auto dot_product(std::array<T, R> const &a1, std::array<U, R> const &a2) {
    if constexpr (R == 0)
      return T{};
    else {
      auto res = a1[0] * a2[0];
      for (int i = 1; i < R; ++i) res += a1[i] * a2[i];
      return res;
    }
  }

  //template <typename T, int R> std::array<T, R> operator+(std::array<T, R> const &a1, std::array<T, R> const &a2) {
  //std::array<T, R> r;
  //for (int i = 0; i < R; ++i) r[i] = a1[i] + a2[i];
  //return r;
  //}

  //template <typename T, int R> std::array<T, R> operator-(std::array<T, R> const &a1, std::array<T, R> const &a2) {
  //std::array<T, R> r;
  //for (int i = 0; i < R; ++i) r[i] = a1[i] - a2[i];
  //return r;
  //}

} // namespace nda
#endif
