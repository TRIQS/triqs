/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2016, P. Seth, I. Krivenko, M. Ferrero and O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once

#include <vector>
#include <triqs/gfs.hpp>
#include "./atom_diag.hpp"

namespace triqs {
namespace atom_diag {

 using namespace triqs::gfs;

#define ATOM_DIAG atom_diag<Complex>
#define ATOM_DIAG_T typename atom_diag<Complex>

/// The atomic partition function
/**
 * @tparam Complex Do we have a diagonalization problem with a complex-valued Hamiltonian?
 * @param atom Solved diagonalization problem.
 * @param beta Inverse temperature.
 * @return Value of the partition function.
 * @include triqs/atom_diag/functions.hpp
 */
template<bool Complex>
double partition_function(ATOM_DIAG const& atom, double beta);

/// The atomic density matrix
/**
 * @tparam Complex Do we have a diagonalization problem with a complex-valued Hamiltonian?
 * @param atom Solved diagonalization problem.
 * @param beta Inverse temperature.
 * @return Gibbs' density matrix of the system.
 * @include triqs/atom_diag/functions.hpp
*/
template<bool Complex>
ATOM_DIAG_T::block_matrix_t atomic_density_matrix(ATOM_DIAG const& atom, double beta);

/// Compute Tr (op * density_matrix)
/**
 * @param density_matrix Density matrix as a list of diagonal blocks for all invariant subspaces in `atom`.
 * @param op Operator to be averaged.
 * @param atom Solved diagonalization problem.
 * @return Operator `op` averaged over the density matrix.
 * @include triqs/atom_diag/functions.hpp
 */
template<bool Complex>
ATOM_DIAG_T::scalar_t trace_rho_op(ATOM_DIAG_T::block_matrix_t const& density_matrix,
                                   ATOM_DIAG_T::many_body_op_t const& op,
                                   ATOM_DIAG const& atom);

/// Act with operator `op` on state `st`
/**
 * @param op Operator to act on the state.
 * @param st Initial state vector in the full Hilbert space, written in the eigenbasis of the Hamiltonian.
 * @param atom Solved diagonalization problem.
 * @return Final state vector in the full Hilbert space.
 * @include triqs/atom_diag/functions.hpp
 */
template<bool Complex>
ATOM_DIAG_T::full_hilbert_space_state_t act(ATOM_DIAG_T::many_body_op_t const& op,
                                            ATOM_DIAG_T::full_hilbert_space_state_t const& st,
                                            ATOM_DIAG const& atom);

/// Compute values of a given quantum number for all eigenstates
/**
 * @param op Observable operator; supposed to be a quantum number (if not -> exception).
 * @param atom Solved diagonalization problem.
 * @return The eigenvalues by block
 * @include triqs/atom_diag/functions.hpp
 */
template<bool Complex>
std::vector<std::vector<quantum_number_t>> quantum_number_eigenvalues(ATOM_DIAG_T::many_body_op_t const& op,
                                                                      ATOM_DIAG const& atom);

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
template<bool Complex>
std::vector<std::vector<quantum_number_t>> quantum_number_eigenvalues_checked(ATOM_DIAG_T::many_body_op_t const& op,
                                                                              ATOM_DIAG const& atom);
}}
