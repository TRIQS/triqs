// Copyright (c) 2023 Simons Foundation
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
// Authors: Dominik Kiese, Nils Wentzell

#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs/gf_sym_grp.hpp>
#include <triqs/gfs.hpp>

TEST(GfSymGrp, ScalarNoProduct) {
  // some dummy gf
  mesh::imfreq m{1, Fermion, 10};
  auto G             = gf<imfreq, scalar_valued>{m};
  using mesh_index_t = decltype(G)::mesh_t::index_t;
  using sym_t        = std::tuple<mesh_index_t, nda::operation>;
  using sym_func_t   = std::function<sym_t(mesh_index_t const &)>;

  // some dummy symmetries
  auto s = [](mesh_index_t const &x) { return sym_t{-x - 1, {}}; };

  // build the symmetry group and test number of classes
  std::vector<sym_func_t> sym_list = {s};
  auto grp                         = triqs::gfs::sym_grp{G, sym_list};

  EXPECT_EQ(grp.num_classes(), 10);

  // init second gf from symmetry group and test if it matches input gf
  for (auto &x : G.data()) x = std::complex{nda::rand(), nda::rand()};

  auto init_func = [&G](mesh_index_t const &x) { return G[x]; };
  grp.init(G, init_func);

  auto Gp = gf<imfreq, scalar_valued>{m};
  grp.init(Gp, init_func);

  EXPECT_GF_NEAR(G, Gp);

  // test symmetrization
  auto const &[max_diff, max_mesh_index, max_target_index] = grp.symmetrize(G);
  EXPECT_NEAR(max_diff, 0.0, 1E-15);

  // test transformation to and initialization from representative data
  auto const vec = grp.get_representative_data(G); 
  grp.init_from_representative_data(Gp, vec);
  EXPECT_GF_NEAR(G, Gp);
}

TEST(GfSymGrp, ScalarProduct) {
  // some dummy gf
  mesh::imfreq m{1, Fermion, 10};
  auto G             = gf<prod<imfreq, imfreq>, scalar_valued>{m * m};
  using mesh_index_t = decltype(G)::mesh_t::index_t;
  using sym_t        = std::tuple<mesh_index_t, nda::operation>;
  using sym_func_t   = std::function<sym_t(mesh_index_t const &)>;

  // some dummy symmetries
  auto s1 = [](mesh_index_t const &x) {
    auto [n_w1, n_w2] = x;
    return sym_t{std::tuple{-n_w1 - 1, n_w2}, {}};
  };

  auto s2 = [](mesh_index_t const &x) {
    auto [n_w1, n_w2] = x;
    return sym_t{std::tuple{n_w1, -n_w2 - 1}, {}};
  };

  // build the symmetry group and test number of classes
  std::vector<sym_func_t> sym_list = {s1, s2};
  auto grp                         = triqs::gfs::sym_grp{G, sym_list};

  EXPECT_EQ(grp.num_classes(), 100);

  // init second gf from symmetry group and test if it matches input gf
  for (auto &x : G.data()) x = std::complex{nda::rand(), nda::rand()};

  auto init_func = [&G](mesh_index_t const &x) { return G[x]; };
  grp.init(G, init_func);

  auto Gp = gf<prod<imfreq, imfreq>, scalar_valued>{m * m};
  grp.init(Gp, init_func);

  EXPECT_GF_NEAR(G, Gp);

  // test symmetrization
  auto const &[max_diff, max_mesh_index, max_target_index] = grp.symmetrize(G);
  EXPECT_NEAR(max_diff, 0.0, 1E-15);

  // test transformation to and initialization from representative data
  auto const vec = grp.get_representative_data(G); 
  grp.init_from_representative_data(Gp, vec);
  EXPECT_GF_NEAR(G, Gp);
}

