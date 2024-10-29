// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2021 Simons Foundation
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

#include "./mean_error.hpp"
#include "./utils.hpp"

#include <mpi/mpi.hpp>
#include <nda/nda.hpp>

#include <cstddef>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

namespace triqs::stat {

  namespace detail {

    // Implementation of the jackknife resampling.
    auto jackknife_impl(std::optional<mpi::communicator> comm, auto &&f, StatCompatibleRange auto &&...rgs) { // NOLINT (ranges need not be forwarded)
      // check that the ranges all have the same size
      auto rgs_tup = std::tie(rgs...);
      auto size    = std::ranges::size(std::get<0>(rgs_tup));
      if (((std::ranges::size(rgs) != size) or ...)) throw std::runtime_error("Error in jackknife: Ranges must have the same size.");

      // all_reduce the size of the ranges if a communicator is provided
      if (comm) size = mpi::all_reduce(size, *comm);

      // get the mean and the jackknife samples of each range and store them in a tuple
      auto [mean_tup, jk_tup] = [&comm, size, &rgs_tup]<std::size_t... Is>(std::index_sequence<Is...>) {
        auto mtup = std::make_tuple(mean_mpi(comm, std::get<Is>(rgs_tup))...);
        auto jtup = std::make_tuple(std::ranges::transform_view(
           std::get<Is>(rgs_tup), [m = std::get<Is>(mtup), size](auto const &x) { return (size * m - x) / (size - 1); })...);
        return std::make_pair(mtup, jtup);
      }(std::make_index_sequence<sizeof...(rgs)>{});

      // get a zipped view of the jackknife samples from all the ranges
      auto zipped = std::apply([](auto &&...jks) { return std::ranges::zip_view(jks...); }, jk_tup);

      // compute the jackknife mean and error (we need regular types to avoid dangling references)
      auto f_wrapped      = [f](auto const &...args) { return nda::make_regular(f(args...)); };
      auto [jk_f, jk_err] = mean_and_err_mpi<error_tag::jk_err>(
         comm, std::ranges::transform_view(zipped, [f_wrapped](auto const &tup) { return std::apply(f_wrapped, tup); }));

      // compute the naive estimate of f(...) and the bias corrected estimate
      auto naive_f = nda::make_regular(std::apply(f, mean_tup));
      auto corr_f  = nda::make_regular(size * naive_f - (size - 1) * jk_f);

      return std::make_tuple(corr_f, jk_err, jk_f, naive_f);
    }

  } // namespace detail

  /**
   * @brief Perform jackknife resampling.
   *
   * @details Calculate the value and error of a general function \$ f \$ of the averages of sampled observables using
   * jackknife resampling.
   *
   * Let \f$ \left( X_1, X_2, \ldots \right) \f$ denote the different observables and let \f$ \left\{ x_j^{(i)} : i =
   * 1, \ldots, N \right\} \f$ be the set of their corresponding samples. We define the deleted averages
   * \f[
   *   \overline{x}_j(i) = \frac{1}{N - 1} \sum_{k \neq i} x_j^{(k)} = \frac{N \overline{x}_j - x_j^{(i)}}{N - 1} \; ,
   * \f]
   * and their corresponding function evaluations
   * \f[
   *   \overline{f}(i) = f\left( \overline{x}_1(i), \overline{x}_2(i), \ldots \right) \; .
   * \f]
   *
   * We can then calculate the following quantities:
   * - naive function estimate: \f$ \overline{f} = f(\overline{x}_1, \overline{x}_2 \ldots) \f$,
   * - jackknife function estimate: \f$ \overline{f}_{\mathrm{JK}} = \frac{1}{N} \sum_{i = 1}^N \overline{f}(i) \f$,
   * - bias corrected function estimate: \f$ f_{\mathrm{corr}} = N \overline{f} - (N - 1) \overline{f}_{\mathrm{JK}}
   * \f$ and
   * - jackknife error estimate: \f$ s_f^2 = \frac{N - 1}{N} \sum_{i = 1}^N \left( \overline{f}(i) -
   * \overline{f}_{\mathrm{JK}} \right)^2 \f$.
   *
   * @tparam F Callable type.
   * @tparam R triqs::stat::StatCompatibleRange type.
   * @tparam Rs triqs::stat::StatCompatibleRange types.
   * @param f Callable to be applied to the averages of the sampled observables.
   * @param rg Range containing the samples of one observable.
   * @param rgs Range objects containing the samples of the other observables.
   * @return `std::tuple` containing the bias corrected function estimate, the jackknife error estimate, the jackknife
   * function estimate and the naive function estimate.
   */
  template <typename F, StatCompatibleRange R, StatCompatibleRange... Rs>
    requires(not std::same_as<std::remove_cvref_t<F>, mpi::communicator>)
  auto jackknife(F &&f, R &&rg, Rs &&...rgs) { // NOLINT (ranges need not be forwarded)
    return detail::jackknife_impl(std::nullopt, std::forward<F>(f), rg, rgs...);
  }

  /**
   * @brief Perform jackknife resampling with MPI support.
   *
   * @details See triqs::stat::jackknife for more details.
   *
   * @tparam F Callable type.
   * @tparam R triqs::stat::StatCompatibleRange type.
   * @tparam Rs triqs::stat::StatCompatibleRange types.
   * @param comm MPI communicator.
   * @param f Callable to be applied to the averages of the sampled observables.
   * @param rg Range containing the samples of one observable.
   * @param rgs Range objects containing the samples of the other observables.
   * @return `std::tuple` containing the bias corrected function estimate, the jackknife error estimate, the jackknife
   * function estimate and the naive function estimate.
   */
  template <typename F, StatCompatibleRange R, StatCompatibleRange... Rs>
  auto jackknife_mpi(mpi::communicator comm, F &&f, R &&rg, Rs &&...rgs) { // NOLINT (ranges need not be forwarded)
    return detail::jackknife_impl(comm, std::forward<F>(f), rg, rgs...);
  }

} // namespace triqs::stat
