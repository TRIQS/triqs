// Copyright (c) 2016-2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2017 Centre national de la recherche scientifique (CNRS)
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

#pragma once

#include <cpp2py/converters/std_array.hpp>
#include <nda_py/cpp2py_converters.hpp>

// in module, triggers the call of import_array
#define TRIQS_IMPORTED_CONVERTERS_ARRAYS

#ifdef TRIQS_INCLUDED_GF
#include "./cpp2py_converters/gf.hpp"
#endif
