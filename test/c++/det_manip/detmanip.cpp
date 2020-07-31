// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Olivier Parcollet

#include <triqs/test_tools/arrays.hpp>
#include <triqs/det_manip/det_manip.hpp>
#include <random>
#include <triqs/arrays/linalg/det_and_inverse.hpp>
#include <triqs/arrays/asserts.hpp>
#include <iostream>

struct fun {

  using result_type=double ;
  using argument_type=double ;

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

using d_t = triqs::det_manip::det_manip<fun>;
const double precision = 1.e-6;
const int seed = 23432;

TEST(DetManip, ChangeRowCol) {

  for (int N = 1; N < 9; N++) { 
    std::cerr << "================================\n N = "<< N << std::endl;
    
  for (int i0 = 0; i0 < N; i0++) 
  for (int j0 = 0; j0 < N; j0++) {

    std::cerr << "------------------------------\n i0 = "<< i0 << "j0 = " << j0 << std::endl;
    
    std::mt19937 gen(seed); 
    std::uniform_real_distribution<> dis(0.0, 10.0);
  
    std::vector<double> X(N), Y(N);
    for (int n=0; n<N; ++n) { X[n] = dis(gen); Y[n] = dis(gen);
    }
   
    auto d =  d_t{fun{}, X, Y };
 
  // result 
    auto x =  dis(gen), y =  dis(gen);
    X[i0] = x; 
    Y[j0] = y;
    auto d2 =  d_t{fun{}, X, Y };
    auto det1 = d.determinant();
    if (std::abs(det1) < 1.e-5) { std::cerr << " Det Too small: not completed";continue;}
 
    // ops
    auto detratio = d.try_change_col_row(i0, j0, x, y);

    d.complete_operation();
    
    auto det = d.determinant();
    auto det2 = d2.determinant();
    auto det_check =  1 / determinant(d.inverse_matrix());

    if (std::abs(detratio - det/det1) > precision) TRIQS_RUNTIME_ERROR << "detratio incorrect : " << detratio <<"  " << det/det1;
    if (std::abs(det - det2) > precision) TRIQS_RUNTIME_ERROR << "Det != d2 : " << det <<"  " << det2;
    if (std::abs(det - det_check) > precision) TRIQS_RUNTIME_ERROR << "Det != det_check : " << det << "  " << det_check;
    triqs::arrays::assert_all_close(make_matrix(inverse(d.matrix())), d.inverse_matrix(), precision, true);
  }

  }
}

MAKE_MAIN;

