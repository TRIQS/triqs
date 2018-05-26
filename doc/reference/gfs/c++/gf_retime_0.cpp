#include <triqs/gfs.hpp>
using namespace triqs::gfs;

int main() {
  double tmin = 0, tmax = 10; // the time interval
  auto n_times = 100;         // we will have 100 points

  // we want a 2x2 matrix-valued Green function
  auto g = gf<retime>{{tmin, tmax, n_times}, {2, 2}};
};
