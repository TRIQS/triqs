// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
// Authors: Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell

#include <triqs/test_tools/gfs.hpp>

#include <triqs/atom_diag/atom_diag.hpp>
#include <triqs/atom_diag/functions.hpp>
#include <triqs/atom_diag/gf.hpp>
//#include <triqs/arrays/blas_lapack/dot.hpp>
#include <h5/h5.hpp>

using namespace triqs::arrays;
using namespace triqs::hilbert_space;
using namespace triqs::atom_diag;
using namespace triqs::operators;

fundamental_operator_set make_fops(int n_orb) {
  fundamental_operator_set fops;
  for (int oidx : range(n_orb)) {
    fops.insert("up", oidx);
    fops.insert("dn", oidx);
  }
  return fops;
}

//
template <typename OP> OP make_hamiltonian(int n_orb, double mu, double U, double J) {

  auto orbs = range(n_orb);

  OP h;

  for (int o : orbs) h += -mu * (n("up", o) + n("dn", o));

  // Density-density interactions
  for (int o : orbs) h += U * n("up", o) * n("dn", o);

  for (int o1 : orbs) {
    for (int o2 : orbs) {
      if (o1 == o2) continue;
      h += (U - 2 * J) * n("up", o1) * n("dn", o2);
    }
  }

  for (int o1 : orbs)
    for (int o2 : orbs) {
      if (o2 >= o1) continue;
      h += (U - 3 * J) * n("up", o1) * n("up", o2);
      h += (U - 3 * J) * n("dn", o1) * n("dn", o2);
    }

  // spin-flip and pair-hopping
  for (int o1 : orbs) {
    for (int o2 : orbs) {
      if (o1 == o2) continue;
      h += -J * c_dag("up", o1) * c_dag("dn", o1) * c("up", o2) * c("dn", o2);
      h += -J * c_dag("up", o1) * c_dag("dn", o2) * c("up", o2) * c("dn", o1);
    }
  }

  return h;
}

TEST(atom_diag_real, f_shell_kanamori) {

  int n_orb = 7;
  auto fops = make_fops(n_orb);

  double U  = 1.0;
  double J  = 0.2;
  double mu = 0.5 * U;

  auto H = make_hamiltonian<many_body_operator_real>(n_orb, mu, U, J);

  int n_min = 0, n_max = 3 * 2;

  auto ad = triqs::atom_diag::atom_diag<false>(H, fops, n_min, n_max);

  std::cout << "Found " << ad.n_subspaces() << " subspaces." << std::endl;
  EXPECT_EQ(ad.n_subspaces(), 1040);
}

MAKE_MAIN;
