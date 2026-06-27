#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs;
using namespace triqs::gfs;

int main() {
  // Create a real-time mesh on interval [tmin, tmax]
  double tmin = 0, tmax = 10;
  int n_times = 101; // number of time-points
  auto t_mesh = mesh::retime{tmin, tmax, n_times};

  // Create a scalar-valued Green function on the t_mesh
  auto g = gf<retime, scalar_valued>{t_mesh};

  // Create a nxm matrix-valued Green function on the t_mesh
  const int n = 2, m = 2;
  auto gm = gf<retime, matrix_valued>{t_mesh, {n, m}};
}
