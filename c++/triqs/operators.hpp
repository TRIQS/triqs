// Copyright (c) 2023 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet

#pragma once

#include <triqs/operators/many_body_operator.hpp>

#ifdef C2PY_INCLUDED 
#include <c2py/c2py.hpp>
#include "./cpp2py_converters/operators_real_complex.hpp"
template <> static constexpr bool c2py::is_wrapped<triqs::operators::many_body_operator>  = true;
#endif


