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
// Authors: Olivier Parcollet, Nils Wentzell

#pragma once

namespace triqs {
  namespace gfs {

    //-------------------------------------------------------
    // closest mesh point on the grid
    // ------------------------------------------------------

    template <typename Target> struct gf_closest_point<imtime, Target> {
      // index_t is int
      template <typename M, typename T> static int invoke(M const &mesh, closest_pt_wrap<T> const &p) {
        double x = double(p.value) + 0.5 * mesh.delta();
        int n    = std::floor(x / mesh.delta());
        return n;
      }
    };

    //-------------------------------------------------------
    // closest mesh point on the grid
    // ------------------------------------------------------

    struct gf_closest_point_linear_mesh {
      // index_t is int
      template <typename M, typename T> static int invoke(M const &mesh, closest_pt_wrap<T> const &p) {
        double x = double(p.value) - mesh.x_min() + 0.5 * mesh.delta();
        int n    = std::floor(x / mesh.delta());
        return n;
      }
    };

    //-------------------------------------------------------
    // closest mesh point on the grid
    // ------------------------------------------------------

    template <typename Target> struct gf_closest_point<brillouin_zone, Target> {

      template <typename T> static gf_mesh<brillouin_zone>::index_t invoke(gf_mesh<brillouin_zone> const &m, closest_pt_wrap<T> const &p) {

        gf_mesh<brillouin_zone>::index_t result; // array<Int, 3>
        // FIXME : works only on square lattice
        auto id = m.get_interpolation_data(p.value);
        for (int u = 0; u < 3; ++u) {
          int i     = (id.wa[0][u] > id.wa[1][u] ? 0 : 1);
          result[u] = id.ia[i][u];
        }
        return result;
      }
    };
    //-------------------------------------------------------
    // For all meshes represented by a linear grid, the code is the same

    //template <typename Target> struct gf_closest_point<imtime, Target> : gf_closest_point_linear_mesh{};
    template <typename Target> struct gf_closest_point<retime, Target> : gf_closest_point_linear_mesh {};
    template <typename Target> struct gf_closest_point<refreq, Target> : gf_closest_point_linear_mesh {};

  } // namespace gfs
} // namespace triqs
