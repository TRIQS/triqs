#include <triqs/test_tools/gfs.hpp>

TEST(Gf, ArrayOf) { 

  triqs::clef::placeholder<0> w_;
  auto agf = array<gf<imfreq>, 2>{2, 3};
  auto bgf = array<gf<imfreq>, 2>{2, 3};
  agf() = gf<imfreq>{{10.0, Fermion}, {1, 1}};
  agf(0, 0)(w_) << 1 / (w_ + 2);

  // test H5
  { 
   h5::file file("ess_array_gf.h5", H5F_ACC_TRUNC);
   h5_write(file, "Agf", agf);
  }
  {
   h5::file file("ess_array_gf.h5", H5F_ACC_RDONLY);
   h5_read(file, "Agf", bgf);
  }
  {
   h5::file file("ess_array_gf2.h5", H5F_ACC_TRUNC);
   h5_write(file, "Agf", bgf);
  }

  for (int i = 0; i < 2; ++i)
   for (int j = 0; j < 2; ++j) EXPECT_GF_NEAR(agf(i, j), bgf(i, j));

 }
MAKE_MAIN;

