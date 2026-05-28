
// C.f. https://numpy.org/doc/1.21/reference/c-api/array.html#importing-the-api
#define PY_ARRAY_UNIQUE_SYMBOL _cpp2py_ARRAY_API
#ifndef CLAIR_C2PY_WRAP_GEN
#ifdef __clang__
// #pragma clang diagnostic ignored "-W#warnings"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wcast-function-type"
#pragma GCC diagnostic ignored "-Wcpp"
#endif

#define C2PY_VERSION_MAJOR 0
#define C2PY_VERSION_MINOR 1

#include <c2py/c2py.hpp>
#include <c2py/serialization/h5.hpp>

using c2py::operator""_a;

// ==================== enums =====================

// ==================== module classes =====================

// --------- class _c2py_cls_0 -----------
using _c2py_cls_0                                            = triqs::atom_diag::atom_diag<false>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs.atom_diag.atom_diag.AtomDiagReal";
static const auto _c2py_init_0                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_0, const triqs::atom_diag::atom_diag<false>::many_body_op_t &,
                       const triqs::hilbert_space::fundamental_operator_set &>("h", "fops"),
   c2py::c_constructor<_c2py_cls_0, const triqs::atom_diag::atom_diag<false>::many_body_op_t &,
                       const triqs::hilbert_space::fundamental_operator_set &, const triqs::atom_diag::atom_diag<false>::many_body_op_t &>(
      "h", "fops", "hyb"),
   c2py::c_constructor<_c2py_cls_0, const triqs::atom_diag::atom_diag<false>::many_body_op_t &,
                       const triqs::hilbert_space::fundamental_operator_set &, int, int>("h", "fops", "n_min", "n_max"),
   c2py::c_constructor<_c2py_cls_0, const triqs::atom_diag::atom_diag<false>::many_body_op_t &,
                       const triqs::hilbert_space::fundamental_operator_set &,
                       const std::vector<triqs::atom_diag::atom_diag<false>::many_body_op_t> &>("h", "fops", "qn_vector")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_0> = c2py::pyfkw_constructor<_c2py_init_0>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_0> =
   _c2py_init_0.doc(R"DOC(
[1] Reduce a Hamiltonian to a block-diagonal form using auto-partitioning, then diagonalize the blocks.

Uses the auto-partition procedure to detect the invariant subspaces of the Hamiltonian, and the QR
algorithm to diagonalize each block. The invariant subspaces are chosen such that every fundamental creation
and annihilation operator from the provided fundamental operator set maps each subspace to a single subspace
(or annihilates it).

------

[2] Reduce a Hamiltonian to a block-diagonal form using auto-partitioning refined by a hybridization term.

Behaves like the two-argument auto-partition constructor, but the partition is required to remain
invariant under the additional many-body operator :math:`\hat V` as well. This is useful when the Hamiltonian 
on its own would yield invariant subspaces that mix when an extra (e.g. hybridization) operator acts, leading 
to matrix blocks that are coarser than what :math:`\hat H` alone would suggest.

------

[3] Diagonalize a Hamiltonian restricted to a particle-number window.

Builds the invariant subspaces by total particle number and keeps only those whose number of particles
lies in the inclusive window :math:`[n_{\text{min}}, n_{\text{max}}]`. The blocks are then diagonalized with
the QR algorithm. Convenient when only a few sectors of fixed occupation are physically relevant.

------

[4] Reduce a Hamiltonian to a block-diagonal form using user-supplied quantum numbers, then diagonalize the
blocks.

Partitions the Hilbert space into common eigenspaces of the provided quantum-number operators. The
quantum numbers must be chosen such that every fundamental creation and annihilation operator from the provided
fundamental operator set maps each common eigenspace to a single common eigenspace (or annihilates it). Each
block of the Hamiltonian is then diagonalized with the QR algorithm.

------

Parameters
----------
h : {par_0}
   Many-body Hamiltonian :math:`\hat H` to be diagonalized.
fops : {par_1}
   Fundamental operator set; must at least contain every fundamental operator appearing in 
   :math:`\hat H`.
hyb : {par_2}
   Additional many-body operator :math:`\hat V` that the auto-partition must respect; every fundamental 
   operator appearing in :math:`\hat V` must also belong to the fundamental operator set.
n_min : {par_3}
   Minimum total particle number to keep.
n_max : {par_4}
   Maximum total particle number to keep.
qn_vector : {par_5}
   List of quantum-number operators.
)DOC",
                    {{c2py::python_typename<const triqs::atom_diag::atom_diag<false>::many_body_op_t &>()},
                     {c2py::python_typename<const triqs::hilbert_space::fundamental_operator_set &>()},
                     {c2py::python_typename<const triqs::atom_diag::atom_diag<false>::many_body_op_t &>()},
                     {c2py::python_typename<int>()},
                     {c2py::python_typename<int>()},
                     {c2py::python_typename<const std::vector<triqs::atom_diag::atom_diag<false>::many_body_op_t> &>()}});
// c_connection
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_0 const &self, int op_linear_index, int sp_index) -> decltype(auto) { return self.c_connection(op_linear_index, sp_index); }, "self",
   "op_linear_index", "sp_index")};

// c_matrix
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_0 const &self, int op_linear_index, int sp_index) -> decltype(auto) { return self.c_matrix(op_linear_index, sp_index); }, "self",
   "op_linear_index", "sp_index")};

// cdag_connection
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_0 const &self, int op_linear_index, int sp_index) -> decltype(auto) { return self.cdag_connection(op_linear_index, sp_index); },
   "self", "op_linear_index", "sp_index")};

// cdag_matrix
static auto const _c2py_fun_3 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_0 const &self, int op_linear_index, int sp_index) -> decltype(auto) { return self.cdag_matrix(op_linear_index, sp_index); }, "self",
   "op_linear_index", "sp_index")};

// flatten_subspace_index
static auto const _c2py_fun_4 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_0 const &self, int sp_index, int i) -> decltype(auto) { return self.flatten_subspace_index(sp_index, i); }, "self", "sp_index", "i")};

// get_eigenvalue
static auto const _c2py_fun_5 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_0 const &self, int sp_index, int i) -> decltype(auto) { return self.get_eigenvalue(sp_index, i); }, "self", "sp_index", "i")};

// get_subspace_dim
static auto const _c2py_fun_6 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_0 const &self, int sp_index) -> decltype(auto) { return self.get_subspace_dim(sp_index); }, "self", "sp_index")};

// get_subspace_dims
static auto const _c2py_fun_7 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 const &self) -> decltype(auto) { return self.get_subspace_dims(); }, "self")};

