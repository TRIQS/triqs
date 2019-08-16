/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012  by M. Ferrero, O. Parcollet
 * Copyright (C) 2018-2019  by Simons Foundation
 *               author : O. Parcollet
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
#include <array>
#include "./mesh_tools.hpp"

// FIXME : MOVE THIS IN std::array util
namespace std {
  template <typename T, size_t R> std::ostream &operator<<(std::ostream &sout, std::array<T, R> const &a) {
    sout << '(';
    for (int u = 0; u < R; ++u) sout << (u ? ", " : "") << a[u];
    return sout << ')';
  }

} // namespace std

// Techniques for linear interpolation

namespace triqs::mesh {

  // 1 in a completely neutral type for the compiler, which can not optimize 1.0 * x a priori.
  struct _universal_unit_t {};
  template <typename T> auto operator*(_universal_unit_t, T &&x) { return std::forward<T>(x); }
  template <typename T> auto operator*(T &&x, _universal_unit_t) { return std::forward<T>(x); }
  inline _universal_unit_t operator*(_universal_unit_t, _universal_unit_t) { return {}; } // avoid ambiguity

  // FIXME : DOC
  template <typename IndexType> struct interpol_data_0d_t {
    static constexpr int n_pts = 1;
    std::array<IndexType, n_pts> idx;
    std::array<_universal_unit_t, n_pts> w;
    interpol_data_0d_t(IndexType n) : idx{std::move(n)}, w{} {}
  };

  // FIXME CHANGE the order fo the template, it looks strange to accesss W
  template <typename IndexType, int Npts> struct interpol_data_lin_t { // dim R = 1-> 2, R = 2 -> 4, R = 3 -> 8
    static constexpr int n_pts = Npts;
    std::array<IndexType, n_pts> idx;
    std::array<double, n_pts> w;
  };

  // FIXME : DOC
  struct interpol_data_all_t {
    static constexpr int n_pts = 1;
    std::array<all_t, n_pts> idx;
    std::array<_universal_unit_t, n_pts> w;
  };

  /**
   * Fit the two closest points for x on [x_min, x_max], with a linear weight w
   * @param x : the point
   * @param i_max : maximum index
   * @param x_min : the window starts. It ends at x_min + i_max* delta_x
   * @param delta_x
   *
   * Throws if x is not in the window
   * */
  inline interpol_data_lin_t<long, 2> interpolate_on_segment(double x, double x_min, double delta_x, long imax) {
    double a = (x - x_min) / delta_x;
    long i   = std::floor(a);
    bool in  = (i >= 0) && (i < imax);
    double w = a - i;
    // We include both x_min and x_max and account
    // for a small rounding error margin of 1e-8 for w
    if (i == imax) {
      --i;
      in = (std::abs(w) < 1.e-8);
      w  = 1.0;
    }
    if (i == -1) {
      i  = 0;
      in = (std::abs(1 - w) < 1.e-8);
      w  = 0.0;
    }
    if (!in) TRIQS_RUNTIME_ERROR << "out of window x= " << x << " xmin = " << x_min << " xmax = " << x_min + imax * delta_x;
    return {{i, i + 1}, {1 - w, w}};
  }

} // namespace triqs::mesh
