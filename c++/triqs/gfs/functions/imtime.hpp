// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2021 Simons Foundation
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
// Authors: Philipp Dumitrescu, Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once

namespace triqs {
  namespace gfs {

    //-------------------------------------------------------
    // rebinning
    // ------------------------------------------------------

    /**
     * @brief Rebin an imaginary-time Green's function onto a coarser uniform mesh.
     *
     * @details The new mesh has ``new_n_tau`` points covering the same \f$ [0, \beta] \f$ interval. Each output point 
     * is an average of the input values whose \f$ \tau \f$ falls in the corresponding bin.
     *
     * @param g The imaginary-time Green's function to rebin.
     * @param new_n_tau Number of points of the output mesh.
     * @return A new imaginary-time Green's function on a mesh of size ``new_n_tau``.
     */
    gf<mesh::imtime> rebinning_tau(gf_const_view<mesh::imtime> const &g, size_t new_n_tau);
  } // namespace gfs
} // namespace triqs
