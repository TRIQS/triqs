// Copyright (c) 2020-2021 Simons Foundation
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
// Authors: Maxime Charlebois, Nils Wentzell

#include <triqs/test_tools/gfs.hpp>

#include <triqs/atom_diag/atom_diag.hpp>
#include <triqs/atom_diag/functions.hpp>
#include <triqs/atom_diag/gf.hpp>

#include <nda/blas/dot.hpp>

#include <h5/h5.hpp>
#include <h5/serialization.hpp>

#include "./hamiltonian.hpp"

using namespace triqs::arrays;
using namespace triqs::hilbert_space;
using namespace triqs::atom_diag;

using namespace h5;

using atom_diag_real = triqs::atom_diag::atom_diag<false>;

//#define GENERATE_REF_H5
#ifdef GENERATE_REF_H5
h5::file out_file("atom_diag_real.h5", 'w');
#else
h5::file ref_file("atom_diag_real.ref.h5", 'r');
#endif


void print_binary(unsigned int n, int total_bits) {

  for (int i = 0; i < total_bits; i++) {
    int bit = (1 << (total_bits - i - 1));
    if ((n & bit) != 0)
      std::printf("1");
    else
      std::printf("0");
  }
  std::printf(" ");
}

void print_matrix(nda::matrix<double> m, double factor) {

  for (int i = 0; i < first_dim(m); i++) {
    std::printf("\n [");
    for (int j = 0; j < second_dim(m); j++) {
      if (std::abs(m(i, j)) == 0.0)
        std::printf("    .      ");
      else
        std::printf("% 10.3e ", m(i, j) * factor);
    }
    std::printf("]");
  }
  std::printf("\n");
}

void print_atom_diag(atom_diag_real const &ad) {

  int N = 0;
  for (int s = 0; s < ad.n_subspaces(); s++) {
    auto sp     = ad.get_eigensystems()[s];
    auto E_Udag = make_regular(dagger(sp.unitary_matrix));
    for (int i = 0; i < sp.eigenvalues.size(); i++) {
      for (int j = 0; j < sp.eigenvalues.size(); j++) E_Udag(i, j) *= sp.eigenvalues[i] + ad.get_gs_energy();
    }   
    auto H = sp.unitary_matrix * E_Udag;

    std::printf("\nblock: %d \n   ", N++);
    std::printf("states:  ");
    auto fss = ad.get_fock_states()[s];
    for (int i = 0; i < ad.get_subspace_dim(s); i++) {
      printf("  ");
      print_binary(fss[i], ad.get_fops().data().size());
    }   
    std::printf("\n");
    print_matrix(H,1.0);
  }   
  std::printf("\n");
}


// Prepare funcdamental operator set
fundamental_operator_set make_fops2() {
  fundamental_operator_set fops;
  for (int o : range(2)) fops.insert("dn", o); 
  for (int o : range(2)) fops.insert("up", o); 
  return fops;
}


// Hamiltonian: 2 orbital Hubbard + hopping between orbitals 0 and 1
template <typename O>
O make_hamiltonian2(typename O::scalar_t mu, typename O::scalar_t U, typename O::scalar_t t) {
  auto orbs = range(2);
  O h;
  // Interaction terms
  for (int o : orbs) h += -mu * (n("up", o) + n("dn", o));
  for (int o : orbs) h += U * n("up", o) * n("dn", o);
  
  // Hopping terms
  h += t * c_dag("up", 0) * c("up", 1) + dagger(t * c_dag("up", 0) * c("up", 1));
  h += t * c_dag("dn", 0) * c("dn", 1) + dagger(t * c_dag("dn", 0) * c("dn", 1));

  return h; 
}

TEST(atom_diag_real, Autopartition2) {
  auto fops = make_fops2();

  double mu = 0.4;
  double U  = 4.0;
  double t  = 1.0;
  auto h    = make_hamiltonian2<many_body_operator_real>(mu, U, t);
  
  many_body_operator_real hyb_effecive;
  hyb_effecive += t * c_dag("up", 0) * c("dn", 0) + dagger(t * c_dag("dn", 0) * c("up", 0));
  hyb_effecive += t * c_dag("up", 1) * c("dn", 1) + dagger(t * c_dag("dn", 1) * c("up", 1));
  //auto orbs = range(1);
  //for (int o : orbs) zero += 1.0 * (n("up", o) + n("dn", o));
  
  std::printf("-----------------\n ad without hyb\n-----------------\n");
  auto ad = atom_diag_real(h, fops);
  print_atom_diag(ad);
  
  EXPECT_EQ(ad.n_subspaces(),9);
  
  std::vector<int> ref_dims = {4,2,2,1,1,1,2,2,1}; 
  
  for (int s = 0; s < 9; s++) {
    EXPECT_EQ(ref_dims[s],ad.get_subspace_dim(s));
  } 
  EXPECT_EQ(16, ad.get_full_hilbert_space_dim());  

  std::vector<std::vector<int>> ref_states {{5,6,9,10},{1,2},{4,8},{3},
                                          {12},{0},{7,11},{13,14},{15}};

  for (int s = 0; s < ad.n_subspaces(); s++) {
    EXPECT_EQ(ref_dims[s],ad.get_subspace_dim(s));
    auto fss = ad.get_fock_states()[s];
    for (int i = 0; i < ad.get_subspace_dim(s); i++) {
      EXPECT_EQ(fss[i], ref_states[s][i]);
    }
  }   
  EXPECT_EQ(16, ad.get_full_hilbert_space_dim());  
  
  
  std::printf("\n\n-----------------\n ad with hyb\n-----------------\n");
  auto ad_hyb = atom_diag_real(h, hyb_effecive, fops);
  print_atom_diag(ad_hyb);
  
  EXPECT_EQ(ad_hyb.n_subspaces(),5);
  
  std::vector<int> ref_dims_hyb = {6,4,1,4,1}; 
  
  std::vector<std::vector<int>> ref_states_hyb { {3,5,6,9,10,12},
                                                 {1,2,4,8},{0},
                                                 {7,11,13,14},{15}}; 
  
  for (int s = 0; s < ad_hyb.n_subspaces(); s++) {
    EXPECT_EQ(ref_dims_hyb[s],ad_hyb.get_subspace_dim(s));
    auto fss = ad_hyb.get_fock_states()[s];
    for (int i = 0; i < ad_hyb.get_subspace_dim(s); i++) {
      //printf("  %lu, ", fss[i]);
      EXPECT_EQ(fss[i], ref_states_hyb[s][i]);
    }
    //printf("\n");
  }   
  EXPECT_EQ(16, ad_hyb.get_full_hilbert_space_dim());  

}


MAKE_MAIN;
