#include <triqs/gfs.hpp>
#include <triqs/mesh.hpp>
using namespace triqs::gfs;
using namespace triqs::mesh;
int main() {

  // Set the parameters
  double beta = 1, a = 1;
  int n_iw  = 1000;
  int n_tau = 6 * n_iw + 1;

  // Construct the meshes
  auto iw_mesh  = gf_mesh<imfreq>{beta, Fermion, n_iw};
  auto tau_mesh = make_adjoint_mesh(iw_mesh);

  // Construct the Green functions
  auto gw = gf<imfreq, scalar_valued>{iw_mesh, {}};
  auto gt = gf<imtime, scalar_valued>{tau_mesh, {}};

  // Initialization
  nda::clef::placeholder<0> om_;
  gw(om_) << 1 / (om_ - a);

  // Fill gt with the fourier transform of gw and provide the first two
  // orders of the high-frequency expansion of gw
  auto known_moments = array<dcomplex, 1>{0.0, 1.0};
  gt()               = fourier(gw, known_moments);

  // Construct a new Green function gw2 from the Fourier transform of gt
  auto gw2 = make_gf_from_fourier(gt, iw_mesh);
}
