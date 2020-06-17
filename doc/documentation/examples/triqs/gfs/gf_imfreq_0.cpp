#include <triqs/gfs.hpp>
using namespace triqs::gfs;
int main() {
  double beta = 10;
  int Nfreq   = 100;

  // --- first a matrix_valued function ------------

  // First give information to build the mesh, second to build the target
  auto g1 = gf<imfreq>{{beta, Fermion, Nfreq}, {1, 1}};
  // or a more verbose/explicit form ...
  auto g2 = gf<imfreq>{gf_mesh<imfreq>{beta, Fermion, Nfreq}, make_shape(1, 1)};

  // Filling the gf with something...
  triqs::clef::placeholder<0> wn_;
  g1(wn_) << 1 / (wn_ + 2);

  // evaluation at n=3
  std::cout << g1(3) << " == " << 1 / (1i * M_PI / beta * (2 * 3 + 1) + 2) << std::endl;
  // the high frequency expansion was automatically computed.
  // std::cout << g1.singularity() << std::endl; // a bit verbose..

  // --- a scalar_valued function ------------

  // same a before, but without the same of the target space ...
  auto g3 = gf<imfreq, scalar_valued>{{beta, Fermion, Nfreq}};
  auto g4 = gf<imfreq, scalar_valued>{gf_mesh<imfreq>{beta, Fermion, Nfreq}};

  g3(wn_) << 1 / (wn_ + 2);

  // evaluation at n=3.
  std::cout << g3(3) << " == " << 1 / (1i * std::acos(-1) / beta * (2 * 3 + 1) + 2) << std::endl;
}
