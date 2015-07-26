#include "./common.hpp"

TEST(Gf, NoTailWithTail) {
  double beta =1;
  auto G =  gf<imfreq, scalar_valued> {{beta, Fermion}};

  triqs::clef::placeholder<0> om_; 
  G(om_) << 1/(om_ + 2.3);

  auto Gt = make_gf_from_inverse_fourier(G);
}
MAKE_MAIN;
