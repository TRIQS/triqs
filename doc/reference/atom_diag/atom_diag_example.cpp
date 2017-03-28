#include <triqs/atom_diag/atom_diag.hpp>
#include <triqs/atom_diag/functions.hpp>
#include <triqs/atom_diag/gf.hpp>

using namespace triqs::gfs;
using namespace triqs::operators;
using namespace triqs::atom_diag;

int main(int argc, const char* argv[]) {

 // Hamiltonian: single band Hubbard atom + spin flips + anomalous terms
 auto h = 2.0 * (n("up",0) - 0.5) * (n("dn",0) - 0.5);
 h += 0.3 * (c_dag("up",0) * c("dn",0) + c_dag("dn",0) * c("up",0));
 h += 0.1 * (c_dag("up",0) * c_dag("dn",0) - c("up",0) * c("dn",0));

 // Set of fundamental operators C/C^+
 triqs::hilbert_space::fundamental_operator_set fops;
 fops.insert("up",0);
 fops.insert("dn",0);

 // Diagonalize the problem ('false' = no complex-valued matrix elements in h)
 auto ad = triqs::atom_diag::atom_diag<false>(h, fops);

 // Inverse temperature \beta = 10 and structure of Green's functions
 double beta = 10;
 gf_struct_t gf_struct = {{"dn", {0}}, {"up", {0}}};

 // Make block_gf<imtime>, block_gf<imfreq>, block_gf<legendre>
 // and block_gf<refreq> objects
 auto G_tau = atomic_g_tau(ad, beta, gf_struct, 200 /* n_tau */);
 auto G_iw  = atomic_g_iw(ad, beta, gf_struct, 100 /* n_iw */);
 auto G_l   = atomic_g_l(ad, beta, gf_struct, 20 /* n_l */);
 auto G_w   = atomic_g_w(ad, beta, gf_struct,
                         {-2.0, 2.0} /* energy window */,
                         200 /* n_w */,
                         0.01 /* broadening */);

 return 0;
}
