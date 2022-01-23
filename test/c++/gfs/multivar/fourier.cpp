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
// Authors: Nils Wentzell

#include <triqs/test_tools/gfs.hpp>

// Generic Fourier test function for different ranks
template <int TARGET_RANK> void test_fourier() {
  double precision = 1e-4;
  triqs::clef::placeholder<0> k_;
  triqs::clef::placeholder<1> iOm_;
  triqs::clef::placeholder<2> iom_;
  double beta = 1;
  size_t N_iw    = 100;
  size_t N_iW    = 4;
  size_t N_k     = 4;

  double E = 1;

  auto BL        = bravais_lattice{matrix<double>{{1, 0}, {0, 1}}};
  auto BZ        = brillouin_zone{BL};
  auto k_mesh    = mesh::brzone(BZ, N_k);
  auto iw_mesh   = mesh::imfreq{beta, Fermion, N_iw};
  auto iW_mesh   = mesh::imfreq{beta, Boson, N_iW};
  auto prod_mesh = k_mesh * iW_mesh * iw_mesh;

  std::array<long, TARGET_RANK> shape{};

  if constexpr (TARGET_RANK == 2) // Matrix_valued
    shape = make_shape(2, 2);
  else if constexpr (TARGET_RANK == 3) // Tensor_valued<3>
    shape = make_shape(2, 2, 2);
  else if constexpr (TARGET_RANK == 4) // Tensor_valued<4>
    shape = make_shape(2, 2, 2, 2);

  using var_t    = prod<brzone, imfreq, imfreq>;
  using target_t = typename _target_from_type_rank<dcomplex, TARGET_RANK>::type;

  // === Test a Green function with a given dispersion relation ===

  auto g = gf<var_t, target_t>{prod_mesh, shape};

  // Initialize the Multivariable Green functions
  g(k_, iOm_, iom_) << 1 / (iom_ + iOm_ - cos(k_[0]) * cos(k_[1]));

  // Fourier Transform 1st mesh and back
  auto r_mesh = mesh::cyclat(BL, N_k);
  {
    auto g_r_iW_iw = make_gf_from_fourier<0>(g, r_mesh);
    auto gb        = make_gf_from_fourier<0>(g_r_iW_iw, k_mesh);
    EXPECT_GF_NEAR(g, gb, precision);
    gb() = fourier<0>(g_r_iW_iw);
    EXPECT_GF_NEAR(g, gb, precision);
  }

  // Fourier Transform 3rd mesh and back
  auto tau_mesh = mesh::imtime{beta, Fermion, static_cast<long>(2 * N_iw + 1)};
  {
    auto g_k_iW_tau = make_gf_from_fourier<2>(g, tau_mesh);
    auto gb         = make_gf_from_fourier<2>(g_k_iW_tau, iw_mesh);
    EXPECT_GF_NEAR(g, gb, precision);

    // Test without maker
    gb() = fourier<2>(g_k_iW_tau);
    EXPECT_GF_NEAR(g, gb, precision);
  }

  // Fourier Transform 1st and 3rd mesh and back
  {
    auto g_r_iW_tau = make_gf_from_fourier<0, 2>(g);

    auto gb = make_gf_from_fourier<0, 2>(g_r_iW_tau);
    EXPECT_GF_NEAR(g, gb, precision);

    ////Test without maker // FIXME Not Implemented
    //gb() = fourier<0, 2>(g_r_iW_tau);
    //EXPECT_GF_NEAR(g, gb, precision);

    // Specify the meshes for the maker
    auto gb1 = make_gf_from_fourier<0, 2>(g_r_iW_tau, k_mesh, iw_mesh);
    EXPECT_GF_NEAR(g, gb1, precision);
  }
}

TEST(FourierMultivar, Scalar) { test_fourier<0>(); }  // NOLINT
TEST(FourierMultivar, Matrix) { test_fourier<2>(); }  // NOLINT
TEST(FourierMultivar, Tensor3) { test_fourier<3>(); } // NOLINT
TEST(FourierMultivar, Tensor4) { test_fourier<4>(); } // NOLINT

MAKE_MAIN;
