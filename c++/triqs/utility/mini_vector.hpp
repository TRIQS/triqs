// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell, tayral

#pragma once
#include "./first_include.hpp"
#include "./macros.hpp"
#include "./exceptions.hpp"
#include <boost/serialization/utility.hpp>
#include <vector>
#include <iostream>

#define TRIQS_MINI_VECTOR_NRANK_MAX 10

namespace triqs {
  namespace utility {

    struct no_init_tag {};

    ///Mini vector class
    /**
  @tparam T element type
  @tparam Rank size of the vector

  */
    template <typename T, int Rank> class mini_vector {
      T _data[(Rank != 0 ? Rank : 1)];
      friend class boost::serialization::access;
      template <class Archive> void serialize(Archive &ar, const unsigned int version) { ar &_data; }
      void init() {
        for (int i = 0; i < Rank; ++i) _data[i] = 0;
      }

      public:
      typedef T value_type;

      mini_vector() { init(); }

      mini_vector(no_init_tag) {}
      ///constructor for mini_vector of size 1
      mini_vector(T x_0) {
        static_assert(Rank == 1, "mini_vector : incorrect number of variables in constructor");
        _data[0] = x_0;
      }
      ///constructor for mini_vector of size 2
      mini_vector(T x_0, T x_1) {
        static_assert(Rank == 2, "mini_vector : incorrect number of variables in constructor");
        _data[0] = x_0;
        _data[1] = x_1;
      }
      ///constructor for mini_vector of size 3
      mini_vector(T x_0, T x_1, T x_2) {
        static_assert(Rank == 3, "mini_vector : incorrect number of variables in constructor");
        _data[0] = x_0;
        _data[1] = x_1;
        _data[2] = x_2;
      }
      ///constructor for mini_vector of size 4
      mini_vector(T x_0, T x_1, T x_2, T x_3) {
        static_assert(Rank == 4, "mini_vector : incorrect number of variables in constructor");
        _data[0] = x_0;
        _data[1] = x_1;
        _data[2] = x_2;
        _data[3] = x_3;
      }
      ///constructor for mini_vector of size 5
      mini_vector(T x_0, T x_1, T x_2, T x_3, T x_4) {
        static_assert(Rank == 5, "mini_vector : incorrect number of variables in constructor");
        _data[0] = x_0;
        _data[1] = x_1;
        _data[2] = x_2;
        _data[3] = x_3;
        _data[4] = x_4;
      }
      ///constructor for mini_vector of size 6
      mini_vector(T x_0, T x_1, T x_2, T x_3, T x_4, T x_5) {
        static_assert(Rank == 6, "mini_vector : incorrect number of variables in constructor");
        _data[0] = x_0;
        _data[1] = x_1;
        _data[2] = x_2;
        _data[3] = x_3;
        _data[4] = x_4;
        _data[5] = x_5;
      }
      mini_vector(T x_0, T x_1, T x_2, T x_3, T x_4, T x_5, T x_6) {
        static_assert(Rank == 7, "mini_vector : incorrect number of variables in constructor");
        _data[0] = x_0;
        _data[1] = x_1;
        _data[2] = x_2;
        _data[3] = x_3;
        _data[4] = x_4;
        _data[5] = x_5;
        _data[6] = x_6;
      }
      mini_vector(T x_0, T x_1, T x_2, T x_3, T x_4, T x_5, T x_6, T x_7) {
        static_assert(Rank == 8, "mini_vector : incorrect number of variables in constructor");
        _data[0] = x_0;
        _data[1] = x_1;
        _data[2] = x_2;
        _data[3] = x_3;
        _data[4] = x_4;
        _data[5] = x_5;
        _data[6] = x_6;
        _data[7] = x_7;
      }
      mini_vector(T x_0, T x_1, T x_2, T x_3, T x_4, T x_5, T x_6, T x_7, T x_8) {
        static_assert(Rank == 9, "mini_vector : incorrect number of variables in constructor");
        _data[0] = x_0;
        _data[1] = x_1;
        _data[2] = x_2;
        _data[3] = x_3;
        _data[4] = x_4;
        _data[5] = x_5;
        _data[6] = x_6;
        _data[7] = x_7;
        _data[8] = x_8;
      }
      mini_vector(T x_0, T x_1, T x_2, T x_3, T x_4, T x_5, T x_6, T x_7, T x_8, T x_9) {
        static_assert(Rank == 10, "mini_vector : incorrect number of variables in constructor");
        _data[0] = x_0;
        _data[1] = x_1;
        _data[2] = x_2;
        _data[3] = x_3;
        _data[4] = x_4;
        _data[5] = x_5;
        _data[6] = x_6;
        _data[7] = x_7;
        _data[8] = x_8;
        _data[9] = x_9;
      };
      ///copy constructor
      mini_vector(const mini_vector &x) { *this = x; }
      ///move constructor
      mini_vector(mini_vector &&x) noexcept { *this = std::move(x); }

      ///construct on another mini_vector with different element type
      template <typename T2> mini_vector(const mini_vector<T2, Rank> &x) { *this = x; }
      ///construct on a std::vector
      template <typename T2> mini_vector(const std::vector<T2> &v) {
        if (v.size() != Rank) TRIQS_RUNTIME_ERROR << "mini_vector construction : vector size incorrect  : expected " << Rank << " got : " << v.size();
        for (int i = 0; i < Rank; ++i) _data[i] = v[i];
      }
      ///copy assignment operator
      mini_vector &operator=(const mini_vector &x) {
        for (int i = 0; i < Rank; ++i) _data[i] = x._data[i];
        return *this;
      }
      ///move assignement operator
      mini_vector &operator=(mini_vector &&x) = default;

      friend void swap(mini_vector &a, mini_vector &b) { std::swap(a._data, b._data); }
      ///size of the mini_vector
      int size() const { return Rank; }

      template <typename T2> mini_vector &operator=(const mini_vector<T2, Rank> &x) {
        for (int i = 0; i < Rank; ++i) _data[i] = x[i];
        return *this;
      }

      mini_vector &operator=(T x) {
        for (int i = 0; i < Rank; ++i) _data[i] = x;
        return *this;
      }

      T &operator[](size_t i) { return _data[i]; }
      const T &operator[](size_t i) const { return _data[i]; }
      ///conversion to std::vector
      std::vector<T> to_vector() const {
        std::vector<T> V(Rank);
        for (int i = 0; i < Rank; ++i) V[i] = _data[i];
        return V;
      }

      T product_of_elements() const {
        T res = 1;
        for (int i = 0; i < Rank; ++i) res *= _data[i];
        return res;
      }

      friend T sum(mini_vector const &a) {
        T res = 0;
        for (int i = 0; i < Rank; ++i) res += a._data[i];
        return res;
      }

      T *ptr() { return _data; }
      const T *ptr() const { return _data; }
      T *data() { return _data; }
      const T *data() const { return _data; }

      std::string to_string() const {
        std::stringstream fs;
        fs << "(";
        for (int i = 0; i < Rank; ++i) fs << (i == 0 ? "" : " ") << (*this)[i];
        fs << ")";
        return fs.str();
      }
      ///append element to mini_vector (increases rank by 1)
      mini_vector<T, Rank + 1> append(T const &x) const {
        mini_vector<T, Rank + 1> res;
        for (int i = 0; i < Rank; ++i) res[i] = _data[i];
        res[Rank] = x;
        return res;
      }

      ///prepend element to mini_vector (increases rank by 1)
      mini_vector<T, Rank + 1> front_append(T const &x) const {
        mini_vector<T, Rank + 1> res;
        for (int i = 0; i < Rank; ++i) res[i + 1] = _data[i];
        res[0] = x;
        return res;
      }

      mini_vector<T, Rank - 1> pop() const {
        mini_vector<T, Rank - 1> res;
        for (int i = 0; i < Rank - 1; ++i) res[i] = _data[i];
        return res;
      }

      mini_vector<T, Rank - 1> front_pop() const {
        mini_vector<T, Rank - 1> res;
        for (int i = 1; i < Rank; ++i) res[i - 1] = _data[i];
        return res;
      }

      template <int N> mini_vector<T, Rank - N> front_mpop() const {
        mini_vector<T, Rank - N> res;
        for (int i = N; i < Rank; ++i) res[i - N] = _data[i];
        return res;
      }

      friend std::ostream &operator<<(std::ostream &out, mini_vector const &v) { return out << v.to_string(); }
      friend std::stringstream &operator<<(std::stringstream &out, mini_vector const &v) {
        out << v.to_string();
        return out;
      }
    }; // class mini_vector

