#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs::gfs;
using namespace triqs;
int main() {
  auto beta = 10.0;
  gf<imfreq> gf{{beta, Fermion}, {1, 1}};
}
