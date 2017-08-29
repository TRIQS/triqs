#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::gfs;

TEST(Gf, Poly) {

 long n_max = 24;
 double beta = 10.0;
 
 auto gpoly = gf<poly3, tensor_valued<4>>{{beta, Fermion, n_max}, {1, 1, 1, 1}};

 for( auto mp : gpoly.mesh() ) {
   gpoly[mp] = 1.2345;
 }

 std::string filename = "data_gf_poly.h5";
 
 triqs::h5::file out_file(filename, 'w');
 h5_write(out_file, "gpoly", gpoly);

 decltype(gpoly) gpoly_ref;
 triqs::h5::file in_file(filename, 'r');
 h5_read(in_file, "gpoly", gpoly_ref);

 EXPECT_GF_NEAR(gpoly, gpoly_ref);
 
}
MAKE_MAIN;
