#include <triqs/gfs.hpp>

using namespace triqs::gfs;
using triqs::clef::placeholder;

int main() {
  double beta = 100.0;
  int n_iw    = 1025;
  double e    = -1.25;

  auto G_iw = gf<imfreq>{{beta, Fermion, n_iw}, {1, 1}};

  triqs::clef::placeholder<0> om_;
  G_iw(om_) << om_ - e;
  G_iw = inverse(G_iw);

  G_iw = make_real_in_tau(G_iw);
}
