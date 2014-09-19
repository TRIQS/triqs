#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp>

namespace h5 = triqs::h5;
using namespace triqs::gfs;
using namespace triqs::clef;
using namespace triqs::arrays;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> " << (X) << std::endl << std::endl;

int main() {
 try {
  double beta = 1;

  auto G = gf<cartesian_product<imfreq, imfreq, imfreq>, tensor_valued<4>>{
      {{beta, Boson, 3}, {beta, Fermion, 10}, {beta, Fermion, 10}}, {2, 2, 2, 2}};

  // try to assign to expression 
  placeholder<2> om_;
  placeholder<0> nu_;
  placeholder<1> nup_;

  G(om_, nu_, nup_) << 1 / (nu_ + nup_ + om_ + 1);

  for (auto w : G.mesh())
   std::cerr  << G[w] <<std::endl;

  // hdf5 
  h5::file file("ess_g_om_nu_nup.h5", H5F_ACC_TRUNC );
  h5_write(file, "g", G);

  // Curry
  //auto g_cur = curry<0>(G);

  auto g0 = partial_eval<0>(G,0);
  //auto gg = g_cur[0];

  //h5_write(file, "g[0]c", gg);
  h5_write(file, "g[0]", g0);

  }
 TRIQS_CATCH_AND_ABORT;
}
