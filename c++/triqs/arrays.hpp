// Copyright (c) 2013-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2017 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2019-2020 Simons Foundation
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

// for python code generator, we need to know what to include...
#define TRIQS_INCLUDED_ARRAYS

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
#include <triqs/arrays/h5.hpp>

// proxy
#include <triqs/arrays/proxy.hxx>

// Regrouping indices
#include <triqs/arrays/group_indices.hpp>

// Reinterpretation of nx1x1 array and co
#include <triqs/arrays/reinterpret.hpp>

// Linear algebra ?? Keep here ?
#include <triqs/arrays/linalg/det_and_inverse.hpp>

#include <triqs/arrays/mpi.hpp>

// immutable_array
#include <triqs/arrays/make_immutable_array.hpp>
