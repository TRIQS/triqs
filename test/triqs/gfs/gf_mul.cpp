#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::gfs;

TEST(BlockGf, Operations) {

 double beta = 10.0;

 auto A = make_block_gf({"up", "down"}, {gf<imfreq>{{beta, Fermion}, {1, 1}}, gf<imfreq>{{beta, Fermion}, {1, 1}}});
 auto B = A;
 auto C = A;

 C = A + A * B;
 C() = A + A() * B();

 // TEST NOTHING 
}
MAKE_MAIN;
