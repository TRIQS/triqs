#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs;
using namespace triqs::gfs;
using nda::clef::placeholder;

int main() {

  // Create a Matsubara-frequency mesh
  double beta  = 1;   // inverse temperature
  int n_iw     = 100; // number of Matsubara frequencies
  auto iw_mesh = mesh::imfreq{beta, Fermion, n_iw};

  // Create and fill a 1x1 Matsubara Green function on the product mesh
  auto g2 = gf{iw_mesh * iw_mesh, {1, 1}};

  //the shortest way to fill a gf
  placeholder<0> iw_;
  placeholder<1> inu_;
  g2[iw_, inu_] << 1 / (iw_ + inu_ - 4);
  std::cout << g2(0, 0) << std::endl;

  // An equivalent way to initialize
  g2() = 0.0;
  for (auto [w, nu] : g2.mesh()) g2[w, nu] = 1 / (w + nu - 4);
  std::cout << g2(0, 0) << std::endl;
}