// get_unitary_matrix
static auto const _c2py_fun_8 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_0 const &self, int sp_index) -> decltype(auto) { return self.get_unitary_matrix(sp_index); }, "self", "sp_index")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC(
Get the target subspace :math:`B'` of the annihilation operator :math:`\hat c_i` acting on subspace
:math:`B`.

.. math::

   \hat c_i\, S_B \subseteq S_{B'} \; ,

with :math:`B' = -1` if :math:`\hat c_i` annihilates :math:`B`. The operator :math:`\hat c_i` is identified by
its linear index :math:`i` in the fundamental operator set provided at construction.

Parameters
----------
op_linear_index : {par_0}
   Linear index :math:`i` of the annihilation operator.
sp_index : {par_1}
   Source subspace index :math:`B`.

Returns
-------
{ret_0}
   Target subspace index :math:`B'`, or :math:`-1` if the operator annihilates the source subspace.
)DOC",
                                                {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_1 = _c2py_fun_1.doc(R"DOC(
Get the matrix block of the annihilation operator :math:`\hat c_i` acting on subspace :math:`B`.

The returned matrix is the representation of :math:`\hat c_i` in the eigenbasis of :math:`\hat H`, 
i.e. :math:`\bigl[\hat c_i\bigr]_{B' \leftarrow B}`, with shape :math:`\dim(B') \times \dim(B)` (not 
necessarily square).

Parameters
----------
op_linear_index : {par_0}
   Linear index :math:`i` of the annihilation operator.
sp_index : {par_1}
   Source subspace index :math:`B`.

Returns
-------
{ret_0}
   Matrix block of the annihilation operator from subspace :math:`B` to subspace :math:`B'`.
)DOC",
                                                {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}},
                                                {c2py::python_typename<const triqs::atom_diag::atom_diag<false>::matrix_t &>()});
static const auto _c2py_doc_2 = _c2py_fun_2.doc(R"DOC(
Get the target subspace :math:`B'` of the creation operator :math:`\hat c^\dagger_i` acting on subspace
:math:`B`.

.. math::

   \hat c^\dagger_i\, S_B \subseteq S_{B'} \; ,

with :math:`B' = -1` if :math:`\hat c^\dagger_i` annihilates :math:`B`. The operator :math:`\hat c^\dagger_i`
is identified by its linear index :math:`i` in the fundamental operator set provided at construction.

Parameters
----------
op_linear_index : {par_0}
   Linear index :math:`i` of the creation operator.
sp_index : {par_1}
   Source subspace index :math:`B`.

Returns
-------
{ret_0}
   Target subspace index :math:`B'`, or :math:`-1` if the operator annihilates the source subspace.
)DOC",
                                                {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_3 = _c2py_fun_3.doc(R"DOC(
Get the matrix block of the creation operator :math:`\hat c^\dagger_i` acting on subspace :math:`B`.

The returned matrix is the representation of :math:`\hat c^\dagger_i` in the eigenbasis of
:math:`\hat H`, i.e. :math:`\bigl[\hat c^\dagger_i\bigr]_{B' \leftarrow B}`, with shape :math:`\dim(B') \times
\dim(B)` (not necessarily square).

Parameters
----------
op_linear_index : {par_0}
   Linear index :math:`i` of the creation operator.
sp_index : {par_1}
   Source subspace index :math:`B`.

Returns
-------
{ret_0}
   Matrix block of the creation operator from subspace :math:`B` to subspace :math:`B'`.
)DOC",
                                                {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}},
                                                {c2py::python_typename<const triqs::atom_diag::atom_diag<false>::matrix_t &>()});
static const auto _c2py_doc_4 = _c2py_fun_4.doc(R"DOC(
Map a subspace-local pair :math:`(B, i)` to its linear index in the full Hilbert space.

The full-Hilbert-space eigenstate index is

.. math::

   d(B, i) = \mathtt{first\_eigenstate\_of\_subspace}[B] + i,
   \quad 0 \le i < \dim(B), \quad 0 \le d < N.

Parameters
----------
sp_index : {par_0}
   Subspace index :math:`B`.
i : {par_1}
   Eigenstate index inside subspace :math:`B`, with :math:`0 \le i < \dim(B)`.

Returns
-------
{ret_0}
   Linear eigenstate index :math:`d(B, i)` in the eigenbasis of the full Hilbert space.
)DOC",
                                                {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}}, {c2py::python_typename<int>()});
static const auto _c2py_doc_5 = _c2py_fun_5.doc(R"DOC(
Get the eigenvalue :math:`E_{B,i}` of the Hamiltonian.

Parameters
----------
sp_index : {par_0}
   Subspace index :math:`B`.
i : {par_1}
   Eigenstate index inside subspace :math:`B`, with :math:`0 \le i < \dim(B)`.

Returns
-------
{ret_0}
   Eigenvalue :math:`E_{B,i}`, with the global ground-state energy subtracted.
)DOC",
                                                {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}}, {c2py::python_typename<double>()});
static const auto _c2py_doc_6 = _c2py_fun_6.doc(R"DOC(
Get the dimension :math:`\dim(B)` of invariant subspace :math:`B`.

Parameters
----------
sp_index : {par_0}
   Subspace index :math:`B`.

Returns
-------
{ret_0}
   Number of eigenstates in subspace :math:`B`.
)DOC",
                                                {{c2py::python_typename<int>()}}, {c2py::python_typename<int>()});
static const auto _c2py_doc_7 = _c2py_fun_7.doc(R"DOC(
Get the dimensions :math:`\dim(B)` of all invariant subspaces.

Returns
-------
{ret_0}
   List of subspace dimensions, indexed by subspace index :math:`B`.
)DOC",
                                                {}, {c2py::python_typename<std::vector<int>>()});
static const auto _c2py_doc_8 = _c2py_fun_8.doc(
   R"DOC(
Get the unitary matrix :math:`U_B` mapping the Fock basis of subspace :math:`B` to its eigenbasis.

Parameters
----------
sp_index : {par_0}
   Subspace index :math:`B`.

Returns
-------
{ret_0}
   Unitary matrix :math:`U_B` such that :math:`H_B = U_B\, \mathrm{diag}(E_B)\, U_B^\dagger`
   within the subspace.
)DOC",
   {{c2py::python_typename<int>()}},
   {c2py::python_typename<
      const nda::basic_array<double, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {
   {"c_connection", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"c_matrix", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"cdag_connection", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"cdag_matrix", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {"flatten_subspace_index", (PyCFunction)c2py::pyfkw<_c2py_fun_4>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_4.c_str()},
   {"get_eigenvalue", (PyCFunction)c2py::pyfkw<_c2py_fun_5>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_5.c_str()},
   {"get_subspace_dim", (PyCFunction)c2py::pyfkw<_c2py_fun_6>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_6.c_str()},
   {"get_subspace_dims", (PyCFunction)c2py::pyfkw<_c2py_fun_7>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_7.c_str()},
   {"get_unitary_matrix", (PyCFunction)c2py::pyfkw<_c2py_fun_8>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_8.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_0>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_h5<_c2py_cls_0>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_h5<_c2py_cls_0>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0  = R"DOC(Get all eigenvalues :math:`E_{B,i}` grouped by invariant subspace.)DOC";
static constexpr auto prop_doc_1  = R"DOC(Get the Fock states of every invariant subspace.)DOC";
static constexpr auto prop_doc_2  = R"DOC(Get the data of the fundamental operator set used at construction.)DOC";
static constexpr auto prop_doc_3  = R"DOC(Get the dimension of the full Hilbert space.)DOC";
static constexpr auto prop_doc_4  = R"DOC(Get the ground-state energy, i.e. the minimum eigenvalue across all invariant subspaces.)DOC";
static constexpr auto prop_doc_5  = R"DOC(Get the Hamiltonian used at construction as a generic many-body operator.)DOC";
static constexpr auto prop_doc_6  = R"DOC(Get the number of invariant subspaces produced by the chosen partitioning scheme.)DOC";
static constexpr auto prop_doc_7  = R"DOC(Get the values of all quantum-number operators, grouped by invariant subspace.)DOC";
static constexpr auto prop_doc_8  = R"DOC(Get the unitary matrices :math:`U_B` for every invariant subspace.)DOC";
static constexpr auto prop_doc_9  = R"DOC(Get the vacuum state as a vector in the full Hilbert space.

The returned vector is expressed in the eigenbasis of the Hamiltonian.)DOC";
static constexpr auto prop_doc_10 = R"DOC(Get the index of the invariant subspace containing the vacuum state.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_0>[] = {

   {"energies", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<false>::get_energies)>, nullptr, prop_doc_0, nullptr},
   {"fock_states", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<false>::get_fock_states)>, nullptr, prop_doc_1, nullptr},
   {"fops", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<false>::get_fops_as_data)>, nullptr, prop_doc_2, nullptr},
   {"full_hilbert_space_dim", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<false>::get_full_hilbert_space_dim)>, nullptr,
    prop_doc_3, nullptr},
   {"gs_energy", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<false>::get_gs_energy)>, nullptr, prop_doc_4, nullptr},
   {"h_atomic", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<false>::get_h_atomic_as_mbop)>, nullptr, prop_doc_5, nullptr},
   {"n_subspaces", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<false>::n_subspaces)>, nullptr, prop_doc_6, nullptr},
   {"quantum_numbers", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<false>::get_quantum_numbers)>, nullptr, prop_doc_7,
    nullptr},
   {"unitary_matrices", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<false>::get_unitary_matrices)>, nullptr, prop_doc_8,
    nullptr},
   {"vacuum_state", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<false>::get_vacuum_state)>, nullptr, prop_doc_9, nullptr},
   {"vacuum_subspace_index", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<false>::get_vacuum_subspace_index)>, nullptr,
    prop_doc_10, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_0> = R"DOC(Lightweight exact diagonalization solver for finite fermionic Hamiltonians.

