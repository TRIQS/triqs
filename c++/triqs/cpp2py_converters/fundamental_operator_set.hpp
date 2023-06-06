// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
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
#include <triqs/hilbert_space/fundamental_operator_set.hpp>

namespace cpp2py {

  // converters to/from a vector<vector<variant<int,string>>> using existing converters and the
  // reduction of fundamental_operator_set to/from it.
  template <>
  struct py_converter<triqs::hilbert_space::fundamental_operator_set>
     : py_converter_generic_cast_construct<triqs::hilbert_space::fundamental_operator_set, triqs::hilbert_space::fundamental_operator_set::data_t> {};
} // namespace cpp2py
