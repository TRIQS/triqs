/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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

#define TRIQS_GF_INCLUDED

// the meshes 
#include "./gfs/meshes.hpp"

// the targets
#include "./gfs/gf/targets.hpp"

// gf class and its mechanisms
#include "./gfs/gf/gf.hxx"
#include "./gfs/make_gf.hpp"
#include "./gfs/gf/auto_assign.hpp"
#include "./gfs/gf/gf_expr.hpp"

// multivar 
#include "./gfs/gf/product.hpp"
#include "./gfs/gf/partial_eval.hpp"

// evaluator
#include "./gfs/evaluator.hpp"

// singularity 
#include "./gfs/singularity/gf_singularity.hpp"
#include "./gfs/singularity/m_tail.hpp"

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
#include "./gfs/functions/functions2.hpp"
#include "./gfs/functions/imfreq.hpp"
#include "./gfs/functions/imtime.hpp"
#include "./gfs/functions/product.hpp"
#include "./gfs/functions/legendre.hpp"
#include "./gfs/functions/density.hpp"

// fourier
#include "./gfs/transform/fourier_matsubara.hpp"
#include "./gfs/transform/fourier_real.hpp"
#include "./gfs/transform/fourier_lattice.hpp"
#include "./gfs/transform/legendre_matsubara.hpp"

