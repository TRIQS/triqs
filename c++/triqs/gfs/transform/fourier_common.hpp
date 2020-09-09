// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Nils Wentzell

#pragma once
#include <triqs/arrays.hpp>
// include only in cpp implementation
#include <fftw3.h>

namespace triqs::gfs {

  using namespace triqs::arrays;
  // call to fftw
  void _fourier_base(array_const_view<dcomplex, 2> in, array_view<dcomplex, 2> out, int rank, int *dims, int fftw_count, int fftw_backward_forward);

} // namespace triqs::gfs
