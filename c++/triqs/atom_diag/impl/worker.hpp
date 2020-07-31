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

#pragma once

#include <vector>
#include <climits>
#include "../atom_diag.hpp"

using namespace triqs::hilbert_space;

namespace triqs {
  namespace atom_diag {

    // Division of Hilbert Space into Hilbert subspaces, using either autopartitioning or quantum numbers.
    template <bool Complex> struct atom_diag_worker {

      //using atom_diag = atom_diag<Complex>;
      using scalar_t       = typename atom_diag<Complex>::scalar_t;
      using matrix_t       = typename atom_diag<Complex>::matrix_t;
      using many_body_op_t = typename atom_diag<Complex>::many_body_op_t;

      atom_diag_worker(atom_diag<Complex> *hdiag, int n_min = 0, int n_max = INT_MAX) : hdiag(hdiag), n_min(n_min), n_max(n_max) {}

      void autopartition();
      void partition_with_qn(std::vector<many_body_op_t> const &qn_vector);

      private:
      atom_diag<Complex> *hdiag;
      int n_min, n_max;

      // Create matrix of an operator acting from one subspace to another
      matrix_t make_op_matrix(many_body_op_t const &op, int from_sp, int to_sp) const;

      void complete();
      bool fock_state_filter(fock_state_t s);
    };

  } // namespace atom_diag
} // namespace triqs
