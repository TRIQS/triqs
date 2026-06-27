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

  // Create and fill a 1x1 Matsubara Green function
  auto g = gf{iw_mesh, {1, 1}};
  placeholder<0> iw_;
  g[iw_] << 1 / (iw_ - 3);
  std::cout << g(0) << std::endl;

  // An equivalent way to initialize
  g() = 0.0;
  for (auto w : g.mesh()) g[w] = 1 / (w - 3);
  std::cout << g(0) << std::endl;

  //an incorrect way : throws exception as expected
  //g(w) returns a const_view: () are to be used for interpolation, see bottom of the page
  //for (auto w : g.mesh())  g(w) = 1/(w-3);
}
