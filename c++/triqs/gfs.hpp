// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
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

// for python code generator, we need to know what has to been included.
#define TRIQS_INCLUDED_GF

// the meshes
#include "./gfs/meshes.hpp"

// debug
#include "./gfs/debug.hpp"

// the targets
#include "./gfs/gf/targets.hpp"

// gf class and its mechanisms
#include "./gfs/gf/gf.hpp"
#include "./gfs/gf/gf_view.hpp"
#include "./gfs/gf/gf_const_view.hpp"
#include "./gfs/make_gf.hpp"
#include "./gfs/gf/auto_assign.hpp"

// multivar
#include "./gfs/meshes/product.hpp"
#include "./gfs/gf/partial_eval.hpp"

// expression template
#include "./gfs/gf/gf_expr.hpp"

// evaluator
#include "./gfs/evaluator.hpp"

// block gf
#include "./gfs/block/block_gf.hxx"
#include "./gfs/block/functions.hxx"
#include "./gfs/block/expr.hpp"
#include "./gfs/block/auto_assign.hpp"
#include "./gfs/block/map.hpp"
#include "./gfs/block/mapped_functions.hxx"

// hdf5
#include "./gfs/h5.hpp"

// functions
#include "./gfs/functions/closest_mesh_pt.hpp"
#include "./gfs/functions/functions2.hpp"
#include "./gfs/functions/imfreq.hpp"
#include "./gfs/functions/imtime.hpp"
#include "./gfs/functions/product.hpp"
#include "./gfs/functions/legendre.hpp"
#include "./gfs/functions/density.hpp"

// fourier
#include "./gfs/transform/fourier.hpp"
#include "./gfs/transform/legendre_matsubara.hpp"
