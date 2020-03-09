/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2016, P. Seth, I. Krivenko, M. Ferrero and O. Parcollet
 * Copyright (C) 2018, The Simons Foundation
 * Author: H. U.R. Strand
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

#include <string>
#include <vector>
#include <map>
#include <triqs/utility/exceptions.hpp>
#include <triqs/arrays/vector.hpp>
#include <triqs/arrays/matrix.hpp>
#include <triqs/arrays/h5.hpp>
#include <triqs/operators/many_body_operator.hpp>
#include <triqs/hilbert_space/fundamental_operator_set.hpp>
#include <triqs/hilbert_space/hilbert_space.hpp>

namespace triqs {
  namespace atom_diag {

    using namespace triqs::hilbert_space;
    using namespace triqs::arrays;

    // Forward declarations
    template <bool C> class atom_diag;
    template <bool C> struct atom_diag_worker;
    template <bool C> std::ostream &operator<<(std::ostream &, atom_diag<C> const &);
    template <bool C> void h5_write(h5::group, std::string const &, atom_diag<C> const &);
    template <bool C> void h5_read(h5::group, std::string const &, atom_diag<C> &);

    using indices_t = fundamental_operator_set::indices_t;
    // Quantum number operators are Hermitian, hence their eigenvalues are real
    using quantum_number_t = double;

