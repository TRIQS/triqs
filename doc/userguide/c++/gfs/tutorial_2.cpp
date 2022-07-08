#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs;
using namespace triqs::gfs;

int main() {
  // Number of time-points to be used
  int n_times = 101;

  // Create a real-time mesh on interval [tmin, tmax]
  double tmin = 0, tmax = 10;
  auto t_mesh = mesh::retime{tmin, tmax, n_times};

  // Create a imaginary-time mesh
  double beta   = 1;
  auto tau_mesh = mesh::imtime{beta, Fermion, n_times};

  // Create Green function g[t,tau] on product-mesh with values of shape (2,2,2)
  auto g = gf<prod<retime, imtime>, tensor_valued<3>>{t_mesh * tau_mesh, {2, 2, 2}};
}
