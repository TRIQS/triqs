#include <triqs/gfs.hpp>

using namespace triqs::gfs;
using namespace triqs;

triqs::clef::placeholder<0> iw_;
triqs::clef::placeholder<1> inu_;

int nw      = 3;
double beta = 20;

int main() {

  // Meshes
  auto iw_mesh = mesh::imfreq{beta, Fermion, nw};
  auto iw_inu_mesh = mesh::cartesian_product{iw_mesh, iw_mesh};

  // -- Product Gf
  auto Gp = gf{iw_inu_mesh, {1, 1}};
  auto Gp1 = gf{Gp};

  // Before commit 68b1d85e the following line was creating a copy
  // of the underlying storage which was immediately freed in the slicing process
  // This lead to Heap use after free (ASAN)
  Gp1(iw_, inu_) << Gp(iw_, inu_)(0, 0);
}