Perform exact diagonalization of a many-body Hamiltonian :math:`\hat H` acting on the Fock space of a 
finite set of fermionic single-particle states. The Hilbert space is split into invariant subspaces of 
:math:`\hat H`, each of which is diagonalized independently.

After construction the solver exposes

- the eigenvalues :math:`E_B` and unitary matrix :math:`U_B` of every invariant subspace :math:`B`,
- the matrix blocks of every fundamental creation/annihilation operator :math:`\hat c_i, \hat c^\dagger_i` in the
  eigenbasis, where :math:`i` is the linear index of the operator in the fundamental operator set,
- the subspace-to-subspace connections induced by every fundamental creation/annihilation operator, i.e. the
  index of the subspace that each operator maps a given subspace to (or :math:`-1` if it annihilates it),
- convenience routines that turn a generic many-body operator into a block-matrix representation in the
  eigenbasis.

Two specializations are provided, one for real-valued and one for complex-valued Hamiltonians.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_0>;
// --------- class _c2py_cls_1 -----------
using _c2py_cls_1                                            = triqs::atom_diag::atom_diag<true>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_1>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_1> = "triqs.atom_diag.atom_diag.AtomDiagComplex";
static const auto _c2py_init_1                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_1, const triqs::atom_diag::atom_diag<true>::many_body_op_t &,
                       const triqs::hilbert_space::fundamental_operator_set &>("h", "fops"),
   c2py::c_constructor<_c2py_cls_1, const triqs::atom_diag::atom_diag<true>::many_body_op_t &, const triqs::hilbert_space::fundamental_operator_set &,
                       const triqs::atom_diag::atom_diag<true>::many_body_op_t &>("h", "fops", "hyb"),
   c2py::c_constructor<_c2py_cls_1, const triqs::atom_diag::atom_diag<true>::many_body_op_t &, const triqs::hilbert_space::fundamental_operator_set &,
                       int, int>("h", "fops", "n_min", "n_max"),
   c2py::c_constructor<_c2py_cls_1, const triqs::atom_diag::atom_diag<true>::many_body_op_t &, const triqs::hilbert_space::fundamental_operator_set &,
                       const std::vector<triqs::atom_diag::atom_diag<true>::many_body_op_t> &>("h", "fops", "qn_vector")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_1> = c2py::pyfkw_constructor<_c2py_init_1>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_1> =
   _c2py_init_1.doc(R"DOC(
[1] Reduce a Hamiltonian to a block-diagonal form using auto-partitioning, then diagonalize the blocks.

Uses the auto-partition procedure to detect the invariant subspaces of the Hamiltonian, and the QR
algorithm to diagonalize each block. The invariant subspaces are chosen such that every fundamental creation
and annihilation operator from the provided fundamental operator set maps each subspace to a single subspace
(or annihilates it).

------

[2] Reduce a Hamiltonian to a block-diagonal form using auto-partitioning refined by a hybridization term.

Behaves like the two-argument auto-partition constructor, but the partition is required to remain
invariant under the additional many-body operator :math:`\hat V` as well. This is useful when the Hamiltonian 
on its own would yield invariant subspaces that mix when an extra (e.g. hybridization) operator acts, leading 
to matrix blocks that are coarser than what :math:`\hat H` alone would suggest.

------

[3] Diagonalize a Hamiltonian restricted to a particle-number window.

Builds the invariant subspaces by total particle number and keeps only those whose number of particles
lies in the inclusive window :math:`[n_{\text{min}}, n_{\text{max}}]`. The blocks are then diagonalized with
the QR algorithm. Convenient when only a few sectors of fixed occupation are physically relevant.

------

[4] Reduce a Hamiltonian to a block-diagonal form using user-supplied quantum numbers, then diagonalize the
blocks.

Partitions the Hilbert space into common eigenspaces of the provided quantum-number operators. The
quantum numbers must be chosen such that every fundamental creation and annihilation operator from the provided
fundamental operator set maps each common eigenspace to a single common eigenspace (or annihilates it). Each
block of the Hamiltonian is then diagonalized with the QR algorithm.

------

Parameters
----------
h : {par_0}
   Many-body Hamiltonian :math:`\hat H` to be diagonalized.
fops : {par_1}
   Fundamental operator set; must at least contain every fundamental operator appearing in 
   :math:`\hat H`.
hyb : {par_2}
   Additional many-body operator :math:`\hat V` that the auto-partition must respect; every fundamental 
   operator appearing in :math:`\hat V` must also belong to the fundamental operator set.
n_min : {par_3}
   Minimum total particle number to keep.
n_max : {par_4}
   Maximum total particle number to keep.
qn_vector : {par_5}
   List of quantum-number operators.
)DOC",
                    {{c2py::python_typename<const triqs::atom_diag::atom_diag<true>::many_body_op_t &>()},
                     {c2py::python_typename<const triqs::hilbert_space::fundamental_operator_set &>()},
                     {c2py::python_typename<const triqs::atom_diag::atom_diag<true>::many_body_op_t &>()},
                     {c2py::python_typename<int>()},
                     {c2py::python_typename<int>()},
                     {c2py::python_typename<const std::vector<triqs::atom_diag::atom_diag<true>::many_body_op_t> &>()}});
// c_connection
static auto const _c2py_fun_9 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_1 const &self, int op_linear_index, int sp_index) -> decltype(auto) { return self.c_connection(op_linear_index, sp_index); }, "self",
   "op_linear_index", "sp_index")};

// c_matrix
static auto const _c2py_fun_10 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_1 const &self, int op_linear_index, int sp_index) -> decltype(auto) { return self.c_matrix(op_linear_index, sp_index); }, "self",
   "op_linear_index", "sp_index")};

// cdag_connection
static auto const _c2py_fun_11 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_1 const &self, int op_linear_index, int sp_index) -> decltype(auto) { return self.cdag_connection(op_linear_index, sp_index); },
   "self", "op_linear_index", "sp_index")};

