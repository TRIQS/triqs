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
#include <type_traits>
#include <vector>
#include <optional>

#include "./average.hpp"
#include "./variance.hpp"
#include "./auto_correlation.hpp"
#include "./binned.hpp"

namespace triqs::stat {

  struct accumulator_cargs {
    bool average                = false;
    bool variance               = false;
    int auto_correlation_n_bins = 0;
    int binning_l_bins          = 0;
    int binning_n_max_bins      = 0;
  };

  /**
   * mc_value<T> contains various analysis for T, with a dynamical choice.
   *
   * T : any regular type
   *
   */
  template <typename T> struct mc_value {

    using V = vec_t<T>;

    std::optional<T> value;
    std::optional<T> variance;
    std::optional<V> auto_correlation;

    template <typename F> void h5_serialize(F &&f) { f("value", value), f("variance", variance), f("auto_correlation", auto_correlation); }

    /**
     *     @return HDF5 scheme name
     */
    static std::string hdf5_scheme() { return "mc_value"; }
  }; // namespace triqs::stat

  // ************************************************************************************************

  /**
   *
   * T : any regular type with additional constraint 
   *   T x; x=0;  
   *
   */
  template <typename T> class accumulator {

    std::optional<accumulators::average<T>> _average;
    std::optional<accumulators::variance<T>> _variance;
    std::optional<accumulators::auto_correlation<T>> _auto_correlation;
    std::optional<accumulators::binned<T>> _bins;

    std::optional<T> _work_var; // Optionally, we can preallocate a work variable for large object. Useful ? Not serialized

    template <typename F> void h5_serialize(F &&f) {
      f("average", _average);
      f("variance", _variance);
      f("auto_correlation", _auto_correlation);
      f("bins", _bins);
    }
    friend class h5::access;

    public:
    accumulator() : accumulator(accumulator_cargs{}) {}

    accumulator(accumulator_cargs args) : accumulator(T{}, args) {}

    accumulator(T zero, accumulator_cargs args) {
      zero = 0;
      if (args.variance and args.average) throw std::invalid_argument("Variance and Average : Variance already produces the average");
      if (args.binning_l_bins) _bins.emplace(zero, args.binning_l_bins, args.binning_n_max_bins);
      if (args.auto_correlation_n_bins) _auto_correlation.emplace(zero, args.auto_correlation_n_bins);
      if (args.variance) _variance.emplace(std::move(zero));
      if (args.average) _average.emplace(std::move(zero));
    }

    T &get_work_var() {
      if (!_work_var) _work_var = zero();
      return *_work_var;
    }

    T zero() const {
      if (_average) return _average->zero();
      if (_variance) return _variance->zero();
      if (_auto_correlation) return  _auto_correlation->zero();
      if (_bins) return  _bins->zero();
      throw std::logic_error("zero can not work for an empty accumulator");
    }

    accumulators::average<T> const &average() const {
      if (!_average) TRIQS_RUNTIME_ERROR << "No average !";
      return *_average;
    }

    accumulators::variance<T> const &variance() const {
      if (!_variance) TRIQS_RUNTIME_ERROR << "No variance !";
      return *_variance;
    }

    accumulators::binned<T> const &bins() const {
      if (!_bins) TRIQS_RUNTIME_ERROR << "No bins !";
      return *_bins;
    }

    accumulators::auto_correlation<T> const &auto_correlation() const {
      if (!_auto_correlation) TRIQS_RUNTIME_ERROR << "No autocorrelation !";
      return *_auto_correlation;
    }

    template <typename U> void operator<<(U const &u) {
      if (_average) *_average << u;
      if (_variance) *_variance << u;
      if (_auto_correlation) *_auto_correlation << u;
      if (_bins) *_bins << u;
      if (_work_var) *_work_var = 0;
    }

    /**
     *     @return HDF5 scheme name
     */
    static std::string hdf5_scheme() { return "accumulator"; }

    private:
    template <typename Lambda> mc_value<T> _reduce_impl(Lambda &&f) const {
      mc_value<T> res;
      if (_average and not _variance) res.value = f(*_average);
      if (_variance) std::tie(res.value, res.variance) = f(*_variance);
      if (_auto_correlation) res.auto_correlation = f(*_auto_correlation);
      return res;
    }

    public:
    friend mc_value<T> reduce(accumulator const &a) {
      return a._reduce_impl([](auto &&x) { return reduce(x); });
    }

    friend mc_value<T> mpi_reduce(accumulator const &a, mpi::communicator c) {
      return a._reduce_impl([c](auto &&x) { return mpi_reduce(x, c); });
    }

    friend mc_value<T> mpi_all_reduce(accumulator const &a, mpi::communicator c) {
      return a._reduce_impl([c](auto &&x) { return mpi_all_reduce(x, c); });
    }

  }; // namespace triqs::stat

  // FIX comple case
  // Fix COVARAINCE

} // namespace triqs::stat
