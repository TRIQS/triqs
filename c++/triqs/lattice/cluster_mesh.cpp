// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Nils Wentzell, tayral

#include "./cluster_mesh.hpp"

namespace triqs {
  namespace gfs {
    utility::mini_vector<int, 3> find_cell_dims(arrays::matrix<double> const &inv_n) {

      arrays::matrix<double> n_mat = inverse(inv_n);
      double Ld                    = arrays::determinant(n_mat);
      double dev                   = std::abs(std::abs(Ld) - round(std::abs(Ld)));
      if (dev > 1e-8)
        TRIQS_RUNTIME_ERROR << "determinant of inverse of inv_n should be an integer, is " << Ld << " instead (deviation =" << dev << ").";
      int L = int(std::abs(Ld));
      clef::placeholder<0> i_;
      std::vector<arrays::vector<int>> units{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
      std::vector<arrays::vector<int>> C{{0, 0, 0}};
      std::vector<int> k_res(3);
      for (int d = 0; d < 3; d++) {
        k_res[d] = L + 1;
        for (auto const &x : C) {
          for (int k = 1; k <= L; k++) {
            bool OK = false;
            for (int dp = 0; dp < 3; dp++) {
              double crit = k * inv_n(d, dp) - sum(inv_n(i_, dp) * x(i_), i_ = range(0, 3));
              if (std::abs(crit - int(crit)) > 1e-8) {
                OK = false;
                break;
              } else {
                OK = true;
              }

            } // dp
            if (OK and k < k_res[d]) {
              k_res[d] = k;
              break;
            }
          } // k
        }   // x

        std::vector<arrays::vector<int>> Cp;
        for (auto const &x : C)
          for (int q = 0; q < k_res[d]; q++) Cp.push_back(x + q * units[d]);
        C = Cp;
      } // d

      return k_res;
    }
  } // namespace gfs
} // namespace triqs
