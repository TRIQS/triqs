
#include <triqs/gfs.hpp>
using namespace triqs::gfs;
int main() {
  double beta = 1.;
  double a = 1;
  int N = 10000;
  auto gw = gf<imfreq, scalar_valued>{{beta, Fermion, N/2}};
  auto gt = gf<imtime, scalar_valued>{{beta, Fermion, N+1}};
  triqs::clef::placeholder<0> om_;
  gw(om_) << 1 / (om_ - a);
  // fills a full *view* of gt with the contents of the FFT
  gt() = inverse_fourier(gw);
  // make a new fresh gf from the FFT of gt with a mesh of size N/2
  auto gw2 = make_gf_from_fourier(gt, N/2);
}
