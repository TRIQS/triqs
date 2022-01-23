// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2017 Igor Krivenko
// Copyright (c) 2018-2020 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Igor Krivenko, Nils Wentzell

#include <triqs/test_tools/gfs.hpp>

#include <triqs/atom_diag/atom_diag.hpp>
#include <triqs/atom_diag/functions.hpp>
#include <triqs/atom_diag/gf.hpp>
//#include <triqs/arrays/blas_lapack/dot.hpp>

#include <h5/h5.hpp>
#include <h5/serialization.hpp>

#include "./hamiltonian.hpp"

using namespace triqs::arrays;
using namespace triqs::hilbert_space;
using namespace triqs::atom_diag;

using namespace h5;

using atom_diag_cplx = triqs::atom_diag::atom_diag<true>;
using nda::blas::dotc;

//#define GENERATE_REF_H5
#ifdef GENERATE_REF_H5
h5::file out_file("atom_diag_complex.h5", 'w');
#else
h5::file ref_file("atom_diag_complex.ref.h5", 'r');
#endif

TEST(atom_diag_complex, QuantumNumbers) {
  auto fops = make_fops();
  auto h    = make_hamiltonian<many_body_operator_complex>(.0, 3.0, 0.3, .0, .0);

  auto N_up = n("up", 0) + n("up", 1) + n("up", 2);
  auto N_dn = n("dn", 0) + n("dn", 1) + n("dn", 2);
  auto N    = N_up + N_dn;

  // Partition with total number of particles
  auto ad1 = atom_diag_cplx(h, fops, {N});

#ifndef GENERATE_REF_H5
  vector<int> sp_dim_ref;
  std::vector<vector<double>> qn1_ref;
  h5_read(ref_file, "/QuantumNumbers/N/sp_dims", sp_dim_ref);
  h5_read(ref_file, "/QuantumNumbers/N/QN", qn1_ref);

  EXPECT_EQ(64, ad1.get_full_hilbert_space_dim());
  EXPECT_EQ(sp_dim_ref.size(), ad1.n_subspaces());
#endif

  auto sp_dim1 = ad1.get_subspace_dims();
  auto qn1     = ad1.get_quantum_numbers();
  for (int sp : range(sp_dim1.size())) {
#ifndef GENERATE_REF_H5
    EXPECT_EQ(sp_dim_ref[sp], sp_dim1[sp]);
    EXPECT_NEAR(qn1_ref[sp][0], qn1[sp][0], 1e-14);
#endif
  }

  // Partition with N_up and N_dn
  auto ad2 = atom_diag_cplx(h, fops, {N_up, N_dn});

#ifndef GENERATE_REF_H5
  h5_read(ref_file, "/QuantumNumbers/N_up_N_dn/sp_dims", sp_dim_ref);
  std::vector<vector<double>> qn2_ref;
  h5_read(ref_file, "/QuantumNumbers/N_up_N_dn/QN", qn2_ref);

  EXPECT_EQ(64, ad2.get_full_hilbert_space_dim());
  EXPECT_EQ(sp_dim_ref.size(), ad2.n_subspaces());
#endif

  auto sp_dim2 = ad2.get_subspace_dims();
  auto qn2     = ad2.get_quantum_numbers();
  for (int sp : range(sp_dim2.size())) {
#ifndef GENERATE_REF_H5
    EXPECT_EQ(sp_dim_ref[sp], sp_dim2[sp]);
    EXPECT_NEAR(qn2_ref[sp][0], qn2[sp][0], 1e-14);
    EXPECT_NEAR(qn2_ref[sp][1], qn2[sp][1], 1e-14);
#endif
  }

#ifdef GENERATE_REF_H5
  auto gr = h5::group(out_file).create_group("QuantumNumbers");
  gr.create_group("N");
  h5_write(out_file, "/QuantumNumbers/N/sp_dims", sp_dim1);
  h5_write(out_file, "/QuantumNumbers/N/QN", qn1);
  gr.create_group("N_up_N_dn");
  h5_write(out_file, "/QuantumNumbers/N_up_N_dn/sp_dims", sp_dim2);
  h5_write(out_file, "/QuantumNumbers/N_up_N_dn/QN", qn2);
#endif
}

