// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once
#include "../domains/legendre.hpp"
#include "./discrete.hpp"

namespace triqs {
  namespace gfs {

    struct legendre {};

    // mesh type and its factories
    template <> struct gf_mesh<legendre> : gf_mesh<discrete<legendre_domain>> {
      using B     = gf_mesh<discrete<legendre_domain>>;
      using var_t = legendre;

      gf_mesh() = default;
      gf_mesh(double beta, statistic_enum S, size_t n_leg) : B(typename B::domain_t(beta, S, n_leg)) {}

      static std::string hdf5_format() { return "MeshLegendre"; }

      friend void h5_write(h5::group fg, std::string const &subgroup_name, gf_mesh const &m) { h5_write_impl(fg, subgroup_name, m, "MeshLegendre"); }

      friend void h5_read(h5::group fg, std::string const &subgroup_name, gf_mesh &m) { h5_read_impl(fg, subgroup_name, m, "MeshLegendre"); }
    };
  } // namespace gfs
} // namespace triqs
