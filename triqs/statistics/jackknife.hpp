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

namespace triqs::stat {

  namespace details {

    template <typename V> struct jackknifed_t {
      V const *_b;
      using T = std::decay_t<decltype(std::declval<V>()[0])>;
      T s;
      long si_minus_one;

      jackknifed_t(V const &b) : _b(&b) {
        si_minus_one = b.size() - 1;
        // compute the sum of B into a new T.
        T sum = b[0];
        for (long i = 1; i < b.size(); ++i) sum += b[i];
        s = sum / si_minus_one;
      }

      jackknifed_t(V const &b, mpi::communicator c) : _b(&b) {
        si_minus_one = mpi_reduce(b.size(), c) - 1;
        // compute the sum of B into a new T.
        T sum = b[0];
        for (long i = 1; i < b.size(); ++i) sum += b[i];

        s = T{mpi_reduce(sum, c)} / si_minus_one;
        mpi_broadcast(s, c);
        mpi_broadcast(si_minus_one, c);
      }
      T operator[](long i) const { return s - (*_b)[i] / si_minus_one; }
    };

    template <typename F, typename Tu, size_t... Is, typename... A> FORCEINLINE auto jack_impl(std::index_sequence<Is...>, long n, F &&f, Tu &&tu) {
      return make_regular(f(std::get<Is>(std::forward<Tu>(tu))[n]...));
    }

    // implementation of jackknife : A are vector like  : [i], size
    template <typename F, typename... A> auto jackknife_impl1(std::tuple<jackknifed_t<A>...> const &jac, F &&f, A const &... a) {

      std::array<long, sizeof...(A)> dims{long(a.size())...};
      if (std::any_of(begin(dims), end(dims), [d0 = dims[0]](auto i) { return i != d0; })) TRIQS_RUNTIME_ERROR << " Jackknife : size mismatch";

      long N = dims[0];
      if (N == 0) TRIQS_RUNTIME_ERROR << "No data !";

      auto Xj = [&jac, &f, _seq = std::index_sequence_for<A...>()](long i) { return details::jack_impl(_seq, i, f, jac); };

      // FIXME : use the better sum technique
      auto sum = make_regular(f(a[0]...));
      using T  = decltype(sum);
      T M      = Xj(0);
      auto Q   = make_regular(0 * conj_r(M) * M); // have a zero with the right dimensions

      for (long i = 1; i < N; ++i) {
        sum += make_regular(f(a[i]...));
        auto x_M = T{Xj(i) - M}; // FIXME we can optimize the T, if we implement sqrt for gf_expr ...
        long k   = i + 1;
        Q += conj_r(x_M) * x_M * (k - 1) / k;
        M += x_M / k;
      }
      sum /= N;
      using std::sqrt;
      auto std_err = make_regular(std::sqrt((N - 1) / double(N)) * sqrt(Q));
      return std::make_tuple(make_regular(N * sum - (N - 1) * M), sum, std_err);
    }

    template <typename F, typename... A> auto jackknife_impl_mpi(mpi::communicator c, F &&f, A const &... a) {
      std::tuple<jackknifed_t<A>...> jac{{a, c}...};
      return details::jackknife_impl1(jac, std::forward<F>(f), a...);
    }

    template <typename F, typename... A> auto jackknife_impl(F &&f, A const &... a) {
      std::tuple<jackknifed_t<A>...> jac{{a}...};
      return details::jackknife_impl1(jac, std::forward<F>(f), a...);
    }

    template <typename T> auto const &_filter(T const &x) { return x; }
    template <typename T> auto const &_filter(accumulator<T> const &x) { return x.linear_bins(); }

  } // namespace details

  // Given a function (a1,a2,a3,...) -> f(a1,a2,a3...),
  // and a list of bins for a1,a2,a3 ... (parameters a...)
  // it returns (average corrected with jackknife bias, naive average, jackknife error)
  // for this quantity
  template <typename F, typename... A> auto jackknife(mpi::communicator c, F &&f, A const &... a) {
    return details::jackknife_impl_mpi(c, std::forward<F>(f), details::_filter(a)...);
    // Do the second reduction of Q and M
    // FIXME : to
  }

  // Given a function (a1,a2,a3,...) -> f(a1,a2,a3...),
  // and a list of bins for a1,a2,a3 ... (parameters a...)
  // it returns (average corrected with jackknife bias, naive average, jackknife error)
  // for this quantity
  template <typename F, typename... A> auto jackknife(F &&f, A const &... a) {
    return details::jackknife_impl(std::forward<F>(f), details::_filter(a)...);
  }

  // Simple variance and average
  template <typename A> auto mean_and_stderr(mpi::communicator c, A const &a) {
    auto [av, av1, stddev] = jackknife(c, [](auto &&x) { return x; }, a);
    return std::make_pair(av, stddev);
  }

  // Simple variance and average
  template <typename A> auto mean_and_stderr(A const &a) {
    auto [av, av1, stddev] = jackknife([](auto &&x) { return x; }, a);
    return std::make_pair(av, stddev);
  }

} // namespace triqs::stat
