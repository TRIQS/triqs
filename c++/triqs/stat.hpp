// Copyright (c) 2014 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2019-2023 Simons Foundation
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
// Authors: Thomas Ayral, Philipp D, Olivier Parcollet, Nils Wentzell

#pragma once

/**
 * @file
 * @brief Umbrella header for the TRIQS statistical-analysis tools.
 */

#include <triqs/stat/histograms.hpp>
#include <triqs/stat/jackknife.hpp>
#include <triqs/stat/lin_binning.hpp>
#include <triqs/stat/log_binning.hpp>
#include <triqs/stat/mean_error.hpp>
#include <triqs/stat/utils.hpp>

#ifdef C2PY_INCLUDED
#include <triqs/stat/histograms.wrap.hxx>
#endif
