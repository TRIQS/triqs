// Copyright (c) 2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2020 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell, tayral

#include <triqs/test_tools/gfs.hpp>

TEST(Gf, BosonicH5Read) {
  auto g = gf<imfreq>{{1, Boson, 10}, {1, 1}};
  h5::file file("bugTA2.h5", 'w');
  h5_write(file, "G", g);
  auto g2 = gf<imfreq>{};
  h5_read(file, "G", g2);
  EXPECT_EQ(g.mesh(), g2.mesh());
}

TEST(MeshGf, BosonicH5Read) {
  auto m = gf_mesh<imfreq>{1, Boson, 10, matsubara_mesh_opt::positive_frequencies_only};
  h5::file file("bugTA2.h5", 'w');
  h5_write(file, "Gmesh", m);
  auto m2 = gf_mesh<imfreq>{};
  h5_read(file, "Gmesh", m2);
  //h5_read(file,"Gmesh", m2);
  EXPECT_EQ(m, m2);
}

TEST(MeshGf, BosonicH5ReadHalfOnly) {
  auto m = gf_mesh<imfreq>{1, Boson, 3, matsubara_mesh_opt::positive_frequencies_only};
  h5::file file("bugTA2.h5", 'w');
  h5_write(file, "Gmesh", m);
  auto m2 = gf_mesh<imfreq>{};
  h5_read(file, "Gmesh", m2);
  EXPECT_EQ(m, m2);
}
TEST(MeshGf, FermionicH5Read) {
  auto m = gf_mesh<imfreq>{1, Fermion, 3};
  h5::file file("bugTA2.h5", 'w');
  h5_write(file, "Gmesh", m);
  auto m2 = gf_mesh<imfreq>{};
  h5_read(file, "Gmesh", m2);
  EXPECT_EQ(m, m2);
}
TEST(MeshGf, FermionicH5ReadHalfOnly) {
  auto m = gf_mesh<imfreq>{1, Fermion, 3, matsubara_mesh_opt::positive_frequencies_only};
  h5::file file("bugTA2.h5", 'w');
  h5_write(file, "Gmesh", m);
  auto m2 = gf_mesh<imfreq>{};
  h5_read(file, "Gmesh", m2);
  EXPECT_EQ(m, m2);
}
MAKE_MAIN;
