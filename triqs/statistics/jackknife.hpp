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

#include "./binned.hpp"

namespace triqs::stat {

  template <typename T> struct jackknifed_t {
    bin_set<T> const *_b;
    T s;
    long si_minus_one;

    jackknifed_t(bin_set<T> const &b, mpi::communicator c) : _b(&b) {
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

  struct _make_jackknife_tag {
    mpi::communicator c;
  };

  // eval for the jackknife technique
  template <typename T> jackknifed_t<T> eval(bin_set<T> const &obs, _make_jackknife_tag x) { return {obs, x.c}; }

  template <typename T> auto eval(jackknifed_t<T> const &j, long i) { return j[i]; }
  template <typename T> auto eval(bin_set<T> const &b, long i) { return b[i]; }

  struct _get_size_visitor {
    long res = 0;
    template <typename T> void operator()(T const &) {}
    template <typename T> void operator()(bin_set<T> const &obs) {
      long i = obs.size();
      if ((res) and (res != i)) TRIQS_RUNTIME_ERROR << "Expression of time series with time mismatch";
      res = i; // keep the result
    }
  };

  // Given an expression e.g. A*B, A/B made of binned<U>,
  // returns (average corrected with jackknife bias, naive average, jackknife error)
  template <typename ObservableExpr> auto jackknife(ObservableExpr const &expr, mpi::communicator c) {

    // compute the size of the series with the visitor
    auto l = _get_size_visitor{0};
    clef::apply_on_each_leaf(l, expr);
    long N = l.res;

    // build a new expression with the observable replaced by the lazy jackknifed.
    auto expr_with_jackknifed_series = eval(expr, _make_jackknife_tag{c});

    if (N == 0) TRIQS_RUNTIME_ERROR << "No data !";

    auto sum   = make_regular(eval(expr, 0l));
    auto sum_j = make_regular(eval(expr_with_jackknifed_series, 0l));

    using op_t = operations<decltype(sum)>;

    typename op_t::prod_result_t sum_j2 = make_regular(op_t::product(sum_j, sum_j));

    for (long i = 1; i < N; ++i) {
      sum += make_regular(eval(expr, i));
      auto tmp = eval(expr_with_jackknifed_series, i);
      sum_j += tmp;
      sum_j2 += op_t::product(tmp, tmp);
    }
    sum /= N;
    sum_j /= N;
    sum_j2 /= N;
    auto j_variance = make_regular((N - 1) * (sum_j2 - (op_t::product(sum_j , sum_j))));
    return std::make_tuple(make_regular(N * sum - (N - 1) * sum_j), sum, op_t::sqrt(j_variance));
  }

  // trait to find out if T models the concept TimeSeries
  template <typename T> struct is_time_series : std::false_type {};
  template <typename T> struct is_time_series<std::vector<T>> : std::true_type {};
  template <typename T> struct is_time_series<bin_set<T>> : std::true_type {};
  template <typename T> struct is_time_series<jackknifed_t<T>> : std::true_type {};

// The principle is :
// All operations between a time_series and anything results in a clef lazy expression
// This implements the case of binary/unary operators when there is no clef expression (which is already handled by clef operators
// hence this avoid ambiguity).
#define TRIQS_TS_OPERATION(TAG, OP)                                                                                                                  \
  template <typename L, typename R>                                                                                                                  \
  std::enable_if_t<(is_time_series<std::decay_t<L>>::value || is_time_series<std::decay_t<R>>::value) && (!clef::is_any_lazy<L, R>::value),          \
                   clef::expr_node_t<clef::tags::TAG, L, R>>                                                                                         \
  operator OP(L &&l, R &&r) {                                                                                                                        \
    return {clef::tags::TAG(), std::forward<L>(l), std::forward<R>(r)};                                                                              \
  }

  TRIQS_TS_OPERATION(plus, +);
  TRIQS_TS_OPERATION(minus, -);
  TRIQS_TS_OPERATION(multiplies, *);
  TRIQS_TS_OPERATION(divides, /);
#undef TRIQS_TS_OPERATION

} // namespace triqs::stat
