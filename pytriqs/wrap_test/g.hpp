#include <triqs/gfs.hpp>
using namespace triqs::gfs;
using namespace triqs;

block_gf_view<imfreq> make_bgf(double a) {

 double beta = 1;
 auto G1 = gf<imfreq>({beta, Fermion}, {2, 2});

 auto B1 = make_block_gf<imfreq>(3, G1);

 {
  H5::H5File file("ess_test_g1.h5", H5F_ACC_TRUNC);
  h5_write(file, "g", B1);
 }

 return B1;
}

void pass_bgf(block_gf_view<imfreq> g) {

 {
  H5::H5File file("ess_test_g2.h5", H5F_ACC_TRUNC);
  h5_write(file, "g", g);
 }
}