    // ------------ specialization for size 0 -------------
    template <typename T> class mini_vector<T, 0> {
      T _data[1];
      static constexpr int Rank = 0;

      public:
      mini_vector() {}
      mini_vector(no_init_tag) {}
      template <typename U> mini_vector(mini_vector<U, 0>) {}
      template <typename T2> mini_vector(const std::vector<T2> &v) {
        if (v.size() != 0) TRIQS_RUNTIME_ERROR << "mini_vector construction : vector size incorrect  : expected " << 0 << " got : " << v.size();
      }

      T &operator[](size_t i) { return _data[i]; }
      const T &operator[](size_t i) const { return _data[i]; }
      T *ptr() { return _data; }
      const T *ptr() const { return _data; }
      T *data() { return _data; }
      const T *data() const { return _data; }
      friend std::ostream &operator<<(std::ostream &out, mini_vector const &v) { return out << "[]"; }

      ///prepend element to mini_vector (increases rank by 1)
      mini_vector<T, Rank + 1> front_append(T const &x) const {
        mini_vector<T, Rank + 1> res;
        for (int i = 0; i < Rank; ++i) res[i + 1] = _data[i];
        res[0] = x;
        return res;
      }

      /// FIXME : temporary to make compile
      T product_of_elements() const {
        T res = 1;
        for (int i = 0; i < Rank; ++i) res *= _data[i];
        return res;
      }
    };