TEST(GfSymGrp, TensorNoProduct) {
  // some dummy gf
  mesh::imfreq m{1, Fermion, 10};
  auto G               = gf<imfreq, matrix_valued>{m, {3, 3}};
  using mesh_index_t   = decltype(G)::mesh_t::index_t;
  using target_index_t = std::array<long, 2>;
  using sym_t          = std::tuple<mesh_index_t, target_index_t, nda::operation>;
  using sym_func_t     = std::function<sym_t(mesh_index_t const &, target_index_t const &)>;

  // some dummy symmetries
  auto s1 = [](mesh_index_t const &x, target_index_t const &y) { return sym_t{-x - 1, y, {}}; };

  auto s2 = [](mesh_index_t const &x, target_index_t const &y) {
    auto [y1, y2] = y;
    return sym_t{x, std::array{y2, y1}, {}};
  };

  // build the symmetry group and test number of classes
  std::vector<sym_func_t> sym_list = {s1, s2};
  auto grp                         = triqs::gfs::sym_grp{G, sym_list};

  EXPECT_EQ(grp.num_classes(), 60);

  // init second gf from symmetry group and test if it matches input gf
  for (auto &x : G.data()) x = std::complex{nda::rand(), nda::rand()};

  auto init_func = [&G](mesh_index_t const &x, target_index_t const &y) { return std::apply(G[x], y); };
  grp.init(G, init_func);

  auto Gp = gf<imfreq, matrix_valued>{m, {3, 3}};
  grp.init(Gp, init_func);

  EXPECT_GF_NEAR(G, Gp);

  // test symmetrization
  auto const &[max_diff, max_mesh_index, max_target_index] = grp.symmetrize(G);
  EXPECT_NEAR(max_diff, 0.0, 1E-15);

  // test transformation to and initialization from representative data
  auto const vec = grp.get_representative_data(G); 
  grp.init_from_representative_data(Gp, vec);
  EXPECT_GF_NEAR(G, Gp);
}

TEST(GfSymGrp, TensorProduct) {
  // some dummy gf
  mesh::imfreq m{1, Fermion, 10};
  auto G               = gf<prod<imfreq, imfreq>, matrix_valued>{m * m, {3, 3}};
  using mesh_index_t   = decltype(G)::mesh_t::index_t;
  using target_index_t = std::array<long, 2>;
  using sym_t          = std::tuple<mesh_index_t, target_index_t, nda::operation>;
  using sym_func_t     = std::function<sym_t(mesh_index_t const &, target_index_t const &)>;

  // some dummy symmetries
  auto s1 = [](mesh_index_t const &x, target_index_t const &y) {
    auto [n_w1, n_w2] = x;
    return sym_t{std::tuple{-n_w1 - 1, n_w2}, y, {}};
  };

  auto s2 = [](mesh_index_t const &x, target_index_t const &y) {
    auto [n_w1, n_w2] = x;
    return sym_t{std::tuple{n_w1, -n_w2 - 1}, y, {}};
  };

  auto s3 = [](mesh_index_t const &x, target_index_t const &y) {
    auto [y1, y2] = y;
    return sym_t{x, std::array{y2, y1}, {}};
  };

  // build the symmetry group and test number of classes
  std::vector<sym_func_t> sym_list = {s1, s2, s3};
  auto grp                         = triqs::gfs::sym_grp{G, sym_list};

  EXPECT_EQ(grp.num_classes(), 600);

  // init second gf from symmetry group and test if it matches input gf
  for (auto &x : G.data()) x = std::complex{nda::rand(), nda::rand()};

  auto init_func = [&G](mesh_index_t const &x, target_index_t const &y) { return std::apply(G[x], y); };
  grp.init(G, init_func);

  auto Gp = gf<prod<imfreq, imfreq>, matrix_valued>{m * m, {3, 3}};
  grp.init(Gp, init_func);

  EXPECT_GF_NEAR(G, Gp);

  // test symmetrization
  auto const &[max_diff, max_mesh_index, max_target_index] = grp.symmetrize(G);
  EXPECT_NEAR(max_diff, 0.0, 1E-15);

  // test transformation to and initialization from representative data
  auto const vec = grp.get_representative_data(G); 
  grp.init_from_representative_data(Gp, vec);
  EXPECT_GF_NEAR(G, Gp);
}

