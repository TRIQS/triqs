// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell, tayral

#pragma once

#include <vector>
#include <utility>

#include "utility/factory.hpp"
#include "utility/tuple_tools.hpp"
#include "utility/macros.hpp"
#include "utility/view_tools.hpp"
#include "utility/expression_template_tools.hpp"
#include "arrays.hpp"
#include "std_addons/complex.hpp"

#include <mpi/vector.hpp>

#include "./mesh/mesh_concepts.hpp"

#include "./mesh/imtime.hpp"
#include "./mesh/imfreq.hpp"
#include "./mesh/retime.hpp"
#include "./mesh/refreq.hpp"
#include "./mesh/legendre.hpp"
#include "./mesh/brzone.hpp"
#include "./mesh/cyclat.hpp"
#include "./mesh/prod.hpp"

#include "./mesh/details/debug.hpp"
#include "./mesh/details/closest_mesh_pt.hpp"

namespace triqs::mesh {

  // -- Backward compat aliases --

  using cyclic_lattice = cyclat;

  template<typename ...T> using cartesian_product = prod<T...>;
}

