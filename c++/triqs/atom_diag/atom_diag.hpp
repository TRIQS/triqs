// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
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
// Authors: Maxime Charlebois, Michel Ferrero, Igor Krivenko, Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell

/**
 * @file
 * @brief Provides a lightweight exact diagonalization solver for fermionic Hamiltonians.
 */

#pragma once

#include "../arrays.hpp"
#include "../hilbert_space/fundamental_operator_set.hpp"
#include "../hilbert_space/hilbert_space.hpp"
#include "../operators/many_body_operator.hpp"
#include "../utility/macros.hpp"

#include <h5/h5.hpp>
#include <itertools/itertools.hpp>
#include <nda/h5.hpp>
#include <nda/nda.hpp>

#include <complex>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>
#include <vector>

namespace triqs::atom_diag {

  // Import tools from the hilbert_space and arrays namespace.
  using namespace triqs::hilbert_space;
  using namespace triqs::arrays;

  // Forward declarations.
  template <bool C> class atom_diag;
  template <bool C> struct atom_diag_worker;
  template <bool C> std::ostream &operator<<(std::ostream &, atom_diag<C> const &);
  template <bool C> void h5_write(h5::group, std::string const &, atom_diag<C> const &);
  template <bool C> void h5_read(h5::group, std::string const &, atom_diag<C> &);

  /**
   * @addtogroup triqs-atom-diag
   * @{
   */

  /// Index type used by the fundamental operator set associated with the diagonalization problem.
  using indices_t = fundamental_operator_set::indices_t;

  /// Type used to store quantum-number values. Quantum-number operators are Hermitian, so their eigenvalues are real.
  using quantum_number_t = double;

