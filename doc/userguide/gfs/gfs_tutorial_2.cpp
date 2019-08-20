#include <triqs/gfs.hpp>
using namespace triqs::gfs;
using namespace triqs::mesh;

int main() {
  double tmin = 0, tmax = 10; // the time interval
  double beta = 1;
  int n_times = 100; // we will have 100 pointspoints
  auto g      = gf<prod<retime, imtime>, tensor_valued<3>>{{{tmin, tmax, n_times}, {beta, Fermion, n_times}}, {2, 2, 2}};
}
