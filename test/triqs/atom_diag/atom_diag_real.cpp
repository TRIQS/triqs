#include <triqs/test_tools/gfs.hpp>

#include <triqs/atom_diag/atom_diag.hpp>
#include <triqs/atom_diag/atom_diag_functions.hpp>
#include <triqs/arrays/blas_lapack/dot.hpp>
#include <triqs/h5.hpp>

#include "./hamiltonian.hpp"

using namespace triqs::arrays;
using namespace triqs::hilbert_space;

triqs::h5::file ref_file("atom_diag_real.ref.h5", H5F_ACC_RDONLY);

TEST(atom_diag_real, QuantumNumbers) {
 auto fops = make_fops();
 auto h = make_hamiltonian<many_body_operator_real>(.0, 3.0, 0.3, .0, .0);

 auto N_up = n("up",0) + n("up",1) + n("up",2);
 auto N_dn = n("dn",0) + n("dn",1) + n("dn",2);
 auto N = N_up + N_dn;

 // Partition with total number of particles
 auto ad1 = triqs::atom_diag::atom_diag<false>(h, fops, {N});

 vector<int> sp_dim_ref;
 vector<double> qn1_ref;
 h5_read(ref_file, "/QuantumNumbers/N/sp_dims", sp_dim_ref);
 h5_read(ref_file, "/QuantumNumbers/N/QN", qn1_ref);

 EXPECT_EQ(64,ad1.get_full_hilbert_space_dim());
 EXPECT_EQ(sp_dim_ref.size(),ad1.n_subspaces());

 auto qn1 = ad1.get_quantum_numbers();
 for(int sp : range(sp_dim_ref.size())) {
  EXPECT_EQ(sp_dim_ref[sp],ad1.get_subspace_dim(sp));
  EXPECT_NEAR(qn1_ref[sp],qn1[sp][0],1e-14);
 }

 // Partition with N_up and N_dn
 auto ad2 = triqs::atom_diag::atom_diag<false>(h, fops, {N_up, N_dn});

 h5_read(ref_file, "/QuantumNumbers/N_up_N_dn/sp_dims", sp_dim_ref);
 array<double,2> qn2_ref;
 h5_read(ref_file, "/QuantumNumbers/N_up_N_dn/QN", qn2_ref);

 EXPECT_EQ(64,ad2.get_full_hilbert_space_dim());
 EXPECT_EQ(sp_dim_ref.size(),ad2.n_subspaces());

 auto qn2 = ad2.get_quantum_numbers();
 for(int sp : range(sp_dim_ref.size())) {
  EXPECT_EQ(sp_dim_ref[sp],ad2.get_subspace_dim(sp));
  EXPECT_NEAR(qn2_ref(sp,0),qn2[sp][0],1e-14);
  EXPECT_NEAR(qn2_ref(sp,1),qn2[sp][1],1e-14);
 }
}

TEST(atom_diag_real, Vacuum) {
 auto h = 2.0 * (n("up") - 0.5) * (n("dn") - 0.5);
 h += 0.3 * (c_dag("up") * c("dn") + c_dag("dn") * c("up"));
 h += 0.1 * (c_dag("up") * c_dag("dn") - c("up") * c("dn"));

 fundamental_operator_set fops;
 fops.insert("up");
 fops.insert("dn");
 auto ad = triqs::atom_diag::atom_diag<false>(h, fops);

 EXPECT_EQ(1,ad.get_vacuum_subspace_index());
 auto vac = ad.get_vacuum_state();
 EXPECT_NEAR(1, dot(vac, vac), 1e-14);
 for(auto s : {"up","dn"})
  EXPECT_NEAR(0, dot(vac, act(n(s), vac, ad)), 1e-14);
}

