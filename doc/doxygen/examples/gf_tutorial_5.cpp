#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs;
using namespace triqs::gfs;

int main() {
  // Create a real-time mesh on the interval [tmin, tmax]
  double tmin = 0, tmax = 10;
  int n_times = 101; // number of time-points
  auto t_mesh = mesh::retime{tmin, tmax, n_times};

  // Create a scalar-valued Green function g[t1,t2] on the product-mesh and initialize
  auto g = gf<prod<retime, retime>, scalar_valued>{t_mesh * t_mesh};
  nda::clef::placeholder<0> t1_;
  nda::clef::placeholder<1> t2_;
  g[t1_, t2_] << 2 * t1_;
}
