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
    template <typename F, typename... A> auto jackknife_impl(mpi::communicator c, F &&f, A const &... a) {

      std::array<long, sizeof...(A)> dims{long(a.size())...};
      if (std::any_of(begin(dims), end(dims), [d0 = dims[0]](auto i) { return i != d0; })) TRIQS_RUNTIME_ERROR << " Jackknife : size mismatch";

      long N = dims[0];
      if (N == 0) TRIQS_RUNTIME_ERROR << "No data !";

      std::tuple<details::jackknifed_t<A>...> jac{{a, c}...};

      auto _seq = std::index_sequence_for<A...>{};

      // FIXME : use the better sum technique 
      auto sum    = make_regular(f(a[0]...));
      auto sum_j  = details::jack_impl(_seq, 0, f, jac);
      auto sum_j2 = make_regular(conj_r(sum_j) * sum_j);

      for (long i = 1; i < N; ++i) {
        sum += make_regular(f(a[i]...));
        auto tmp = details::jack_impl(_seq, i, f, jac);
        sum_j += tmp;
        sum_j2 += conj_r(tmp) * tmp;
      }
      sum /= N;
      sum_j /= N;
      sum_j2 /= N;
      auto j_variance = make_regular((N - 1) * (sum_j2 - (conj_r(sum_j) * sum_j)));
      using std::sqrt;
      return std::make_tuple(make_regular(N * sum - (N - 1) * sum_j), sum, make_regular(sqrt(j_variance)));
    }

    template <typename T> auto const &_filter(T const &x) { return x; }
    template <typename T> auto const &_filter(accumulator<T> const &x) { return x.linear_bins(); }
  } // namespace details

  // Given a function (a1,a2,a3,...) -> f(a1,a2,a3...),
  // and a list of bins for a1,a2,a3 ... (parameters a...)
  // it returns (average corrected with jackknife bias, naive average, jackknife error)
  // for this quantity
  template <typename F, typename... A> auto jackknife(mpi::communicator c, F &&f, A const &... a) {
    return details::jackknife_impl(c, std::forward<F>(f), details::_filter(a)...);
  }

  // Should we return the variances ? or the stddev ??
  // Simple variance and average
  // FIXME : rewrite a simpler code than calling jacknife . Faster ??
  template <typename A> auto average_and_stddev(mpi::communicator c, A const &a) {
    auto [av, av1, stddev] = jackknife(c, [](auto &&x) { return x; }, a);
    return std::make_pair(av, stddev);
  }

} // namespace triqs::stat
