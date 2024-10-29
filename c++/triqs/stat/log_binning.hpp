// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
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
#include "mean_error.hpp"
#include <mpi/mpi.hpp>
#include <mpi/vector.hpp>
#include <optional>
#include <triqs/arrays.hpp>
#include <nda/clef/clef.hpp>
#include <h5/h5.hpp>
#include <tuple>
#include <type_traits>
#include <vector>
#include <algorithm>

namespace triqs::stat {

  // ************************************************************************************************
  // Updating Formulae
  // Mk \equiv \frac{1}{k} \sum_{i=1}^k x_i
  // Qk \equiv \sum_{i=1}^k (x_i - M_k)^2
  // Update formula
  // M_{k} = M_{k-1} + \frac{x- M_{k-1}}{k}
  // Q_{k} = Q_{k-1} + \frac{(k-1)(x- M_{k-1})^2}{k}
  // Ref: see e.g. Chan, Golub, LeVeque. Am. Stat. 37, 242 (1983) and therein
  template <typename T> struct log_binning {

    std::vector<T> Mk; // Cf comments
    using Q_t = get_real_t<T>;
    std::vector<Q_t> Qk; // Cf comments
    int max_n_bins = 0;
    std::vector<T> acc;         // partial accumulators at size 2^(n+1). WARNING: acc[n] correspond to sum[n+1] to save unecessary temporaries
    std::vector<int> acc_count; // number of elements for partial accumulators at size 2^(n+1)
    long count = 0;             // Number of elements added to accumulator

    //static std::string hdf5_format() { return "log_binning"; }

    log_binning() = default;

    log_binning(T const &data_instance, int max_n_bins) : max_n_bins(max_n_bins) {
      if (max_n_bins == 0) return;
      T data_instance_local = data_instance;
      data_instance_local   = 0;
      if (max_n_bins > 0) {
        Qk.reserve(max_n_bins);
        Mk.reserve(max_n_bins);
        acc.reserve(max_n_bins - 1);
        acc_count.reserve(max_n_bins - 1);
      }
      // If max_n_bins == 1, we don't need acc / acc_count, otherwise we initialize the first element
      if (max_n_bins != 1) {
        acc.emplace_back(data_instance_local);
        acc_count.push_back(0);
      }
      // Multiply to ensure element can be element-wise multiplied [FIXME with concepts]
      Qk.emplace_back(abs_square(data_instance_local));
      Mk.emplace_back(std::move(data_instance_local));
    }

    [[nodiscard]] long n_bins() const { return Qk.size(); }

    template <typename U> log_binning<T> &operator<<(U const &x) {
      if (max_n_bins == 0) return *this;

      using nda::conj;
      using nda::real;

      ++count;

      // If max_n_bins == 1, there is only one (Mk, Qk) and we skip direclty to updating that below
      if (max_n_bins != 1) {
        // go up in n as long as the acc_count becomes full and add the acc in the
        // then go down, and store the acc

        int n = 0;
        for (; n < acc.size(); ++n) {
          if (n == 0) {
            acc[n] += x; // n = 0 case is special, as it involves new data input
          } else {
            acc[n] += acc[n - 1];
          }
          acc_count[n]++;
          if (acc_count[n] < 2) break;
        }

        // When reaching power of 2 nr data points, add a new pair of (Mk, Qk) = (acc.back(), 0)
        // and add an extra space in acc, acc_count
        if (count == (1 << acc.size()) && (max_n_bins < 0 || n_bins() < max_n_bins)) {
          T last_acc = acc.back(); // Force copy

          if (max_n_bins < 0 || n_bins() < (max_n_bins - 1)) {
            acc.emplace_back(last_acc);
            acc_count.push_back(1);
          }

          Mk.emplace_back(std::move(last_acc));
          auto data_instance_q = Qk[0];
          data_instance_q      = 0;
          Qk.emplace_back(std::move(data_instance_q));
        }

        n--;
        for (; n >= 0; n--) {
          auto bin_capacity = (1ul << (n + 1));                    // 2^(n+1)
          T x_m             = (acc[n] / bin_capacity - Mk[n + 1]); // Force T if expression template.
          auto k            = count / bin_capacity;
          Qk[n + 1] += static_cast<nda::get_value_t<Q_t>>((k - 1) / double(k)) * abs_square(x_m);
          Mk[n + 1] += x_m / k;
          acc_count[n] = 0;
          acc[n]       = 0;
        }
      }

      // Update the (Mk, Qk) pair with no binning (bin capacity: 2^0)
      auto k = count;
      T x_m  = (x - Mk[0]);
      Qk[0] += static_cast<nda::get_value_t<Q_t>>((k - 1) / double(k)) * abs_square(x_m);
      Mk[0] += x_m / k;

      return *this;
    }
  };

  // HDF5
  template <typename T> void h5_write(h5::group g, std::string const &name, log_binning<T> const &l) {
    auto gr = g.create_group(name);
    h5_write(gr, "Qk", l.Qk);
    h5_write(gr, "Mk", l.Mk);
    h5_write(gr, "acc", l.acc);
    h5_write(gr, "count", l.count);
    h5_write(gr, "acc_count", l.acc_count);
  }

  template <typename T> void h5_read(h5::group g, std::string const &name, log_binning<T> &l) {
    auto gr = g.open_group(name);
    h5_read(gr, "Qk", l.Qk);
    h5_read(gr, "Mk", l.Mk);
    h5_read(gr, "acc", l.acc);
    h5_read(gr, "count", l.count);
    h5_read(gr, "acc_count", l.acc_count);
  }

} // namespace triqs::stat
