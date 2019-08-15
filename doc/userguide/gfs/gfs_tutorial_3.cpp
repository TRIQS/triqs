#include <triqs/gfs.hpp>
using namespace triqs::gfs;

int main() {
  double beta = 10;  // the time interval
  int n_times = 100; // we will have 100
  auto g      = gf<imtime, scalar_valued>{{beta, Fermion, n_times}};
}
