#include <triqs/test_tools/gfs.hpp>
using namespace triqs::gfs;
using namespace triqs::arrays;
namespace h5 = triqs::h5;

TEST(Gf, PlaceHolder) {

 double wmax = 10;
 int N = 10;
 auto G1 = gf<refreq>{{-wmax, wmax, N}, {1, 1}};
 auto G2 = gf<refreq>{{-wmax, wmax, N * 2}, {1, 1}};

 triqs::clef::placeholder<0> om_;

 G1(om_) << om_ + 0.1_j;
 G2(om_) << om_ + 0.1_j;

 auto g1 = G1;
 g1(om_) << g1(om_);
 EXPECT_GF_NEAR(G1,g1);
 
 // the placeholder is evaluated -> mesh_point_t -> domain::point_t -> evaluated ...
 G1(om_) << om_ + 0.1_j - G2(om_);
 EXPECT_ARRAY_ZERO(G1.data());

 auto g = G1;
 g(om_) << g(om_);
 EXPECT_GF_NEAR(G1,g);
}
MAKE_MAIN;
