// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once
#include "../details/mesh_tools.hpp"
#include "./linear.hpp"
#include "../domains/real_complex.hpp"

namespace triqs::mesh {

  /** A linear mesh on a segment on R */
  struct segment_mesh : linear_mesh<real_domain> {
    using B        = linear_mesh<real_domain>;
    segment_mesh() = default;
    segment_mesh(double x_min, double x_max, int n_pts) : B(real_domain{}, x_min, x_max, n_pts) {}
  };

} // namespace triqs::mesh
