// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Olivier Parcollet, Nils Wentzell

#include <triqs/test_tools/gfs.hpp>
using namespace triqs::clef;
using namespace triqs::lattice;
using namespace triqs::gfs;

using gf_bz_imfreq_mat = gf<prod<brzone, imfreq>, matrix_valued>;
auto _                 = all_t{};

static_assert(nda::get_algebra<nda::expr_unary<'-', nda::basic_array<std::complex<double>, 2,
      nda::C_layout, 'M', nda::heap<>> &>> == 'M', "oops");

// --------------------------------------------------------------------------------

gf_bz_imfreq_mat compute_gg_fft(gf_bz_imfreq_mat const &G_k_w) {
  placeholder<0> r_;
  placeholder<1> tau_;
  placeholder<2> k_;

  auto w_mesh = std::get<1>(G_k_w.mesh());
  auto k_mesh = std::get<0>(G_k_w.mesh());
  double beta = w_mesh.domain().beta;
  int nw      = w_mesh.last_index() + 1;
  int nnu     = 2;
  int ntau    = nw * 4;
  int nk      = k_mesh.dims()[0];

  auto chi0_q_nu = gf_bz_imfreq_mat{{k_mesh, {beta, Boson, nnu}}, {1, 1}};

  auto chi0_q_tau = gf<prod<brzone, imtime>, matrix_valued>{{k_mesh, {beta, Boson, ntau}}, {1, 1}};
  auto chi0_R_tau = gf<prod<cyclat, imtime>, matrix_valued>{{{nk, nk}, {beta, Boson, ntau}}, {1, 1}};
  auto G_k_tau    = gf<prod<brzone, imtime>, matrix_valued>{{k_mesh, {beta, Fermion, ntau}}, {1, 1}};
  auto G_R_tau    = gf<prod<cyclat, imtime>, matrix_valued>{{{nk, nk}, {beta, Fermion, ntau}}, {1, 1}};

  for (auto const &k : std::get<0>(G_k_tau.mesh())) G_k_tau[k, _] = fourier(G_k_w[k, _]);

  for (auto const &tau : std::get<1>(G_R_tau.mesh())) G_R_tau[_, tau] = fourier(G_k_tau[_, tau]);

  chi0_R_tau(r_, tau_) << -G_R_tau(r_, tau_) * G_R_tau(-r_, beta - tau_);

  /*
 h5::file file("bubble.h5", 'w' );
 h5_write(file, "G_R_tau", G_R_tau);
 h5_write(file, "chi0_R_tau", chi0_R_tau);
 */

  for (auto const &tau : std::get<1>(chi0_q_tau.mesh())) chi0_q_tau[_, tau] = fourier(chi0_R_tau[_, tau]);

  for (auto const &k : std::get<0>(chi0_q_nu.mesh())) chi0_q_nu[k, _] = fourier(chi0_q_tau[k, _], make_zero_tail(chi0_R_tau[_, 0]));

  return chi0_q_nu;
}

// --------------------------------------------------------------------------------

gf_bz_imfreq_mat compute_gg(gf_bz_imfreq_mat const &G_k_w) {

  placeholder<0> k_;
  placeholder<1> q_;
  placeholder<3> iw_;
  placeholder<4> inu_;
  auto const &w_mesh = std::get<1>(G_k_w.mesh());
  auto const &k_mesh = std::get<0>(G_k_w.mesh());
  double beta        = w_mesh.domain().beta;
  int nnu            = 2;

  auto chi0_q_nu = gf_bz_imfreq_mat{{k_mesh, {beta, Boson, nnu}}, {1, 1}};

  chi0_q_nu(q_, inu_) << sum(G_k_w(k_, iw_) * G_k_w(k_ + q_, inu_ + iw_), k_ = k_mesh, iw_ = w_mesh) / k_mesh.size() / beta;

  return chi0_q_nu;
}

// --------------------------------------------------------------------------------

TEST(Gf, Bubble) {
  int nw      = 40;
  int nk      = 6;
  auto bz     = brillouin_zone{bravais_lattice{{{1., 0.}, {0.5, sqrt(3) / 2.}}}};
  double beta = 5, mu = 0.5;
  placeholder<0> k_;
  placeholder<4> inu_;
  auto eps_k_ = -2 * (cos(k_(0)) + cos(k_(1)));
  auto G_k_w  = gf_bz_imfreq_mat{{{bz, nk}, {beta, Fermion, nw}}, {1, 1}};
  G_k_w(k_, inu_) << 1.0 / (inu_ + mu - eps_k_);

  auto chi_q_nu_fft = compute_gg_fft(G_k_w);
  auto chi_q_nu     = compute_gg(G_k_w);

  /*
  h5::file file("bubble.h5", 'a' );
  h5_write(file, "chi_fft", chi_q_nu_fft);
  h5_write(file, "chi", chi_q_w);
 */

  // Error due to finite summation range in compute_gg
  EXPECT_ARRAY_NEAR(chi_q_nu_fft.data(), chi_q_nu.data(), 1e-2);
}
MAKE_MAIN;
