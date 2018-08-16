/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by O. Parcollet, T. Ayral.
 * Copyright (C) 2018 by Simons Foundation
 *   author : O. Parcollet, H. Strand, P. Dumitrescu
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
#include <triqs/utility/itertools.hpp>
#include <type_traits>
#include <vector>
#include "./common.hpp"

namespace triqs::stat::accumulators {

  template <typename T> class binned {

    vec_t<T> data;
    std::vector<long> counts;
    int bid        = 0;
    long bin_size  = 1;
    int max_n_bins = 0;

    template <typename F> void h5_serialize(F &&f) {
      f("data", data), f("counts", counts), f("bid", bid, h5::as_attribute), f("bin_size", bin_size, h5::as_attribute),
         f("max_n_bins", max_n_bins, h5::as_attribute);
    }
    friend class h5::access;

    public:
    static std::string hdf5_scheme() { return "binned_series"; }

    binned() = default;

    binned(T const &zero, long bin_size, int max_n_bins) : data(2, zero), counts(2, 0), bin_size(bin_size), max_n_bins(max_n_bins) {}

    //binned(long bin_size, int max_n_bins = 0) : binned(T{}, bin_size, max_n_bins) {}

    // debug only
    void print() {
      for (int i = 0; i < data.size(); ++i) std::cout << i << "   " << data[i] << "  " << counts[i] << std::endl;
    }

    /// THIS << x : add a new x in the binned series
    template <typename U> void operator<<(U const &u) {
      data[bid] += u;

      // Advance the counters.
      //for (int i = 0; i < data.size(); ++i)  std::cout  << " ++ "<< i << "   " << data[i] << "  "<< counts[i] << std::endl;
      counts[bid]++;
      if (counts[bid] == bin_size) {
        if (bid < data.size() - 1)
          bid++;
        else {
          if ((max_n_bins == 0) or (data.size() < max_n_bins)) {
            for (int i = 0; i < 2; ++i) { // add 2 bins
              data.push_back(data[0]);
              data[data.size() - 1] = 0;
              counts.push_back(0);
            }
            ++bid;
          } else {
            compress();
            bid = data.size() / 2;
            bin_size *= 2;
          }
        }
      }
    }

    // Make a new zero
    T zero() const {
      T r = data[0];
      r   = 0;
      return r;
    }

    // Result is a vector of variance
    friend vec_t<T> reduce(binned const &x) {
      vec_t<T> r(x.data);                                                                   // FIXME : avoid copy
      for (int n = 0; n < x.data.size(); ++n) r[n] = x.data[n] / std::max(1l, x.counts[n]); // if counts[n] ==0, data is 0, then it stays 0.
      return r;
    }

    private:
    // simple compression.
    void compress() {
      const int L = data.size() / 2;
      //std::cerr << " COMPRESS" << std::endl;
      //for (int i = 0; i < 2 * L; ++i) std::cout << i << "   " << data[i] << "  " << counts[i] << std::endl;
      //std::cout << " -------" << std::endl;

      data[0] += data[1];
      counts[0] += counts[1];
      for (int i = 1; i < L; ++i) {
        data[i]   = data[2 * i] + data[2 * i + 1];
        counts[i] = counts[2 * i] + counts[2 * i + 1];
      }
      for (int i = L; i < 2 * L; ++i) {
        data[i]   = 0;
        counts[i] = 0;
      }
      //for (int i = 0; i < 2 * L; ++i) std::cout << i << "   " << data[i] << "  " << counts[i] << std::endl;
      //std::cerr << "END COMPRESS" << std::endl;
    }
  };

} // namespace triqs::stat::accumulators
