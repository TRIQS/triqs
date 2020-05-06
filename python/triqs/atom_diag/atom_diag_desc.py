##############################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2016, P. Seth, I. Krivenko, M. Ferrero and O. Parcollet
#
# TRIQS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TRIQS. If not, see <http://www.gnu.org/licenses/>.
#
##############################################################################
from cpp2py.wrap_generator import *

module = module_(full_name = "triqs.atom_diag.atom_diag",
                 doc = "Lightweight exact diagonalization solver",
                 app_name = "triqs")

module.add_imports("triqs.gf", "triqs.operators", "h5._h5py")

module.add_preamble("""
#include <cpp2py/converters/pair.hpp>
#include <cpp2py/converters/vector.hpp>
#include <cpp2py/converters/map.hpp>
#include <cpp2py/converters/variant.hpp>

#include <triqs/atom_diag/atom_diag.hpp>
#include <triqs/atom_diag/functions.hpp>
#include <triqs/atom_diag/gf.hpp>

#include <triqs/cpp2py_converters/arrays.hpp>
#include <triqs/cpp2py_converters/gf.hpp>
#include <triqs/cpp2py_converters/operators_real_complex.hpp>
#include <triqs/cpp2py_converters/fundamental_operator_set.hpp>

using namespace triqs::arrays;
using namespace triqs::operators;
using namespace triqs::hilbert_space;
using namespace triqs::atom_diag;
""")

# Wrap class atom_diag
for c_py, c_cpp, in (('Real','false'),('Complex','true')):
    c_type = "triqs::atom_diag::atom_diag<%s>" % c_cpp

    c = class_(
        py_type = "AtomDiag%s" % c_py,
        c_type = c_type,
        is_printable = True,
        hdf5 = True,
        doc = "Lightweight exact diagonalization solver (%s version)" % c_py
    )

    c.add_constructor("(many_body_operator h, fundamental_operator_set fops)",
                      doc = "Reduce a given Hamiltonian to a block-diagonal form and diagonalize it")

    c.add_constructor("(many_body_operator h, fundamental_operator_set fops, std::vector<%s::many_body_op_t> qn_vector)" % c_type,
                      doc = "Reduce a given Hamiltonian to a block-diagonal form and diagonalize it using quantum numbers")

    c.add_method("int get_subspace_dim (int sp_index)", doc = "The dimension of subspace sp_index")

    c.add_method("int flatten_subspace_index (int sp_index, int i)",
                 doc = "Returns the state index in the full Hilbert space given a subspace index and an inner index")

    c.add_method("double get_eigenvalue (int sp_index, int i)",
                 doc = "Get the i-th eigenvalue of subspace sp_index")

    c.add_method("long c_connection (int op_linear_index, int sp_index)",
                 doc = "Subspace-to-subspace connections for fundamental operator C_{op_linear_index}\n"
                       "(returns final subspace for initial subspace sp_index)")

    c.add_method("long cdag_connection (int op_linear_index, int sp_index)",
                 doc = "Subspace-to-subspace connections for fundamental operator C^\\dagger_{op_linear_index}\n"
                       "(returns final subspace for initial subspace sp_index)")

    c.add_method("matrix<%s::scalar_t> c_matrix (int op_linear_index, int sp_index)" % c_type,
                 doc = "Matrix block for fundamental operator C_{op_linear_index}\n"
                       "(sp_index is index of the initial subspace)")

    c.add_method("matrix<%s::scalar_t> cdag_matrix (int op_linear_index, int sp_index)" % c_type,
                 doc = "Matrix block for fundamental operator C^\\dagger_{op_linear_index}\n"
                       "(sp_index is index of the initial subspace)")

    c.add_property(name = "h_atomic",
                   getter = cfunction("%s::many_body_op_t get_h_atomic ()" % c_type),
                   doc = "The Hamiltonian used at construction")

    c.add_property(name = "fops",
                   getter = cfunction("triqs::hilbert_space::fundamental_operator_set get_fops ()"),
                   doc = "The fundamental operator set used at construction")

    c.add_property(name = "full_hilbert_space_dim",
                   getter = cfunction("int get_full_hilbert_space_dim ()"),
                   doc = "Dimension of the full Hilbert space")

    c.add_property(name = "n_subspaces",
                   getter = cfunction("int n_subspaces ()"),
                   doc = "Number of invariant subspaces")

    c.add_property(name = "fock_states",
                   getter = cfunction("std::vector<std::vector<fock_state_t>> get_fock_states ()"),
                   doc = "The list of Fock states for each subspace")

    c.add_property(name = "unitary_matrices",
                   getter = cfunction("std::vector<matrix<%s::scalar_t>> get_unitary_matrices ()" % c_type),
                   doc = "Unitary matrices that transform from Fock states to eigenstates")

    c.add_property(name = "energies",
                   getter = cfunction("std::vector<std::vector<double>> get_energies ()"),
                   doc = "A vector of all the energies, grouped by subspace")

    c.add_property(name = "quantum_numbers",
                   getter = cfunction("std::vector<std::vector<double>> get_quantum_numbers ()"),
                   doc = "A vector of all the quantum numbers, grouped by subspace")

    c.add_property(name = "gs_energy",
                   getter = cfunction("double get_gs_energy ()"),
                   doc = "Ground state energy (i.e. min of all subspaces)")

    c.add_property(name = "vacuum_subspace_index",
                   getter = cfunction("int get_vacuum_subspace_index ()"),
                   doc = "Returns invariant subspace containing the vacuum state")

    c.add_property(name = "vacuum_state",
                   getter = cfunction("%s::full_hilbert_space_state_t get_vacuum_state ()" % c_type),
                   doc = "Returns the vacuum state as a vector in the full Hilbert space")

    module.add_class(c)

