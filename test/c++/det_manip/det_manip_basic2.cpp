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
#include <triqs/arrays/linalg/det_and_inverse.hpp>
#include <triqs/arrays/asserts.hpp>
#include <iostream>

struct fun {
  typedef double result_type;
  typedef double argument_type;
  double operator()(double x, double y) const { return 0.5 * (y >= x ? 1 : -1); }
};

template <class T1, class T2> void assert_close(T1 const &A, T2 const &B, double precision) {
  if (std::abs(A - B) > precision) TRIQS_RUNTIME_ERROR << "assert_close error : " << A << "\n" << B;
}
const double PRECISION = 1.e-12;

struct test {

  fun f;
  triqs::det_manip::det_manip_basic<fun> D;
  double det_old, detratio;

  test() : f(), D(f, 100) {}

  void check() {
#define PRINT_ALLL
#ifndef PRINT_ALL
    std::cerr << "det = " << D.determinant() << " == " << double(determinant(D.matrix())) << std::endl;
#else
    std::cerr << "det = " << D.determinant() << " == " << double(determinant(D.matrix())) << std::endl;
    std::cerr << "inverse_matrix = " << D.inverse_matrix() << std::endl;
    //std::cerr  << "inverse inverse_matrix = " << triqs::arrays::matrix<double>(inverse(D.inverse_matrix()) ) << std::endl;
    std::cerr << "matrix = " << D.matrix() << std::endl;
    std::cerr << triqs::arrays::matrix<double>(inverse(D.matrix())) << std::endl;
#endif
    assert_close(D.determinant(), 0.5, PRECISION);
    assert_close(D.determinant(), 1 / determinant(D.inverse_matrix()), PRECISION);
    triqs::arrays::assert_all_close(inverse(D.matrix()), D.inverse_matrix(), PRECISION);
    assert_close(det_old * detratio, D.determinant(), PRECISION);
  }

  void run() {
    triqs::mc_tools::random_generator RNG("mt19937", 23432);
    for (size_t i = 0; i < 100; ++i) {
      std::cerr << " ------------------------------------------------" << std::endl;
      std::cerr << " i = " << i << " size = " << D.size() << std::endl;
      // choose a move
      size_t s = D.size();
      size_t w, w1;
      det_old  = D.determinant();
      detratio = 1;
      double x, x1;

      switch (RNG((i > 10 ? 4 : 1))) {
        case 0:
          std::cout << "Insert" << std::endl;
          x        = RNG(10.0);
          w        = RNG(s);
          detratio = D.try_insert(w, w, x, x);
          break;
        case 1:
          std::cout << "Remove" << std::endl;
          w = RNG(s);
          if (s > 0) detratio = D.try_remove(w, w);
          break;
        case 2:
          x  = RNG(10.0);
          x1 = RNG(10.0);
          w  = RNG(s);
          w1 = RNG(s + 1);
          if (w != w1) {
            std::cout << " Insert2" << std::endl;
            detratio = D.try_insert2(w, w1, w, w1, x, x1, x, x1);
          }
          break;
        case 3:
          std::cout << " Remove2" << std::endl;
          if (D.size() >= 2) {
            w  = RNG(s);
            w1 = RNG(s);
            if (w != w1) detratio = D.try_remove2(w, w1, w, w1);
          }
          break;
        default: TRIQS_RUNTIME_ERROR << " TEST INTERNAL ERROR";
      };

      std::cout << "about to complete ..." << std::endl;
      D.complete_operation();
      std::cout << " .. done" << std::endl;
      if (D.size() > 0) check();
      std::cout << " check done" << std::endl;
    }
  }
};

int main(int argc, char **argv) { test().run(); }