// cdag_matrix
static auto const _c2py_fun_12 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_1 const &self, int op_linear_index, int sp_index) -> decltype(auto) { return self.cdag_matrix(op_linear_index, sp_index); }, "self",
   "op_linear_index", "sp_index")};

// flatten_subspace_index
static auto const _c2py_fun_13 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_1 const &self, int sp_index, int i) -> decltype(auto) { return self.flatten_subspace_index(sp_index, i); }, "self", "sp_index", "i")};

// get_eigenvalue
static auto const _c2py_fun_14 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_1 const &self, int sp_index, int i) -> decltype(auto) { return self.get_eigenvalue(sp_index, i); }, "self", "sp_index", "i")};

// get_subspace_dim
static auto const _c2py_fun_15 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_1 const &self, int sp_index) -> decltype(auto) { return self.get_subspace_dim(sp_index); }, "self", "sp_index")};

// get_subspace_dims
static auto const _c2py_fun_16 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_1 const &self) -> decltype(auto) { return self.get_subspace_dims(); }, "self")};

// get_unitary_matrix
static auto const _c2py_fun_17 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_1 const &self, int sp_index) -> decltype(auto) { return self.get_unitary_matrix(sp_index); }, "self", "sp_index")};

static const auto _c2py_doc_9  = _c2py_fun_9.doc(R"DOC(
Get the target subspace :math:`B'` of the annihilation operator :math:`\hat c_i` acting on subspace
:math:`B`.

.. math::

   \hat c_i\, S_B \subseteq S_{B'} \; ,

with :math:`B' = -1` if :math:`\hat c_i` annihilates :math:`B`. The operator :math:`\hat c_i` is identified by
its linear index :math:`i` in the fundamental operator set provided at construction.

Parameters
----------
op_linear_index : {par_0}
   Linear index :math:`i` of the annihilation operator.
sp_index : {par_1}
   Source subspace index :math:`B`.

Returns
-------
{ret_0}
   Target subspace index :math:`B'`, or :math:`-1` if the operator annihilates the source subspace.
)DOC",
                                                 {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_10 = _c2py_fun_10.doc(R"DOC(
Get the matrix block of the annihilation operator :math:`\hat c_i` acting on subspace :math:`B`.

The returned matrix is the representation of :math:`\hat c_i` in the eigenbasis of :math:`\hat H`, 
i.e. :math:`\bigl[\hat c_i\bigr]_{B' \leftarrow B}`, with shape :math:`\dim(B') \times \dim(B)` (not 
necessarily square).

Parameters
----------
op_linear_index : {par_0}
   Linear index :math:`i` of the annihilation operator.
sp_index : {par_1}
   Source subspace index :math:`B`.

Returns
-------
{ret_0}
   Matrix block of the annihilation operator from subspace :math:`B` to subspace :math:`B'`.
)DOC",
                                                  {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}},
                                                  {c2py::python_typename<const triqs::atom_diag::atom_diag<true>::matrix_t &>()});
static const auto _c2py_doc_11 = _c2py_fun_11.doc(R"DOC(
Get the target subspace :math:`B'` of the creation operator :math:`\hat c^\dagger_i` acting on subspace
:math:`B`.

.. math::

   \hat c^\dagger_i\, S_B \subseteq S_{B'} \; ,

with :math:`B' = -1` if :math:`\hat c^\dagger_i` annihilates :math:`B`. The operator :math:`\hat c^\dagger_i`
is identified by its linear index :math:`i` in the fundamental operator set provided at construction.

Parameters
----------
op_linear_index : {par_0}
   Linear index :math:`i` of the creation operator.
sp_index : {par_1}
   Source subspace index :math:`B`.

Returns
-------
{ret_0}
   Target subspace index :math:`B'`, or :math:`-1` if the operator annihilates the source subspace.
)DOC",
                                                  {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_12 = _c2py_fun_12.doc(R"DOC(
Get the matrix block of the creation operator :math:`\hat c^\dagger_i` acting on subspace :math:`B`.

The returned matrix is the representation of :math:`\hat c^\dagger_i` in the eigenbasis of
:math:`\hat H`, i.e. :math:`\bigl[\hat c^\dagger_i\bigr]_{B' \leftarrow B}`, with shape :math:`\dim(B') \times
\dim(B)` (not necessarily square).

Parameters
----------
op_linear_index : {par_0}
   Linear index :math:`i` of the creation operator.
sp_index : {par_1}
   Source subspace index :math:`B`.

Returns
-------
{ret_0}
   Matrix block of the creation operator from subspace :math:`B` to subspace :math:`B'`.
)DOC",
                                                  {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}},
                                                  {c2py::python_typename<const triqs::atom_diag::atom_diag<true>::matrix_t &>()});
static const auto _c2py_doc_13 = _c2py_fun_13.doc(R"DOC(
Map a subspace-local pair :math:`(B, i)` to its linear index in the full Hilbert space.

The full-Hilbert-space eigenstate index is

.. math::

   d(B, i) = \mathtt{first\_eigenstate\_of\_subspace}[B] + i,
   \quad 0 \le i < \dim(B), \quad 0 \le d < N.

Parameters
----------
sp_index : {par_0}
   Subspace index :math:`B`.
i : {par_1}
   Eigenstate index inside subspace :math:`B`, with :math:`0 \le i < \dim(B)`.

Returns
-------
{ret_0}
   Linear eigenstate index :math:`d(B, i)` in the eigenbasis of the full Hilbert space.
)DOC",
                                                  {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}}, {c2py::python_typename<int>()});
static const auto _c2py_doc_14 =
   _c2py_fun_14.doc(R"DOC(
Get the eigenvalue :math:`E_{B,i}` of the Hamiltonian.

Parameters
----------
sp_index : {par_0}
   Subspace index :math:`B`.
i : {par_1}
   Eigenstate index inside subspace :math:`B`, with :math:`0 \le i < \dim(B)`.

Returns
-------
{ret_0}
   Eigenvalue :math:`E_{B,i}`, with the global ground-state energy subtracted.
)DOC",
                    {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}}, {c2py::python_typename<double>()});
static const auto _c2py_doc_15 = _c2py_fun_15.doc(R"DOC(
Get the dimension :math:`\dim(B)` of invariant subspace :math:`B`.

Parameters
----------
sp_index : {par_0}
   Subspace index :math:`B`.

Returns
-------
{ret_0}
   Number of eigenstates in subspace :math:`B`.
)DOC",
                                                  {{c2py::python_typename<int>()}}, {c2py::python_typename<int>()});
static const auto _c2py_doc_16 = _c2py_fun_16.doc(R"DOC(
Get the dimensions :math:`\dim(B)` of all invariant subspaces.

Returns
-------
{ret_0}
   List of subspace dimensions, indexed by subspace index :math:`B`.
)DOC",
                                                  {}, {c2py::python_typename<std::vector<int>>()});
