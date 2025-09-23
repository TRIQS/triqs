// Copyright (c) 2025
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

#include <triqs/mesh/adjoint.hpp>
#include <triqs/test_tools/arrays.hpp>

#include <nda/nda.hpp>

#include <numbers>

using namespace triqs::mesh;

TEST(TRIQSMesh, AdjointImtimeToImfreq) {
  // choose N s.t. (N-1) is divisible by 6
  const double beta = 3.0;
  const auto stat   = Fermion;
  const long N_tau  = 13; // (13 - 1) / 6 = 2

  auto const m_tau = imtime{beta, stat, N_tau};

  // implicit N_iw
  auto const m_iw = make_adjoint_mesh(m_tau);
  EXPECT_EQ(m_iw, (imfreq{beta, stat, (N_tau - 1) / 6}));

  // explicit N_iw
  auto const m_iw2 = make_adjoint_mesh(m_tau, 5);
  EXPECT_EQ(m_iw2, (imfreq{beta, stat, 5}));
}

TEST(TRIQSMesh, AdjointImfreqToImtime) {
  const double beta = 4.2;

  // all frequencies, Boson: indices [-n_iw+1, ..., n_iw-1] => last_index = n_iw-1
  auto const iw_all   = imfreq{beta, Boson, 3};
  auto const tau_all  = make_adjoint_mesh(iw_all);
  auto const last_all = iw_all.last_index();
  auto const nexp_all = 6 * (last_all + 1) + 1; // 6*3 + 1 = 19
  EXPECT_EQ(tau_all, (imtime{beta, Boson, nexp_all}));

  // explicit override
  EXPECT_EQ(make_adjoint_mesh(iw_all, 25), (imtime{beta, Boson, 25}));

  // positive frequencies only, Fermion
  auto const iw_pos   = imfreq{beta, Fermion, 7, imfreq::option::positive_frequencies_only};
  auto const tau_pos  = make_adjoint_mesh(iw_pos);
  auto const last_pos = iw_pos.last_index();
  auto const nexp_pos = 6 * (last_pos + 1) + 1; // last=6 => 37? actually 6*(6+1)+1 = 43
  EXPECT_EQ(nexp_pos, 43l);
  EXPECT_EQ(tau_pos, (imtime{beta, Fermion, nexp_pos}));
}

TEST(TRIQSMesh, AdjointDLRImtimeToImfreqAndBack) {
  const double beta = 10.0;
  const double wmax = 0.5;
  const double eps  = 1e-6;

  // fermion
  auto const mit          = dlr_imtime{beta, Fermion, wmax, eps};
  auto const mif_from_mit = make_adjoint_mesh(mit);
  EXPECT_EQ(mif_from_mit, dlr_imfreq{mit});

  // boson
  auto const mif          = dlr_imfreq{beta, Boson, wmax, eps};
  auto const mit_from_mif = make_adjoint_mesh(mif);
  EXPECT_EQ(mit_from_mif, dlr_imtime{mif});
}

TEST(TRIQSMesh, AdjointRetimeToRefreq) {
  using std::numbers::pi;

  // symmetric time window [-1, 1] with N=11
  auto const mt    = retime{-1.0, 1.0, 11};
  auto const N     = static_cast<double>(mt.size());
  auto const wmax  = pi * (N - 1) / (N * mt.delta());
  auto const shift = pi / (N * mt.delta());

  // no shift
  auto const mf          = make_adjoint_mesh(mt, false);
  auto const mf_expected = refreq{-wmax, wmax, mt.size()};
  EXPECT_EQ(mf, mf_expected);

  // with half-bin shift
  auto const mf_shift          = make_adjoint_mesh(mt, true);
  auto const mf_expected_shift = refreq{-wmax + shift, wmax + shift, mt.size()};
  EXPECT_EQ(mf_shift, mf_expected_shift);
}

TEST(TRIQSMesh, AdjointRefreqToRetime) {
  using std::numbers::pi;

  // asymmetric frequency window with N=10
  auto const mf    = refreq{-2.0, 3.0, 10};
  auto const N     = static_cast<double>(mf.size());
  auto const tmax  = pi * (N - 1) / (N * mf.delta());
  auto const shift = pi / (N * mf.delta());

  // no shift
  auto const mt          = make_adjoint_mesh(mf, false);
  auto const mt_expected = retime{-tmax, tmax, mf.size()};
  EXPECT_EQ(mt, mt_expected);

  // with half-bin shift
  auto const mt_shift          = make_adjoint_mesh(mf, true);
  auto const mt_expected_shift = retime{-tmax + shift, tmax + shift, mf.size()};
  EXPECT_EQ(mt_shift, mt_expected_shift);
}

TEST(TRIQSMesh, AdjointCyclatToBrzoneAndBack) {
  using std::numbers::pi;

  // nontrivial lattice and dims
  auto const bl   = bravais_lattice{nda::eye<double>(3) * 0.75};
  auto const dims = std::array<long, 3>{2, 3, 4};

  auto const m_lat = cyclat{bl, dims};
  auto const m_bz  = make_adjoint_mesh(m_lat);

  // expect same dims and BZ built from same lattice
  EXPECT_EQ(m_bz, (brzone{brillouin_zone{bl}, dims}));

  // back to cyclat
  auto const m_lat2 = make_adjoint_mesh(m_bz);
  EXPECT_EQ(m_lat2, (cyclat{bl, dims}));
}

MAKE_MAIN;
