#include <triqs/atom_diag/atom_diag.hpp>
#include <triqs/atom_diag/atom_diag_functions.hpp>

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

 // Create atomic_block_gf object for \beta = 10
 double beta = 10;
 gf_struct_t gf_struct = {{"dn",{0}},{"up",{0}}};
 auto agf = atomic_gf(ad, beta, gf_struct);

 // Construct block_gf<imtime>, block_gf<imfreq> and block_gf<legendre> objects
 auto g_tau = gf<imtime>({beta, Fermion, 400}, {1, 1});
 auto g_iw = gf<imfreq>({beta, Fermion, 100}, {1, 1});
 auto g_l = gf<legendre>({beta, Fermion, 20}, {1, 1});

 auto G_tau = make_block_gf<imtime>({"dn","up"},g_tau);
 auto G_iw = make_block_gf<imfreq>({"dn","up"},g_iw);
 auto G_l = make_block_gf<legendre>({"dn","up"},g_l);

 // Fill the block GFs with data
 G_tau() = agf;
 G_iw() = agf;
 G_l() = agf;

 return 0;
}