TEST(atom_diag_complex, Vacuum) {
  auto h = 2.0 * (n("up") - 0.5) * (n("dn") - 0.5);
  h += 0.3 * (c_dag("up") * c("dn") + c_dag("dn") * c("up"));
  h += 0.1i * (c_dag("up") * c_dag("dn") + c("up") * c("dn"));

  fundamental_operator_set fops;
  fops.insert("up");
  fops.insert("dn");
  auto ad = atom_diag_cplx(h, fops);

  EXPECT_EQ(1, ad.get_vacuum_subspace_index());
  auto vac = ad.get_vacuum_state();
  EXPECT_COMPLEX_NEAR(1, dotc(vac, vac), 1e-14);
  for (auto s : {"up", "dn"}) EXPECT_COMPLEX_NEAR(0, dotc(vac, act(n(s), vac, ad)), 1e-14);
}

TEST(atom_diag_complex, Serialization) {
  auto h = 2.0 * (n("up") - 0.5) * (n("dn") - 0.5);
  h += 0.3 * (c_dag("up") * c("dn") + c_dag("dn") * c("up"));
  h += 0.1i * (c_dag("up") * c_dag("dn") + c("up") * c("dn"));

  fundamental_operator_set fops;
  fops.insert("up");
  fops.insert("dn");
  auto ad = atom_diag_cplx(h, fops);

  auto ad_ser = deserialize<atom_diag_cplx>(serialize(ad));
#if __cplusplus > 201703L
  EXPECT_EQ(ad, ad_ser);
#endif
}

