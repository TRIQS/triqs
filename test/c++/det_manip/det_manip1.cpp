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
  triqs::det_manip::det_manip<fun> D;
  double det_old, detratio;

  test() : f(), D(f, 100) {}

//#define PRINT_ALL
  void check() {
    std::cerr << "---- check --- "<<std::endl;
#ifndef PRINT_ALL
    std::cerr << "det = " << D.determinant() << " == " << double(determinant(D.matrix())) << std::endl;
#else
    std::cerr << "det = " << D.determinant() << " == " << double(determinant(D.matrix())) << std::endl
              << D.inverse_matrix() << D.matrix() << nda::matrix<double>(inverse(D.matrix())) << std::endl;
    std::cerr << "det_old = " << det_old << "detratio = " << detratio << " determin " << D.determinant() << std::endl;
#endif
    auto diff = nda::matrix<double>( inverse(D.matrix())- D.inverse_matrix());
    //std::cerr  << diff <<std::endl;
    //std::cerr  << max_element(abs(diff)) <<std::endl;
    nda::assert_all_close(inverse(D.matrix()), D.inverse_matrix(), PRECISION, true);
    assert_close(det_old * detratio, D.determinant(), PRECISION);
    assert_close(D.determinant(), 1 / determinant(D.inverse_matrix()), PRECISION);
    std::cerr << "---- end check --- "<<std::endl;
  }

  void run() {
    triqs::mc_tools::random_generator RNG("mt19937", 23432);
    for (size_t i = 0; i < 5000; ++i) {
      std::cerr << " ------------------------------------------------" << std::endl;
      std::cerr << " i = " << i << " size = " << D.size() << std::endl;
      //std::cerr  << D.matrix() <<std::endl;
      // choose a move
      size_t s = D.size();
      size_t i0, j0, i1, j1;
      det_old  = D.determinant();
      detratio = 1;
      double x, y, x1, y1;
      bool do_something = true;

      switch (RNG((s > 10 ? 7 : 1))) {
        case 0:
          std::cerr << " Insert1" << std::endl;
          x = RNG(10.0), y = RNG(10.0);
          std::cerr << " x,y = " << x << "  " << y << std::endl;
          detratio = D.try_insert(RNG(s), RNG(s), x, y);
          break;
        case 1:
          std::cerr << " Remove1" << std::endl;
          if (s > 0) detratio = D.try_remove(RNG(s), RNG(s));
          break;
        case 2: {
          long k = 2 + RNG(6);
          std::cerr << " Insert" << k << std::endl;
          std::vector<double> x(k);
          std::vector<double> y(k);
          std::vector<long> i(k);
          std::vector<long> j(k);
          for (auto m : itertools::range(k)) {
            x.at(m) = RNG(10.0);
            y.at(m) = RNG(10.0);
            i.at(m) = RNG(s + m);
            j.at(m) = RNG(s + m);
          }
          std::sort(i.begin(), i.end());
          std::sort(j.begin(), j.end());
          if (std::unique(i.begin(), i.end()) == i.end() && std::unique(j.begin(), j.end()) == j.end()) {
            detratio = D.try_insert_k(i, j, x, y);
          } else
            do_something = false;
          break;
        }
        case 3: {
          long k = 2 + RNG(6);
          std::cerr << " Remove" << k << std::endl;
          if (D.size() >= k) {
            std::vector<long> i(k);
            std::vector<long> j(k);
            for (auto m : itertools::range(k)) {
              i.at(m) = RNG(s);
              j.at(m) = RNG(s);
            }
            std::sort(i.begin(), i.end());
            std::sort(j.begin(), j.end());
            if (std::unique(i.begin(), i.end()) == i.end() && std::unique(j.begin(), j.end()) == j.end()) {
              detratio = D.try_remove_k(i, j);
            } else
              do_something = false;
          }
          break;
        }
        case 4:
          if (D.size()==0) break;
	  y        = RNG(10.0);
          i0       = RNG(s);
          std::cerr << " try_change_col" << i0 << std::endl;
          detratio = D.try_change_col(i0, y);
          break;
        case 5:
          if (D.size()==0) break;
          y        = RNG(10.0);
          i0       = RNG(s);
          std::cerr << " try_change_row" << i0 <<std::endl;
          detratio = D.try_change_row(i0, y);
          break;
        case 6:
          if (D.size()==0) break;
          x        = RNG(10.0);
          y        = RNG(10.0);
          i0       = RNG(s);
          j0       = RNG(s);

          std::cerr << " try_change_col_row" << i0 << "  "<< j0 << std::endl;
         
	  //D.try_change_col(j0,y);
	  //D.try_change_row(i0,x);
          //D.reject_last_try();


          detratio = D.try_change_col_row(i0, j0, x, y);
          break;
        default: TRIQS_RUNTIME_ERROR << " TEST INTERNAL ERROR";
      };

      if (do_something) {
        if (std::abs(detratio * det_old) > 1.e-3) {
          D.complete_operation();
          if (D.size() > 0) check();
        } else {
          std::cerr << " reject  since new det is = " << std::abs(detratio * det_old) << std::endl;
          D.reject_last_try();
        }
      }
    }
  }
};

int main(int argc, char **argv) { test().run(); }
