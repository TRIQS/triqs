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
    binned<T> const *_b;
    T s;
    jackknifed_t(mpi::communicator c, T const &zero, binned<T> const &b)
       : _b(&b), s(mpi_all_reduce(c, b->sum(zero)) / mpi_all_reduce(c, b->bins().size())) {}
    auto operator[](int i) const { return s - b->bins()[i]; }
  };

  struct _make_jacknife_tag {
    mpi::communicator c;
  };

  // eval for the jacknife technique
  template <typename T> jackknifed_t<T> eval(observable<T> const &obs, _make_jacknife_tag x) { return {x.c, obs.zero(), obs.bins()}; }

  template <typename T> auto eval(jackknifed_t<T> const &j, long i) { return j[i]; }

  struct _get_size_visitor {
    long res = 0;
    template <typename T> void operator()(T const &) {}
    template <typename T> operator()(observable<T> const &obs) {
      long i = obs.size();
      if ((res) and (res != i)) TRIQS_RUNTIME_ERROR << "Expression of time series with time mismatch";
      res = i; // keep the result
    }
  };

  template <typename ObservableExpr> auto jackknife(ObservableExpr const &expr, mpi::communicator c) {

    // compute the size of the series with the visitor
    auto l = _get_size_visitor{0};
    clef::apply_on_each_leaf(l, x);
    long size = l.res;

    // build a new expression with the observable replaced by the lazy jackknifed.
    auto expr_with_jacknifed_series = eval(expr, _make_jacknife_tag{c});

    if (si == 0) TRIQS_RUNTIME_ERROR << "No data !";

    auto sum  = eval(expr_with_jacknifed_series, 0);
    auto sum2 = sum;
    sum2 *= sum;
    for (int i = 1; i < si; ++i) {
      auto tmp = eval(expr_with_jacknifed_series, i);
      sum += tmp;
      sum2 += tmp * tmp;
    }
    return std::make_pair(sum / size, sum2 / size - (sum * sum) / (size * size));
  }

} // namespace triqs::stat

namespace triqs::clef {

  // observable and jackknifed_t are nodes of lazy expressions
  template <typename T> struct is_any_lazy<observable<T>>::std::true_type {};
  template <typename T> struct is_any_lazy<jackknifed_t<T>>::std::true_type {};

} // namespace triqs::clef