static const auto _c2py_doc_17 = _c2py_fun_17.doc(
   R"DOC(
Get the unitary matrix :math:`U_B` mapping the Fock basis of subspace :math:`B` to its eigenbasis.

Parameters
----------
sp_index : {par_0}
   Subspace index :math:`B`.

Returns
-------
{ret_0}
   Unitary matrix :math:`U_B` such that :math:`H_B = U_B\, \mathrm{diag}(E_B)\, U_B^\dagger`
   within the subspace.
)DOC",
   {{c2py::python_typename<int>()}},
   {c2py::python_typename<
      const nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_1>[] = {
   {"c_connection", (PyCFunction)c2py::pyfkw<_c2py_fun_9>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_9.c_str()},
   {"c_matrix", (PyCFunction)c2py::pyfkw<_c2py_fun_10>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_10.c_str()},
   {"cdag_connection", (PyCFunction)c2py::pyfkw<_c2py_fun_11>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_11.c_str()},
   {"cdag_matrix", (PyCFunction)c2py::pyfkw<_c2py_fun_12>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_12.c_str()},
   {"flatten_subspace_index", (PyCFunction)c2py::pyfkw<_c2py_fun_13>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_13.c_str()},
   {"get_eigenvalue", (PyCFunction)c2py::pyfkw<_c2py_fun_14>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_14.c_str()},
   {"get_subspace_dim", (PyCFunction)c2py::pyfkw<_c2py_fun_15>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_15.c_str()},
   {"get_subspace_dims", (PyCFunction)c2py::pyfkw<_c2py_fun_16>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_16.c_str()},
   {"get_unitary_matrix", (PyCFunction)c2py::pyfkw<_c2py_fun_17>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_17.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_1>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_h5<_c2py_cls_1>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_h5<_c2py_cls_1>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_11 = R"DOC(Get all eigenvalues :math:`E_{B,i}` grouped by invariant subspace.)DOC";
static constexpr auto prop_doc_12 = R"DOC(Get the Fock states of every invariant subspace.)DOC";
static constexpr auto prop_doc_13 = R"DOC(Get the data of the fundamental operator set used at construction.)DOC";
static constexpr auto prop_doc_14 = R"DOC(Get the dimension of the full Hilbert space.)DOC";
static constexpr auto prop_doc_15 = R"DOC(Get the ground-state energy, i.e. the minimum eigenvalue across all invariant subspaces.)DOC";
static constexpr auto prop_doc_16 = R"DOC(Get the Hamiltonian used at construction as a generic many-body operator.)DOC";
static constexpr auto prop_doc_17 = R"DOC(Get the number of invariant subspaces produced by the chosen partitioning scheme.)DOC";
static constexpr auto prop_doc_18 = R"DOC(Get the values of all quantum-number operators, grouped by invariant subspace.)DOC";
static constexpr auto prop_doc_19 = R"DOC(Get the unitary matrices :math:`U_B` for every invariant subspace.)DOC";
static constexpr auto prop_doc_20 = R"DOC(Get the vacuum state as a vector in the full Hilbert space.

The returned vector is expressed in the eigenbasis of the Hamiltonian.)DOC";
static constexpr auto prop_doc_21 = R"DOC(Get the index of the invariant subspace containing the vacuum state.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_1>[] = {

   {"energies", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<true>::get_energies)>, nullptr, prop_doc_11, nullptr},
   {"fock_states", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<true>::get_fock_states)>, nullptr, prop_doc_12, nullptr},
   {"fops", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<true>::get_fops_as_data)>, nullptr, prop_doc_13, nullptr},
   {"full_hilbert_space_dim", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<true>::get_full_hilbert_space_dim)>, nullptr,
    prop_doc_14, nullptr},
   {"gs_energy", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<true>::get_gs_energy)>, nullptr, prop_doc_15, nullptr},
   {"h_atomic", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<true>::get_h_atomic_as_mbop)>, nullptr, prop_doc_16, nullptr},
   {"n_subspaces", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<true>::n_subspaces)>, nullptr, prop_doc_17, nullptr},
   {"quantum_numbers", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<true>::get_quantum_numbers)>, nullptr, prop_doc_18,
    nullptr},
   {"unitary_matrices", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<true>::get_unitary_matrices)>, nullptr, prop_doc_19,
    nullptr},
   {"vacuum_state", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<true>::get_vacuum_state)>, nullptr, prop_doc_20, nullptr},
   {"vacuum_subspace_index", c2py::getter_from_method<c2py::castmc<>(&triqs::atom_diag::atom_diag<true>::get_vacuum_subspace_index)>, nullptr,
    prop_doc_21, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_1> = R"DOC(Lightweight exact diagonalization solver for finite fermionic Hamiltonians.

Perform exact diagonalization of a many-body Hamiltonian :math:`\hat H` acting on the Fock space of a 
finite set of fermionic single-particle states. The Hilbert space is split into invariant subspaces of 
:math:`\hat H`, each of which is diagonalized independently.

After construction the solver exposes

- the eigenvalues :math:`E_B` and unitary matrix :math:`U_B` of every invariant subspace :math:`B`,
- the matrix blocks of every fundamental creation/annihilation operator :math:`\hat c_i, \hat c^\dagger_i` in the
  eigenbasis, where :math:`i` is the linear index of the operator in the fundamental operator set,
- the subspace-to-subspace connections induced by every fundamental creation/annihilation operator, i.e. the
  index of the subspace that each operator maps a given subspace to (or :math:`-1` if it annihilates it),
- convenience routines that turn a generic many-body operator into a block-matrix representation in the
  eigenbasis.

Two specializations are provided, one for real-valued and one for complex-valued Hamiltonians.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_1>;

// ==================== module functions ====================

// act
static auto const _c2py_fun_18 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const typename triqs::atom_diag::atom_diag<false>::many_body_op_t &op,
                                         const typename triqs::atom_diag::atom_diag<false>::full_hilbert_space_state_t &st,
                                         const triqs::atom_diag::atom_diag<0> &atom) { return triqs::atom_diag::act<false>(op, st, atom); },
                                      "op", "st", "atom"),
                           c2py::cfun([](const typename triqs::atom_diag::atom_diag<true>::many_body_op_t &op,
                                         const typename triqs::atom_diag::atom_diag<true>::full_hilbert_space_state_t &st,
                                         const triqs::atom_diag::atom_diag<1> &atom) { return triqs::atom_diag::act<true>(op, st, atom); },
                                      "op", "st", "atom")};

// atomic_density_matrix
static auto const _c2py_fun_19 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::atom_diag::atom_diag<0> &atom, double beta) { return triqs::atom_diag::atomic_density_matrix<false>(atom, beta); },
              "atom", "beta"),
   c2py::cfun([](const triqs::atom_diag::atom_diag<1> &atom, double beta) { return triqs::atom_diag::atomic_density_matrix<true>(atom, beta); },
              "atom", "beta")};

// atomic_g_iw
static auto const _c2py_fun_20 =
   c2py::dispatcher_f_kw_t{c2py::cfun(
                              [](const triqs::atom_diag::atom_diag<0> &atom, double beta, const triqs::gfs::gf_struct_t &gf_struct, int n_iw,
                                 const triqs::atom_diag::excluded_states_t &excluded_states) {
                                return triqs::atom_diag::atomic_g_iw<false>(atom, beta, gf_struct, n_iw, excluded_states);
                              },
                              "atom", "beta", "gf_struct", "n_iw", "excluded_states"_a = triqs::atom_diag::excluded_states_t{}),
                           c2py::cfun(
                              [](const triqs::atom_diag::atom_diag<1> &atom, double beta, const triqs::gfs::gf_struct_t &gf_struct, int n_iw,
                                 const triqs::atom_diag::excluded_states_t &excluded_states) {
                                return triqs::atom_diag::atomic_g_iw<true>(atom, beta, gf_struct, n_iw, excluded_states);
                              },
                              "atom", "beta", "gf_struct", "n_iw", "excluded_states"_a = triqs::atom_diag::excluded_states_t{})};

