#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs::gfs;
using namespace triqs;

int main() {
  double wmin = 0, wmax = 10; // Frequencies interval
  auto n_freq = 100;          // Number of points

  // We want a 2x2 matrix valued function on this mesh...
  auto g = gf<refreq>{{wmin, wmax, n_freq}, {2, 2}};
};
