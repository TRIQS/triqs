// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

// Cross-check the QR-based det_manip_qr against the reference (SMW) det_manip: the same random move
// sequence is driven through both and matrix/determinant/inverse are compared at every step. The QR
// determinant is additionally checked against an independent LU recomputation (nda::linalg::det).

#include <triqs/det_manip/det_manip.hpp>
#include <triqs/det_manip/det_manip_qr.hpp>
#include <triqs/mc_tools/random_generator.hpp>
#include <nda/linalg/det.hpp>
#include <nda/linalg/inv.hpp>
#include <iostream>
#include "./old_test_tool.hpp"

struct fun {

  typedef double result_type;
  typedef double argument_type;

  double operator()(double x, double y) const {
    const double pi   = acos(-1);
    const double beta = 10.0;
    const double epsi = 0.1;
    double tau        = x - y;
    bool s            = (tau > 0);
    tau               = (s ? tau : beta + tau);
    double r          = epsi + tau / beta * (1 - 2 * epsi);
    return -2 * (pi / beta) / std::sin(pi * r);
  }
};

template <class T1, class T2> void assert_close(T1 const &A, T2 const &B, double precision) {
  if (std::abs(A - B) > precision) TRIQS_RUNTIME_ERROR << "assert_close error : " << A << "\n" << B;
}
const double PRECISION = 1.e-6;

struct test {

  fun f;
  triqs::det_manip::det_manip<fun> D;
  triqs::det_manip::det_manip_qr<fun> D_qr;
  double det_old, detratio, detratio_qr;

  test() : f(), D(f, 100), D_qr(f, 100) {}

  void check() {
    // QR engine vs reference SMW engine
    triqs::arrays::assert_all_close(D_qr.matrix(), D.matrix(), PRECISION, true);
    assert_close(D_qr.determinant(), D.determinant(), PRECISION);
    triqs::arrays::assert_all_close(D_qr.inverse_matrix(), D.inverse_matrix(), PRECISION, true);

    // QR engine self-consistency / independent ground truth
    assert_close(double(nda::linalg::det(D_qr.matrix())), D_qr.determinant(), PRECISION);
    triqs::arrays::assert_all_close(nda::linalg::inv(D_qr.matrix()), D_qr.inverse_matrix(), PRECISION, true);
    assert_close(det_old * detratio_qr, D_qr.determinant(), PRECISION);
  }

  void run() {
    triqs::mc_tools::random_generator RNG("mt19937", 23432);
    for (size_t i = 0; i < 20000; ++i) {
      size_t s = D.size();
      size_t i0, j0, i1, j1;
      det_old     = D_qr.determinant();
      detratio    = 1;
      detratio_qr = 1;
      double x, y, x1, y1;
      bool do_something = true;

      switch (RNG((s > 10 ? 7 : 1))) {
        case 0:
          x  = RNG(5.0), y = RNG(5.0);
          i0 = RNG(s);
          j0 = RNG(s);
          detratio    = D.try_insert(i0, j0, x, y);
          detratio_qr = D_qr.try_insert(i0, j0, x, y);
          break;
        case 1:
          if (s > 0) {
            i0          = RNG(s);
            j0          = RNG(s);
            detratio    = D.try_remove(i0, j0);
            detratio_qr = D_qr.try_remove(i0, j0);
          }
          break;
        case 2:
          x  = RNG(5.0);
          x1 = RNG(5.0);
          y  = RNG(5.0);
          y1 = RNG(5.0);
          i0 = RNG(s);
          i1 = RNG(s + 1);
          j0 = RNG(s);
          j1 = RNG(s + 1);
          if ((i0 != i1) && (j0 != j1)) {
            detratio    = D.try_insert2(i0, i1, j0, j1, x, x1, y, y1);
            detratio_qr = D_qr.try_insert2(i0, i1, j0, j1, x, x1, y, y1);
          } else
            do_something = false;
          break;
        case 3:
          if (D.size() >= 2) {
            i0 = RNG(s);
            i1 = RNG(s);
            j0 = RNG(s);
            j1 = RNG(s);
            if ((i0 != i1) && (j0 != j1)) {
              detratio    = D.try_remove2(i0, i1, j0, j1);
              detratio_qr = D_qr.try_remove2(i0, i1, j0, j1);
            } else
              do_something = false;
          }
          break;
        case 4:
          if (D.size() == 0) break;
          y  = RNG(5.0);
          i0 = RNG(s);
          detratio    = D.try_change_col(i0, y);
          detratio_qr = D_qr.try_change_col(i0, y);
          break;
        case 5:
          if (D.size() == 0) break;
          y  = RNG(5.0);
          i0 = RNG(s);
          detratio    = D.try_change_row(i0, y);
          detratio_qr = D_qr.try_change_row(i0, y);
          break;
        case 6:
          if (D.size() == 0) break;
          x  = RNG(5.0);
          y  = RNG(5.0);
          i0 = RNG(s);
          j0 = RNG(s);
          detratio    = D.try_change_col_row(i0, j0, x, y);
          detratio_qr = D_qr.try_change_col_row(i0, j0, x, y);
          break;
        default: TRIQS_RUNTIME_ERROR << " TEST INTERNAL ERROR";
      };

      if (do_something) {
        if (std::abs(detratio * det_old) > 1.e-4) {
          D.complete_operation();
          D_qr.complete_operation();
          if (D.size() > 0) check();
        } else {
          D.reject_last_try();
          D_qr.reject_last_try();
        }
      }
    }
  }
};

int main() { test().run(); }
