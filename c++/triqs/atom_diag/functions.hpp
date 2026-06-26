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

/**
 * @file
 * @brief Free functions that compute thermodynamic averages and act with operators on a solved diagonalization problem.
 */

#pragma once

#include <vector>
#include <triqs/gfs.hpp>
#include "./atom_diag.hpp"

namespace triqs::atom_diag {

  using namespace triqs::gfs;

  /**
   * @addtogroup triqs-atom-diag
   * @{
   */

  /**
   * @brief Compute the atomic partition function at inverse temperature \f$ \beta \f$.
   *
   * @details Sums the Boltzmann weights of all eigenstates of the Hamiltonian,
   * \f[
   *   Z = \sum_{B} \sum_{i=0}^{\dim(B)-1} e^{-\beta E_{B,i}},
   * \f]
   * where the eigenvalues \f$ E_{B,i} \f$ are taken with respect to the ground-state energy as zero.
   *
   * @tparam Complex Specialization of the diagonalization problem (real or complex Hamiltonian).
   * @param atom Solved diagonalization problem.
   * @param beta Inverse temperature \f$ \beta > 0 \f$.
   * @return Value of the partition function \f$ Z \f$.
   */
  template <bool Complex> double partition_function(atom_diag<Complex> const &atom, double beta);

  /**
   * @brief Compute the atomic density matrix at inverse temperature \f$ \beta \f$.
   *
   * @details Returns the Gibbs density matrix
   * \f$ \hat\rho = e^{-\beta \hat H} / Z \f$ as a block-diagonal matrix, with one diagonal block per invariant
   * subspace \f$ B \f$. The density matrix is expressed in the eigenbasis, hence each block is itself diagonal,
   * \f[
   *   \rho_B = \mathrm{diag}\!\Bigl( e^{-\beta E_{B,i}} / Z \Bigr)_{i=0}^{\dim(B)-1}.
   * \f]
   *
   * @tparam Complex Specialization of the diagonalization problem (real or complex Hamiltonian).
   * @param atom Solved diagonalization problem.
   * @param beta Inverse temperature \f$ \beta > 0 \f$.
   * @return Gibbs density matrix of the system, as a list of diagonal blocks indexed by subspace index \f$ B \f$.
   */
  template <bool Complex> typename atom_diag<Complex>::block_matrix_t atomic_density_matrix(atom_diag<Complex> const &atom, double beta);

  /**
   * @brief Compute the trace of a many-body operator weighted by a block-diagonal density matrix.
   *
   * @details Evaluates
   * \f[
   *   \mathrm{Tr}\,(\hat\rho\, \hat O) = \sum_{B} \mathrm{Tr}\,(\rho_B\, O_{BB}),
   * \f]
   * where \f$ \hat\rho \f$ is provided as a list of diagonal blocks (one per invariant subspace of `atom`) and only
   * the diagonal blocks \f$ O_{BB} \f$ of \f$ \hat O \f$ contribute. Useful for computing expectation values of
   * arbitrary observables once a density matrix has been built.
   *
   * @tparam Complex Specialization of the diagonalization problem (real or complex Hamiltonian).
   * @param density_matrix Density matrix as a list of diagonal blocks, indexed by subspace index \f$ B \f$.
   * @param op Operator to be averaged.
   * @param atom Solved diagonalization problem.
   * @return Expectation value of \f$ \hat O \f$ under the given density matrix.
   */
  template <bool Complex>
  typename atom_diag<Complex>::scalar_t trace_rho_op(typename atom_diag<Complex>::block_matrix_t const &density_matrix,
                                                     typename atom_diag<Complex>::many_body_op_t const &op, atom_diag<Complex> const &atom);

  /**
   * @brief Act with a many-body operator on a state vector, \f$ |\psi'\rangle = \hat O\, |\psi\rangle \f$.
   *
   * @details Both the input and the output state are vectors in the full Hilbert space expressed in the eigenbasis
   * of the Hamiltonian. The operator is converted internally into its block-matrix representation in the eigenbasis
   * before being applied to the state. Block-wise, for every source subspace \f$ B \f$ with target subspace
   * \f$ B' \f$,
   * \f[
   *   \psi'_{B'} \mathrel{+}= O_{B' \leftarrow B}\, \psi_{B}.
   * \f]
   *
   * @tparam Complex Specialization of the diagonalization problem (real or complex Hamiltonian).
   * @param op Many-body operator to apply.
   * @param st Input state vector in the full Hilbert space, expressed in the eigenbasis of the Hamiltonian.
   * @param atom Solved diagonalization problem providing the eigenbasis.
   * @return Output state vector in the full Hilbert space, expressed in the eigenbasis of the Hamiltonian.
   */
  template <bool Complex>
  typename atom_diag<Complex>::full_hilbert_space_state_t act(typename atom_diag<Complex>::many_body_op_t const &op,
                                                              typename atom_diag<Complex>::full_hilbert_space_state_t const &st,
                                                              atom_diag<Complex> const &atom);

  /**
   * @brief Tabulate the eigenvalues \f$ q_{B,i} = \langle B,i\,|\,\hat Q\,|\,B,i\rangle \f$ of a quantum-number
   * operator \f$ \hat Q \f$ over all eigenstates of the Hamiltonian.
   *
   * @details Assumes that \f$ \hat Q \f$ is a quantum number, i.e. that it commutes with \f$ \hat H \f$ and is 
   * therefore block-diagonal in the eigenbasis with diagonal blocks. The diagonal entries are returned, grouped by 
   * invariant subspace \f$ B \f$. Use `quantum_number_eigenvalues_checked` for a variant that explicitly verifies the
   * block-diagonal property.
   *
   * @tparam Complex Specialization of the diagonalization problem (real or complex Hamiltonian).
   * @param op Observable operator; expected to be a quantum number (otherwise an exception is raised).
   * @param atom Solved diagonalization problem.
   * @return Eigenvalues \f$ q_{B,i} \f$ grouped by invariant subspace \f$ B \f$.
   */
  template <bool Complex>
  std::vector<std::vector<quantum_number_t>> quantum_number_eigenvalues(typename atom_diag<Complex>::many_body_op_t const &op,
                                                                        atom_diag<Complex> const &atom);

  /**
   * @brief Tabulate the eigenvalues \f$ q_{B,i} \f$ of a quantum-number operator \f$ \hat Q \f$, also checking that 
   * the operator is diagonal in the eigenbasis.
   *
   * @details Same as `quantum_number_eigenvalues`, but also verifies that the supplied operator is diagonal in the
   * eigenbasis of the Hamiltonian. If it is not, an exception is raised. Useful when constructing a quantum number
   * from scratch and one wants to confirm that it really is a conserved quantity of `atom`.
   *
   * @tparam Complex Specialization of the diagonalization problem (real or complex Hamiltonian).
   * @param op Observable operator; expected to be a quantum number (otherwise an exception is raised).
   * @param atom Solved diagonalization problem.
   * @return Eigenvalues \f$ q_{B,i} \f$ grouped by invariant subspace \f$ B \f$.
   */
  template <bool Complex>
  std::vector<std::vector<quantum_number_t>> quantum_number_eigenvalues_checked(typename atom_diag<Complex>::many_body_op_t const &op,
                                                                                atom_diag<Complex> const &atom);

  /** @} */

} // namespace triqs::atom_diag
