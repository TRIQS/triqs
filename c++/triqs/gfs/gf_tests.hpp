// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once

namespace triqs {
  namespace gfs {

    // check gfs are close
    template <typename X, typename Y> void assert_gfs_are_close(X const &x, Y const &y, double precision) {

      // meshes must be identical and data close
      if (x.mesh() != y.mesh()) TRIQS_RUNTIME_ERROR << "GFs have different meshes\n -- 1st Mesh \n" << x.mesh() << "\n -- 2nd Mesh \n" << y.mesh();
      if (max_element(abs(x.data() - y.data())) > precision)
        TRIQS_RUNTIME_ERROR << "Gfs have different data. max(abs(x-y)) = " << max_element(abs(x.data() - y.data()));
    }

    // check block gfs are close
    template <typename X, typename Y> void assert_block_gfs_are_close(X const &x, Y const &y, double precision) {

      if (x.size() != y.size()) TRIQS_RUNTIME_ERROR << "Block GFs have different number of blocks";
      if (x.block_names() != y.block_names()) TRIQS_RUNTIME_ERROR << "Block GFs have different block_names";
      for (int u = 0; u < x.size(); ++u) assert_gfs_are_close(x[u], y[u], precision);
    }

    // check block2 gfs are close
    template <typename X, typename Y> void assert_block2_gfs_are_close(X const &x, Y const &y, double precision) {

      if (x.size() != y.size()) TRIQS_RUNTIME_ERROR << "Block2 GFs have different number of blocks";
      if (x.block_names() != y.block_names()) TRIQS_RUNTIME_ERROR << "Block2 GFs have different block_names";
      for (int i = 0; i < x.size1(); ++i)
        for (int j = 0; j < x.size2(); ++j) assert_gfs_are_close(x(i, j), y(i, j), precision);
    }

  } // namespace gfs
} // namespace triqs
