#include <triqs/gfs.hpp>
using namespace triqs::gfs;
int main() {
  double tmin = 0, tmax = 1.0;
  int nt = 100;
  auto g = gf<cartesian_product<retime, retime>, scalar_valued>{{{tmin, tmax, nt}, {tmin, tmax, nt}}};
}
