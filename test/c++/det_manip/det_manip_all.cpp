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

#include <triqs/det_manip/det_manip.hpp>
#include <triqs/det_manip/det_manip_basic.hpp>
#include <triqs/mc_tools/random_generator.hpp>
#include <triqs/utility/timer.hpp>
#include <nda/linalg/det_and_inverse.hpp>
#include <iostream>
#include "./old_test_tool.hpp"

struct fun {

  typedef double result_type;
  typedef double argument_type;

  /*double f(double eps, double tau, double beta) const {
   bool s = (tau>0);
   tau = (s ? tau : beta + tau);
   return (s ? 1 : -1 ) * exp( - eps* (tau)) / (1 + exp(- beta *eps));
   }*/

  double operator()(double x, double y) const {
    const double pi   = acos(-1);
    const double beta = 10.0;
    const double epsi = 0.1;
    double tau        = x - y;
    bool s            = (tau > 0);
    tau               = (s ? tau : beta + tau);
    double r          = epsi + tau / beta * (1 - 2 * epsi);
    return -2 * (pi / beta) / std::sin(pi * r);

    //return - 0.5*(f(0.12, x-y, beta) + f(0.43,x-y,beta) ); //+ f(-0.7,x-y,beta));
  }
};

template <class T1, class T2> void assert_close(T1 const &A, T2 const &B, double precision) {
  if (std::abs(A - B) > precision) TRIQS_RUNTIME_ERROR << "assert_close error : " << A << "\n" << B;
}
const double PRECISION = 1.e-6;

struct test {

  fun f;
  triqs::det_manip::det_manip_basic<fun> DetM_Basic;
  triqs::det_manip::det_manip<fun> DetM_Schur;

  // double det_old, detratio;

  test() : f(), DetM_Basic(f, 10), DetM_Schur(f, 10) {}

  // #define PRINT_ALL
  void check(triqs::det_manip::det_manip_basic<fun> &DetM, double det_old, double detratio) {

#ifndef PRINT_ALL
    // std::cerr << "det = " << DetM.determinant() << " == " << double(determinant(DetM.matrix())) << std::endl;
#else
    // std::cerr << "det = " << DetM.determinant() << " == " << double(determinant(DetM.matrix())) << std::endl
    << DetM.inverse_matrix() << DetM.matrix() << nda::matrix<double>(inverse(DetM.matrix())) << std::endl;
    // std::cerr << "det_old = " << det_old << "detratio = " << detratio << " determin " << DetM.determinant() << std::endl;
#endif

    if (DetM.size() > 0) {
      nda::assert_all_close(DetM.build_matrix_scratch(), DetM.matrix(), PRECISION, true);
      nda::assert_all_close(inverse(DetM.matrix()), DetM.inverse_matrix(), PRECISION, true);
    } else {
      assert(DetM.build_matrix_scratch().size() == 0);
      assert(DetM.matrix().size() == 0);
      assert(DetM.inverse_matrix().size() == 0);
    }

    assert_close(DetM.determinant(), 1 / determinant(DetM.inverse_matrix()), PRECISION);
    assert_close(double(determinant(DetM.matrix())), DetM.determinant(), PRECISION);

    assert_close(det_old * detratio, DetM.determinant(), PRECISION);
  }

