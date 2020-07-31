// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2017 Igor Krivenko
// Copyright (c) 2018 Simons Foundation
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

#pragma once

#include <triqs/hilbert_space/fundamental_operator_set.hpp>
#include <triqs/operators/many_body_operator.hpp>

using triqs::hilbert_space::fundamental_operator_set;
using namespace triqs::operators;

// Prepare funcdamental operator set
fundamental_operator_set make_fops() {
  fundamental_operator_set fops;
  for (int o : range(3)) fops.insert("dn", o);
  for (int o : range(3)) fops.insert("up", o);
  return fops;
}

// Hamiltonian: 3 orbital Kanamori + hopping between orbitals 0 and 1
template <typename O>
O make_hamiltonian(typename O::scalar_t mu, typename O::scalar_t U, typename O::scalar_t J, typename O::scalar_t b, typename O::scalar_t t) {
  auto orbs = range(3);
  O h;
  // Interaction terms
  for (int o : orbs) h += -mu * (n("up", o) + n("dn", o));
  for (int o : orbs) h += b * (n("up", o) - n("dn", o));
  for (int o : orbs) h += U * n("up", o) * n("dn", o);
  for (int o1 : orbs)
    for (int o2 : orbs) {
      if (o1 == o2) continue;
      h += (U - 2 * J) * n("up", o1) * n("dn", o2);
    }
  for (int o1 : orbs)
    for (int o2 : orbs) {
      if (o2 >= o1) continue;
      h += (U - 3 * J) * n("up", o1) * n("up", o2);
      h += (U - 3 * J) * n("dn", o1) * n("dn", o2);
    }
  for (int o1 : orbs)
    for (int o2 : orbs) {
      if (o1 == o2) continue;
      h += -J * c_dag("up", o1) * c_dag("dn", o1) * c("up", o2) * c("dn", o2);
      h += -J * c_dag("up", o1) * c_dag("dn", o2) * c("up", o2) * c("dn", o1);
    }

  // Hopping terms
  h += t * c_dag("up", 0) * c("up", 1) + dagger(t * c_dag("up", 0) * c("up", 1));
  h += t * c_dag("dn", 0) * c("dn", 1) + dagger(t * c_dag("dn", 0) * c("dn", 1));

  return h;
}
