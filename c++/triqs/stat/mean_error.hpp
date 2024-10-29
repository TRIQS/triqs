// Copyright (c) 2019-2021 Simons Foundation
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
// Authors: Philipp Dumitrescu, Olivier Parcollet, Nils Wentzell

#pragma once

#include "./utils.hpp"
#include <itertools/itertools.hpp>
#include <mpi/vector.hpp>
#include <triqs/arrays.hpp>

namespace triqs::stat {
  /// Calculate the arithmetic mean [REF] of data in an iterable container.
  /// @tparam V iterable type. Pre-conditions: elements must be addable to each other
  /// @param data Container with data
  /// @return Mean of data; type is deduced from first element of :param:`data`
  /// @example triqs/stat/mean_1.cpp
  /// @brief Calculate mean
  template <typename V> auto mean(V const &data) {
    // ENSURE(data.size() > 0);
    auto mean_calc = data[0];
    mean_calc      = 0;
    for (auto const &[n, x] : itertools::enumerate(data)) { mean_calc += (x - mean_calc) / (n + 1); }
    return mean_calc;
  }

  /// Calculate the arithmetic mean [REF] of data spread over multiple MPI threads. Reduces the answer to all threads.
  /// @tparam V Iterable type. Pre-conditions: elements must be addable to each other and must be MPI reducible
  /// @param data Container with data
  /// @return Mean of data on all threads reduced to all threads; type is deduced from first element of :param:`data`
  /// @example triqs/stat/mean_2.cpp
  /// @brief Calculate mean (MPI Version)
  template <typename V> auto mean_mpi(mpi::communicator c, V const &data) {
    // ENSURE(data.size() > 0);
    auto M = mean(data);
    long N = mpi::all_reduce(data.size(), c);
    M *= double(data.size()) / N;
    mpi::all_reduce_in_place(M, c); //M = mpi::all_reduce(M, c, 0);
    return M;
  }

  /// Calculate arithmetic mean and statistical error [REF] of data in an iterable container.
  /// @tparam V Iterable type. Pre-conditions: elements must be: addable to each other, have element-wise multiplication, have complex conjugation defined via conj [REF] .
  /// @param data Container with data
  /// @return std::pair with [mean, statistical error]; types are deduced from first element of :param:`data`
  /// @example triqs/stat/mean_err_1.cpp
  /// @brief Calculate mean and statistical error
  template <typename V> auto mean_and_err(V const &data) {
    // ENSURE(data.size() > 0);
    long length    = data.size();
    auto mean_calc = mean(data);
    auto err_calc  = make_real(mean_calc);
    err_calc       = 0;
    for (auto const &x : data) { err_calc += abs_square(x - mean_calc) / (length * (length - 1)); }
    err_calc = std::sqrt(err_calc);
    return std::make_pair(mean_calc, err_calc);
  }

  /// Calculate arithmetic mean and statistical error [REF] of data spread over multiple MPI threads. Reduces the answer to all threads.
  /// @tparam V Iterable type. Pre-conditions: elements must be: addable to each other, have element-wise multiplication, have complex conjugation defined via conj [REF], be MPI reducable.
  /// @param data Container with data
  /// @return std::pair with [mean, statistical error]; types are deduced from first element of :param:`data`
  /// @example triqs/stat/mean_err_2.cpp
  /// @brief Calculate mean and statistical error (MPI Version)
  template <typename V> auto mean_and_err_mpi(mpi::communicator c, V const &data) {
    // ENSURE(data.size() > 0);
    long length    = mpi::all_reduce(data.size(), c);
    auto mean_calc = mean_mpi(c, data);
    auto err_calc  = make_real(mean_calc);
    err_calc       = 0;
    for (auto const &x : data) { err_calc += abs_square(x - mean_calc) / (length * (length - 1)); }
    mpi::all_reduce_in_place(err_calc, c);
    err_calc = std::sqrt(err_calc);
    return std::make_pair(mean_calc, err_calc);
  }

  namespace details {

    template <typename T> auto mpi_reduce_MQ(const T &Mi, const get_real_t<T> &Qi, const long &count_i, mpi::communicator c, int root = 0) {
      using nda::conj;
      using nda::real;

      long count_total  = mpi::all_reduce(count_i, c);
      double count_frac = double(count_i) / count_total;

      T M = Mi * count_frac;
      mpi::all_reduce_in_place(M, c);

      T diff          = Mi - M;
      get_real_t<T> Q = Qi + count_i * make_real(conj(diff) * diff);

      mpi::reduce_in_place(Q, c, root);

      return std::make_tuple(M, Q, count_total);
    }

  } // namespace details

} // namespace triqs::stat