  void run(int N) {
    using itertools::range;

    triqs::mc_tools::random_generator RNG("mt19937", 23432);

    // Work at fixed order:
    for (auto i : range(N)) {
      auto x = RNG(20.0);
      auto y = RNG(20.0);
      DetM_Basic.try_insert(0, 0, x, y);
      DetM_Basic.complete_operation();

      DetM_Schur.try_insert(0, 0, x, y);
      DetM_Schur.complete_operation();
    }
    triqs::utility::timer timer_basic;
    triqs::utility::timer timer_schur;

    std::cerr << " ------------------------------------------------" << std::endl;
    std::cerr << " size = " << N << std::endl;

    // std::cerr << " ------------------------------------------------" << std::endl;
    std::cerr << " try_insert" << std::endl;
    for (auto [i, j] : product(range(0, N), range(0, N))) {
      // std::cerr << "[i, j] = [" << i << ", " << j << "]" << std::endl;

      auto DetM_Basic2 = DetM_Basic;
      auto DetM_Schur2 = DetM_Schur;

      auto x = RNG(20.0);
      auto y = RNG(20.0);

      double det_old = DetM_Basic2.determinant();

      timer_basic.start();
      double detratio = DetM_Basic2.try_insert(i, j, x, y);
      DetM_Basic2.complete_operation();
      timer_basic.stop();

      check(DetM_Basic2, det_old, detratio);

      timer_schur.start();
      DetM_Schur2.try_insert(i, j, x, y);
      DetM_Schur2.complete_operation();
      timer_schur.stop();
      nda::assert_all_close(DetM_Basic2.matrix(), DetM_Schur2.matrix(), PRECISION, true);
      nda::assert_all_close(DetM_Basic2.inverse_matrix(), DetM_Schur2.inverse_matrix(), PRECISION, true);
    }

    std::cout << "   . timer_basic : " << double(timer_basic) << std::endl;
    std::cout << "   . timer_schur : " << double(timer_schur) << std::endl;

    timer_basic = triqs::utility::timer{};
    timer_schur = triqs::utility::timer{};

    // std::cerr << " ------------------------------------------------" << std::endl;
    std::cerr << " try_remove" << std::endl;
    for (auto [i, j] : product(range(0, N), range(0, N))) {
      // std::cerr << "[i, j] = [" << i << ", " << j << "]" << std::endl;

      auto DetM_Basic2 = DetM_Basic;
      auto DetM_Schur2 = DetM_Schur;

      double det_old = DetM_Basic2.determinant();

      timer_basic.start();
      double detratio = DetM_Basic2.try_remove(i, j);
      DetM_Basic2.complete_operation();
      timer_basic.stop();

      check(DetM_Basic2, det_old, detratio);

      timer_schur.start();
      DetM_Schur2.try_remove(i, j);
      DetM_Schur2.complete_operation();
      timer_schur.stop();

      if (N > 1) {
        nda::assert_all_close(DetM_Basic2.matrix(), DetM_Schur2.matrix(), PRECISION, true);
        nda::assert_all_close(DetM_Basic2.inverse_matrix(), DetM_Schur2.inverse_matrix(), PRECISION, true);
      }
    }

    std::cout << "   . timer_basic : " << double(timer_basic) << std::endl;
    std::cout << "   . timer_schur : " << double(timer_schur) << std::endl;

    timer_basic = triqs::utility::timer{};
    timer_schur = triqs::utility::timer{};

    // std::cerr << " ------------------------------------------------" << std::endl;
    std::cerr << " try_insert_2" << std::endl;
    for (auto [i0, j0, i1, j1] : product(range(0, N), range(0, N), range(0, N + 1), range(0, N + 1))) {
      // std::cerr << "[i0, j0, i1, j1] = [" << i0 << ", " << j0 << ", " << i1 << ", " << j1 << "]" << std::endl;
      if (not((i0 != i1) && (j0 != j1))) { continue; }

      auto DetM_Basic2 = DetM_Basic;
      auto DetM_Schur2 = DetM_Schur;

      auto x0 = RNG(20.0);
      auto y0 = RNG(20.0);
      auto x1 = RNG(20.0);
      auto y1 = RNG(20.0);

      double det_old = DetM_Basic2.determinant();

      timer_basic.start();
      double detratio = DetM_Basic2.try_insert2(i0, i1, j0, j1, x0, y0, x1, y1);
      DetM_Basic2.complete_operation();
      timer_basic.stop();

      check(DetM_Basic2, det_old, detratio);

      timer_schur.start();
      DetM_Schur2.try_insert2(i0, i1, j0, j1, x0, y0, x1, y1);
      DetM_Schur2.complete_operation();
      timer_schur.stop();

      nda::assert_all_close(DetM_Basic2.matrix(), DetM_Schur2.matrix(), PRECISION, true);
      nda::assert_all_close(DetM_Basic2.inverse_matrix(), DetM_Schur2.inverse_matrix(), PRECISION, true);
    }

    std::cout << "   . timer_basic : " << double(timer_basic) << std::endl;
    std::cout << "   . timer_schur : " << double(timer_schur) << std::endl;

    timer_basic = triqs::utility::timer{};
    timer_schur = triqs::utility::timer{};

    // std::cerr << " ------------------------------------------------" << std::endl;
    std::cerr << " try_remove_2" << std::endl;
    for (auto [i0, j0, i1, j1] : product(range(0, N), range(0, N), range(0, N), range(0, N))) {
      // std::cerr << "[i0, j0, i1, j1] = [" << i0 << ", " << j0 << ", " << i1 << ", " << j1 << "]" << std::endl;
      if (not((i0 != i1) && (j0 != j1))) { continue; }

      auto DetM_Basic2 = DetM_Basic;
      auto DetM_Schur2 = DetM_Schur;

      double det_old = DetM_Basic2.determinant();

      timer_basic.start();
      double detratio = DetM_Basic2.try_remove2(i0, i1, j0, j1);
      DetM_Basic2.complete_operation();
      timer_basic.stop();

      check(DetM_Basic2, det_old, detratio);

      timer_schur.start();
      DetM_Schur2.try_remove2(i0, i1, j0, j1);
      DetM_Schur2.complete_operation();
      timer_schur.stop();

      if (N > 2) {
        nda::assert_all_close(DetM_Basic2.matrix(), DetM_Schur2.matrix(), PRECISION, true);
        nda::assert_all_close(DetM_Basic2.inverse_matrix(), DetM_Schur2.inverse_matrix(), PRECISION, true);
      }
    }

    std::cout << "   . timer_basic : " << double(timer_basic) << std::endl;
    std::cout << "   . timer_schur : " << double(timer_schur) << std::endl;

    timer_basic = triqs::utility::timer{};
    timer_schur = triqs::utility::timer{};

    // std::cerr << " ------------------------------------------------" << std::endl;
    std::cerr << " try_change_col" << std::endl;
    for (auto j : range(0, N)) {
      // std::cerr << "[j] = [" << i << "]" << std::endl;

      auto DetM_Basic2 = DetM_Basic;
      auto DetM_Schur2 = DetM_Schur;

      auto y = RNG(20.0);

      double det_old = DetM_Basic2.determinant();

      timer_basic.start();
      double detratio = DetM_Basic2.try_change_col(j, y);
      DetM_Basic2.complete_operation();
      timer_basic.stop();

      check(DetM_Basic2, det_old, detratio);

      timer_schur.start();
      DetM_Schur2.try_change_col(j, y);
      DetM_Schur2.complete_operation();
      timer_schur.stop();

      nda::assert_all_close(DetM_Basic2.matrix(), DetM_Schur2.matrix(), PRECISION, true);
      nda::assert_all_close(DetM_Basic2.inverse_matrix(), DetM_Schur2.inverse_matrix(), PRECISION, true);
    }

    std::cout << "   . timer_basic : " << double(timer_basic) << std::endl;
    std::cout << "   . timer_schur : " << double(timer_schur) << std::endl;

    timer_basic = triqs::utility::timer{};
    timer_schur = triqs::utility::timer{};

    // std::cerr << " ------------------------------------------------" << std::endl;
    std::cerr << " try_change_row" << std::endl;
    for (auto i : range(0, N)) {
      // std::cerr << "[i] = [" << i << "]" << std::endl;

      auto DetM_Basic2 = DetM_Basic;
      auto DetM_Schur2 = DetM_Schur;

      auto x = RNG(20.0);

      double det_old = DetM_Basic2.determinant();

      timer_basic.start();
      double detratio = DetM_Basic2.try_change_row(i, x);
      DetM_Basic2.complete_operation();
      timer_basic.stop();

      check(DetM_Basic2, det_old, detratio);

      timer_schur.start();
      DetM_Schur2.try_change_row(i, x);
      DetM_Schur2.complete_operation();
      timer_schur.stop();

      nda::assert_all_close(DetM_Basic2.matrix(), DetM_Schur2.matrix(), PRECISION, true);
      nda::assert_all_close(DetM_Basic2.inverse_matrix(), DetM_Schur2.inverse_matrix(), PRECISION, true);
    }

    std::cout << "   . timer_basic : " << double(timer_basic) << std::endl;
    std::cout << "   . timer_schur : " << double(timer_schur) << std::endl;

    timer_basic = triqs::utility::timer{};
    timer_schur = triqs::utility::timer{};

    // std::cerr << " ------------------------------------------------" << std::endl;
    std::cerr << " try_change_col_row" << std::endl;
    for (auto [i, j] : product(range(0, N), range(0, N))) {
      // std::cerr << "[i, j] = [" << i << ", " << j << "]" << std::endl;

      auto DetM_Basic2 = DetM_Basic;
      auto DetM_Schur2 = DetM_Schur;

      auto x = RNG(20.0);
      auto y = RNG(20.0);

      double det_old = DetM_Basic2.determinant();

      timer_basic.start();
      double detratio = DetM_Basic2.try_change_col_row(i, j, x, y);
      DetM_Basic2.complete_operation();
      timer_basic.stop();

      check(DetM_Basic2, det_old, detratio);

      timer_schur.start();
      DetM_Schur2.try_change_col_row(i, j, x, y);
      DetM_Schur2.complete_operation();
      timer_schur.stop();

      nda::assert_all_close(DetM_Basic2.matrix(), DetM_Schur2.matrix(), PRECISION, true);
      nda::assert_all_close(DetM_Basic2.inverse_matrix(), DetM_Schur2.inverse_matrix(), PRECISION, true);
    }

    std::cout << "   . timer_basic : " << double(timer_basic) << std::endl;
    std::cout << "   . timer_schur : " << double(timer_schur) << std::endl;

    timer_basic = triqs::utility::timer{};
    timer_schur = triqs::utility::timer{};
  }
};

int main(int argc, char **argv) {
  for (auto i : {1, 2, 3, 5, 8}) { test().run(i); }
}