// atomic_g_l
static auto const _c2py_fun_21 =
   c2py::dispatcher_f_kw_t{c2py::cfun(
                              [](const triqs::atom_diag::atom_diag<0> &atom, double beta, const triqs::gfs::gf_struct_t &gf_struct, int n_l,
                                 const triqs::atom_diag::excluded_states_t &excluded_states) {
                                return triqs::atom_diag::atomic_g_l<false>(atom, beta, gf_struct, n_l, excluded_states);
                              },
                              "atom", "beta", "gf_struct", "n_l", "excluded_states"_a = triqs::atom_diag::excluded_states_t{}),
                           c2py::cfun(
                              [](const triqs::atom_diag::atom_diag<1> &atom, double beta, const triqs::gfs::gf_struct_t &gf_struct, int n_l,
                                 const triqs::atom_diag::excluded_states_t &excluded_states) {
                                return triqs::atom_diag::atomic_g_l<true>(atom, beta, gf_struct, n_l, excluded_states);
                              },
                              "atom", "beta", "gf_struct", "n_l", "excluded_states"_a = triqs::atom_diag::excluded_states_t{})};

// atomic_g_tau
static auto const _c2py_fun_22 =
   c2py::dispatcher_f_kw_t{c2py::cfun(
                              [](const triqs::atom_diag::atom_diag<0> &atom, double beta, const triqs::gfs::gf_struct_t &gf_struct, int n_tau,
                                 const triqs::atom_diag::excluded_states_t &excluded_states) {
                                return triqs::atom_diag::atomic_g_tau<false>(atom, beta, gf_struct, n_tau, excluded_states);
                              },
                              "atom", "beta", "gf_struct", "n_tau", "excluded_states"_a = triqs::atom_diag::excluded_states_t{}),
                           c2py::cfun(
                              [](const triqs::atom_diag::atom_diag<1> &atom, double beta, const triqs::gfs::gf_struct_t &gf_struct, int n_tau,
                                 const triqs::atom_diag::excluded_states_t &excluded_states) {
                                return triqs::atom_diag::atomic_g_tau<true>(atom, beta, gf_struct, n_tau, excluded_states);
                              },
                              "atom", "beta", "gf_struct", "n_tau", "excluded_states"_a = triqs::atom_diag::excluded_states_t{})};

// atomic_g_w
static auto const _c2py_fun_23 = c2py::dispatcher_f_kw_t{
   c2py::cfun(
      [](const triqs::atom_diag::atom_diag<0> &atom, double beta, const triqs::gfs::gf_struct_t &gf_struct,
         const std::pair<double, double> &energy_window, int n_w, double broadening, const triqs::atom_diag::excluded_states_t &excluded_states) {
        return triqs::atom_diag::atomic_g_w<false>(atom, beta, gf_struct, energy_window, n_w, broadening, excluded_states);
      },
      "atom", "beta", "gf_struct", "energy_window", "n_w", "broadening"_a = 0, "excluded_states"_a = triqs::atom_diag::excluded_states_t{}),
   c2py::cfun(
      [](const triqs::atom_diag::atom_diag<1> &atom, double beta, const triqs::gfs::gf_struct_t &gf_struct,
         const std::pair<double, double> &energy_window, int n_w, double broadening, const triqs::atom_diag::excluded_states_t &excluded_states) {
        return triqs::atom_diag::atomic_g_w<true>(atom, beta, gf_struct, energy_window, n_w, broadening, excluded_states);
      },
      "atom", "beta", "gf_struct", "energy_window", "n_w", "broadening"_a = 0, "excluded_states"_a = triqs::atom_diag::excluded_states_t{})};

// partition_function
static auto const _c2py_fun_24 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::atom_diag::atom_diag<0> &atom, double beta) { return triqs::atom_diag::partition_function<false>(atom, beta); }, "atom",
              "beta"),
   c2py::cfun([](const triqs::atom_diag::atom_diag<1> &atom, double beta) { return triqs::atom_diag::partition_function<true>(atom, beta); }, "atom",
              "beta")};

// quantum_number_eigenvalues
static auto const _c2py_fun_25 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const typename triqs::atom_diag::atom_diag<false>::many_body_op_t &op,
                 const triqs::atom_diag::atom_diag<0> &atom) { return triqs::atom_diag::quantum_number_eigenvalues<false>(op, atom); },
              "op", "atom"),
   c2py::cfun([](const typename triqs::atom_diag::atom_diag<true>::many_body_op_t &op,
                 const triqs::atom_diag::atom_diag<1> &atom) { return triqs::atom_diag::quantum_number_eigenvalues<true>(op, atom); },
              "op", "atom")};

// quantum_number_eigenvalues_checked
static auto const _c2py_fun_26 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const typename triqs::atom_diag::atom_diag<false>::many_body_op_t &op,
                 const triqs::atom_diag::atom_diag<0> &atom) { return triqs::atom_diag::quantum_number_eigenvalues_checked<false>(op, atom); },
              "op", "atom"),
   c2py::cfun([](const typename triqs::atom_diag::atom_diag<true>::many_body_op_t &op,
                 const triqs::atom_diag::atom_diag<1> &atom) { return triqs::atom_diag::quantum_number_eigenvalues_checked<true>(op, atom); },
              "op", "atom")};

// trace_rho_op
static auto const _c2py_fun_27 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const typename triqs::atom_diag::atom_diag<false>::block_matrix_t &density_matrix,
                 const typename triqs::atom_diag::atom_diag<false>::many_body_op_t &op,
                 const triqs::atom_diag::atom_diag<0> &atom) { return triqs::atom_diag::trace_rho_op<false>(density_matrix, op, atom); },
              "density_matrix", "op", "atom"),
   c2py::cfun([](const typename triqs::atom_diag::atom_diag<true>::block_matrix_t &density_matrix,
                 const typename triqs::atom_diag::atom_diag<true>::many_body_op_t &op,
                 const triqs::atom_diag::atom_diag<1> &atom) { return triqs::atom_diag::trace_rho_op<true>(density_matrix, op, atom); },
              "density_matrix", "op", "atom")};

static const auto _c2py_doc_18 = _c2py_fun_18.doc(
   R"DOC(
Act with a many-body operator on a state vector, :math:`|\psi'\rangle = \hat O\, |\psi\rangle`.

Both the input and the output state are vectors in the full Hilbert space expressed in the eigenbasis
of the Hamiltonian. The operator is converted internally into its block-matrix representation in the eigenbasis
before being applied to the state. Block-wise, for every source subspace :math:`B` with target subspace
:math:`B'`,

.. math::

   \psi'_{B'} \mathrel{+}= O_{B' \leftarrow B}\, \psi_{B}.

Parameters
----------
op : {par_0}
   Many-body operator to apply.
st : {par_1}
   Input state vector in the full Hilbert space, expressed in the eigenbasis of the Hamiltonian.
atom : {par_2}
   Solved diagonalization problem providing the eigenbasis.

Returns
-------
[1] : {ret_0}
   Output state vector in the full Hilbert space, expressed in the eigenbasis of the Hamiltonian.

[2] : {ret_1}
   Output state vector in the full Hilbert space, expressed in the eigenbasis of the Hamiltonian.
)DOC",
   {{c2py::python_typename<const typename triqs::atom_diag::atom_diag<false>::many_body_op_t &>(),
     c2py::python_typename<const typename triqs::atom_diag::atom_diag<true>::many_body_op_t &>()},
    {c2py::python_typename<const typename triqs::atom_diag::atom_diag<false>::full_hilbert_space_state_t &>(),
     c2py::python_typename<const typename triqs::atom_diag::atom_diag<true>::full_hilbert_space_state_t &>()},
    {c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()}},
   {c2py::python_typename<typename triqs::atom_diag::atom_diag<false>::full_hilbert_space_state_t>(),
    c2py::python_typename<typename triqs::atom_diag::atom_diag<true>::full_hilbert_space_state_t>()});
