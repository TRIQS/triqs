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
#include <triqs/mc_tools/random_generator.hpp>
#include <triqs/arrays/linalg/det_and_inverse.hpp>
#include <triqs/arrays/asserts.hpp>
#include <iostream>
#include <triqs/det_manip/det_manip_basic.hpp>

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
  triqs::det_manip::det_manip<fun> D;
  triqs::det_manip::det_manip_basic<fun> D_basic;
  double det_old, detratio, detratio_basic;

  test() : f(), D(f, 100), D_basic(f, 100) {}

  //#define PRINT_ALL
  void check() {
    std::cerr << "--- check ---" << std::endl;
#ifndef PRINT_ALL
    std::cerr << "det = " << D.determinant() << " == " << double(determinant(D.matrix())) << std::endl;
#else
    std::cerr << "det = " << D.determinant() << " == " << double(determinant(D.matrix())) << std::endl
              << D.inverse_matrix() << D.matrix() << triqs::arrays::matrix<double>(inverse(D.matrix())) << std::endl;
    std::cerr << "det_old = " << det_old << "detratio = " << detratio << " determin " << D.determinant() << std::endl;
#endif
    std::cerr << "D.size() = " << D.size() << std::endl;
    std::cerr << "      D.get_x = ";
    for (auto x : D.get_x()) std::cerr << x << " ";
    std::cerr << std::endl;

    std::cerr << "D_basic.get_x = ";
    for (auto x : D_basic.get_x()) std::cerr << x << " ";
    std::cerr << std::endl;

    std::cerr << "      D.get_y = ";
    for (auto x : D.get_y()) std::cerr << x << " ";
    std::cerr << std::endl;

    std::cerr << "D_basic.get_y = ";
    for (auto x : D_basic.get_y()) std::cerr << x << " ";
    std::cerr << std::endl;

    // basic
    triqs::arrays::assert_all_close(D_basic.matrix(), D.matrix(), PRECISION, true);
    
    assert_close(D.determinant(), 1 / determinant(D.inverse_matrix()), PRECISION);
    triqs::arrays::assert_all_close(inverse(D.matrix()), D.inverse_matrix(), PRECISION, true);
    assert_close(det_old * detratio, D.determinant(), PRECISION);
    assert_close(double(determinant(D.matrix())), D.determinant(), PRECISION);
    assert_close(D.determinant(), 1 / determinant(D.inverse_matrix()), PRECISION);
    std::cerr << "---- end check --- " << std::endl;
  }

  void run() {
    triqs::mc_tools::random_generator RNG("mt19937", 23432);
    for (size_t i = 0; i < 50000; ++i) {
      std::cerr << " ------------------------------------------------" << std::endl;
      std::cerr << " i = " << i << " size = " << D.size() << std::endl;
      // choose a move
      size_t s = D.size();
      size_t i0, j0, i1, j1;
      det_old        = D.determinant();
      detratio       = 1;
      detratio_basic = 1;
      double x, y, x1, y1;
      bool do_something = true;

      switch (RNG((s > 10 ? 7 : 1))) {
        case 0:
          x = RNG(10.0), y = RNG(10.0);
          std::cerr << " x,y = " << x << "  " << y << std::endl;
          i0             = RNG(s);
          j0             = RNG(s);
          detratio       = D.try_insert(i0, j0, x, y);
          detratio_basic = D_basic.try_insert(i0, j0, x, y);
          break;
        case 1:
          if (s > 0) {
            i0             = RNG(s);
            j0             = RNG(s);
            detratio       = D.try_remove(i0, j0);
            detratio_basic = D_basic.try_remove(i0, j0);
          }
          break;
        case 2:
          std::cerr << " Insert2" << std::endl;
          x  = RNG(10.0);
          x1 = RNG(10.0);
          y  = RNG(10.0);
          y1 = RNG(10.0);
          i0 = RNG(s);
          i1 = RNG(s + 1);
          j0 = RNG(s);
          j1 = RNG(s + 1);
          std::cerr << " i=" << i0 << "  " << i1 << std::endl;
          std::cerr << " j=" << j0 << "  " << j1 << std::endl;
          std::cerr << " x=" << x << "  " << x1 << std::endl;
          std::cerr << " y=" << y << "  " << y1 << std::endl;
          if ((i0 != i1) && (j0 != j1)) {
            auto Dcopy  = D;
            auto Dcopy2 = D;

            detratio       = D.try_insert2(i0, i1, j0, j1, x, x1, y, y1);
            detratio_basic = D_basic.try_insert2(i0, i1, j0, j1, x, x1, y, y1);

            std::cerr << "Dinit.get_x = ";
            for (auto x : Dcopy.get_x()) std::cerr << x << " ";
            std::cerr << std::endl;

#if 1
	    if (i0>i1) { std::swap(i0,i1); std::swap(x,x1);}
	    if (j0>j1) { std::swap(j0,j1); std::swap(y,y1);}
            Dcopy.try_insert(i0, j0, x, y);
            Dcopy.complete_operation();
            Dcopy.try_insert(i1, j1, x1, y1);
            Dcopy.complete_operation();
            Dcopy2.try_insert2(i0, i1, j0, j1, x, x1, y, y1);
            Dcopy2.complete_operation();

            std::cerr << "Dcopy.get_x = ";
            for (auto x : Dcopy.get_x()) std::cerr << x << " ";
            std::cerr << std::endl;

            std::cerr << "Dcopy2.get_x = ";
            for (auto x : Dcopy2.get_x()) std::cerr << x << " ";
            std::cerr << std::endl;

            std::cerr << "Dcopy.get_y = ";
            for (auto x : Dcopy.get_y()) std::cerr << x << " ";
            std::cerr << std::endl;

            std::cerr << "Dcopy2.get_y = ";
            for (auto x : Dcopy2.get_y()) std::cerr << x << " ";
            std::cerr << std::endl;

            triqs::arrays::assert_all_close(Dcopy.matrix(), Dcopy2.matrix(), PRECISION, true);
#endif

          } else
            do_something = false;
          break;
        case 3:
          std::cerr << " Remove2" << std::endl;
          if (D.size() >= 2) {
            i0 = RNG(s);
            i1 = RNG(s);
            j0 = RNG(s);
            j1 = RNG(s);
            std::cerr << " i=" << i0 << "  " << i1 << std::endl;
            std::cerr << " j=" << j0 << "  " << j1 << std::endl;
            if ((i0 != i1) && (j0 != j1)) {
              detratio       = D.try_remove2(i0, i1, j0, j1);
              detratio_basic = D_basic.try_remove2(i0, i1, j0, j1);
            } else
              do_something = false;
          }
          break;
        case 4:
          if (D.size() == 0) break;
          y  = RNG(10.0);
          i0 = RNG(s);
          std::cerr << " try_change_col" << i0 << std::endl;
          detratio       = D.try_change_col(i0, y);
          detratio_basic = D_basic.try_change_col(i0, y);
          break;
        case 5:
          if (D.size() == 0) break;
          y  = RNG(10.0);
          i0 = RNG(s);
          std::cerr << " try_change_row" << i0 << std::endl;
          detratio       = D.try_change_row(i0, y);
          detratio_basic = D_basic.try_change_row(i0, y);
          break;
        case 6:
          if (D.size() == 0) break;
          x  = RNG(10.0);
          y  = RNG(10.0);
          i0 = RNG(s);
          j0 = RNG(s);

          std::cerr << " try_change_col_row" << i0 << "  " << j0 << std::endl;

          //D.try_change_col(j0,y);
          //D.try_change_row(i0,x);
          //D.reject_last_try();

          detratio       = D.try_change_col_row(i0, j0, x, y);
          detratio_basic = D_basic.try_change_col_row(i0, j0, x, y);
          break;
        default: TRIQS_RUNTIME_ERROR << " TEST INTERNAL ERROR";
      };

      //assert_close(detratio, detratio_basic, PRECISION);

      if (do_something) {
        if (std::abs(detratio * det_old) > 1.e-4) {
          D.complete_operation();
          D_basic.complete_operation();
          if (D.size() > 0) check();
        } else {
          std::cerr << " reject  since new det is = " << std::abs(detratio * det_old) << std::endl;
          D.reject_last_try();
          D_basic.reject_last_try();
        }
      }
    }
  }
};

int main(int argc, char **argv) { test().run(); }
