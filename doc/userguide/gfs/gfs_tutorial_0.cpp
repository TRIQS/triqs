#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs::gfs;
using namespace triqs::mesh;

int main() {
  double tmin = 0, tmax = 10; // the time interval
  int n_times = 100;          // we will have 100 points
  auto g      = gf<retime, scalar_valued>{{tmin, tmax, n_times}};
}
