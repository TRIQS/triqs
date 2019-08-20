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

// for python code generator, we need to know what has to been included.
#define TRIQS_INCLUDED_GF

// the meshes
#include "./mesh.hpp"

// the targets
#include "./gfs/gf/defs.hpp"
#include "./gfs/gf/targets.hpp"

// gf class and its mechanisms
#include "./gfs/gf/gf.hpp"
#include "./gfs/gf/gf_view.hpp"
#include "./gfs/gf/gf_const_view.hpp"
#include "./gfs/make_gf.hpp"
#include "./gfs/gf/auto_assign.hpp"

// multivar
#include "./mesh/product.hpp"
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
#include "./gfs/functions/functions2.hpp"
#include "./gfs/functions/imfreq.hpp"
#include "./gfs/functions/imtime.hpp"
#include "./gfs/functions/product.hpp"
#include "./gfs/functions/legendre.hpp"
#include "./gfs/functions/density.hpp"

// fourier
#include "./gfs/transform/fourier.hpp"
#include "./gfs/transform/legendre_matsubara.hpp"


//----------------------------------------------------------------
// Backward compatibility :  aliases, etc..
//----------------------------------------------------------------
namespace triqs::gfs {

  template <typename M, typename Requires = std::enable_if_t<mesh::models_mesh_concept_v<M>>>
  using gf_mesh [[deprecated("mesh::X is deprecated since TRIQS 2.3. Replace simply by M. Cf documentation.")]] = M;

  // Get shape of the data or of the target
  template <typename G> TRIQS_DEPRECATED("use X.data_shape() instead") auto get_gf_data_shape(G const &g) { return g.data_shape(); }
  template <typename G> TRIQS_DEPRECATED("use X.target_shape() instead") auto get_target_shape(G const &g) { return g.target_shape(); }

} // namespace triqs::gfs

