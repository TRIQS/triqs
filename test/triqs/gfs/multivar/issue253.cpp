#include <triqs/test_tools/gfs.hpp>

using namespace triqs::gfs;
using namespace triqs::lattice;

TEST(Gf, Issue253) {

 int nw=50;
 int nk=4;
 double beta=1.0;
 auto bz = brillouin_zone{bravais_lattice{{{1,0},{0,1}}}};

 auto G_r_tau = gf<cartesian_product<cyclic_lattice, imtime>, matrix_valued>{{ {nk, nk},{beta, Fermion, 2*nw+1}},{1,1}};
 auto expr_1 = curry<0>(G_r_tau);
 auto expr_2 = curry<1>(G_r_tau); //does not compile
}

MAKE_MAIN;
