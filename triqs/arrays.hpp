/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once

// for python code generator, we need to know what to include...
#define TRIQS_ARRAYS_INCLUDED

// The basic classes
#include <triqs/arrays/array.hpp>
#include <triqs/arrays/matrix.hpp>
#include <triqs/arrays/vector.hpp>

//
#include <triqs/arrays/functional/map.hpp>
#include <triqs/arrays/functional/fold.hpp>
#include <triqs/arrays/math_functions.hpp>
#include <triqs/arrays/algorithms.hpp>

// HDF5 interface
#include <triqs/arrays/h5/simple_read_write.hpp>
#include <triqs/arrays/h5/array_of_non_basic.hpp>

// proxy
#include <triqs/arrays/proxy.hpp>

// Regrouping indices
#include <triqs/arrays/group_indices.hpp>

// Linear algebra ?? Keep here ?
#include <triqs/arrays/linalg/det_and_inverse.hpp>

#include <triqs/arrays/mpi.hpp>

