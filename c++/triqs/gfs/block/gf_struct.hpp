// Copyright (c) 2021 Simons Foundation
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
#pragma once

#include <h5/h5.hpp>

namespace triqs::gfs {

  /*----------------------------------------------------------
   *   gf_struct
   *--------------------------------------------------------*/

  /// Type describing the structure of a Block Green's function
  using gf_struct_t = std::vector<std::pair<std::string, long>>;

  /// h5_read function with backward compatibility layer for old gf_struct type
  inline void h5_read_gf_struct(h5::group g, std::string const &name, gf_struct_t &gf_struct) {
    {
      auto gobj = g.open_group(name);
      if (gobj.has_subgroup("0")) {
        auto bl0 = gobj.open_group("0");
        // Each block of old gf_struct type has subgroup "1"
        if (bl0.has_subgroup("1")) {
          auto gf_struct_bkwd = std::vector<std::pair<std::string, std::vector<std::variant<int, std::string>>>>{};
          h5::read(g, name, gf_struct_bkwd);
          gf_struct.clear();
          for (auto &[bl, idx_lst] : gf_struct_bkwd) gf_struct.push_back({bl, idx_lst.size()});
          return;
        }
      }
    }
    h5::read(g, name, gf_struct);
  }

} // namespace triqs::gfs
