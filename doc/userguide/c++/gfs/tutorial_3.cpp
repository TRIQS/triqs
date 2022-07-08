#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs;
using namespace triqs::gfs;

int main() {
  // Create a imaginary-time mesh
  double beta   = 1;
  int n_times   = 101; // number of time-points
  auto tau_mesh = mesh::imtime{beta, Fermion, n_times};

  // Create a scalar-valued Green function g[tau] on the tau_mesh
  auto g = gf<imtime, scalar_valued>{tau_mesh};
}