# Wrap free functions
for c_py, c_cpp, in (('Real','false'),('Complex','true')):
    c_type = "triqs::atom_diag::atom_diag<%s>" % c_cpp

    module.add_function("double partition_function (%s atom, double beta)" % c_type,
                        doc = "The atomic partition function")

    module.add_function("%s::block_matrix_t atomic_density_matrix (%s atom, double beta)" % (c_type,c_type),
                        doc = "The atomic density matrix")

    module.add_function("%s::scalar_t trace_rho_op (%s::block_matrix_t density_matrix, "
                        "many_body_operator op, %s atom)" % ((c_type,)*3),
                        doc = "Compute Tr (op * density_matrix)")

    module.add_function ("%s::full_hilbert_space_state_t act (many_body_operator op, "
                         "%s::full_hilbert_space_state_t st, %s atom)" % ((c_type,)*3),
                         doc = "Act with operator `op` on state `st`")

    module.add_function ("std::vector<std::vector<double>> quantum_number_eigenvalues "
                         "(many_body_operator op, %s atom)" % c_type,
                         doc = "Compute values of a given quantum number for all eigenstates")

    module.add_function ("std::vector<std::vector<double>> quantum_number_eigenvalues_checked "
                         "(many_body_operator op, %s atom)" % c_type,
                         doc = "Compute values of a given quantum number for all eigenstates, "
                               "checking that `op` is a quantum number indeed.")

    module.add_function ("block_gf<imtime> atomic_g_tau(%s atom, double beta, gf_struct_t gf_struct, int n_tau, "
                         "std::vector<std::pair<int, int>> excluded_states = {})" % c_type,
                         doc = "The atomic imaginary time Green's function, possibly with excluded states (none by default)")

    module.add_function ("block_gf<imfreq> atomic_g_iw(%s atom, double beta, gf_struct_t gf_struct, int n_iw, "
                         "std::vector<std::pair<int, int>> excluded_states = {})" % c_type,
                         doc = "The atomic Matsubara Green's function, possibly with excluded states (none by default)")

    module.add_function ("block_gf<triqs::gfs::legendre> atomic_g_l(%s atom, double beta, gf_struct_t gf_struct, int n_l, "
                         "std::vector<std::pair<int, int>> excluded_states = {})" % c_type,
                         doc = "The atomic Green's function in Legendre representation, possibly with excluded states (none by default)")

    module.add_function ("block_gf<refreq> atomic_g_w(%s atom, double beta, gf_struct_t gf_struct, "
                         "std::pair<double, double> energy_window, int n_w, double broadening = 0, "
                         "std::vector<std::pair<int, int>> excluded_states = {})" % c_type,
                         doc = "The atomic real frequency Green's function, possibly with excluded states (none by default)")

module.generate_code()
