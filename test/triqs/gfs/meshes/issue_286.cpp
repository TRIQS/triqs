#include <triqs/test_tools/gfs.hpp>

TEST(Gf, BosonicH5Read) {
  auto g = gf<imfreq>{{1, Boson, 10}, {1, 1}};
  h5::file file("bugTA2.h5", H5F_ACC_TRUNC);
  h5_write(file, "G", g);
  auto g2 = gf<imfreq>{};
  h5_read(file, "G", g2);
  EXPECT_EQ(g.mesh(), g2.mesh());
}

TEST(MeshGf, BosonicH5Read) {
  auto m = mesh::imfreq{1, Boson, 10, mesh::imfreq::option::positive_frequencies_only};
  h5::file file("bugTA2.h5", H5F_ACC_TRUNC);
  h5_write(file, "Gmesh", m);
  auto m2 = mesh::imfreq{};
  h5_read(file, "Gmesh", m2);
  //h5_read(file,"Gmesh", m2);
  EXPECT_EQ(m, m2);
}

TEST(MeshGf, BosonicH5ReadHalfOnly) {
  auto m = mesh::imfreq{1, Boson, 3, mesh::imfreq::option::positive_frequencies_only};
  h5::file file("bugTA2.h5", H5F_ACC_TRUNC);
  h5_write(file, "Gmesh", m);
  auto m2 = mesh::imfreq{};
  h5_read(file, "Gmesh", m2);
  EXPECT_EQ(m, m2);
}
TEST(MeshGf, FermionicH5Read) {
  auto m = mesh::imfreq{1, Fermion, 3};
  h5::file file("bugTA2.h5", H5F_ACC_TRUNC);
  h5_write(file, "Gmesh", m);
  auto m2 = mesh::imfreq{};
  h5_read(file, "Gmesh", m2);
  EXPECT_EQ(m, m2);
}
TEST(MeshGf, FermionicH5ReadHalfOnly) {
  auto m = mesh::imfreq{1, Fermion, 3, mesh::imfreq::option::positive_frequencies_only};
  h5::file file("bugTA2.h5", H5F_ACC_TRUNC);
  h5_write(file, "Gmesh", m);
  auto m2 = mesh::imfreq{};
  h5_read(file, "Gmesh", m2);
  EXPECT_EQ(m, m2);
}
MAKE_MAIN;