static const auto _c2py_doc_19 = _c2py_fun_19.doc(
   R"DOC(
Compute the atomic density matrix at inverse temperature :math:`\beta`.

Returns the Gibbs density matrix
:math:`\hat\rho = e^{-\beta \hat H} / Z` as a block-diagonal matrix, with one diagonal block per invariant
subspace :math:`B`. The density matrix is expressed in the eigenbasis, hence each block is itself diagonal,

.. math::

   \rho_B = \mathrm{diag}\!\Bigl( e^{-\beta E_{B,i}} / Z \Bigr)_{i=0}^{\dim(B)-1}.

Parameters
----------
atom : {par_0}
   Solved diagonalization problem.
beta : {par_1}
   Inverse temperature :math:`\beta > 0`.

Returns
-------
[1] : {ret_0}
   Gibbs density matrix of the system, as a list of diagonal blocks indexed by subspace index :math:`B`.

[2] : {ret_1}
   Gibbs density matrix of the system, as a list of diagonal blocks indexed by subspace index :math:`B`.
)DOC",
   {{c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()},
    {c2py::python_typename<double>()}},
   {c2py::python_typename<typename triqs::atom_diag::atom_diag<false>::block_matrix_t>(),
    c2py::python_typename<typename triqs::atom_diag::atom_diag<true>::block_matrix_t>()});
static const auto _c2py_doc_20 = _c2py_fun_20.doc(
   R"DOC(
Build the atomic Matsubara Green's function directly from a solved diagonalization problem.

Internally builds the Lehmann representation and evaluates

.. math::

   G(i\omega) = \sum_p \frac{r_p}{i\omega - p}

on the requested mesh.

Parameters
----------
atom : {par_0}
   Solved diagonalization problem.
beta : {par_1}
   Inverse temperature :math:`\beta > 0`.
gf_struct : {par_2}
   Block structure of the Green's function: block name -> list of inner indices.
n_iw : {par_3}
   Number of positive Matsubara frequencies.
excluded_states : {par_4}
   Eigenstates to exclude from the Lehmann sum, as :math:`(B, i)` pairs.

Returns
-------
{ret_0}
   Atomic Green's function :math:`G_{ab}(i\omega)`.
)DOC",
   {{c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<const triqs::gfs::gf_struct_t &>()},
    {c2py::python_typename<int>()},
    {c2py::python_typename<const triqs::atom_diag::excluded_states_t &>()}},
   {c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::imfreq>>()});
static const auto _c2py_doc_21 = _c2py_fun_21.doc(
   R"DOC(
Build the atomic Green's function in the Legendre basis directly from a solved diagonalization problem.

Internally builds the Lehmann representation and evaluates the corresponding Legendre coefficients

.. math::

   G_\ell = \sqrt{2\ell + 1}\, \int_0^\beta d\tau\, P_\ell(2\tau/\beta - 1)\, G(\tau) \;.

Parameters
----------
atom : {par_0}
   Solved diagonalization problem.
beta : {par_1}
   Inverse temperature :math:`\beta > 0`.
gf_struct : {par_2}
   Block structure of the Green's function: block name -> list of inner indices.
n_l : {par_3}
   Number of Legendre coefficients to compute.
excluded_states : {par_4}
   Eigenstates to exclude from the Lehmann sum, as :math:`(B, i)` pairs.

Returns
-------
{ret_0}
   Atomic Green's function :math:`G_{ab}(\ell)`.
)DOC",
   {{c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<const triqs::gfs::gf_struct_t &>()},
    {c2py::python_typename<int>()},
    {c2py::python_typename<const triqs::atom_diag::excluded_states_t &>()}},
   {c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::legendre>>()});
static const auto _c2py_doc_22 = _c2py_fun_22.doc(
   R"DOC(
Build the atomic imaginary-time Green's function directly from a solved diagonalization problem.

Internally builds the Lehmann representation and evaluates it on the requested mesh,

.. math::

   G(\tau) = \sum_p r_p \, \frac{-e^{-\tau p}}{1 + e^{-\beta p}}, \quad \tau \in [0, \beta].

Parameters
----------
atom : {par_0}
   Solved diagonalization problem.
beta : {par_1}
   Inverse temperature :math:`\beta > 0`.
gf_struct : {par_2}
   Block structure of the Green's function: block name -> list of inner indices.
n_tau : {par_3}
   Number of imaginary-time points.
excluded_states : {par_4}
   Eigenstates to exclude from the Lehmann sum, as :math:`(B, i)` pairs.

Returns
-------
{ret_0}
   Atomic Green's function :math:`G_{ab}(\tau)`.
)DOC",
   {{c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<const triqs::gfs::gf_struct_t &>()},
    {c2py::python_typename<int>()},
    {c2py::python_typename<const triqs::atom_diag::excluded_states_t &>()}},
   {c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::imtime>>()});
static const auto _c2py_doc_23 = _c2py_fun_23.doc(
   R"DOC(
Build the atomic retarded Green's function on a real-frequency mesh directly from a solved
diagonalization problem.

Internally builds the Lehmann representation, constructs a real-frequency mesh from the requested energy 
window and number of frequency points, and evaluates 

.. math::

   G(\omega) = \sum_p \frac{r_p}{\omega + i\eta - p} \; ,

with the broadening :math:`\eta`.

Parameters
----------
atom : {par_0}
   Solved diagonalization problem.
beta : {par_1}
   Inverse temperature :math:`\beta > 0`.
gf_struct : {par_2}
   Block structure of the Green's function: block name -> list of inner indices.
energy_window : {par_3}
   Energy window :math:`(\omega_{\text{min}}, \omega_{\text{max}})` of the real-frequency
   mesh.
n_w : {par_4}
   Number of frequency points.
broadening : {par_5}
   Lorentzian broadening :math:`\eta` of the spectrum (small positive imaginary-frequency shift).
excluded_states : {par_6}
   Eigenstates to exclude from the Lehmann sum, as :math:`(B, i)` pairs.

Returns
-------
{ret_0}
   Atomic Green's function :math:`G_{ab}(\omega)`.
)DOC",
   {{c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<const triqs::gfs::gf_struct_t &>()},
    {c2py::python_typename<const std::pair<double, double> &>()},
    {c2py::python_typename<int>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<const triqs::atom_diag::excluded_states_t &>()}},
   {c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::refreq>>()});
static const auto _c2py_doc_24 = _c2py_fun_24.doc(
   R"DOC(
Compute the atomic partition function at inverse temperature :math:`\beta`.

Sums the Boltzmann weights of all eigenstates of the Hamiltonian,

.. math::

   Z = \sum_{B} \sum_{i=0}^{\dim(B)-1} e^{-\beta E_{B,i}},

where the eigenvalues :math:`E_{B,i}` are taken with respect to the ground-state energy as zero.

Parameters
----------
atom : {par_0}
   Solved diagonalization problem.
beta : {par_1}
   Inverse temperature :math:`\beta > 0`.

Returns
-------
{ret_0}
   Value of the partition function :math:`Z`.
)DOC",
   {{c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()},
    {c2py::python_typename<double>()}},
   {c2py::python_typename<double>()});
