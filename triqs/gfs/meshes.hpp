/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2016 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the gfied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/".
 *
 ******************************************************************************/
#pragma once

#include "../utility/first_include.hpp"

#include <vector>
#include <utility>
#include <boost/iterator/iterator_facade.hpp>

#include "../utility/factory.hpp"
#include "../utility/tuple_tools.hpp"
#include "../utility/c17.hpp"
#include "../utility/macros.hpp"
#include "../utility/complex_ops.hpp"
#include "../utility/view_tools.hpp"
#include "../utility/expression_template_tools.hpp"
#include "../arrays.hpp"
#include "../mpi/vector.hpp"


#include "./meshes/imtime.hpp"
#include "./meshes/imfreq.hpp"
#include "./meshes/retime.hpp"
#include "./meshes/refreq.hpp"
#include "./meshes/legendre.hpp"
#include "./domains/R.hpp"
#include "../lattice/regular_bz_mesh.hpp"
#include "../lattice/cyclic_lattice.hpp"