TEST(atom_diag_complex, Autopartition) {
  auto fops = make_fops();

  dcomplex mu = 0.4;
  dcomplex U  = 1.0;
  dcomplex J  = 0.3;
  dcomplex b  = 0.03;
  dcomplex t  = 0.2i;
  auto h      = make_hamiltonian<many_body_operator_complex>(mu, U, J, b, t);

  auto ad = atom_diag_cplx(h, fops);

  using triqs::hilbert_space::hilbert_space;
#ifndef GENERATE_REF_H5
  EXPECT_EQ(fops, ad.get_fops());
  EXPECT_TRUE((ad.get_h_atomic() - h).is_zero());
  EXPECT_EQ(64, ad.get_full_hilbert_space_dim());

  vector<int> sp_dim_ref;
  h5_read(ref_file, "/Autopartition/sp_dims", sp_dim_ref);

  EXPECT_EQ(hilbert_space(fops), ad.get_full_hilbert_space());
  EXPECT_EQ(sp_dim_ref.size(), ad.n_subspaces());
#endif

  // Check Fock state lists
  auto fock_states = ad.get_fock_states();

#ifndef GENERATE_REF_H5
  std::vector<std::vector<fock_state_t>> fock_states_ref;
  h5_read(ref_file, "/Autopartition/fock_states", fock_states_ref);
  EXPECT_EQ(fock_states_ref, ad.get_fock_states());
#endif

  // Check eigensystems
#ifndef GENERATE_REF_H5
  std::vector<vector<double>> energies_ref;
  std::vector<matrix<dcomplex>> umat_ref;
  h5_read(ref_file, "/Autopartition/energies", energies_ref);
  h5_read(ref_file, "/Autopartition/umat", umat_ref);
#endif

  auto energies     = ad.get_energies();
  auto umat         = ad.get_unitary_matrices();
  auto eigensystems = ad.get_eigensystems();

  int flat_index = 0;
  vector<int> sp_dim(ad.n_subspaces());
  for (int sp : range(ad.n_subspaces())) {
    sp_dim[sp] = ad.get_subspace_dim(sp);
#ifndef GENERATE_REF_H5
    EXPECT_EQ(sp_dim_ref[sp], sp_dim[sp]);
#endif

    auto const &eigs = eigensystems[sp];
    auto const &en   = energies[sp];
#ifndef GENERATE_REF_H5
    EXPECT_ARRAY_NEAR(energies_ref[sp], eigs.eigenvalues);
    EXPECT_ARRAY_NEAR(umat[sp], eigs.unitary_matrix);
    EXPECT_ARRAY_NEAR(nda::eye<dcomplex>(sp_dim[sp]), umat[sp] * dagger(umat[sp]));

    EXPECT_EQ(flat_index, ad.index_range_of_subspace(sp).first());
    matrix<dcomplex> e_diag(sp_dim[sp], sp_dim[sp]);
    e_diag() = 0;
    for (int i : range(sp_dim[sp])) {
      e_diag(i, i) = en[i];
      EXPECT_CLOSE(energies_ref[sp](i), en[i]);
      EXPECT_CLOSE(energies_ref[sp](i), ad.get_eigenvalue(sp, i));
      EXPECT_EQ(flat_index, ad.flatten_subspace_index(sp, i));
      ++flat_index;
    }
    EXPECT_ARRAY_NEAR(umat_ref[sp] * e_diag * dagger(umat_ref[sp]), umat[sp] * e_diag * dagger(umat[sp]));

    EXPECT_EQ(flat_index, ad.index_range_of_subspace(sp).last());
#endif
  }

  // Check matrix elements of C and C^+

#ifndef GENERATE_REF_H5
  std::vector<vector<long>> c_connections_ref, cdag_connections_ref;
  h5_read(ref_file, "/Autopartition/c_connections", c_connections_ref);
  h5_read(ref_file, "/Autopartition/cdag_connections", cdag_connections_ref);
  std::vector<std::vector<matrix<dcomplex>>> c_matrices_ref, cdag_matrices_ref;
  h5_read(ref_file, "/Autopartition/c_matrices", c_matrices_ref);
  h5_read(ref_file, "/Autopartition/cdag_matrices", cdag_matrices_ref);
#endif

  std::vector<vector<long>> c_connections(fops.size(), vector<long>(ad.n_subspaces()));
  std::vector<vector<long>> cdag_connections(fops.size(), vector<long>(ad.n_subspaces()));
  std::vector<std::vector<matrix<dcomplex>>> c_matrices(fops.size(), std::vector<matrix<dcomplex>>(ad.n_subspaces()));
  std::vector<std::vector<matrix<dcomplex>>> cdag_matrices(fops.size(), std::vector<matrix<dcomplex>>(ad.n_subspaces()));
  for (int op : range(fops.size())) {
    for (int sp : range(ad.n_subspaces())) {
      c_connections[op][sp]    = ad.c_connection(op, sp);
      cdag_connections[op][sp] = ad.cdag_connection(op, sp);
#ifndef GENERATE_REF_H5
      EXPECT_EQ(c_connections_ref[op][sp], c_connections[op][sp]);
      EXPECT_EQ(cdag_connections_ref[op][sp], cdag_connections[op][sp]);
#endif
      long c_conn = c_connections[op][sp];
      if (c_conn != -1) {
        c_matrices[op][sp] = ad.c_matrix(op, sp);
#ifndef GENERATE_REF_H5
        EXPECT_ARRAY_NEAR(umat_ref[c_conn] * c_matrices_ref[op][sp] * dagger(umat_ref[sp]), umat[c_conn] * c_matrices[op][sp] * dagger(umat[sp]));
#endif
      }
      long cdag_conn = cdag_connections[op][sp];
      if (cdag_conn != -1) {
        cdag_matrices[op][sp] = ad.cdag_matrix(op, sp);
#ifndef GENERATE_REF_H5
        EXPECT_ARRAY_NEAR(umat_ref[cdag_conn] * cdag_matrices_ref[op][sp] * dagger(umat_ref[sp]),
                          umat[cdag_conn] * cdag_matrices[op][sp] * dagger(umat[sp]));
#endif
      }
    }
  }

#ifdef GENERATE_REF_H5
  h5::group(out_file).create_group("Autopartition");
  h5_write(out_file, "/Autopartition/sp_dims", sp_dim);
  h5_write(out_file, "/Autopartition/fock_states", fock_states);
  h5_write(out_file, "/Autopartition/energies", energies);
  h5_write(out_file, "/Autopartition/umat", umat);
  h5_write(out_file, "/Autopartition/c_connections", c_connections);
  h5_write(out_file, "/Autopartition/cdag_connections", cdag_connections);
  h5_write(out_file, "/Autopartition/c_matrices", c_matrices);
  h5_write(out_file, "/Autopartition/cdag_matrices", cdag_matrices);
#endif
}

