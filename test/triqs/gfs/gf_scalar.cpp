#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp> 
#include <triqs/gfs/functions/functions.hpp> 
using namespace triqs::gfs;
namespace h5 = triqs::h5;

#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

int main() {
 try { 
  double beta =1;
  auto G =  gf<imfreq, scalar_valued> {{beta, Fermion}};

  std::cout << G.singularity() << std::endl ;
  triqs::clef::placeholder<0> om_;
  G(om_) << 1/(om_ + 2.3);

  auto n = triqs::gfs::density(G);
  TEST(n);

  // test hdf5 
  h5::file file("gf_scalar.h5", H5F_ACC_TRUNC);
  h5_write(file, "g", G);
  h5_write(file, "gm", reinterpret_scalar_valued_gf_as_matrix_valued(G));

 }
TRIQS_CATCH_AND_ABORT;

}
