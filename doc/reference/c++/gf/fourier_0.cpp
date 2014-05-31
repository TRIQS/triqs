#include <triqs/gfs.hpp>
using namespace triqs::gfs;
int main() {
 double beta = 1, a = 1;
 int N = 10000;
 auto gw = gf<imfreq>{{beta, Fermion, N}, {1, 1}};
 auto gt = gf<imtime>{{beta, Fermion, N}, {1, 1}};

 triqs::clef::placeholder<0> om_;
 gw(om_) << 1 / (om_ - a);

 // fills a full *view* of gt with the contents of the FFT
 // NB : the mesh of gt *must* have the same size as the mesh of gw.
 gt() = inverse_fourier(gw);

 // make a new fresh gf, with the same size mesh, from the FFT of gt
 auto gw2 = make_gf_from_fourier(gt);
}

