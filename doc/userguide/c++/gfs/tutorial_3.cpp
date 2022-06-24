#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs::gfs;
using namespace triqs::mesh;

int main() {
  double beta = 10;  // the time interval
  int n_times = 100; // we will have 100
  auto g      = gf<imtime, scalar_valued>{{beta, Fermion, n_times}};
}
