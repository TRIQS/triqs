#include <triqs/atom_diag/atom_diag.hpp>
#include <triqs/atom_diag/functions.hpp>
#include <triqs/atom_diag/gf.hpp>

using namespace triqs::gfs;
using namespace triqs::mesh;
using namespace triqs::operators;
using namespace triqs::atom_diag;

int main(int argc, const char *argv[]) {

  // Hamiltonian: single band Hubbard atom + spin flips + anomalous terms
  auto h = 2.0 * (n("up", 0) - 0.5) * (n("dn", 0) - 0.5);
  h += 0.3 * (c_dag("up", 0) * c("dn", 0) + c_dag("dn", 0) * c("up", 0));
  h += 0.1 * (c_dag("up", 0) * c_dag("dn", 0) - c("up", 0) * c("dn", 0));

  // Set of fundamental operators C/C^+
  triqs::hilbert_space::fundamental_operator_set fops;
  fops.insert("up", 0);
  fops.insert("dn", 0);

  // Diagonalize the problem ('false' = no complex-valued matrix elements in h)
  auto ad = triqs::atom_diag::atom_diag<false>(h, fops);

  // Inverse temperature \beta = 10 and structure of Green's functions
  double beta           = 10;
  gf_struct_t gf_struct = {{"dn", {0}}, {"up", {0}}};

  // Direct computation of atomic Green's functions in
  // diferent representations

  int n_tau        = 200;
  size_t n_iw         = 100;
  unsigned int n_l = 20;
  int n_w          = 200;

  // Imaginary time
  auto G_tau = atomic_g_tau(ad, beta, gf_struct, n_tau);
  // Matsubara frequencies
  auto G_iw = atomic_g_iw(ad, beta, gf_struct, n_iw);
  // Legendre polynomial basis
  auto G_l = atomic_g_l(ad, beta, gf_struct, n_l);
  // Real frequencies
  auto G_w = atomic_g_w(ad, beta, gf_struct, {-2.0, 2.0} /* energy window */, n_w, 0.01 /* broadening */);

  // Indirect computation of atomic Green's functions via
  // Lehmann representation

  auto lehmann = atomic_g_lehmann(ad, beta, gf_struct);

  // Lehmann representation object can be reused but it requires
  // extra memory to store
  auto G_tau_ind = atomic_g_tau<false>(lehmann, gf_struct, {beta, Fermion, n_tau});
  auto G_iw_ind  = atomic_g_iw<false>(lehmann, gf_struct, {beta, Fermion, n_iw});
  auto G_l_ind   = atomic_g_l<false>(lehmann, gf_struct, {beta, Fermion, n_l});
  auto G_w_ind   = atomic_g_w<false>(lehmann, gf_struct, {-2.0, 2.0, n_w}, 0.01);

  return 0;
}
