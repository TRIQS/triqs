#include <triqs/gfs.hpp>
using namespace triqs::gfs;
using triqs::clef::placeholder;
int main() {

  // Cf gf<imfreq> specialisation page for the constructor
  double beta = 10;
  int Nfreq   = 100;
  auto g      = gf<imfreq>{{beta, Fermion, Nfreq}, {1, 1}};

  // Filling the gf with something...
  placeholder<0> wn_;
  g(wn_) << 1 / (wn_ + 2);
  g(wn_) << 1 / (wn_ + 2 + g(wn_));
}
