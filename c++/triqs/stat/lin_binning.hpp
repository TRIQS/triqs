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

  template <typename T> struct lin_binning {
    long max_n_bins     = 0; // Maximum number of bins
    long bin_capacity   = 1; // Current capacity of each bin (must be >= 1)
    long last_bin_count = 0; // Number of data points the currently active bin [bins.back()]
    std::vector<T> bins;     // Bins with accumulated data (stores means)
    long count = 0;          // Total number of elements added to accumulator: Information only

    // Callback function after compressing bins
    using callback_t    = std::function<void(std::vector<T> const &)>;
    callback_t callback = {};

    // static std::string hdf5_format() { return "linear_bins"; }

    lin_binning() = default;

    lin_binning(T const &data_instance, long max_n_bins, long bin_capacity) : max_n_bins(max_n_bins), bin_capacity(bin_capacity) {
      if (max_n_bins == 0) return;
      T data_instance_local = data_instance;
      data_instance_local   = 0;
      if (max_n_bins > 0) { bins.reserve(max_n_bins); }
      bins.emplace_back(std::move(data_instance_local));
    }

    // Optional constructor setting the callback function
    lin_binning(T const &data_instance, long max_n_bins, long bin_capacity, callback_t const &callback)
       : lin_binning(data_instance, max_n_bins, bin_capacity) {
      this->callback = callback;
    }

    [[nodiscard]] long n_bins() const { return bins.size(); }

    template <typename U> lin_binning<T> &operator<<(U &&x) {
      ++count;
      if (max_n_bins == 0) return *this;
      // Check if all bins are full and compress if needed
      if (max_n_bins > 1 && n_bins() == max_n_bins && last_bin_count == bin_capacity) {
        if (callback) callback(bins); // Callback if defined
        compress(2);                  // Adjusts bin_capacity & last_bin_count
      }
      // Check if current bin full: push new bin or add data to current bin
      if (last_bin_count == bin_capacity && max_n_bins != 1) {
        bins.emplace_back(std::forward<U>(x));
        last_bin_count = 1;
      } else {
        last_bin_count++;
        bins.back() += (x - bins.back()) / last_bin_count;
      }
      return *this;
    }

    // Compresses bins by scaling up bin_capacity by compression_factor (>= 2).
    void compress(int compression_factor) {
      if (max_n_bins == 0 || compression_factor < 2) return;
      const int bins_left   = n_bins() % compression_factor;
      int n_bins_new        = n_bins() / compression_factor;
      int n_bins_last_chunk = compression_factor;
      if (bins_left != 0) {
        n_bins_new++;
        n_bins_last_chunk = bins_left;
      }
      // Compress data into new bins, except the last new bin
      for (int i = 0; i < n_bins_new - 1; ++i) {
        if (i != 0) { bins[i] = std::move(bins[compression_factor * i]); }
        for (int j = 1; j < compression_factor; j++) { bins[i] += bins[compression_factor * i + j]; }
        bins[i] /= compression_factor;
      }
      // Last new bin is special: last old bin could be filled below capacity
      int new_last_bin_count = last_bin_count + (n_bins_last_chunk - 1) * bin_capacity;
      auto &new_last_bin     = bins[n_bins_new - 1];
      new_last_bin           = std::move(bins[compression_factor * (n_bins_new - 1)]);
      // If n_bins_last_chunk == 1, we have already copied its value above
      if (n_bins_last_chunk > 1) {
        for (int j = 1; j < n_bins_last_chunk - 1; j++) { new_last_bin += bins[compression_factor * (n_bins_new - 1) + j]; }
        new_last_bin *= bin_capacity; // full bins in last chunk
        new_last_bin += bins[compression_factor * (n_bins_new - 1) + (n_bins_last_chunk - 1)] * last_bin_count;
        new_last_bin /= new_last_bin_count;
      }
      // Adjust final parameters
      bins.resize(n_bins_new);
      last_bin_count = new_last_bin_count;
      bin_capacity *= compression_factor;
    }
  };

  template <typename T> void h5_write(h5::group g, std::string const &name, lin_binning<T> const &l) {
    auto gr = g.create_group(name);
    h5_write(gr, "bins", l.bins);
    h5_write(gr, "last_bin_count", l.last_bin_count);
    h5_write(gr, "bin_capacity", l.bin_capacity);
    h5_write(gr, "max_n_bins", l.max_n_bins);
  }

  template <typename T> void h5_read(h5::group g, std::string const &name, lin_binning<T> &l) {
    auto gr = g.open_group(name);
    h5_read(gr, "bins", l.bins);
    h5_read(gr, "last_bin_count", l.last_bin_count);
    h5_read(gr, "bin_capacity", l.bin_capacity);
    h5_read(gr, "max_n_bins", l.max_n_bins);
  }

} // namespace triqs::stat
