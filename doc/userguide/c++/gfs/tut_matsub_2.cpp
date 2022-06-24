#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs::gfs;
using namespace triqs::mesh;
using nda::clef::placeholder;

int main() {

  double beta = 1;
  int nw      = 100;
  auto g2     = gf<prod<imfreq, imfreq>>{{{beta, Fermion, nw}, {beta, Fermion, nw}}, {1, 1}};

  //the shortest way to fill a gf
  placeholder<0> w_;
  placeholder<1> nu_;
  g2(w_, nu_) << 1 / (w_ + nu_ - 4);
  std::cout << g2(0, 0) << std::endl;

  g2() = 0.0;
  //equivalent way
  for (auto const &w : std::get<0>(g2.mesh().components()))
    for (auto const &nu : std::get<1>(g2.mesh().components())) g2[{w, nu}] = 1 / (w + nu - 4);
  std::cout << g2(0, 0) << std::endl;
}