static const auto _c2py_doc_25 = _c2py_fun_25.doc(
   R"DOC(
Tabulate the eigenvalues :math:`q_{B,i} = \langle B,i\,|\,\hat Q\,|\,B,i\rangle` of a quantum-number
operator :math:`\hat Q` over all eigenstates of the Hamiltonian.

Assumes that :math:`\hat Q` is a quantum number, i.e. that it commutes with :math:`\hat H` and is 
therefore block-diagonal in the eigenbasis with diagonal blocks. The diagonal entries are returned, grouped by 
invariant subspace :math:`B`. Use `quantum_number_eigenvalues_checked` for a variant that explicitly verifies the
block-diagonal property.

Parameters
----------
op : {par_0}
   Observable operator; expected to be a quantum number (otherwise an exception is raised).
atom : {par_1}
   Solved diagonalization problem.

Returns
-------
{ret_0}
   Eigenvalues :math:`q_{B,i}` grouped by invariant subspace :math:`B`.
)DOC",
   {{c2py::python_typename<const typename triqs::atom_diag::atom_diag<false>::many_body_op_t &>(),
     c2py::python_typename<const typename triqs::atom_diag::atom_diag<true>::many_body_op_t &>()},
    {c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()}},
   {c2py::python_typename<std::vector<std::vector<double>>>()});
static const auto _c2py_doc_26 = _c2py_fun_26.doc(
   R"DOC(
Tabulate the eigenvalues :math:`q_{B,i}` of a quantum-number operator :math:`\hat Q`, also checking that 
the operator is diagonal in the eigenbasis.

Same as `quantum_number_eigenvalues`, but also verifies that the supplied operator is diagonal in the
eigenbasis of the Hamiltonian. If it is not, an exception is raised. Useful when constructing a quantum number
from scratch and one wants to confirm that it really is a conserved quantity of `atom`.

Parameters
----------
op : {par_0}
   Observable operator; expected to be a quantum number (otherwise an exception is raised).
atom : {par_1}
   Solved diagonalization problem.

Returns
-------
{ret_0}
   Eigenvalues :math:`q_{B,i}` grouped by invariant subspace :math:`B`.
)DOC",
   {{c2py::python_typename<const typename triqs::atom_diag::atom_diag<false>::many_body_op_t &>(),
     c2py::python_typename<const typename triqs::atom_diag::atom_diag<true>::many_body_op_t &>()},
    {c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()}},
   {c2py::python_typename<std::vector<std::vector<double>>>()});
static const auto _c2py_doc_27 = _c2py_fun_27.doc(
   R"DOC(
Compute the trace of a many-body operator weighted by a block-diagonal density matrix.

Evaluates

.. math::

   \mathrm{Tr}\,(\hat\rho\, \hat O) = \sum_{B} \mathrm{Tr}\,(\rho_B\, O_{BB}),

where :math:`\hat\rho` is provided as a list of diagonal blocks (one per invariant subspace of `atom`) and only
the diagonal blocks :math:`O_{BB}` of :math:`\hat O` contribute. Useful for computing expectation values of
arbitrary observables once a density matrix has been built.

Parameters
----------
density_matrix : {par_0}
   Density matrix as a list of diagonal blocks, indexed by subspace index :math:`B`.
op : {par_1}
   Operator to be averaged.
atom : {par_2}
   Solved diagonalization problem.

Returns
-------
[1] : {ret_0}
   Expectation value of :math:`\hat O` under the given density matrix.

[2] : {ret_1}
   Expectation value of :math:`\hat O` under the given density matrix.
)DOC",
   {{c2py::python_typename<const typename triqs::atom_diag::atom_diag<false>::block_matrix_t &>(),
     c2py::python_typename<const typename triqs::atom_diag::atom_diag<true>::block_matrix_t &>()},
    {c2py::python_typename<const typename triqs::atom_diag::atom_diag<false>::many_body_op_t &>(),
     c2py::python_typename<const typename triqs::atom_diag::atom_diag<true>::many_body_op_t &>()},
    {c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()}},
   {c2py::python_typename<double>(), c2py::python_typename<typename triqs::atom_diag::atom_diag<true>::scalar_t>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"act", (PyCFunction)c2py::pyfkw<_c2py_fun_18>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_18.c_str()},
   {"atomic_density_matrix", (PyCFunction)c2py::pyfkw<_c2py_fun_19>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_19.c_str()},
   {"atomic_g_iw", (PyCFunction)c2py::pyfkw<_c2py_fun_20>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_20.c_str()},
   {"atomic_g_l", (PyCFunction)c2py::pyfkw<_c2py_fun_21>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_21.c_str()},
   {"atomic_g_tau", (PyCFunction)c2py::pyfkw<_c2py_fun_22>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_22.c_str()},
   {"atomic_g_w", (PyCFunction)c2py::pyfkw<_c2py_fun_23>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_23.c_str()},
   {"partition_function", (PyCFunction)c2py::pyfkw<_c2py_fun_24>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_24.c_str()},
   {"quantum_number_eigenvalues", (PyCFunction)c2py::pyfkw<_c2py_fun_25>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_25.c_str()},
   {"quantum_number_eigenvalues_checked", (PyCFunction)c2py::pyfkw<_c2py_fun_26>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_26.c_str()},
   {"trace_rho_op", (PyCFunction)c2py::pyfkw<_c2py_fun_27>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_27.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "atom_diag", /* name of module */
                                        R"RAWDOC(Exact diagonalization of finite fermionic Hamiltonians.

This module exposes a lightweight exact diagonalization solver for the atomic (local) problem of a quantum impurity,
together with helpers that build derived quantities from a solved eigensystem. The main classes are:

- :class:`AtomDiagReal` and :class:`AtomDiagComplex`: hold the block-diagonal Hamiltonian, its eigensystem, and the
  matrix representations of the fundamental creation/annihilation operators in the eigenbasis. The
  :func:`AtomDiag` factory dispatches between the real and complex variant based on the Hamiltonian.

A second group of free functions takes a solved :class:`AtomDiagReal` / :class:`AtomDiagComplex` and produces derived
quantities: thermodynamic averages (:func:`partition_function`, :func:`atomic_density_matrix`,
:func:`trace_rho_op`), application of an operator to a state (:func:`act`), tabulation of conserved-quantity
eigenvalues (:func:`quantum_number_eigenvalues`, :func:`quantum_number_eigenvalues_checked`), and the atomic Green's
function on different meshes (:func:`atomic_g_tau`, :func:`atomic_g_iw`, :func:`atomic_g_l`, :func:`atomic_g_w`).
)RAWDOC",                                            /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_atom_diag() {

  if (not c2py::check_python_version("atom_diag")) return NULL;

  // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_0>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_1>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
#define _add_type(T, N) c2py::add_type_object_to_main<T>(N, m, conv_table)
  _add_type(_c2py_cls_0, "AtomDiagReal");
  _add_type(_c2py_cls_1, "AtomDiagComplex");
#undef _add_type

  c2py::pyref module = c2py::pyref::module("h5.formats");
  if (not module) return nullptr;
  c2py::pyref register_class = module.attr("register_class");

  register_h5_type<_c2py_cls_0>(register_class);
  register_h5_type<_c2py_cls_1>(register_class);

  return m;
}
#endif
// CLAIR_WRAP_GEN
