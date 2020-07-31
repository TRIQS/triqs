// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Olivier Parcollet, Nils Wentzell

#include "./gf_indices.hpp"

namespace triqs {
  namespace gfs {

    //------------------------------------------------

    void h5_write(h5::group fg, std::string subgroup_name, gf_indices const &g) {
      if (g.empty()) return;
      auto gr = fg.create_group(subgroup_name);
      write_hdf5_format(gr, g);
      if (g.rank() == 2) { // special case for backward compat.
        h5_write(gr, "left", g._data[0]);
        h5_write(gr, "right", g._data[1]);
      } else
        for (int i = 0; i < g.rank(); i++) h5_write(gr, "r" + std::to_string(i), g._data[i]);
    }

    //------------------------------------------------

    void h5_read(h5::group fg, std::string subgroup_name, gf_indices &g) {
      if (!fg.has_key(subgroup_name)) {
        g = gf_indices{}; // empty, no file
        return;
      }
      auto gr = fg.open_group(subgroup_name);
      assert_hdf5_format(gr, g, true);
      g._data.clear();
      auto read = [&](std::string const &x) {
        auto v = h5::h5_read<std::vector<std::string>>(gr, x);
        g._data.emplace_back(v);
      };
      if (gr.has_key("left")) {
        read("left");
        read("right");
      } else {
        for (int i = 0; i < 10; i++) {
          auto k = "r" + std::to_string(i);
          if (!gr.has_key(k)) break;
          read(k);
        }
      }
    }
  } // namespace gfs
} // namespace triqs