TEST(atom_diag_real, Autopartition) {
 auto fops = make_fops();

 double mu = 0.4;
 double U = 1.0;
 double J = 0.3;
 double b = 0.03;
 double t = 0.2;
 auto h = make_hamiltonian<many_body_operator_real>(mu, U, J, b, t);

 auto ad = triqs::atom_diag::atom_diag<false>(h, fops);

 using triqs::hilbert_space::hilbert_space;
 EXPECT_EQ(fops, ad.get_fops());
 EXPECT_TRUE((ad.get_h_atomic() - h).is_zero());
 EXPECT_EQ(64,ad.get_full_hilbert_space_dim());

 vector<int> sp_dim_ref;
 h5_read(ref_file, "/Autopartition/sp_dims", sp_dim_ref);

 EXPECT_EQ(hilbert_space(fops), ad.get_full_hilbert_space());
 EXPECT_EQ(sp_dim_ref.size(),ad.n_subspaces());

 // Check Fock state lists
 std::vector<std::vector<fock_state_t>> fock_states_ref;
 h5_read(ref_file, "/Autopartition/fock_states", fock_states_ref);
 EXPECT_EQ(fock_states_ref, ad.get_fock_states());

 // Check eigensystems
 std::vector<vector<double>> energies_ref;
 std::vector<matrix<double>> umat_ref;
 h5_read(ref_file, "/Autopartition/energies", energies_ref);
 h5_read(ref_file, "/Autopartition/umat", umat_ref);

 auto energies = ad.get_energies();
 auto umat = ad.get_unitary_matrices();
 auto eigensystems = ad.get_eigensystems();

 int flat_index = 0;
 for(int sp : range(ad.n_subspaces())) {
  EXPECT_EQ(sp_dim_ref[sp], ad.get_subspace_dim(sp));

  auto const& eigs = eigensystems[sp];
  auto energies = ad.get_energies()[sp];
  EXPECT_ARRAY_NEAR(energies_ref[sp], eigs.eigenvalues);
  EXPECT_ARRAY_NEAR(umat[sp], eigs.unitary_matrix);
  EXPECT_ARRAY_NEAR(make_unit_matrix<double>(ad.get_subspace_dim(sp)),
                    umat[sp] * transpose(umat[sp]));

  EXPECT_EQ(flat_index, ad.index_range_of_subspace(sp).first());
  for(int i : range(ad.get_subspace_dim(sp))) {
   EXPECT_CLOSE(energies_ref[sp](i), energies[i]);
   EXPECT_CLOSE(energies_ref[sp](i), ad.get_eigenvalue(sp,i));
   EXPECT_CLOSE(1, std::abs(dot(umat_ref[sp](i,range()), umat[sp](i,range()))));
   EXPECT_EQ(flat_index, ad.flatten_subspace_index(sp,i));
   ++flat_index;
  }
  EXPECT_EQ(flat_index, ad.index_range_of_subspace(sp).last());

  // Check matrix elements of C and C^+
  std::vector<vector<long>> c_connections_ref, cdag_connections_ref;
  h5_read(ref_file, "/Autopartition/c_connections", c_connections_ref);
  h5_read(ref_file, "/Autopartition/cdag_connections", cdag_connections_ref);
  std::vector<std::vector<matrix<double>>> c_matrices_ref, cdag_matrices_ref;
  h5_read(ref_file, "/Autopartition/c_matrices", c_matrices_ref);
  h5_read(ref_file, "/Autopartition/cdag_matrices", cdag_matrices_ref);

  for(int op : range(fops.size())) {
   for(int sp : range(ad.n_subspaces())) {
    EXPECT_EQ(c_connections_ref[op][sp], ad.c_connection(op,sp));
    EXPECT_EQ(cdag_connections_ref[op][sp], ad.cdag_connection(op,sp));
    if(c_connections_ref[op][sp] != -1)
     EXPECT_ARRAY_NEAR(c_matrices_ref[op][sp], ad.c_matrix(op,sp));
    if(cdag_connections_ref[op][sp] != -1)
     EXPECT_ARRAY_NEAR(cdag_matrices_ref[op][sp], ad.cdag_matrix(op,sp));
   }
  }
 }
}
/*
TEST(atom_diag_real, Functions) {
 auto fops = make_fops();

 double mu = 0.4;
 double U = 1.0;
 double J = 0.3;
 double b = 0.03;
 double t = 0.2;
 double beta = 10;

 auto N_up = n("up",0) + n("up",1) + n("up",2);
 auto N_dn = n("dn",0) + n("dn",1) + n("dn",2);
 auto N = N_up + N_dn;

 auto h = make_hamiltonian<many_body_operator_real>(mu, U, J, b, t);

 auto ad = triqs::atom_diag::atom_diag<false>(h, fops);

 // Partition function
 double z_ref;
 h5_read(ref_file, "/Functions/Z", z_ref);
 EXPECT_CLOSE(z_ref, partition_function(ad, beta));

 // Density matrix
 std::vector<matrix<double>> rho_ref;
 h5_read(ref_file, "/Functions/rho", rho_ref);
 auto rho = atomic_density_matrix(ad, beta);
 for(int sp : range(ad.n_subspaces()))
  EXPECT_ARRAY_NEAR(rho_ref[sp], rho[sp]);

 // Static observables
 double static_obs_ref;
 h5_read(ref_file, "/Functions/N_up", static_obs_ref);
 EXPECT_CLOSE(static_obs_ref, trace_rho_op(rho, N_up, ad));
 h5_read(ref_file, "/Functions/N_dn", static_obs_ref);
 EXPECT_CLOSE(static_obs_ref, trace_rho_op(rho, N_dn, ad));
 h5_read(ref_file, "/Functions/N", static_obs_ref);
 EXPECT_CLOSE(static_obs_ref, trace_rho_op(rho, N, ad));
 h5_read(ref_file, "/Functions/N2", static_obs_ref);
 EXPECT_CLOSE(static_obs_ref, trace_rho_op(rho, N*N, ad));

 // Quantum numbers
 std::vector<vector<double>> qn_N_up_ref, qn_N_dn_ref;
 h5_read(ref_file, "/Functions/QN_N_up", qn_N_up_ref);
 h5_read(ref_file, "/Functions/QN_N_dn", qn_N_dn_ref);
 auto qn_N_up = quantum_number_eigenvalues(N_up, ad);
 auto qn_N_dn = quantum_number_eigenvalues(N_dn, ad);
 auto qn_checked_N_up = quantum_number_eigenvalues_checked(N_up, ad);
 auto qn_checked_N_dn = quantum_number_eigenvalues_checked(N_dn, ad);
 for(int sp : range(ad.n_subspaces()))
  for(int i : range(ad.get_subspace_dim(sp))) {
   EXPECT_CLOSE(qn_N_up_ref[sp](i), qn_N_up[sp][i]);
   EXPECT_CLOSE(qn_N_dn_ref[sp](i), qn_N_dn[sp][i]);
   EXPECT_CLOSE(qn_N_up_ref[sp](i), qn_checked_N_up[sp][i]);
   EXPECT_CLOSE(qn_N_dn_ref[sp](i), qn_checked_N_dn[sp][i]);
  }

 // act()
 auto vac = ad.get_vacuum_state();
 auto op = c_dag("dn",0) * c_dag("dn",1) * c_dag("dn",2) *
           c_dag("up",0) * c_dag("up",1) * c_dag("up",2);

 vector<double> st_ref(ad.get_full_hilbert_space_dim(),0);
 st_ref(63) = 1;
 auto st = act(op, vac, ad);
 EXPECT_ARRAY_NEAR(st_ref, st);

 // GF
 gf_struct_t gf_struct = {{"up",{0,1,2}},{"dn",{0,1,2}}};

 int n_tau, n_iw, n_l;
 h5_read(ref_file, "/Functions/n_tau", n_tau);
 h5_read(ref_file, "/Functions/n_iw", n_iw);
 h5_read(ref_file, "/Functions/n_l", n_l);
 auto g_tau = gf<imtime>({beta, Fermion, n_tau}, {3, 3});
 auto g_iw = gf<imfreq>({beta, Fermion, n_iw}, {3, 3});
 auto g_l = gf<legendre>({beta, Fermion, static_cast<size_t>(n_l)}, {3, 3});

 auto G_tau_ref = make_block_gf<imtime>({"dn","up"},g_tau), G_tau = G_tau_ref;
 auto G_iw_ref = make_block_gf<imfreq>({"dn","up"},g_iw), G_iw = G_iw_ref;
 auto G_l_ref = make_block_gf<legendre>({"dn","up"},g_l), G_l = G_l_ref;

 auto agf = atomic_gf(ad, beta, gf_struct, {{2,0},{2,2},{2,4},{3,1},{3,3}});

 h5_read(ref_file, "/Functions/G_tau", G_tau_ref);
 G_tau() = agf;
 EXPECT_BLOCK_GF_NEAR(G_tau_ref,G_tau);
 h5_read(ref_file, "/Functions/G_iw", G_iw_ref);
 G_iw() = agf;
 EXPECT_BLOCK_GF_NEAR(G_iw_ref,G_iw);
 h5_read(ref_file, "/Functions/G_l", G_l_ref);
 G_l() = agf;
 EXPECT_BLOCK_GF_NEAR(G_l_ref,G_l);
}*/

MAKE_MAIN;
