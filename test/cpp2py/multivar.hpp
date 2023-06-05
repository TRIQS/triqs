// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
// Copyright (c) 2016 Igor Krivenko
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
// Authors: Igor Krivenko, Olivier Parcollet, Nils Wentzell

#include <triqs/gfs.hpp>
using namespace triqs::gfs;
using namespace triqs;

using mesh_t              = prod<imfreq, imfreq, imfreq>;
using target_t            = tensor_valued<4>;
using vertex_t            = gf<mesh_t, target_t>;
using vertex_view_t       = gf_view<mesh_t, target_t>;
using block_vertex_t      = block2_gf<mesh_t, target_t>;
using block_vertex_view_t = block2_gf_view<mesh_t, target_t>;

vertex_t make_vertex(double a) {
  double beta = 10;
  auto m_b    = gf_mesh<imfreq>{beta, Boson, 10};
  auto m_f    = gf_mesh<imfreq>{beta, Fermion, 5};

  auto gamma = vertex_t{{m_b, m_f, m_f}, {2, 2, 2, 2}};
  gamma()    = a;

  h5::file file("make_vertex.h5", 'w');
  h5_write(file, "gamma", gamma);

  return gamma;
}

void pass_vertex(vertex_view_t gamma) {
  h5::file file("pass_vertex.h5", 'w');
  h5_write(file, "gamma", gamma);
}

block_vertex_t make_block_vertex(double a) {
  double beta = 10;
  auto m_b    = gf_mesh<imfreq>{beta, Boson, 10};
  auto m_f    = gf_mesh<imfreq>{beta, Fermion, 5};

  auto gamma_uu = vertex_t{{m_b, m_f, m_f}, {2, 2, 2, 2}};
  auto gamma_dd = vertex_t{{m_b, m_f, m_f}, {2, 2, 2, 2}};
  auto gamma_ud = vertex_t{{m_b, m_f, m_f}, {2, 2, 2, 2}};
  auto gamma_du = vertex_t{{m_b, m_f, m_f}, {2, 2, 2, 2}};

  gamma_uu() = a;
  gamma_dd() = -a;

  auto block_gamma = make_block2_gf<mesh_t, target_t>({"up", "dn"}, {"up", "dn"}, {{gamma_uu, gamma_ud}, {gamma_du, gamma_dd}});

  h5::file file("make_block_vertex.h5", 'w');
  h5_write(file, "gamma", block_gamma);

  return block_gamma;
}

void pass_block_vertex(block_vertex_view_t gamma) {
  h5::file file("pass_block_vertex.h5", 'w');
  h5_write(file, "gamma", gamma);
}
