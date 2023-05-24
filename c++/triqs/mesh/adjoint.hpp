// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
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

#include "./imtime.hpp"
#include "./imfreq.hpp"
#include "./dlr_imtime.hpp"
#include "./dlr_imfreq.hpp"
#include "./retime.hpp"
#include "./refreq.hpp"
#include "./cyclat.hpp"
#include "./brzone.hpp"

namespace triqs::mesh {

  inline imfreq make_adjoint_mesh(imtime const &m, long n_iw = -1) {
    if (n_iw == -1) n_iw = (m.size() - 1) / 6;
    return {m.beta(), m.statistic(), n_iw};
  }
  inline imtime make_adjoint_mesh(imfreq const &m, long n_tau = -1) {
    if (n_tau == -1) n_tau = 6 * (m.last_index() + 1) + 1;
    return {m.beta(), m.statistic(), n_tau};
  }

  inline dlr_imfreq make_adjoint_mesh(dlr_imtime const &m) { return dlr_imfreq{m}; }
  inline dlr_imtime make_adjoint_mesh(dlr_imfreq const &m) { return dlr_imtime{m}; }

  inline refreq make_adjoint_mesh(retime const &m, bool shift_half_bin = false) {
    int L       = m.size();
    double wmax = M_PI * (L - 1) / (L * m.delta());
    if (shift_half_bin) return {-wmax + M_PI / L / m.delta(), wmax + M_PI / L / m.delta(), L};
    return {-wmax, wmax, L};
  }
  inline retime make_adjoint_mesh(refreq const &m, bool shift_half_bin = false) {
    int L       = m.size();
    double tmax = M_PI * (L - 1) / (L * m.delta());
    if (shift_half_bin) return {-tmax + M_PI / L / m.delta(), tmax + M_PI / L / m.delta(), L};
    return {-tmax, tmax, L};
  }

  inline brzone make_adjoint_mesh(cyclat const &m) { return {brillouin_zone{m.lattice()}, m.dims()}; }
  inline cyclat make_adjoint_mesh(brzone const &m) { return {m.bz().lattice(), m.dims()}; }
} // namespace triqs::mesh
