#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs;
using namespace triqs::gfs;

int main() {
  // Create a real-time mesh
  double tmin = 0, tmax = 10; // the time interval
  int n_times = 101;          // number of time-points
  auto t_mesh = mesh::retime{tmin, tmax, n_times};

  // Create a scalar-valued Green function on the t_mesh
  auto g = gf<retime, scalar_valued>{t_mesh};
}