  /**
   * @brief Lightweight exact diagonalization solver for finite fermionic Hamiltonians.
   *
   * @details Perform exact diagonalization of a many-body Hamiltonian \f$ \hat H \f$ acting on the Fock space of a 
   * finite set of fermionic single-particle states. The Hilbert space is split into invariant subspaces of 
   * \f$ \hat H \f$, each of which is diagonalized independently.
   *
   * After construction the solver exposes
   *
   * - the eigenvalues \f$ E_B \f$ and unitary matrix \f$ U_B \f$ of every invariant subspace \f$ B \f$,
   * - the matrix blocks of every fundamental creation/annihilation operator \f$ \hat c_i, \hat c^\dagger_i \f$ in the
   *   eigenbasis, where \f$ i \f$ is the linear index of the operator in the fundamental operator set,
   * - the subspace-to-subspace connections induced by every fundamental creation/annihilation operator, i.e. the
   *   index of the subspace that each operator maps a given subspace to (or \f$ -1 \f$ if it annihilates it),
   * - convenience routines that turn a generic many-body operator into a block-matrix representation in the
   *   eigenbasis.
   *
   * Two specializations are provided, one for real-valued and one for complex-valued Hamiltonians.
   *
   * @tparam Complex Allow the Hamiltonian to be complex-valued.
   */
  template <bool Complex> class atom_diag {
    public:
    /// Scalar type of the matrix elements: `double` or `std::complex<double>`.
    using scalar_t = std::conditional_t<Complex, std::complex<double>, double>;

    /// Dense matrix type with scalar entries of type scalar_t.
    using matrix_t = matrix<scalar_t>;

    /// Block-diagonal matrix type, one dense block per invariant subspace.
    using block_matrix_t = std::vector<matrix_t>;

    /// State vector type of the full Hilbert space.
    using full_hilbert_space_state_t = vector<scalar_t>;

    /// Many-body operator type compatible with scalar_t.
    using many_body_op_t = triqs::operators::many_body_operator_generic<scalar_t>;

    /// Data type of a fundamental operator set.
    using fops_data_t = triqs::hilbert_space::fundamental_operator_set::data_t;

    /**
     * @brief Eigensystem of a single invariant subspace of the Hamiltonian \f$ H_B \f$.
     *
     * @details Holds the eigenvalues \f$ E_B \f$ (with the global ground-state energy subtracted, so that the smallest 
     * eigenvalue across all subspaces is zero) and the unitary matrix \f$ U_B \f$ that maps the Fock basis of the 
     * subspace to the eigenbasis.
     */
    struct C2PY_IGNORE eigensystem_t {
      /// Eigenvalues \f$ E_B \f$ sorted in ascending order.
      vector<double> eigenvalues;

      /// Unitary matrix \f$ U_B \f$ mapping the Fock basis to the eigenbasis.
      matrix_t unitary_matrix;

      /// Default equal-to operator compares the eigenvalues and unitary matrix.
      bool operator==(eigensystem_t const &) const = default;

      /**
       * @brief Broadcast an eigensystem to all ranks of an MPI communicator.
       *
       * @param es Eigensystem to broadcast.
       * @param c MPI communicator.
       * @param root Rank holding the source eigensystem.
       */
      C2PY_IGNORE friend void mpi_broadcast(eigensystem_t &es, mpi::communicator c = {}, int root = 0) {
        mpi::broadcast(es.eigenvalues, c, root);
        mpi::broadcast(es.unitary_matrix, c, root);
      }

      /// Get the HDF5 format tag.
      [[nodiscard]] static std::string hdf5_format() { return "atom_diag::eigensystem_t"; }

      /**
       * @brief Write an eigensystem to HDF5.
       *
       * @param g HDF5 group to write into.
       * @param name Name of the subgroup to create.
       * @param es Eigensystem to write.
       */
      friend void h5_write(h5::group g, std::string const &name, eigensystem_t const &es) {
        auto gr = g.create_group(name);
        h5_write(gr, "eigenvalues", es.eigenvalues);
        h5_write(gr, "unitary_matrix", es.unitary_matrix);
      }

      /**
       * @brief Read an eigensystem from HDF5.
       *
       * @param g HDF5 group to read from.
       * @param name Name of the subgroup containing the eigensystem.
       * @param es Eigensystem to read into.
       */
      friend void h5_read(h5::group g, std::string const &name, eigensystem_t &es) {
        auto gr = g.open_group(name);
        h5_read(gr, "eigenvalues", es.eigenvalues);
        h5_read(gr, "unitary_matrix", es.unitary_matrix);
      }
    };

    /**
     * @brief Block-matrix representation of an operator \f$ \hat{O} \f$ that respects the block structure of the 
     * Hamiltonian.
     *
     * @details Stores, for every invariant subspace \f$ B \f$, the target subspace \f$ B' \f$ (or \f$ -1 \f$ if the 
     * operator annihilates \f$ B \f$) together with the dense matrix block describing the action \f$ B \to B' \f$ in 
     * the Hamiltonian eigenbasis.
     * 
     * We say that an operator \f$ \hat O \f$ respects the block structure of the Hamiltonian if it maps every invariant
     * subspace \f$ B \f$ to a single invariant subspace \f$ B' \f$ (or annihilates it).
     */
    struct C2PY_IGNORE op_block_mat_t {
      /// Array of subspace-to-subspace connections induced by the operator.
      array<long, 1> connection;

      /// Vector of dense matrix blocks describing the action \f$ B \to B' \f$ in the eigenbasis.
      std::vector<matrix_t> block_mat;

      /**
       * @brief Construct an empty block-matrix representation for a problem with the given number of subspaces.
       *
       * @details All connections are initialised to \f$ -1 \f$ (i.e. no target subspace) and the matrix blocks are left
       * empty.
       *
       * @param n_blocks Number of invariant subspaces of the underlying Hamiltonian.
       */
      op_block_mat_t(int n_blocks) : connection(n_blocks), block_mat(n_blocks) { connection(range::all) = -1; };

      /// Number of invariant subspaces.
      int n_blocks() const { return block_mat.size(); }

      /**
       * @brief Pretty-print the block matrix to a stream.
       *
       * @param os Output stream.
       * @param op_mat Block matrix to print.
       * @return Reference to the output stream.
       */
      friend std::ostream &operator<<(std::ostream &os, op_block_mat_t const &op_mat) {
        os << "Operator block matrix:\n"
           << " n_blocks = " << op_mat.n_blocks() << "\n";
        for (auto bidx : range(op_mat.n_blocks())) {
          auto &m = op_mat.block_mat[bidx];
          auto b2 = op_mat.connection(bidx);
          os << "Block: "
             << "blocks (" << bidx << ", " << b2 << ") "
             << "size (" << first_dim(m) << ", " << second_dim(m) << ") " << op_mat.block_mat[bidx] << "\n";
        }
        return os;
      };
    };

    /// Default constructor leaves the solver in an uninitialized state.
    C2PY_IGNORE atom_diag() = default;

    /**
     * @brief Reduce a Hamiltonian to a block-diagonal form using auto-partitioning, then diagonalize the blocks.
     *
     * @details Uses the auto-partition procedure to detect the invariant subspaces of the Hamiltonian, and the QR
     * algorithm to diagonalize each block. The invariant subspaces are chosen such that every fundamental creation
     * and annihilation operator from the provided fundamental operator set maps each subspace to a single subspace
     * (or annihilates it).
     *
     * @param h Many-body Hamiltonian \f$ \hat H \f$ to be diagonalized.
     * @param fops Fundamental operator set; must at least contain every fundamental operator appearing in 
     * \f$ \hat H \f$.
     */
    atom_diag(many_body_op_t const &h, fundamental_operator_set const &fops);

    /**
     * @brief Reduce a Hamiltonian to a block-diagonal form using auto-partitioning refined by a hybridization term.
     *
     * @details Behaves like the two-argument auto-partition constructor, but the partition is required to remain
     * invariant under the additional many-body operator \f$ \hat V \f$ as well. This is useful when the Hamiltonian 
     * on its own would yield invariant subspaces that mix when an extra (e.g. hybridization) operator acts, leading 
     * to matrix blocks that are coarser than what \f$ \hat H \f$ alone would suggest.
     *
     * @param h Many-body Hamiltonian \f$ \hat H \f$ to be diagonalized.
     * @param fops Fundamental operator set; must at least contain every fundamental operator appearing in 
     * \f$ \hat H \f$.
     * @param hyb Additional many-body operator \f$ \hat V \f$ that the auto-partition must respect; every fundamental 
     * operator appearing in \f$ \hat V \f$ must also belong to the fundamental operator set.
     */
    atom_diag(many_body_op_t const &h, fundamental_operator_set const &fops, many_body_op_t const &hyb);

    /**
     * @brief Diagonalize a Hamiltonian restricted to a particle-number window.
     *
     * @details Builds the invariant subspaces by total particle number and keeps only those whose number of particles
     * lies in the inclusive window \f$ [n_{\text{min}}, n_{\text{max}}] \f$. The blocks are then diagonalized with
     * the QR algorithm. Convenient when only a few sectors of fixed occupation are physically relevant.
     *
     * @param h Many-body Hamiltonian \f$ \hat H \f$ to be diagonalized.
     * @param fops Fundamental operator set; must at least contain every fundamental operator appearing in 
     * \f$ \hat H \f$.
     * @param n_min Minimum total particle number to keep.
     * @param n_max Maximum total particle number to keep.
     */
    atom_diag(many_body_op_t const &h, fundamental_operator_set const &fops, int n_min, int n_max);

    /**
     * @brief Reduce a Hamiltonian to a block-diagonal form using user-supplied quantum numbers, then diagonalize the
     * blocks.
     *
     * @details Partitions the Hilbert space into common eigenspaces of the provided quantum-number operators. The
     * quantum numbers must be chosen such that every fundamental creation and annihilation operator from the provided
     * fundamental operator set maps each common eigenspace to a single common eigenspace (or annihilates it). Each
     * block of the Hamiltonian is then diagonalized with the QR algorithm.
     *
     * @param h Many-body Hamiltonian \f$ \hat H \f$ to be diagonalized.
     * @param fops Fundamental operator set; must at least contain every fundamental operator appearing in 
     * \f$ \hat H \f$.
     * @param qn_vector List of quantum-number operators.
     */
    atom_diag(many_body_op_t const &h, fundamental_operator_set const &fops, std::vector<many_body_op_t> const &qn_vector);

    /**
     * @brief Initializer-list overload of #atom_diag(many_body_op_t const &, fundamental_operator_set const &,
     * std::vector<many_body_op_t> const &).
     *
     * @param h Many-body Hamiltonian \f$ \hat H \f$ to be diagonalized.
     * @param fops Fundamental operator set; must at least contain every fundamental operator appearing in 
     * \f$ \hat H \f$.
     * @param init_lst Braced list of quantum-number operators.
     */
    C2PY_IGNORE atom_diag(many_body_op_t const &h, fundamental_operator_set const &fops, std::initializer_list<many_body_op_t> const &init_lst)
       : atom_diag(h, fops, std::vector<many_body_op_t>{init_lst}) {};

    /// Get the Hamiltonian used at construction, with its native scalar type.
    C2PY_IGNORE many_body_op_t const &get_h_atomic() const { return h_atomic; }

    /// Get the Hamiltonian used at construction as a generic many-body operator.
    C2PY_PROPERTY_GET(h_atomic) operators::many_body_operator get_h_atomic_as_mbop() const { return h_atomic; }

    /// Get the fundamental operator set used at construction.
    C2PY_IGNORE fundamental_operator_set const &get_fops() const { return fops; }

    /// Get the data of the fundamental operator set used at construction.
    C2PY_PROPERTY_GET(fops) fops_data_t const &get_fops_as_data() const { return fops.data(); }

    /// Get the full Hilbert space over which the diagonalization problem is defined.
    C2PY_IGNORE class hilbert_space const &get_full_hilbert_space() const { return full_hs; }

    /// Get the dimension of the full Hilbert space.
    /// @note After truncation this still returns the original (untruncated) dimension. Use
    ///       get_total_eigenstate_count() for the number of retained eigenstates.
    C2PY_PROPERTY_GET(full_hilbert_space_dim) int get_full_hilbert_space_dim() const { return full_hs.size(); }

    /// Get the number of invariant subspaces produced by the chosen partitioning scheme.
    C2PY_PROPERTY_GET(n_subspaces) int n_subspaces() const { return eigensystems.size(); }

    /**
     * @brief Get the dimension \f$ \dim(B) \f$ of invariant subspace \f$ B \f$.
     *
     * @param sp_index Subspace index \f$ B \f$.
     * @return Number of eigenstates in subspace \f$ B \f$.
     */
    int get_subspace_dim(int sp_index) const { return eigensystems[sp_index].eigenvalues.size(); }

    /**
     * @brief Get the dimensions \f$ \dim(B) \f$ of all invariant subspaces.
     *
     * @return List of subspace dimensions, indexed by subspace index \f$ B \f$.
     */
    std::vector<int> get_subspace_dims() const {
      auto dims = std::vector<int>(n_subspaces());
      for (long i : range(n_subspaces())) dims[i] = get_subspace_dim(i);
      return dims;
    }

    /**
     * @brief Get the Fock states spanning invariant subspace \f$ B \f$.
     *
     * @param sp_index Subspace index \f$ B \f$.
     * @return List of \f$ \dim(B) \f$ Fock states (encoded as 64-bit integers) spanning subspace \f$ B \f$.
     *
     * @note After truncation this returns ALL original Fock states of the subspace, which may exceed the number of
     *       retained eigenstates. Use get_subspace_dim(sp_index) for the eigenstate count.
     */
    C2PY_IGNORE std::vector<fock_state_t> const &get_fock_states(int sp_index) const { return sub_hilbert_spaces[sp_index].get_all_fock_states(); }

    /**
     * @brief Get the Fock states of every invariant subspace.
     *
     * @return Outer list indexed by subspace index \f$ B \f$, inner list of length \f$ \dim(B) \f$ giving the Fock
     * states (encoded as 64-bit integers) spanning that subspace.
     *
     * @note See the single-subspace overload for truncation behavior.
     */
    C2PY_PROPERTY_GET(fock_states) std::vector<std::vector<fock_state_t>> get_fock_states() const {
      std::vector<std::vector<fock_state_t>> fock_states(n_subspaces());
      for (auto i : range(n_subspaces())) fock_states[i] = sub_hilbert_spaces[i].get_all_fock_states();
      return fock_states;
    }

    /**
     * @brief Get the unitary matrix \f$ U_B \f$ mapping the Fock basis of subspace \f$ B \f$ to its eigenbasis.
     *
     * @param sp_index Subspace index \f$ B \f$.
     * @return Unitary matrix \f$ U_B \f$ such that \f$ H_B = U_B\, \mathrm{diag}(E_B)\, U_B^\dagger \f$
     * within the subspace.
     */
    matrix<scalar_t> const &get_unitary_matrix(int sp_index) const { return eigensystems[sp_index].unitary_matrix; }

    /**
     * @brief Get the unitary matrices \f$ U_B \f$ for every invariant subspace.
     *
     * @return List of unitary matrices, indexed by subspace index \f$ B \f$.
     */
    C2PY_PROPERTY_GET(unitary_matrices) std::vector<matrix<scalar_t>> get_unitary_matrices() const {
      std::vector<matrix<scalar_t>> umat(n_subspaces());
      for (auto i : range(n_subspaces())) umat[i] = get_eigensystems()[i].unitary_matrix;
      return umat;
    }

    /**
     * @brief Map a subspace-local pair \f$ (B, i) \f$ to its linear index in the full Hilbert space.
     *
     * @details The full-Hilbert-space eigenstate index is
     * \f[
     *   d(B, i) = \mathtt{first\_eigenstate\_of\_subspace}[B] + i,
     *   \quad 0 \le i < \dim(B), \quad 0 \le d < N.
     * \f]
     *
     * @param sp_index Subspace index \f$ B \f$.
     * @param i Eigenstate index inside subspace \f$ B \f$, with \f$ 0 \le i < \dim(B) \f$.
     * @return Linear eigenstate index \f$ d(B, i) \f$ in the eigenbasis of the full Hilbert space.
     */
    int flatten_subspace_index(int sp_index, int i) const { return first_eigenstate_of_subspace[sp_index] + i; }

    /**
     * @brief Get the range of full-Hilbert-space indices corresponding to subspace \f$ B \f$.
     *
     * @param sp_index Subspace index \f$ B \f$.
     * @return Half-open range \f$ [d(B, 0),\, d(B, 0) + \dim(B)) \f$ of full-Hilbert-space eigenstate indices
     * belonging to subspace \f$ B \f$.
     */
    C2PY_IGNORE range index_range_of_subspace(int sp_index) const {
      return range{first_eigenstate_of_subspace[sp_index], first_eigenstate_of_subspace[sp_index] + get_subspace_dim(sp_index)};
    }

    /// Get the eigensystems of all invariant subspaces.
    C2PY_IGNORE std::vector<eigensystem_t> const &get_eigensystems() const { return eigensystems; }

    /**
     * @brief Get the eigenvalue \f$ E_{B,i} \f$ of the Hamiltonian.
     *
     * @param sp_index Subspace index \f$ B \f$.
     * @param i Eigenstate index inside subspace \f$ B \f$, with \f$ 0 \le i < \dim(B) \f$.
     * @return Eigenvalue \f$ E_{B,i} \f$, with the global ground-state energy subtracted.
     */
    double get_eigenvalue(int sp_index, int i) const { return eigensystems[sp_index].eigenvalues[i]; }

    /**
     * @brief Get all eigenvalues \f$ E_{B,i} \f$ grouped by invariant subspace.
     *
     * @return Outer list indexed by subspace index \f$ B \f$, inner list of length \f$ \dim(B) \f$ giving the
     * eigenvalues \f$ E_{B,i} \f$ sorted in ascending order.
     */
    C2PY_PROPERTY_GET(energies) std::vector<std::vector<double>> get_energies() const;

    /**
     * @brief Get the values of all quantum-number operators, grouped by invariant subspace.
     *
     * @return Outer list indexed by subspace index \f$ B \f$, inner list giving the value of each quantum-number
     * operator on subspace \f$ B \f$.
     */
    C2PY_PROPERTY_GET(quantum_numbers) std::vector<std::vector<quantum_number_t>> const &get_quantum_numbers() const { return quantum_numbers; }

    /// Get the ground-state energy, i.e. the minimum eigenvalue across all invariant subspaces.
    C2PY_PROPERTY_GET(gs_energy) double get_gs_energy() const { return gs_energy; }

    /// Check whether the vacuum state \f$ |0\rangle \f$ is present in the (possibly truncated) Hilbert space.
    C2PY_PROPERTY_GET(has_vacuum) bool has_vacuum() const { return vacuum_subspace_index != -1; }

    /// Get the index of the invariant subspace containing the vacuum state.
    /// @note Returns -1 if the vacuum subspace was removed during truncation. Check has_vacuum() first.
    C2PY_PROPERTY_GET(vacuum_subspace_index) long get_vacuum_subspace_index() const { return vacuum_subspace_index; }

    /**
     * @brief Get the vacuum state as a vector in the full Hilbert space.
     *
     * @details The returned vector is expressed in the eigenbasis of the Hamiltonian \f$ \hat H \f$.
     *
     * @note After truncation this is a projection into the truncated eigenbasis (not unit-normalized), and it is a
     * zero vector if the vacuum subspace was removed. Check has_vacuum() first.
     *
     * @return Vacuum state vector.
     */
    C2PY_PROPERTY_GET(vacuum_state) full_hilbert_space_state_t const &get_vacuum_state() const { return vacuum; }

    /// Check whether this atom_diag has been truncated.
    C2PY_PROPERTY_GET(is_truncated) bool is_truncated() const { return truncated_; }

    /// Get the total number of retained eigenstates across all invariant subspaces.
    C2PY_PROPERTY_GET(total_eigenstate_count) int get_total_eigenstate_count() const {
      if (n_subspaces() == 0) return 0;
      return first_eigenstate_of_subspace.back() + get_subspace_dim(n_subspaces() - 1);
    }

    /**
     * @brief Create a truncated copy of this atom_diag by discarding high-energy eigenstates.
     *
     * @details Keeps the eigenstates whose energy (relative to the ground state) does not exceed @p energy_cutoff,
     * subject to a global cap of @p max_states states across all subspaces. Subspaces left without any retained
     * eigenstate are dropped. Note that a degenerate multiplet straddling the cutoff (or the @p max_states boundary)
     * may be split, keeping only some of its states.
     *
     * @param energy_cutoff Keep eigenstates with energy \f$ \le \f$ energy_cutoff (relative to the ground state).
     * @param max_states Keep at most this many eigenstates globally across all subspaces (-1 means unlimited).
     * @return New atom_diag restricted to the retained eigenstates.
     */
    [[nodiscard]] atom_diag truncate(double energy_cutoff = std::numeric_limits<double>::infinity(), int max_states = -1) const;

    /**
     * @brief Get the target subspace \f$ B' \f$ of the annihilation operator \f$ \hat c_i \f$ acting on subspace
     * \f$ B \f$.
     *
     * @details
     * \f[
     *   \hat c_i\, S_B \subseteq S_{B'} \; ,
     * \f]
     * with \f$ B' = -1 \f$ if \f$ \hat c_i \f$ annihilates \f$ B \f$. The operator \f$ \hat c_i \f$ is identified by
     * its linear index \f$ i \f$ in the fundamental operator set provided at construction.
     *
     * @param op_linear_index Linear index \f$ i \f$ of the annihilation operator.
     * @param sp_index Source subspace index \f$ B \f$.
     * @return Target subspace index \f$ B' \f$, or \f$ -1 \f$ if the operator annihilates the source subspace.
     */
    long c_connection(int op_linear_index, int sp_index) const { return annihilation_connection(op_linear_index, sp_index); }

    /**
     * @brief Get the target subspace \f$ B' \f$ of the creation operator \f$ \hat c^\dagger_i \f$ acting on subspace
     * \f$ B \f$.
     *
     * @details
     * \f[
     *   \hat c^\dagger_i\, S_B \subseteq S_{B'} \; ,
     * \f]
     * with \f$ B' = -1 \f$ if \f$ \hat c^\dagger_i \f$ annihilates \f$ B \f$. The operator \f$ \hat c^\dagger_i \f$
     * is identified by its linear index \f$ i \f$ in the fundamental operator set provided at construction.
     *
     * @param op_linear_index Linear index \f$ i \f$ of the creation operator.
     * @param sp_index Source subspace index \f$ B \f$.
     * @return Target subspace index \f$ B' \f$, or \f$ -1 \f$ if the operator annihilates the source subspace.
     */
    long cdag_connection(int op_linear_index, int sp_index) const { return creation_connection(op_linear_index, sp_index); }

    /**
     * @brief Get the matrix block of the annihilation operator \f$ \hat c_i \f$ acting on subspace \f$ B \f$.
     *
     * @details The returned matrix is the representation of \f$ \hat c_i \f$ in the eigenbasis of \f$ \hat H \f$, 
     * i.e. \f$ \bigl[\hat c_i\bigr]_{B' \leftarrow B} \f$, with shape \f$ \dim(B') \times \dim(B) \f$ (not 
     * necessarily square).
     *
     * @param op_linear_index Linear index \f$ i \f$ of the annihilation operator.
     * @param sp_index Source subspace index \f$ B \f$.
     * @return Matrix block of the annihilation operator from subspace \f$ B \f$ to subspace \f$ B' \f$.
     */
    matrix_t const &c_matrix(int op_linear_index, int sp_index) const { return c_matrices[op_linear_index][sp_index]; }

    /**
     * @brief Get the matrix block of the creation operator \f$ \hat c^\dagger_i \f$ acting on subspace \f$ B \f$.
     *
     * @details The returned matrix is the representation of \f$ \hat c^\dagger_i \f$ in the eigenbasis of
     * \f$ \hat H \f$, i.e. \f$ \bigl[\hat c^\dagger_i\bigr]_{B' \leftarrow B} \f$, with shape \f$ \dim(B') \times 
     * \dim(B) \f$ (not necessarily square).
     *
     * @param op_linear_index Linear index \f$ i \f$ of the creation operator.
     * @param sp_index Source subspace index \f$ B \f$.
     * @return Matrix block of the creation operator from subspace \f$ B \f$ to subspace \f$ B' \f$.
     */
    matrix_t const &cdag_matrix(int op_linear_index, int sp_index) const { return cdag_matrices[op_linear_index][sp_index]; }

    /**
     * @brief Get the matrix representation of a monomial operator restricted to source subspace \f$ B \f$.
     *
     * @details For a monomial
     * \f[
     *   \hat m = \hat c^{(\dagger)}_{i_k} \cdots \hat c^{(\dagger)}_{i_1},
     * \f]
     * the action on subspace \f$ B \f$ lands in a single target subspace \f$ B' \f$ (or annihilates \f$ B \f$):
     * \f$ \hat m\, S_B \subseteq S_{B'} \f$. The returned matrix is the corresponding block in the eigenbasis of
     * \f$ \hat H \f$ with shape \f$ \dim(B') \times \dim(B) \f$ (not necessarily square).
     *
     * @param op_vec Monomial (ordered product of fundamental creation/annihilation operators).
     * @param B Source subspace index.
     * @return Pair \f$ (B',\, \mathrm{matrix}) \f$, where the matrix gives the action of the monomial from subspace
     * \f$ B \f$ to subspace \f$ B' \f$. \f$ B' = -1 \f$ if the monomial annihilates \f$ B \f$.
     */
    C2PY_IGNORE std::pair<int, matrix_t> get_matrix_element_of_monomial(operators::monomial_t const &op_vec, int B) const;

    /**
     * @brief Get the block-matrix representation of a generic many-body operator.
     *
     * @details Decomposes the operator into monomials and assembles the resulting matrix blocks in the eigenbasis.
     * Throws if the operator does not respect the block structure used by the diagonalization, i.e. if it maps some
     * subspace to a superposition of subspaces.
     *
     * @param op Many-body operator.
     * @return Block-matrix representation of the operator in the eigenbasis.
     */
    C2PY_IGNORE op_block_mat_t get_op_mat(many_body_op_t const &op) const;

    /// Default equal-to operator compares all data members.
    bool operator==(atom_diag const &rhs) const = default;

    /// Get the HDF5 format tag (`"AtomDiagReal"` or `"AtomDiagComplex"`).
    [[nodiscard]] static std::string hdf5_format() { return Complex ? "AtomDiagComplex" : "AtomDiagReal"; }

    // Friend declarations.
    friend struct atom_diag_worker<Complex>;
    friend std::ostream &operator<< <Complex>(std::ostream &os, atom_diag const &ss);
    friend void h5_write<Complex>(h5::group gr, std::string const &name, atom_diag const &);
    friend void h5_read<Complex>(h5::group gr, std::string const &name, atom_diag &);

    private:
    void fill_first_eigenstate_of_subspace();
    void compute_vacuum();

    private:
    many_body_op_t h_atomic;                                    // Hamiltonian
    fundamental_operator_set fops;                              // Keep it to compute the Green's function
    class hilbert_space full_hs;                                // Full Hilbert space of the problem
    std::vector<sub_hilbert_space> sub_hilbert_spaces;          // The invariant subspaces, i.e. the lists of Fock states
    std::vector<eigensystem_t> eigensystems;                    // Eigensystem in each subspace
    matrix<long> creation_connection;                           // creation_connection(i, B) -> B', target subspace of c†_i acting on B
    matrix<long> annihilation_connection;                       // idem for annihilation operators
    std::vector<std::vector<matrix_t>> cdag_matrices;           // cdag_matrices[i][B] = block of c†_i mapping B -> B'
    std::vector<std::vector<matrix_t>> c_matrices;              // idem for annihilation operators
    double gs_energy{0.0};                                      // Energy of the ground state
    long vacuum_subspace_index{-1};                             // Invariant subspace containing |0>
    full_hilbert_space_state_t vacuum;                          // Vacuum vector (in the eigenbasis)
    std::vector<std::vector<quantum_number_t>> quantum_numbers; // Values of the quantum numbers for each subspace
    std::vector<int> first_eigenstate_of_subspace;              // Index of the first eigenstate of each subspace
    bool truncated_{false};                                     // Flag indicating whether truncation was applied
  };

  /** @} */

} // namespace triqs::atom_diag
