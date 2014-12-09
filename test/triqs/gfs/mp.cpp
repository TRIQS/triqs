#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include "./common.hpp"
using namespace triqs::gfs;
using namespace triqs::arrays;
namespace h5 = triqs::h5;

int main() {
 try {

  // double beta = 1;
  double wmax = 10;
  int N = 10;
  auto G1 = gf<refreq>{{-wmax, wmax, N}, {1, 1}};
  auto G2 = gf<refreq>{{-wmax, wmax, N*2}, {1, 1}};

  triqs::clef::placeholder<0> om_;

  G1(om_) << om_ + 0.1_j;
  G2(om_) << om_ + 0.1_j;

  //std::cerr  << G1.data() << std::endl;
  //std::cerr  << G2.data() << std::endl;
  //std::cerr  << G1._evaluator(&G1,G1.mesh()[0]) << std::endl;
  //std::cerr  << G1(G1.mesh()[0]) << std::endl;

  // the placeholder is evaluated -> mesh_point_t -> domain::point_t -> evaluated ...
  G1(om_) << om_ + 0.1_j - G2(om_);
  assert_zero_array(G1.data());
  
 }
 TRIQS_CATCH_AND_ABORT;
}