    // ------------- + ou - --------------------------------------

    template <typename T, int R> mini_vector<T, R> operator+(mini_vector<T, R> const &v1, mini_vector<T, R> const &v2) {
      mini_vector<T, R> r(no_init_tag{});
      for (int i = 0; i < R; ++i) r[i] = v1[i] + v2[i];
      return r;
    }

    template <typename T, int R> mini_vector<T, R> operator-(mini_vector<T, R> const &v1, mini_vector<T, R> const &v2) {
      mini_vector<T, R> r(no_init_tag{});
      for (int i = 0; i < R; ++i) r[i] = v1[i] - v2[i];
      return r;
    }

    // ------------- Comparison --------------------------------------

    template <typename T, int R> bool operator==(mini_vector<T, R> const &v1, mini_vector<T, R> const &v2) {
      for (int i = 0; i < R; ++i) {
        if (v1[i] != v2[i]) return false;
      }
      return true;
    }

    template <typename T, int R> bool operator!=(mini_vector<T, R> const &v1, mini_vector<T, R> const &v2) { return (!(v1 == v2)); }

    // ------------- join  --------------------------------------
    template <typename T1, typename T2, int R1, int R2> mini_vector<T1, R1 + R2> join(mini_vector<T1, R1> const &v1, mini_vector<T2, R2> const &v2) {
      mini_vector<T1, R1 + R2> res;
      for (int i = 0; i < R1; ++i) res[i] = v1[i];
      for (int i = 0; i < R2; ++i) res[R1 + i] = v2[i];
      return res;
    }

    // ------------- dot --------------------------------------
    template <typename T1, typename T2, int Rank> T1 dot_product(mini_vector<T1, Rank> const &v1, mini_vector<T2, Rank> const &v2) {
      T1 res = 0;
      for (int i = 0; i < Rank; ++i) res += v1[i] * v2[i];
      return res;
    }

    // ------- index maker ----------------

    constexpr bool __and() { return true; }
    template <typename... B> constexpr bool __and(bool b, B... bs) { return b && __and(bs...); }

    template <typename... U> mini_vector<long, sizeof...(U)> mindex(U... x) {
      static constexpr bool is_all_ints = __and(std::is_integral<U>::value...);
      static_assert(is_all_ints, "Every argument of the mindex function must be an integer type");
      return mini_vector<long, sizeof...(U)>(x...); // better error message than with { }
    }


  } // namespace utility
} // namespace triqs

namespace itertools{
  // Extend the product_range functionality for mini_vector
  // FIXME This should go together with the removal of mini_vector!
  namespace details {
    template <typename Int, size_t... Is>
    [[gnu::always_inline]] auto product_range_impl(triqs::utility::mini_vector<Int, sizeof...(Is)> const &mini_vec, std::index_sequence<Is...>) {
      return product_range(mini_vec[Is]...);
    }
  } // namespace details

  template <typename Int, int N, typename EnableIf = std::enable_if_t<std::is_integral_v<Int>, int>>
  auto product_range(triqs::utility::mini_vector<Int, N> const &idx_tpl) {
    return details::product_range_impl(idx_tpl, std::make_index_sequence<N>{});
  }
}

namespace std { // overload std::get to work with it

  template <int i, typename T, int R> auto get(triqs::utility::mini_vector<T, R> &v) DECL_AND_RETURN(v[i]);
  template <int i, typename T, int R> auto get(triqs::utility::mini_vector<T, R> const &v) DECL_AND_RETURN(v[i]);
  template <typename T, int R> class tuple_size<triqs::utility::mini_vector<T, R>> : public std::integral_constant<size_t, R> {};

} // namespace std
