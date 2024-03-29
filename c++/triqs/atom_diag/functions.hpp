// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
// Copyright (c) 2017 Igor Krivenko
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
// Authors: Michel Ferrero, Igor Krivenko, Nils Wentzell

#pragma once

#include <vector>
#include <triqs/gfs.hpp>
#include "./atom_diag.hpp"

namespace triqs {
  namespace atom_diag {

    using namespace triqs::gfs;

    /// The atomic partition function
    /**
 * @tparam Complex Do we have a diagonalization problem with a complex-valued Hamiltonian?
 * @param atom Solved diagonalization problem.
 * @param beta Inverse temperature.
 * @return Value of the partition function.
 * @include triqs/atom_diag/functions.hpp
 */
    template <bool Complex> double partition_function(atom_diag<Complex> const &atom, double beta);

    /// The atomic density matrix
    /**
 * @tparam Complex Do we have a diagonalization problem with a complex-valued Hamiltonian?
 * @param atom Solved diagonalization problem.
 * @param beta Inverse temperature.
 * @return Gibbs' density matrix of the system.
 * @include triqs/atom_diag/functions.hpp
*/
    template <bool Complex> typename atom_diag<Complex>::block_matrix_t atomic_density_matrix(atom_diag<Complex> const &atom, double beta);

    /// Compute Tr (op * density_matrix)
    /**
 * @param density_matrix Density matrix as a list of diagonal blocks for all invariant subspaces in `atom`.
 * @param op Operator to be averaged.
 * @param atom Solved diagonalization problem.
 * @return Operator `op` averaged over the density matrix.
 * @include triqs/atom_diag/functions.hpp
 */
    template <bool Complex>
    typename atom_diag<Complex>::scalar_t trace_rho_op(typename atom_diag<Complex>::block_matrix_t const &density_matrix,
                                                       typename atom_diag<Complex>::many_body_op_t const &op, atom_diag<Complex> const &atom);

    /// Act with operator `op` on state `st`
    /**
 * @param op Operator to act on the state.
 * @param st Initial state vector in the full Hilbert space, written in the eigenbasis of the Hamiltonian.
 * @param atom Solved diagonalization problem.
 * @return Final state vector in the full Hilbert space.
 * @include triqs/atom_diag/functions.hpp
 */
    template <bool Complex>
    typename atom_diag<Complex>::full_hilbert_space_state_t act(typename atom_diag<Complex>::many_body_op_t const &op,
                                                                typename atom_diag<Complex>::full_hilbert_space_state_t const &st,
                                                                atom_diag<Complex> const &atom);

    /// Compute values of a given quantum number for all eigenstates
    /**
 * @param op Observable operator; supposed to be a quantum number (if not -> exception).
 * @param atom Solved diagonalization problem.
 * @return The eigenvalues by block
 * @include triqs/atom_diag/functions.hpp
 */
    template <bool Complex>
    std::vector<std::vector<quantum_number_t>> quantum_number_eigenvalues(typename atom_diag<Complex>::many_body_op_t const &op,
                                                                          atom_diag<Complex> const &atom);

    /// Compute values of a given quantum number for all eigenstates
    /**
 * This function is similar to :ref:`quantum_number_eigenvalues()` except it checks that
 * the quantum number operator is diagonal in the eigenbasis.
 *
 * @param op Observable operator; supposed to be a quantum number (if not -> exception).
 * @param atom Solved diagonalization problem.
 * @return The eigenvalues by block
 * @include triqs/atom_diag/functions.hpp
 */
    template <bool Complex>
    std::vector<std::vector<quantum_number_t>> quantum_number_eigenvalues_checked(typename atom_diag<Complex>::many_body_op_t const &op,
                                                                                  atom_diag<Complex> const &atom);
  } // namespace atom_diag
} // namespace triqs