    /// Lightweight exact diagonalization solver
    /**
     * This class is provided as a simple tool to diagonalize Hamiltonians of
     * finite fermionic systems of a moderate size.
     *
     * @tparam Complex Allow the Hamiltonian to be complex.
     * @include triqs/atom_diag/atom_diag.hpp
     */
    template <bool Complex> class atom_diag {

      friend struct atom_diag_worker<Complex>;
      static constexpr bool is_complex = Complex;

      public:
      /// Type of matrix elements (double/dcomplex)
      using scalar_t = std::conditional_t<Complex, std::complex<double>, double>;
      /// Matrix type
      using matrix_t = matrix<scalar_t>;
      /// Block-diagonal matrix type
      using block_matrix_t = std::vector<matrix_t>;
      /// State vector in the full Hilbert space, written in the eigenbasis of :math:`\hat H`.
      using full_hilbert_space_state_t = vector<scalar_t>;
      /// Many-body operator type used by this atom_diag specialization.
      using many_body_op_t = triqs::operators::many_body_operator_generic<scalar_t>;

      /// Eigensystem within one invariant subspace of the Hamiltonian
      struct eigensystem_t {
        /// Eigenvalues, in ascending order; The ground state energy is set to 0 at initialisation.
        vector<double> eigenvalues;
        /// Unitary transformation matrix :math:`\hat U` from the Fock basis to the eigenbasis.
        /// Defined according to :math:`\hat H = \hat  U \mathrm{diag}(E) * \hat U^\dagger`.
        matrix_t unitary_matrix;

        // HDF5
        static std::string hdf5_format() { return "atom_diag::eigensystem_t"; }

        friend void h5_write(h5::group fg, std::string const &name, eigensystem_t const &es) {
          auto gr = fg.create_group(name);
          h5_write(gr, "eigenvalues", es.eigenvalues);
          h5_write(gr, "unitary_matrix", es.unitary_matrix);
        }
        friend void h5_read(h5::group fg, std::string const &name, eigensystem_t &es) {
          auto gr = fg.open_group(name);
          h5_read(gr, "eigenvalues", es.eigenvalues);
          h5_read(gr, "unitary_matrix", es.unitary_matrix);
        }
      };

      /// Block matrix representation for operators
      struct op_block_mat_t {
	op_block_mat_t(int n_blocks) : connection(n_blocks), block_mat(n_blocks) { connection(range()) = -1; };
	array<long, 1> connection;
	std::vector<matrix_t> block_mat;
	int n_blocks() const { return block_mat.size(); }
	friend std::ostream &operator<<(std::ostream &out, op_block_mat_t const & op_mat) {
	  out << "Operator block matrix:\n"
	      << " n_blocks = " << op_mat.n_blocks() << "\n";
	  for( int bidx : range(op_mat.n_blocks()) ) {
	    auto &m = op_mat.block_mat[bidx];
	    auto b2 = op_mat.connection(bidx);
	    out << "Block: "
		<< "blocks (" << bidx << ", " << b2 << ") "
		<< "size (" << first_dim(m) << ", " << second_dim(m) << ") "
	        << op_mat.block_mat[bidx] << "\n";
	  }
	  return out;
	};
      };

      /// Construct in an uninitialized state.
      TRIQS_CPP2PY_IGNORE atom_diag() = default;

      /// Reduce a given Hamiltonian to a block-diagonal form and diagonalize it
      /**
       * This constructor calls the auto-partition procedure, and the QR algorithm
       * to diagonalize the blocks. The invariant subspaces of the Hamiltonian are
       * chosen such that all creation and annihilation operators from the provided
       * fundamental operator set map one subspace to one subspace.
       *
       * @param h Hamiltonian operator to be diagonalized.
       * @param fops Fundamental operator set; Must at least contain all fundamental operators met in `h`.
       * @note See :ref:`space_partition` for more details on the auto-partition scheme.
       */
      atom_diag(many_body_op_t const &h, fundamental_operator_set const &fops);

      atom_diag(many_body_op_t const &h, fundamental_operator_set const &fops, int n_min, int n_max);
      
      /// Reduce a given Hamiltonian to a block-diagonal form and diagonalize it
      /**
       * This constructor uses quantum number operators to partition the Hilbert space into
       * invariant subspaces, and the QR algorithm to diagonalize the blocks of the Hamiltonian.
       * The quantum numbers must be chosen such that all creation and annihilation operators from
       * the provided fundamental operator set map one subspace to one subspace.
       *
       * @param h Hamiltonian operator to be diagonalized.
       * @param fops Fundamental operator set; Must at least contain all fundamental operators met in `h`.
       * @param qn_vector Vector of quantum number operators.
       */
      atom_diag(many_body_op_t const &h, fundamental_operator_set const &fops, std::vector<many_body_op_t> const &qn_vector);

      /// The Hamiltonian used at construction
      many_body_op_t const &get_h_atomic() const { return h_atomic; }

      /// The fundamental operator set used at construction
      fundamental_operator_set const &get_fops() const { return fops; }

      /// The full Hilbert space
      TRIQS_CPP2PY_IGNORE class hilbert_space const &get_full_hilbert_space() const { return full_hs; }

      /// Dimension of the full Hilbert space
      int get_full_hilbert_space_dim() const { return full_hs.size(); }

      /// Number of invariant subspaces
      int n_subspaces() const { return eigensystems.size(); }

      /// The dimension of a subspace
      /**
       * @param sp_index Index of the invariant subspace.
       */
      int get_subspace_dim(int sp_index) const { return eigensystems[sp_index].eigenvalues.size(); }

      /// The list of Fock states for each subspace
      std::vector<std::vector<fock_state_t>> get_fock_states() const {
        std::vector<std::vector<fock_state_t>> fock_states(n_subspaces());
        for (int i : range(n_subspaces())) fock_states[i] = sub_hilbert_spaces[i].get_all_fock_states();
        return fock_states;
      }

      /// Unitary matrices that transform from Fock states to eigenstates
      std::vector<matrix<scalar_t>> get_unitary_matrices() const {
        std::vector<matrix<scalar_t>> umat(n_subspaces());
        for (int i : range(n_subspaces())) umat[i] = get_eigensystems()[i].unitary_matrix;
        return umat;
      }

      /// Returns the state index in the full Hilbert space given a subspace index and an inner index
      /**
       * @param sp_index Index of the invariant subspace.
       * @param i State index within the subspace.
       */
      int flatten_subspace_index(int sp_index, int i) const { return first_eigenstate_of_subspace[sp_index] + i; }

      /// Return the range of indices of subspace sp_index
      /**
       * @param sp_index Index of the invariant subspace.
       */
      TRIQS_CPP2PY_IGNORE range index_range_of_subspace(int sp_index) const {
        return range{first_eigenstate_of_subspace[sp_index], first_eigenstate_of_subspace[sp_index] + get_subspace_dim(sp_index)};
      }

      /// Get the eigensystems for all subspaces
      TRIQS_CPP2PY_IGNORE std::vector<eigensystem_t> const &get_eigensystems() const { return eigensystems; }

      /// Get the i-th eigenvalue of subspace sp_index
      /**
       * @param sp_index Index of the invariant subspace.
       * @param i State index within the subspace.
       */
      double get_eigenvalue(int sp_index, int i) const { return eigensystems[sp_index].eigenvalues[i]; }

      /// A vector of all the energies, grouped by subspace
      /**
       * @return result[sp_index][i] is the energy.
       */
      std::vector<std::vector<double>> get_energies() const;

      /// A vector of all the quantum numbers, grouped by subspace
      /**
       * @return result[sp_index][qn_index] is the qunatum number value.
       */
      std::vector<std::vector<quantum_number_t>> const &get_quantum_numbers() const { return quantum_numbers; }

      /// Ground state energy (i.e. min of all subspaces)
      double get_gs_energy() const { return gs_energy; }

      /// Returns invariant subspace containing the vacuum state
      long get_vacuum_subspace_index() const { return vacuum_subspace_index; }

      /// Returns the vacuum state as a vector in the full Hilbert space
      /**
       * This vector is written in the eigenbasis of the Hamiltonian.
       */
      full_hilbert_space_state_t const &get_vacuum_state() const { return vacuum; }

      /// Subspace-to-subspace connections for fundamental operator :math:`C`
      /**
       * @param op_linear_index The linear index (i.e. number) of the annihilation operator, as defined by the fundamental operator set.
       * @param sp_index The index of the initial subspace.
       * @return The index of the final subspace.
       */
      long c_connection(int op_linear_index, int sp_index) const { return annihilation_connection(op_linear_index, sp_index); }

      /// Subspace-to-subspace connections for fundamental operator :math:`C^\dagger`
      /** *
       * @param op_linear_index The linear index (i.e. number) of the creation operator, as defined by the fundamental operator set.
       * @param sp_index The index of the initial subspace.
       * @return The index of the final subspace.
       */
      long cdag_connection(int op_linear_index, int sp_index) const { return creation_connection(op_linear_index, sp_index); }

      /// Matrix block for fundamental operator :math:`C`
      /**
       * @param op_linear_index The linear index (i.e. number) of the annihilation operator, as defined by the fundamental operator set.
       * @param sp_index The index of the initial subspace.
       * @return The index of the final subspace.
       */
      matrix_t const &c_matrix(int op_linear_index, int sp_index) const { return c_matrices[op_linear_index][sp_index]; }

      /// Matrix block for fundamental operator :math:`C^\dagger`
      /**
       *
       * @param op_linear_index The linear index (i.e. number) of the creation operator, as defined by the fundamental operator set.
       * @param sp_index The index of the initial subspace.
       * @return The index of the final subspace.
       */
      matrix_t const &cdag_matrix(int op_linear_index, int sp_index) const { return cdag_matrices[op_linear_index][sp_index]; }


      /// Get block matrix representation for general operator
      /**
       * @param op Many body operator
       * @return The block matrix representation of the operator (in the Hamiltonian eigen basis)
       *
       * Throws, in case the provided operator does not respect the block symmetries used in the diagonalization.
       */
      op_block_mat_t get_op_mat(many_body_op_t const &op) const;
      

      private:
      /// ------------------  DATA  -----------------

      many_body_op_t h_atomic;                           // Hamiltonian
      fundamental_operator_set fops;                     // Keep it to compute the Green's function
      class hilbert_space full_hs;                       // Full Hilbert space of the problem
      std::vector<sub_hilbert_space> sub_hilbert_spaces; // The invariant subspaces, i.e. the lists of Fock states
      std::vector<eigensystem_t> eigensystems;           // Eigensystem in each subspace
      matrix<long> creation_connection;                  // creation_connection[operator_linear_index][B] -> B', final subspace
      matrix<long> annihilation_connection;              // idem for annihilation operators
      std::vector<std::vector<matrix_t>> cdag_matrices;  // cdag_matrices[operator_linear_index][B] = matrix from subspace B to subspace B'
      std::vector<std::vector<matrix_t>> c_matrices;     // idem for annihilation operators
      double gs_energy;                                  // Energy of the ground state
      long vacuum_subspace_index;                        // Invariant subspace containing |0>
      full_hilbert_space_state_t vacuum;                 // Vacuum vector (in the eigenbasis)

      std::vector<std::vector<quantum_number_t>> quantum_numbers; // Values of the quantum numbers for each subspace

      std::vector<int> first_eigenstate_of_subspace; // Index of the first eigenstate of each subspace
      void fill_first_eigenstate_of_subspace();
      void compute_vacuum();
      std::pair<int, matrix_t> get_matrix_element_of_monomial(operators::monomial_t const &op_vec, int B) const;

      /* Friend declarations of a template class are a bit ugly */
      friend std::ostream &operator<<<Complex>(std::ostream &os, atom_diag const &ss);
      friend void h5_write<Complex>(h5::group gr, std::string const &name, atom_diag const &);
      friend void h5_read<Complex>(h5::group gr, std::string const &name, atom_diag &);

      public:
      static std::string hdf5_format() { return Complex ? "AtomDiagComplex" : "AtomDiagReal"; }
    };

  } // namespace atom_diag
} // namespace triqs
