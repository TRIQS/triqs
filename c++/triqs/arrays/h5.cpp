// Copyright (c) 2020 Simons Foundation
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

#include "h5.hpp"

namespace triqs::arrays::h5_details {

  // implementation of the write
  void write(h5::group g, std::string const &name, h5::datatype ty, void *start, int rank, bool is_complex, size_t const *lens, long const *strides,
             size_t total_size) {

    h5::array_interface::h5_array_view v{ty, start, rank, is_complex};

    auto [L_tot, strides_h5] = h5::array_interface::get_L_tot_and_strides_h5(strides, rank, total_size);

    for (int u = 0; u < rank; ++u) { // size of lhs may be size of hte rhs vector + 1 if complex. Can not simply use =
      v.slab.count[u]  = lens[u];
      v.slab.stride[u] = strides_h5[u];
      v.L_tot[u]       = L_tot[u];
    }

    h5::array_interface::write(g, name, v, true);
  }
} // namespace triqs::arrays::h5_details
