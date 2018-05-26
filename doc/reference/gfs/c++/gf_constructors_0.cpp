#include <triqs/gfs.hpp>
using namespace triqs::gfs;
int main() {
  auto beta = 10.0;
  gf<imfreq> gf{{beta, Fermion}, {1, 1}};
}
