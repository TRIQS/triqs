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
        si_minus_one = original_series.size() - 1;
        // compute the sum of B into a new T.
        T sum = original_series[0];
        for (long i = 1; i < original_series.size(); ++i) sum += original_series[i];
        s = sum / si_minus_one;
      }

      jackknifed_t(V const &bins, mpi::communicator c) : original_series(bins) {
        si_minus_one = mpi::all_reduce(original_series.size(), c) - 1;
        // compute the sum of B into a new T.
        T sum = original_series[0];
        for (long i = 1; i < original_series.size(); ++i) sum += original_series[i];
        s = T{mpi::all_reduce(sum, c)} / si_minus_one;
      }

      T operator[](long i) const { return s - original_series[i] / si_minus_one; }
    };

    //template <class B> jackknifed_t(B b)->jackknifed_t<B>;

    //------------------------------------------------------

    // implementation of jackknife : A are vector like  : [i], size
    // @param f the function
    // @return (M, Q, N, sum) for X = f(a...)
    //         M : average of f(jackknifed(a))
    //         Q : variance of f(jackknifed(a))
    //         N : number of samples
    //         sum : sum of f(a...);
    // compute on ONE node. no MPI here.
    template <typename F, typename... Jacknifed> auto jackknife_impl1(F &&f, Jacknifed const &... ja) {

      // Check that the size of the series are equal
      std::array<long, sizeof...(Jacknifed)> dims{long(ja.original_series.size())...};
      long N = dims[0];
      if (N == 0) TRIQS_RUNTIME_ERROR << "No data !";
      if (!std::all_of(dims.begin(), dims.end(), [N](long i) { return i == N; })) TRIQS_RUNTIME_ERROR << " Jackknife : size mismatch";

      auto M   = make_regular(f(ja[0]...));
      auto sum = make_regular(f(ja.original_series[0]...));
      auto Q   = M;
      auto x_M = M; // temporary with same size as M
      Q        = 0;

      for (long i = 1; i < N; ++i) {
        sum += make_regular(f(ja.original_series[i]...));
        x_M = f(ja[i]...) - M;
        long k = i + 1;
        Q += conj_r(x_M) * x_M * (k - 1) / k;
        M += x_M / k;
      }
      return std::make_tuple(M, Q, N, sum);
    }

    // a small struct with 3 real or complexes, with the addition for the MPI reduction
    template <typename RC> // real or complex
    struct MQN_t {
      RC M, Q, n;

      friend MQN_t operator+(MQN_t const &x, MQN_t const &y) {
        RC n  = x.n + y.n;
        RC M  = (x.n * x.M + y.n * y.M) / n;
        RC dM = x.M - y.M;
        RC Q  = x.Q + y.Q + (x.n * y.n) / n * conj_r(dM) * dM;
        return {M, Q, n};
      }
    };

  } // namespace details

  /**
   * Compute the average and error with jackknife method
   *
   * @param c (TRIQS) MPI communicator
   * @tparam F type of g
   * @param a list of series 
   * @param f a function (a1,a2,a3,...) -> f(a1,a2,a3...)
   * @return  (average corrected with jackknife bias, jackknife error, average of jacknife series, naive average)
   *       FIXME : Write the math formula
   *
   * The calculation is performed over the nodes.
   * Each node compute the average and variance, which are then reduced (not all-reduced) to the node 0.
   * Precondition : a series all have the same size
   */
  template <typename F, typename... A> auto jackknife(mpi::communicator c, F &&f, A const &... a) {

    auto [M, Q, N, sum] = details::jackknife_impl1(std::forward<F>(f), details::jackknifed_t(a, c)...);

    // Now the reduction over the nodes.
    sum = mpi::reduce(sum, c);
    N   = mpi::reduce(N, c);
    sum /= N;

    int root  = 0;
    using M_t = decltype(M);
    // first case : M, Q are an array
    if constexpr (triqs::arrays::is_amv_value_or_view_class<M_t>::value) {

      using T     = typename M_t::value_type;
      using MQN_t = details::MQN_t<T>;

      arrays::array<T, M_t::rank + 1> MQ(M.shape().append(3));
      arrays::ellipsis ___;
      MQ(___, 0) = M;
      MQ(___, 1) = Q;
      MQ(___, 2) = N; // constant... it is a waste of space.

      auto MQ_final = MQ;

      // reinterpret for mpi reduction
      auto *p         = MQ.data_start();
      MPI_Datatype dt = mpi::make_datatype_from_tie(std::tie(*(p), *(p + 1), *(p + 2))); // tie the first M, Q, N

      MPI_Reduce(MQ.data_start(), MQ_final.data_start(), M.size(), dt, mpi::map_add<MQN_t>(), root, c.get());

      auto M       = MQ(___, 0);
      auto Q       = MQ(___, 1);
      auto std_err = make_regular(std::sqrt((N - 1) / double(N)) * sqrt(Q));
      auto average = make_regular(N * (sum - M) + M);
      return std::make_tuple(average, std_err, M, sum);

    } else { //M, Q are scalar, not arrays

      // invert condition : REAL OR COMPLEX, else array
      using T     = M_t;
      using MQN_t = details::MQN_t<T>;

      MQN_t s{M, Q, T(N)};
      MQN_t s_final;
      MPI_Datatype dt = mpi::make_datatype_from_tie(std::tie(s.M, s.Q, s.n));
      MPI_Reduce(&s, &s_final, 1, dt, mpi::map_add<MQN_t>(), root, c.get());

      auto std_err = std::sqrt((N - 1) / double(N)) * std::sqrt(s.Q);
      auto M       = s.M;
      auto average = N * (sum - M) + M;
      return std::make_tuple(average, std_err, M, sum);
    }
  }

  /**
   * Same 
   * @tparam T 
   */
  template <typename F, typename... T> auto jackknife(mpi::communicator c, F &&f, accumulator<T> const &... a) {
    return jackknife(c, std::forward<F>(f), a.linear_bins()...);
  }

  /**
   * Compute the average and error with jackknife method
   *
   * @tparam F type of g
   * @param a list of series 
   * @param f a function (a1,a2,a3,...) -> f(a1,a2,a3...)
   * @return  (average corrected with jackknife bias, jackknife error, average of jacknife series, naive average)
   *       FIXME : Write the math formula
   *
   * The calculation is performed on the current node only
   * Each node compute the average and variance, which are then reduced (not all-reduced) to the node 0.
   * Precondition : a series all have the same size
   */
  template <typename F, typename... A> auto jackknife_serial(F &&f, A const &... a) {
    auto [M, Q, N, sum] = details::jackknife_impl1(std::forward<F>(f), details::jackknifed_t(a)...);
    sum /= N;
    auto std_err = make_regular(std::sqrt((N - 1) / double(N)) * sqrt(Q));
    auto average = make_regular(N * (sum - M) + M);
    return std::make_tuple(average, std_err, M, sum);
  }

  /**
   * Same 
   * @tparam T 
   */
  template <typename F, typename... T> auto jackknife_serial(F &&f, accumulator<T> const &... a) {
    return jackknife_serial(std::forward<F>(f), a.linear_bins()...);
  }

  // FIXME Simple variance and average
  template <typename A> auto mean_and_stderr(mpi::communicator c, A const &a) {
    auto [av, stddev, avJ, avA] = jackknife(
       c, [](auto &&x) { return x; }, a);
    return std::make_pair(av, stddev);
  }

  // FIXME Simple variance and average
  template <typename A> auto mean_and_stderr(A const &a) {
    auto [av, stddev, avJ, avA] = jackknife_serial([](auto &&x) { return x; }, a);
    return std::make_pair(av, stddev);
  }

} // namespace triqs::stat
