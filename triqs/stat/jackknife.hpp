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
#include <functional>
#include <numeric>
#include <type_traits>
#include <triqs/arrays.hpp>
#include <triqs/arrays/math_functions.hpp>
#include <mpi/vector.hpp>

namespace triqs::stat {

  using triqs::arrays::conj_r;

  namespace details {

    // Represent the jacknifed series.
    // @tparam V A vector like object, with size() and []
    //
    template <typename V> struct jackknifed_t {
      V const &original_series;
      using T = std::decay_t<decltype(original_series[0])>;
      T s;
      long si_minus_one;

      jackknifed_t(V const &bins) : original_series(bins) {
        // ENSURE(bins.size()>0)
        si_minus_one = original_series.size() - 1;
        // compute the sum of B into a new T.
        T sum = original_series[0];
        for (long i = 1; i < original_series.size(); ++i) sum += original_series[i];
        s = sum / si_minus_one;
      }

      jackknifed_t(V const &bins, mpi::communicator c) : original_series(bins) {
        // ENSURE(bins.size()>0)
        si_minus_one = mpi::all_reduce(original_series.size(), c) - 1;
        // compute the sum of B into a new T.
        T sum = original_series[0];
        for (long i = 1; i < original_series.size(); ++i) sum += original_series[i];
        s = T{mpi::all_reduce(sum, c)} / si_minus_one;
      }

      // Call. NB : if T is an array, returns an expression template to retard the evaluation
      auto operator[](long i) const { return s - original_series[i] / si_minus_one; }
    };

    //----------------------------------------------------------------

    // implementation of jacknife.
    // mpi iif the c pointer is not null. If null the computation is on this node only.
    template <typename F, typename... Jacknifed> auto jackknife_impl(mpi::communicator *c, F &&f, Jacknifed const &... ja) {

      // N is the size of the series, it should be all equal and !=0
      std::array<long, sizeof...(Jacknifed)> dims{long(ja.original_series.size())...};
      long N = dims[0];
      if (N == 0) TRIQS_RUNTIME_ERROR << "No data !";
      if (not((ja.original_series.size() == N) and ...)) TRIQS_RUNTIME_ERROR << " Jackknife : size mismatch";

      using std::real;
      using std::sqrt;
      using triqs::arrays::abs2;

      // WARNING NB : In this function, we must be very careful never to store an expression template using f.
      // f is provided by the user. It will be provided with ja[i] which
      // might be an expression template itself (hence a temporary).
      // The user is likely be careless with rvalue references
      // e.g. f = [](auto const & x, auto const y){ return x/y;};
      // in such cases, the ja[0] will be dangling unless we use the expression template
      // containing f immediately.
      // Cf below, M, sum and Q computation.
      // Check by ASAN sanitizer on tests
      auto M   = make_regular(f(ja[0]...));
      auto sum = make_regular(f(ja.original_series[0]...));

      for (long i = 1; i < N; ++i) {
        sum += (f(ja.original_series[i]...) - sum) / (i + 1); // Cf NB
        M += (f(ja[i]...) - M) / (i + 1);                     // cf NB
      }

      // Now the reduction over the nodes.
      long Ntot = N;
      if (c) { // MPI version

        Ntot = mpi::reduce(N, *c);

        // sum
        sum *= double(N) / Ntot;             // remove the 1/N
        sum = mpi::reduce(sum, *c, 0, true); // FIXME : mpi API All reduce

        //
        M *= double(N) / Ntot;           // remove the 1/N
        M = mpi::reduce(M, *c, 0, true); // FIXME == mpi_all_reduce_in_place
      }

      // Compute the variance
      auto Q = make_regular(real(M));
      Q      = 0;
      for (long i = 0; i < N; ++i) {
        Q += abs2(f(ja[i]...) - M); // Cf NB
      }

      // reduce Q
      if (c) Q = mpi::reduce(Q, *c, 0, true); // FIXME : mpi API All reduce

      auto std_err = make_regular(std::sqrt((Ntot - 1) / double(Ntot)) * sqrt(Q));
      auto average = make_regular(N * (sum - M) + M);
      return std::make_tuple(average, std_err, M, sum);
    }

  } // namespace details


  // ------------------------------------------------------------------------------

  /// This function can apply a general mathematical function on a series of data or a list of series of data.
  /// It then computes the average and standard error using jackknife resampling.
  // 
  /// FIXME : Write the math formula
  ///
  /// @tparam F type of g
  /// @param a list of series with data
  /// Precondition: the series all have the same size
  /// @param f a function which will act element wise on the series passed in a **a** (a1,a2,a3,...) -> f(a1,a2,a3...)
  /// @return  (average corrected with jackknife bias, jackknife error, average of jacknife series, naive average)
  /// @brief Calculate mean and error of derived data using jackknife resampling
  template <typename F, typename... A> auto jackknife(F &&f, A const &... a) {
    static_assert(not std::is_same_v<std::decay_t<F>, mpi::communicator>,
                  "I see that you pass a mpi:::communicator, you probably want to use jackknife_mpi");
    return details::jackknife_impl(nullptr, std::forward<F>(f), details::jackknifed_t{a}...);
  }

  /// Same
  /// @tparam T
  template <typename F, typename... T> auto jackknife(F &&f, accumulator<T> const &... a) {
    static_assert(not std::is_same_v<std::decay_t<F>, mpi::communicator>,
                  "I see that you pass a mpi:::communicator, you probably want to use jackknife_mpi");
    return jackknife(std::forward<F>(f), a.linear_bins()...);
  }

  // ------------------------------------------------------------------------------

  /// Compute the average and error with jackknife method
  /// The calculation is performed over the nodes.
  /// Each node compute the average and variance, which are then reduced (not all-reduced) to the node 0.
  /// Precondition : a series all have the same size
  /// @param c (TRIQS) MPI communicator
  /// @tparam F type of g
  /// @param a list of series
  /// @param f a function (a1,a2,a3,...) -> f(a1,a2,a3...)
  /// @return  (average corrected with jackknife bias, jackknife error, average of jacknife series, naive average)
  /// @brief Calculate mean and error of derived data using jackknife resampling (MPI Version)
  template <typename F, typename... A> auto jackknife_mpi(mpi::communicator c, F &&f, A const &... a) {
    return details::jackknife_impl(&c, std::forward<F>(f), details::jackknifed_t{a}...);
  }

  /// Same
  /// @tparam T
  template <typename F, typename... T> auto jackknife_mpi(mpi::communicator c, F &&f, accumulator<T> const &... a) {
    return jackknife_mpi(c, std::forward<F>(f), a.linear_bins()...);
  }

} // namespace triqs::stat
