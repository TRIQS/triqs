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

  auto G = gf<cartesian_product<imfreq, imfreq>, tensor_valued<4>>{{{beta, Fermion, 100}, {beta, Fermion, 100}}, {2, 2, 2, 2},
                                                                   make_memory_layout(0, 2, 3, 1, 4, 5)};

  // try to assign to expression 
  placeholder<0> nu_;
  placeholder<1> nup_;

  G(nu_, nup_) << 1 / (nu_ + nup_ + 1);

  //for (auto w : G.mesh())
  // std::cerr  << G[w](0,0,0,0) <<std::endl;

  // Check print the memory_layout
  std::cerr << G.data().indexmap().get_memory_layout()<<std::endl;

  // inverse 
  //auto Ginv = inverse(G);

  // hdf5 
  h5::file file("ess_g_nu_nup.h5", H5F_ACC_TRUNC );
  h5_write(file, "g", G);
 

 }
 TRIQS_CATCH_AND_ABORT;
}
