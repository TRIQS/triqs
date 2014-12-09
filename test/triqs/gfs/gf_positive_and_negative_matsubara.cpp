//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/gfs.hpp> 
#include <triqs/gfs/local/functions.hpp> 
using namespace triqs::gfs;
#define TEST(X) std::cout << BOOST_PP_STRINGIZE((X)) << " ---> "<< (X) <<std::endl<<std::endl;

int main() {
 try { 
  double beta =1;int n_im_freq=400;
  auto G =  gf<imfreq, scalar_valued> {{beta, Fermion, n_im_freq,false}};
  auto G_pos_only =  gf<imfreq, scalar_valued> {{beta, Fermion, n_im_freq,true}};

  TEST(G.mesh().positive_only());
  TEST(G_pos_only.mesh().positive_only());
  //std::cout << G.singularity() << std::endl ;
  triqs::clef::placeholder<0> om_;
  G(om_) << 1/(om_ + 2.3);
  G_pos_only(om_) << 1/(om_ + 2.3);

  auto n = triqs::gfs::density(G);
  auto n_pos_only = triqs::gfs::density(G_pos_only);
  TEST(n);
  TEST(n_pos_only);

  // test hdf5 
  //h5::file file("gf_scalar.h5", H5F_ACC_TRUNC);
  //h5_write(file, "g", G);
  //h5_write(file, "gm", reinterpret_scalar_valued_gf_as_matrix_valued(G));

 }
TRIQS_CATCH_AND_ABORT;

}