TEST(atom_diag_complex, Functions) {
  auto fops = make_fops();

  dcomplex mu = 0.4;
  dcomplex U  = 1.0;
  dcomplex J  = 0.3;
  dcomplex b  = 0.03;
  dcomplex t  = 0.2i;
  double beta = 10;

  auto N_up = n("up", 0) + n("up", 1) + n("up", 2);
  auto N_dn = n("dn", 0) + n("dn", 1) + n("dn", 2);
  auto N    = N_up + N_dn;

  auto h = make_hamiltonian<many_body_operator_complex>(mu, U, J, b, t);

  auto ad = atom_diag_cplx(h, fops);

  // Partition function
  double z = partition_function(ad, beta);
#ifndef GENERATE_REF_H5
  double z_ref;
  h5_read(ref_file, "/Functions/Z", z_ref);
  EXPECT_CLOSE(z_ref, z);
#endif

  // Density matrix
  auto rho = atomic_density_matrix(ad, beta);
#ifndef GENERATE_REF_H5
  std::vector<matrix<dcomplex>> rho_ref;
  h5_read(ref_file, "/Functions/rho", rho_ref);
  for (int sp : range(ad.n_subspaces())) EXPECT_ARRAY_NEAR(rho_ref[sp], rho[sp]);
#endif

  // Static observables
  dcomplex N_up_obs = trace_rho_op(rho, N_up, ad);
  dcomplex N_dn_obs = trace_rho_op(rho, N_dn, ad);
  dcomplex N_obs    = trace_rho_op(rho, N, ad);
  dcomplex N2_obs   = trace_rho_op(rho, N * N, ad);
#ifndef GENERATE_REF_H5
  dcomplex static_obs_ref;
  h5_read(ref_file, "/Functions/N_up", static_obs_ref);
  EXPECT_CLOSE(static_obs_ref, N_up_obs);
  h5_read(ref_file, "/Functions/N_dn", static_obs_ref);
  EXPECT_CLOSE(static_obs_ref, N_dn_obs);
  h5_read(ref_file, "/Functions/N", static_obs_ref);
  EXPECT_CLOSE(static_obs_ref, N_obs);
  h5_read(ref_file, "/Functions/N2", static_obs_ref);
  EXPECT_CLOSE(static_obs_ref, N2_obs);
#endif

  // Quantum numbers
  auto qn_N_up         = quantum_number_eigenvalues(N_up, ad);
  auto qn_N_dn         = quantum_number_eigenvalues(N_dn, ad);
  auto qn_checked_N_up = quantum_number_eigenvalues_checked(N_up, ad);
  auto qn_checked_N_dn = quantum_number_eigenvalues_checked(N_dn, ad);
#ifndef GENERATE_REF_H5
  std::vector<vector<double>> qn_N_up_ref, qn_N_dn_ref;
  h5_read(ref_file, "/Functions/QN_N_up", qn_N_up_ref);
  h5_read(ref_file, "/Functions/QN_N_dn", qn_N_dn_ref);
  for (int sp : range(ad.n_subspaces()))
    for (int i : range(ad.get_subspace_dim(sp))) {
      EXPECT_CLOSE(qn_N_up_ref[sp](i), qn_N_up[sp][i]);
      EXPECT_CLOSE(qn_N_dn_ref[sp](i), qn_N_dn[sp][i]);
      EXPECT_CLOSE(qn_N_up_ref[sp](i), qn_checked_N_up[sp][i]);
      EXPECT_CLOSE(qn_N_dn_ref[sp](i), qn_checked_N_dn[sp][i]);
    }
#endif

  // act()
  auto vac = ad.get_vacuum_state();
  auto op  = c_dag("dn", 0) * c_dag("dn", 1) * c_dag("dn", 2) * c_dag("up", 0) * c_dag("up", 1) * c_dag("up", 2);

  vector<dcomplex> st_ref(nda::zeros<dcomplex>(ad.get_full_hilbert_space_dim()));
  st_ref(63) = 1;
  auto st    = act(op, vac, ad);
  EXPECT_ARRAY_NEAR(st_ref, st);

  // GF
  gf_struct_t gf_struct = {{"dn", 3}, {"up", 3}};

  int n_tau   = 1000;
  size_t n_iw = 400;
  int n_l     = 25;
  int n_w     = 1000;

  triqs::atom_diag::excluded_states_t excluded_states = {{1, 0}, {1, 1}, {3, 0}, {3, 1}, {3, 2}, {3, 3}};
  auto G_tau                                          = atomic_g_tau(ad, beta, gf_struct, n_tau, excluded_states);
  auto G_iw                                           = atomic_g_iw(ad, beta, gf_struct, n_iw, excluded_states);
  auto G_l                                            = atomic_g_l(ad, beta, gf_struct, n_l, excluded_states);
  auto G_w                                            = atomic_g_w(ad, beta, gf_struct, {-2.0, 2.0}, n_w, 0.01, excluded_states);

  auto lehmann = atomic_g_lehmann(ad, beta, gf_struct, excluded_states);

#ifndef GENERATE_REF_H5
  auto G_tau_ref = G_tau;
  h5_read(ref_file, "/Functions/G_tau", G_tau_ref);
  EXPECT_BLOCK_GF_NEAR(G_tau_ref, G_tau);
  auto G_iw_ref = G_iw;
  h5_read(ref_file, "/Functions/G_iw", G_iw_ref);
  EXPECT_BLOCK_GF_NEAR(G_iw_ref, G_iw);
  auto G_l_ref = G_l;
  h5_read(ref_file, "/Functions/G_l", G_l_ref);
  EXPECT_BLOCK_GF_NEAR(G_l_ref, G_l);
  auto G_w_ref = G_w;
  h5_read(ref_file, "/Functions/G_w", G_w_ref);
  EXPECT_BLOCK_GF_NEAR(G_w_ref, G_w);

  // Check indirect GF calculation via Lehmann representation
  auto G_tau_ind = atomic_g_tau<true>(lehmann, gf_struct, {beta, Fermion, n_tau});
  EXPECT_BLOCK_GF_NEAR(G_tau_ref, G_tau_ind);
  auto G_iw_ind = atomic_g_iw<true>(lehmann, gf_struct, {beta, Fermion, n_iw});
  EXPECT_BLOCK_GF_NEAR(G_iw_ref, G_iw_ind);
  auto G_l_ind = atomic_g_l<true>(lehmann, gf_struct, {beta, Fermion, n_l});
  EXPECT_BLOCK_GF_NEAR(G_l_ref, G_l_ind);
  auto G_w_ind = atomic_g_w<true>(lehmann, gf_struct, {-2.0, 2.0, n_w}, 0.01);
  EXPECT_BLOCK_GF_NEAR(G_w_ref, G_w_ind);
#endif

#ifdef GENERATE_REF_H5
  h5::group(out_file).create_group("Functions");
  h5_write(out_file, "/Functions/Z", z);
  h5_write(out_file, "/Functions/rho", rho);
  h5_write(out_file, "/Functions/N_up", N_up_obs);
  h5_write(out_file, "/Functions/N_dn", N_dn_obs);
  h5_write(out_file, "/Functions/N", N_obs);
  h5_write(out_file, "/Functions/N2", N2_obs);
  h5_write(out_file, "/Functions/QN_N_up", qn_N_up);
  h5_write(out_file, "/Functions/QN_N_dn", qn_N_dn);
  h5_write(out_file, "/Functions/G_tau", G_tau);
  h5_write(out_file, "/Functions/G_iw", G_iw);
  h5_write(out_file, "/Functions/G_l", G_l);
  h5_write(out_file, "/Functions/G_w", G_w);
#endif
}

MAKE_MAIN;
