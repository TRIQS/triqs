// Copyright (c) 2014-2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2016 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2019-2023 Simons Foundation
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
// Authors: Philipp Dumitrescu, Olivier Parcollet, Nils Wentzell

#pragma once

// for python code generator, we need to know what has to been included.
#define TRIQS_INCLUDED_MESH

#include <vector>
#include <utility>

#include "utility/factory.hpp"
#include "utility/tuple_tools.hpp"
#include "utility/macros.hpp"
#include "utility/view_tools.hpp"

#include <mpi/vector.hpp>

#include "arrays.hpp"
#include "std_addons/complex.hpp"

#include "./mesh/utils.hpp"

#include "./mesh/imtime.hpp"
#include "./mesh/imfreq.hpp"
#include "./mesh/retime.hpp"
#include "./mesh/refreq.hpp"

#include "./mesh/discrete.hpp"

#include "./mesh/legendre.hpp"

#include "./mesh/dlr.hpp"
#include "./mesh/dlr_imtime.hpp"
#include "./mesh/dlr_imfreq.hpp"

#include "./mesh/brzone.hpp"
#include "./mesh/cyclat.hpp"

#include "./mesh/prod.hpp"

#include "./mesh/adjoint.hpp"

#include "./mesh/evaluate.hpp"

#ifdef C2PY_INCLUDED
#include "./cpp2py_converters/mesh.hpp"
#endif

namespace triqs::mesh {

  // -- Backward compat aliases --
  using cyclic_lattice                         = cyclat;
  template <Mesh... M> using cartesian_product = prod<M...>;
  template <Mesh M> using mesh_point           = typename M::mesh_point_t;

} // namespace triqs::mesh
