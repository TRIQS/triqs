// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2025 Simons Foundation
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
// Authors: Nils Wentzell

// Test for try_insert_k and try_remove_k operations on det_manip_basic

#include <triqs/det_manip/det_manip_basic.hpp>
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
  triqs::det_manip::det_manip_basic<fun> D;
  double det_old, detratio;

  test() : f(), D(f, 100) {}

  void check() {
    assert_close(det_old * detratio, D.determinant(), PRECISION);
    // Skip matrix checks for empty matrix (N=0)
    if (D.size() == 0) {
      assert_close(D.determinant(), 1.0, PRECISION);
      return;
    }
    assert_close(D.determinant(), double(nda::linalg::det(D.matrix())), PRECISION);
    assert_close(D.determinant(), 1 / nda::linalg::det(D.inverse_matrix()), PRECISION);
    nda::assert_all_close(nda::linalg::inv(D.matrix()), D.inverse_matrix(), PRECISION, true);
    nda::assert_all_close(D.build_matrix_scratch(), D.matrix(), PRECISION, true);
  }

  void run() {
    triqs::mc_tools::random_generator RNG("mt19937_64", 23432);

    // Test insert_k with k=3
    for (int iter = 0; iter < 100; ++iter) {
      det_old = D.determinant();
      long s  = D.size();

      std::vector<long> i_vec, j_vec;
      std::vector<double> x_vec, y_vec;

      long k = 3;
      // Generate k unique random positions
      std::vector<long> i_pos, j_pos;
      for (long l = 0; l <= s + k - 1; ++l) {
        i_pos.push_back(l);
        j_pos.push_back(l);
      }

      // Shuffle and pick first k
      for (long l = 0; l < k; ++l) {
        long idx    = RNG(static_cast<int>(i_pos.size()));
        i_vec.push_back(i_pos[idx]);
        i_pos.erase(i_pos.begin() + idx);

        idx = RNG(static_cast<int>(j_pos.size()));
        j_vec.push_back(j_pos[idx]);
        j_pos.erase(j_pos.begin() + idx);

        x_vec.push_back(RNG(10.0));
        y_vec.push_back(RNG(10.0));
      }

      detratio = D.try_insert_k(i_vec, j_vec, x_vec, y_vec);

      if (std::abs(detratio * det_old) > 1.e-3) {
        D.complete_operation();
        check();
      } else {
        D.reject_last_try();
      }
    }

    // Test remove_k with k=3
    while (D.size() >= 3) {
      det_old = D.determinant();
      long s  = D.size();

      long k = 3;
      std::vector<long> i_vec, j_vec;

      // Generate k unique random positions
      std::vector<long> i_pos, j_pos;
      for (long l = 0; l < s; ++l) {
        i_pos.push_back(l);
        j_pos.push_back(l);
      }

      for (long l = 0; l < k; ++l) {
        long idx = RNG(static_cast<int>(i_pos.size()));
        i_vec.push_back(i_pos[idx]);
        i_pos.erase(i_pos.begin() + idx);

        idx = RNG(static_cast<int>(j_pos.size()));
        j_vec.push_back(j_pos[idx]);
        j_pos.erase(j_pos.begin() + idx);
      }

      detratio = D.try_remove_k(i_vec, j_vec);

      if (std::abs(detratio * det_old) > 1.e-3) {
        D.complete_operation();
        check();
      } else {
        D.reject_last_try();
      }
    }

    // Test mixed operations with varying k
    for (int iter = 0; iter < 200; ++iter) {
      det_old = D.determinant();
      long s  = D.size();

      bool do_insert = (s < 5) || (s < 15 && RNG(2) == 0);
      long k         = 1 + RNG(3); // k in {1, 2, 3}

      if (do_insert) {
        std::vector<long> i_vec, j_vec;
        std::vector<double> x_vec, y_vec;

        // Generate k unique positions from available pool
        std::vector<long> i_pool, j_pool;
        for (long l = 0; l <= s + k - 1; ++l) {
          i_pool.push_back(l);
          j_pool.push_back(l);
        }

        for (long l = 0; l < k; ++l) {
          long idx = RNG(static_cast<int>(i_pool.size()));
          i_vec.push_back(i_pool[idx]);
          i_pool.erase(i_pool.begin() + idx);

          idx = RNG(static_cast<int>(j_pool.size()));
          j_vec.push_back(j_pool[idx]);
          j_pool.erase(j_pool.begin() + idx);

          x_vec.push_back(RNG(10.0));
          y_vec.push_back(RNG(10.0));
        }

        detratio = D.try_insert_k(i_vec, j_vec, x_vec, y_vec);
      } else {
        if (s < k) k = s;
        if (k == 0) continue;

        std::vector<long> i_vec, j_vec;

        std::vector<long> i_pos, j_pos;
        for (long l = 0; l < s; ++l) {
          i_pos.push_back(l);
          j_pos.push_back(l);
        }

        for (long l = 0; l < k; ++l) {
          long idx = RNG(static_cast<int>(i_pos.size()));
          i_vec.push_back(i_pos[idx]);
          i_pos.erase(i_pos.begin() + idx);

          idx = RNG(static_cast<int>(j_pos.size()));
          j_vec.push_back(j_pos[idx]);
          j_pos.erase(j_pos.begin() + idx);
        }

        detratio = D.try_remove_k(i_vec, j_vec);
      }

      if (std::abs(detratio * det_old) > 1.e-3) {
        D.complete_operation();
        check();
      } else {
        D.reject_last_try();
      }
    }
  }
};

int main() { test().run(); }
