// Copyright (c) 2014 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2020-2021 Simons Foundation
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

#include <nda/clef.hpp>

namespace triqs::utility {
  template <typename T1, typename T2>
  requires(std::is_arithmetic_v<std::decay_t<T1>> and std::is_arithmetic_v<std::decay_t<T2>>)
  inline bool kronecker(T1 &&i, T2 &&j) {
    return i == j;
  }
} // namespace triqs::utility

namespace nda::clef {
  using triqs::utility::kronecker;
  CLEF_MAKE_FNT_LAZY(kronecker);
} // namespace nda::clef
