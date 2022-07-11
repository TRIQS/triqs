#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs::gfs;
using namespace triqs::mesh;
int main() {
  double beta = 1;
  int n_pts   = 4;
  gf_mesh<imfreq> m(beta, Fermion, n_pts);
  std::cout << "Mesh m with positive and negative Matsubara frequencies : " << std::endl;
  for (auto const &w : m) std::cout << w << std::endl;
  std::cout << "m.first_idx() = " << m.first_idx() << std::endl;
  std::cout << "m.last_idx() = " << m.last_idx() << std::endl;
  std::cout << "m.size() = " << m.size() << std::endl;
  return 0;
}
