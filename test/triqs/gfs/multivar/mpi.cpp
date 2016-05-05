#include <triqs/test_tools/gfs.hpp>

using gf3_s = gf<cartesian_product<imfreq, imfreq, imfreq>, scalar_valued>;

TEST(Gfs, MPI_multivar) {

 mpi::communicator world;

 int nw = 2, nbw = 10;
 double beta = 10;
 clef::placeholder<0> k_;
 clef::placeholder<1> q_;
 clef::placeholder<2> r_;
 clef::placeholder<3> iw_;
 clef::placeholder<4> inu_;
 clef::placeholder<5> inup_;

 auto g = gf3_s{{{beta, Boson, nbw}, {beta, Fermion, nw}, {beta, Fermion, nw}}};
 g(iw_, inu_, inup_) << inu_ + 10 * inup_ + 100 * iw_;
 
 auto g2 = g;
 g2 = mpi_reduce(g, world);
 if (world.rank() == 0) EXPECT_ARRAY_NEAR(g2.data(), g.data() * world.size());

 mpi_broadcast(g2, world);
 if (world.rank() == 1) EXPECT_ARRAY_NEAR(g2.data(), g.data()* world.size());

 gf3_s g3 = mpi_all_reduce(g, world);
 EXPECT_ARRAY_NEAR(g3.data(), g.data() * world.size());

 gf3_s g4 = mpi_scatter(g);
 g2(iw_, inu_, inup_) << g2(iw_, inu_, inup_) * (1 + world.rank());
 g4 = mpi_gather(g2);
 // Test the result ?

 auto G = make_block_gf({g});
 auto g0 = gf<imfreq, scalar_valued>{{beta, Boson, nbw}};
 auto G2 = make_block_gf({g0});

 mpi_broadcast(G, world);
 mpi_broadcast(G2, world);
}
MAKE_MAIN;

