// Copyright (c) 2015-2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2016 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet

#include <triqs/gfs.hpp>

using namespace triqs::gfs;

template <typename M, typename T> void fct_2_blocks(block_gf_view<M,T> x, block_gf_view<M,T> y, double precision = 1.e-10) {
  if (x.size() != y.size()) throw "Block GFs have different number of blocks";
}
