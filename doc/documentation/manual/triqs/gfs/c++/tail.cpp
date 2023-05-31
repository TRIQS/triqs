#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>

using namespace triqs::gfs;
using namespace triqs;

int main() {

  nda::clef::placeholder<0> iw_;

  double beta  = 10;
  auto iw_mesh = mesh::imfreq{beta, Fermion, 100};

  auto G = gf<imfreq, scalar_valued>{iw_mesh, make_shape()};
  G(iw_) << 1.0 / iw_ + 2.0 / iw_ / iw_ + 3.0 / iw_ / iw_ / iw_;

  // We provide both the zeroth and first order coefficient to the fit
  auto known_moments = array<dcomplex, 1>{0.0, 1.0};
  auto [tail, err]   = fit_tail(G, known_moments);

  std::cout << "tail: " << tail << "\nerror: " << err;
}
