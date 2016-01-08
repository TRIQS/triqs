#include <triqs/test_tools/gfs.hpp>

TEST(Gf, Issue274) {
 double beta=1 ; int nbw= 5;
 triqs::mpi::communicator world;
 auto g0 = gf<imfreq, scalar_valued>{{beta, Boson, nbw}};
 auto G2 = make_block_gf<imfreq, scalar_valued>({g0});
 if(world.rank()==0) G2[0][0]=1;
 mpi_broadcast(G2, world);
 EXPECT_CLOSE(G2[0][0],1);
}
MAKE_MAIN;
