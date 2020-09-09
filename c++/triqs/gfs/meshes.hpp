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

#include "../utility/first_include.hpp"

#include <vector>
#include <utility>

#include "../utility/factory.hpp"
#include "../utility/tuple_tools.hpp"
#include "../utility/macros.hpp"
#include "../utility/view_tools.hpp"
#include "../utility/expression_template_tools.hpp"
#include "../arrays.hpp"
#include "../std_addons/complex.hpp"
#include <mpi/vector.hpp>

#include "./meshes/multivar_eval.hpp"

#include "./meshes/imtime.hpp"
#include "./meshes/imfreq.hpp"
#include "./meshes/retime.hpp"
#include "./meshes/refreq.hpp"
#include "./meshes/legendre.hpp"
#include "./domains/R.hpp"
#include "../lattice/gf_mesh_brillouin_zone.hpp"
#include "../lattice/gf_mesh_cyclic_lattice.hpp"
