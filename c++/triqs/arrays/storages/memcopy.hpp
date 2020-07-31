// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
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

#pragma once

#include <cstring>

namespace triqs::arrays::mem {

  // FIXME : This is only for assignment. Move it.
  template <typename T1, typename T2> void memcopy(T1 * p1, const T2 * p2, size_t size) {

    constexpr bool use_memcpy = std::is_trivially_copyable_v<T1> and std::is_same_v<T1, T2>;

    if constexpr (use_memcpy) {
      if (std::abs(p2 - p1) > size) { // guard against overlapping of data
	 std::memcpy(p1, p2, size * sizeof(T1));
        return;
      }
      for (size_t i = 0; i < size; ++i) p1[i] = p2[i];
    }
  }

} // namespace triqs::arrays::mem
