// Copyright (c) 2019 Simons Foundation
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
// Authors: Olivier Parcollet

#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/arrays.hpp>
#include <cmath>
#include <limits>
#include <triqs/arrays.hpp>

using triqs::arrays::array;

// FIXME replace by shape_t when the lib is cleaned
// currently triqs::arrays::shape_t is a minivec of long, and the shape of an array is a mininvec of size_t
// FIXME
template <int R> using myshape_t = mini_vector<size_t, R>;

// variables for the test
range _;
ellipsis ___;
