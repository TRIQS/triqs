// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
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

/**
 * @file
 * @brief Provides a function to capture the current C++ stack trace as a string.
 */

#pragma once

#include "./first_include.hpp"

#include <string>

namespace triqs::utility {

  /**
   * @ingroup triqs-utility-runtime
   * @brief Capture the current call stack and return it as a demangled, human-readable string.
   *
   * @return The current stack trace, one frame per line.
   */
  std::string stack_trace();

} // namespace triqs::utility
