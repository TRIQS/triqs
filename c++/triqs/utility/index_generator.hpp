// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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

#pragma once

namespace triqs {
  namespace utility {

    // FIXME : used only ONCE in mesh/bases/cluster_mesh.hpp
    // Should we move it ??
    class index3_generator {
      std::array<long, 3> d, i; 
      long i_flat  = 0;
      bool _at_end = false;

      public:
      index3_generator() = default;
      index3_generator(std::array<long, 3> const &dims, std::array<long, 3> const &i) : d(dims), i(i) {
        i_flat = i[2] + (i[1] + i[0] * d[1]) * d[2];
      }
      // void advance() {
      //   ++i_flat;
      //   ++i[2];
      //   if (i[2] < d[2]) return;
      //   i[2] = 0;
      //   ++i[1];
      //   if (i[1] < d[1]) return;
      //   i[1] = 0;
      //   ++i[0];
      //   if (i[0] < d[0]) return;
      //   // i[0]=0;
      //   _at_end = true;
      // }
      std::array<long, 3> const &index() const { return i; }
      long linear_index() const { return i_flat; }
      // bool at_end() const { return _at_end; }
      // void reset() {
      //   _at_end = false;
      //   i_flat  = 0;
      //   i[0]    = 0;
      //   i[1]    = 0;
      //   i[2]    = 0;
      // }
    };
  } // namespace utility
} // namespace triqs