TEST(GfSymGrp, MomentumNoFrequency) {
  // some dummy gf
  auto BZ            = brillouin_zone{bravais_lattice{nda::eye<double>(2)}};
  auto G             = gf<brzone, scalar_valued>{{BZ, 4}};
  using mesh_index_t = decltype(G)::mesh_t::index_t;
  using sym_t        = std::tuple<mesh_index_t, nda::operation>;
  using sym_func_t   = std::function<sym_t(mesh_index_t const &)>;

  // some dummy symmetries
  auto s1 = [m = G.mesh()](mesh_index_t const &x) { return sym_t{m.index_modulo(-x), {}}; };
  auto s2 = [m = G.mesh()](mesh_index_t const &x) { return sym_t{std::array{x[1], x[0], x[2]}, {}}; };

  // build the symmetry group and test number of classes
  std::vector<sym_func_t> sym_list = {s1, s2};
  auto grp                         = triqs::gfs::sym_grp{G, sym_list};

  EXPECT_EQ(grp.num_classes(), 7);

  // init second gf from symmetry group and test if it matches input gf
  for (auto &x : G.data()) x = std::complex{nda::rand(), nda::rand()};

  auto init_func = [&G](mesh_index_t const &x) { return G[x]; };
  grp.init(G, init_func);

  auto Gp = gf<brzone, scalar_valued>{{BZ, 4}};
  grp.init(Gp, init_func);

  EXPECT_GF_NEAR(G, Gp);

  // test symmetrization
  auto const &[max_diff, max_mesh_index, max_target_index] = grp.symmetrize(G);
  EXPECT_NEAR(max_diff, 0.0, 1E-15);

  // test transformation to and initialization from representative data
  auto const vec = grp.get_representative_data(G); 
  grp.init_from_representative_data(Gp, vec);
  EXPECT_GF_NEAR(G, Gp);
}

TEST(GfSymGrp, MomentumFrequencyOmp) {
  // some dummy gf
  mesh::imfreq m{1, Fermion, 10};
  auto BZ            = brillouin_zone{bravais_lattice{nda::eye<double>(2)}};
  auto G             = gf<prod<brzone, brzone, imfreq, imfreq>, scalar_valued>{{{BZ, 4}, {BZ, 4}, m, m}};
  using mesh_index_t = decltype(G)::mesh_t::index_t;
  using sym_t        = std::tuple<mesh_index_t, nda::operation>;
  using sym_func_t   = std::function<sym_t(mesh_index_t const &)>;

  // some dummy symmetries
  auto s1 = [](mesh_index_t const &x) {
    auto const &[k1, k2, n_w1, n_w2] = x;
    return sym_t{std::tuple{k1, k2, -n_w1 - 1, n_w2}, {}};
  };

  auto s2 = [](mesh_index_t const &x) {
    auto const &[k1, k2, n_w1, n_w2] = x;
    return sym_t{std::tuple{k1, k2, n_w1, -n_w2 - 1}, {}};
  };

  auto s3 = [&m = std::get<0>(G.mesh())](mesh_index_t const &x) {
    auto const &[k1, k2, n_w1, n_w2] = x;
    return sym_t{std::tuple{m.index_modulo(-k1), m.index_modulo(-k2), n_w1, n_w2}, {}};
  };

  auto s4 = [](mesh_index_t const &x) {
    auto const &[k1, k2, n_w1, n_w2] = x;
    return sym_t{std::tuple{std::array{k1[1], k1[0], k1[2]}, std::array{k2[1], k2[0], k2[2]}, n_w1, n_w2}, {}};
  };

  // build the symmetry group and test number of classes
  std::vector<sym_func_t> sym_list = {s1, s2, s3, s4};
  auto grp                         = triqs::gfs::sym_grp{G, sym_list};

  EXPECT_EQ(grp.num_classes(), 7600);

  // init second gf from symmetry group and test if it matches input gf (omp enabled)
  for (auto &x : G.data()) x = std::complex{nda::rand(), nda::rand()};
  auto init_func = [&G](mesh_index_t const &x) { return G[x]; };
  grp.init(G, init_func, true);

  auto Gp = gf<prod<brzone, brzone, imfreq, imfreq>, scalar_valued>{{{BZ, 4}, {BZ, 4}, m, m}};
  grp.init(Gp, init_func, true);

  EXPECT_GF_NEAR(G, Gp);

  // test symmetrization
  auto const &[max_diff, max_mesh_index, max_target_index] = grp.symmetrize(G);
  EXPECT_NEAR(max_diff, 0.0, 1E-15);

  // test transformation to and initialization from representative data
  auto const vec = grp.get_representative_data(G); 
  grp.init_from_representative_data(Gp, vec);
  EXPECT_GF_NEAR(G, Gp);
}

MAKE_MAIN;
