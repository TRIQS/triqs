
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
static auto _c2py_init_0                                     = c2py::dispatcher_c_kw_t{
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
[1] Reduce a given Hamiltonian to a block-diagonal form and diagonalize it

This constructor calls the auto-partition procedure, and the QR algorithm
to diagonalize the blocks. The invariant subspaces of the Hamiltonian are
chosen such that all creation and annihilation operators from the provided
fundamental operator set map one subspace to one subspace.

.. note::

   See :ref:`space_partition` for more details on the auto-partition scheme.

------

[4] Reduce a given Hamiltonian to a block-diagonal form and diagonalize it

This constructor uses quantum number operators to partition the Hilbert space into
invariant subspaces, and the QR algorithm to diagonalize the blocks of the Hamiltonian.
The quantum numbers must be chosen such that all creation and annihilation operators from
the provided fundamental operator set map one subspace to one subspace.

------

Parameters
----------
h : {par_0}
   Hamiltonian operator to be diagonalized.
fops : {par_1}
   Fundamental operator set; Must at least contain all fundamental operators met in `h`.
qn_vector : {par_2}
   Vector of quantum number operators.
)DOC",
                    {{c2py::python_typename<const triqs::atom_diag::atom_diag<false>::many_body_op_t &>()},
                     {c2py::python_typename<const triqs::hilbert_space::fundamental_operator_set &>()},
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
Subspace-to-subspace connections for fundamental operator :math:`C`

Parameters
----------
op_linear_index : {par_0}
   The linear index (i.e. number) of the annihilation operator, as defined by the fundamental operator set.
sp_index : {par_1}
   The index of the initial subspace.

Returns
-------
{ret_0}
   The index of the final subspace.
)DOC",
                                                {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_1 = _c2py_fun_1.doc(R"DOC(
Matrix block for fundamental operator :math:`C`

Parameters
----------
op_linear_index : {par_0}
   The linear index (i.e. number) of the annihilation operator, as defined by the fundamental operator set.
sp_index : {par_1}
   The index of the initial subspace.

Returns
-------
{ret_0}
   The index of the final subspace.
)DOC",
                                                {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}},
                                                {c2py::python_typename<const triqs::atom_diag::atom_diag<false>::matrix_t &>()});
static const auto _c2py_doc_2 = _c2py_fun_2.doc(R"DOC(
Subspace-to-subspace connections for fundamental operator :math:`C^`
*

Parameters
----------
op_linear_index : {par_0}
   The linear index (i.e. number) of the creation operator, as defined by the fundamental operator set.
sp_index : {par_1}
   The index of the initial subspace.

Returns
-------
{ret_0}
   The index of the final subspace.
)DOC",
                                                {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_3 = _c2py_fun_3.doc(R"DOC(
Matrix block for fundamental operator :math:`C^`

Parameters
----------
op_linear_index : {par_0}
   The linear index (i.e. number) of the creation operator, as defined by the fundamental operator set.
sp_index : {par_1}
   The index of the initial subspace.

Returns
-------
{ret_0}
   The index of the final subspace.
)DOC",
                                                {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}},
                                                {c2py::python_typename<const triqs::atom_diag::atom_diag<false>::matrix_t &>()});
static const auto _c2py_doc_4 = _c2py_fun_4.doc(R"DOC(
Returns the state index in the full Hilbert space given a subspace index and an inner index

Parameters
----------
sp_index : {par_0}
   Index of the invariant subspace.
i : {par_1}
   State index within the subspace.
)DOC",
                                                {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}});
static const auto _c2py_doc_5 = _c2py_fun_5.doc(R"DOC(
Get the i-th eigenvalue of subspace sp_index

Parameters
----------
sp_index : {par_0}
   Index of the invariant subspace.
i : {par_1}
   State index within the subspace.
)DOC",
                                                {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}});
static const auto _c2py_doc_6 = _c2py_fun_6.doc(R"DOC(
The dimension of a subspace

Parameters
----------
sp_index : {par_0}
   Index of the invariant subspace.
)DOC",
                                                {{c2py::python_typename<int>()}});
static const auto _c2py_doc_7 = _c2py_fun_7.doc(R"DOC(
Get the dimensions of all subspaces
)DOC");
static const auto _c2py_doc_8 = _c2py_fun_8.doc(R"DOC(
Unitary matrix for given subspace that transform from Fock states to eigenstates
)DOC");

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

static constexpr auto prop_doc_0  = R"DOC(A vector of all the energies, grouped by subspace)DOC";
static constexpr auto prop_doc_1  = R"DOC(The list of Fock states for each subspace)DOC";
static constexpr auto prop_doc_2  = R"DOC(Get the data of the fundamental operator set used at construction.)DOC";
static constexpr auto prop_doc_3  = R"DOC(Dimension of the full Hilbert space)DOC";
static constexpr auto prop_doc_4  = R"DOC(Ground state energy (i.e. min of all subspaces))DOC";
static constexpr auto prop_doc_5  = R"DOC(Get the Hamiltonian used at construction as a triqs::operators::many_body_operator.)DOC";
static constexpr auto prop_doc_6  = R"DOC(Number of invariant subspaces)DOC";
static constexpr auto prop_doc_7  = R"DOC(A vector of all the quantum numbers, grouped by subspace)DOC";
static constexpr auto prop_doc_8  = R"DOC(Unitary matrices that transform from Fock states to eigenstates)DOC";
static constexpr auto prop_doc_9  = R"DOC(Returns the vacuum state as a vector in the full Hilbert space

This vector is written in the eigenbasis of the Hamiltonian.)DOC";
static constexpr auto prop_doc_10 = R"DOC(Returns invariant subspace containing the vacuum state)DOC";

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
const std::string c2py::tp_doc<_c2py_cls_0> = R"DOC(Lightweight exact diagonalization solver

This class is provided as a simple tool to diagonalize Hamiltonians of
finite fermionic systems of a moderate size.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_0>;
// --------- class _c2py_cls_1 -----------
using _c2py_cls_1                                            = triqs::atom_diag::atom_diag<true>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_1>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_1> = "triqs.atom_diag.atom_diag.AtomDiagComplex";
static auto _c2py_init_1                                     = c2py::dispatcher_c_kw_t{
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
[1] Reduce a given Hamiltonian to a block-diagonal form and diagonalize it

This constructor calls the auto-partition procedure, and the QR algorithm
to diagonalize the blocks. The invariant subspaces of the Hamiltonian are
chosen such that all creation and annihilation operators from the provided
fundamental operator set map one subspace to one subspace.

.. note::

   See :ref:`space_partition` for more details on the auto-partition scheme.

------

[4] Reduce a given Hamiltonian to a block-diagonal form and diagonalize it

This constructor uses quantum number operators to partition the Hilbert space into
invariant subspaces, and the QR algorithm to diagonalize the blocks of the Hamiltonian.
The quantum numbers must be chosen such that all creation and annihilation operators from
the provided fundamental operator set map one subspace to one subspace.

------

Parameters
----------
h : {par_0}
   Hamiltonian operator to be diagonalized.
fops : {par_1}
   Fundamental operator set; Must at least contain all fundamental operators met in `h`.
qn_vector : {par_2}
   Vector of quantum number operators.
)DOC",
                    {{c2py::python_typename<const triqs::atom_diag::atom_diag<true>::many_body_op_t &>()},
                     {c2py::python_typename<const triqs::hilbert_space::fundamental_operator_set &>()},
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
Subspace-to-subspace connections for fundamental operator :math:`C`

Parameters
----------
op_linear_index : {par_0}
   The linear index (i.e. number) of the annihilation operator, as defined by the fundamental operator set.
sp_index : {par_1}
   The index of the initial subspace.

Returns
-------
{ret_0}
   The index of the final subspace.
)DOC",
                                                 {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_10 = _c2py_fun_10.doc(R"DOC(
Matrix block for fundamental operator :math:`C`

Parameters
----------
op_linear_index : {par_0}
   The linear index (i.e. number) of the annihilation operator, as defined by the fundamental operator set.
sp_index : {par_1}
   The index of the initial subspace.

Returns
-------
{ret_0}
   The index of the final subspace.
)DOC",
                                                  {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}},
                                                  {c2py::python_typename<const triqs::atom_diag::atom_diag<true>::matrix_t &>()});
static const auto _c2py_doc_11 = _c2py_fun_11.doc(R"DOC(
Subspace-to-subspace connections for fundamental operator :math:`C^`
*

Parameters
----------
op_linear_index : {par_0}
   The linear index (i.e. number) of the creation operator, as defined by the fundamental operator set.
sp_index : {par_1}
   The index of the initial subspace.

Returns
-------
{ret_0}
   The index of the final subspace.
)DOC",
                                                  {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_12 = _c2py_fun_12.doc(R"DOC(
Matrix block for fundamental operator :math:`C^`

Parameters
----------
op_linear_index : {par_0}
   The linear index (i.e. number) of the creation operator, as defined by the fundamental operator set.
sp_index : {par_1}
   The index of the initial subspace.

Returns
-------
{ret_0}
   The index of the final subspace.
)DOC",
                                                  {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}},
                                                  {c2py::python_typename<const triqs::atom_diag::atom_diag<true>::matrix_t &>()});
static const auto _c2py_doc_13 = _c2py_fun_13.doc(R"DOC(
Returns the state index in the full Hilbert space given a subspace index and an inner index

Parameters
----------
sp_index : {par_0}
   Index of the invariant subspace.
i : {par_1}
   State index within the subspace.
)DOC",
                                                  {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}});
static const auto _c2py_doc_14 = _c2py_fun_14.doc(R"DOC(
Get the i-th eigenvalue of subspace sp_index

Parameters
----------
sp_index : {par_0}
   Index of the invariant subspace.
i : {par_1}
   State index within the subspace.
)DOC",
                                                  {{c2py::python_typename<int>()}, {c2py::python_typename<int>()}});
static const auto _c2py_doc_15 = _c2py_fun_15.doc(R"DOC(
The dimension of a subspace

Parameters
----------
sp_index : {par_0}
   Index of the invariant subspace.
)DOC",
                                                  {{c2py::python_typename<int>()}});
static const auto _c2py_doc_16 = _c2py_fun_16.doc(R"DOC(
Get the dimensions of all subspaces
)DOC");
static const auto _c2py_doc_17 = _c2py_fun_17.doc(R"DOC(
Unitary matrix for given subspace that transform from Fock states to eigenstates
)DOC");

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

static constexpr auto prop_doc_11 = R"DOC(A vector of all the energies, grouped by subspace)DOC";
static constexpr auto prop_doc_12 = R"DOC(The list of Fock states for each subspace)DOC";
static constexpr auto prop_doc_13 = R"DOC(Get the data of the fundamental operator set used at construction.)DOC";
static constexpr auto prop_doc_14 = R"DOC(Dimension of the full Hilbert space)DOC";
static constexpr auto prop_doc_15 = R"DOC(Ground state energy (i.e. min of all subspaces))DOC";
static constexpr auto prop_doc_16 = R"DOC(Get the Hamiltonian used at construction as a triqs::operators::many_body_operator.)DOC";
static constexpr auto prop_doc_17 = R"DOC(Number of invariant subspaces)DOC";
static constexpr auto prop_doc_18 = R"DOC(A vector of all the quantum numbers, grouped by subspace)DOC";
static constexpr auto prop_doc_19 = R"DOC(Unitary matrices that transform from Fock states to eigenstates)DOC";
static constexpr auto prop_doc_20 = R"DOC(Returns the vacuum state as a vector in the full Hilbert space

This vector is written in the eigenbasis of the Hamiltonian.)DOC";
static constexpr auto prop_doc_21 = R"DOC(Returns invariant subspace containing the vacuum state)DOC";

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
const std::string c2py::tp_doc<_c2py_cls_1> = R"DOC(Lightweight exact diagonalization solver

This class is provided as a simple tool to diagonalize Hamiltonians of
finite fermionic systems of a moderate size.)DOC"
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
Act with operator `op` on state `st`

Parameters
----------
op : {par_0}
   Operator to act on the state.
st : {par_1}
   Initial state vector in the full Hilbert space, written in the eigenbasis of the Hamiltonian.
atom : {par_2}
   Solved diagonalization problem.

Returns
-------
[1] : {ret_0}
   Final state vector in the full Hilbert space.

[2] : {ret_1}
   Final state vector in the full Hilbert space.
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
The atomic density matrix

Parameters
----------
atom : {par_0}
   Solved diagonalization problem.
beta : {par_1}
   Inverse temperature.

Returns
-------
[1] : {ret_0}
   Gibbs' density matrix of the system.

[2] : {ret_1}
   Gibbs' density matrix of the system.
)DOC",
   {{c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()},
    {c2py::python_typename<double>()}},
   {c2py::python_typename<typename triqs::atom_diag::atom_diag<false>::block_matrix_t>(),
    c2py::python_typename<typename triqs::atom_diag::atom_diag<true>::block_matrix_t>()});
static const auto _c2py_doc_20 = _c2py_fun_20.doc(
   R"DOC(
The atomic Matsubara Green's function, possibly with excluded states (none by default)

Parameters
----------
atom : {par_0}
   Solved diagonalization problem.
beta : {par_1}
   Inverse temperature.
gf_struct : {par_2}
   Block structure of the Green's function, block name -> list of inner indices.
n_iw : {par_3}
   Number of Matsubara frequencies.
excluded_states : {par_4}
   Excluded eigenstates as pairs (subspace index, inner index).

Returns
-------
{ret_0}
   Atomic Green's function :math:`G_{at}(i)`.
)DOC",
   {{c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<const triqs::gfs::gf_struct_t &>()},
    {c2py::python_typename<int>()},
    {c2py::python_typename<const triqs::atom_diag::excluded_states_t &>()}},
   {c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::imfreq>>()});
static const auto _c2py_doc_21 = _c2py_fun_21.doc(
   R"DOC(
The atomic Green's function in Legendre basis, possibly with excluded states (none by default)

Parameters
----------
atom : {par_0}
   Solved diagonalization problem.
beta : {par_1}
   Inverse temperature.
gf_struct : {par_2}
   Block structure of the Green's function, block name -> list of inner indices.
n_l : {par_3}
   Number of Legendre coefficients.
excluded_states : {par_4}
   Excluded eigenstates as pairs (subspace index, inner index).

Returns
-------
{ret_0}
   Atomic Green's function :math:`G_{at}()`.
)DOC",
   {{c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<const triqs::gfs::gf_struct_t &>()},
    {c2py::python_typename<int>()},
    {c2py::python_typename<const triqs::atom_diag::excluded_states_t &>()}},
   {c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::legendre>>()});
static const auto _c2py_doc_22 = _c2py_fun_22.doc(
   R"DOC(
The atomic imaginary time Green's function, possibly with excluded states (none by default)

Parameters
----------
atom : {par_0}
   Solved diagonalization problem.
beta : {par_1}
   Inverse temperature.
gf_struct : {par_2}
   Block structure of the Green's function, block name -> list of inner indices.
n_tau : {par_3}
   Number of imaginary time points.
excluded_states : {par_4}
   Excluded eigenstates as pairs (subspace index, inner index).

Returns
-------
{ret_0}
   Atomic Green's function :math:`G_{at}()`
)DOC",
   {{c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<const triqs::gfs::gf_struct_t &>()},
    {c2py::python_typename<int>()},
    {c2py::python_typename<const triqs::atom_diag::excluded_states_t &>()}},
   {c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::imtime>>()});
static const auto _c2py_doc_23 = _c2py_fun_23.doc(
   R"DOC(
The atomic retarded Green's function, possibly with excluded states (none by default)

Parameters
----------
atom : {par_0}
   Solved diagonalization problem.
beta : {par_1}
   Inverse temperature.
gf_struct : {par_2}
   Block structure of the Green's function, block name -> list of inner indices.
energy_window : {par_3}
   Energy window :math:`({min}, {max})`.
n_w : {par_4}
   Number of frequency points.
broadening : {par_5}
   Lorentian broadening of the spectrum (imaginary frequency shift).
excluded_states : {par_6}
   Excluded eigenstates as pairs (subspace index, inner index).

Returns
-------
{ret_0}
   Atomic Green's function :math:`G_{at}()`.
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
The atomic partition function

Parameters
----------
atom : {par_0}
   Solved diagonalization problem.
beta : {par_1}
   Inverse temperature.

Returns
-------
{ret_0}
   Value of the partition function.
)DOC",
   {{c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()},
    {c2py::python_typename<double>()}},
   {c2py::python_typename<double>()});
static const auto _c2py_doc_25 = _c2py_fun_25.doc(
   R"DOC(
Compute values of a given quantum number for all eigenstates

Parameters
----------
op : {par_0}
   Observable operator; supposed to be a quantum number (if not -> exception).
atom : {par_1}
   Solved diagonalization problem.

Returns
-------
{ret_0}
   The eigenvalues by block
)DOC",
   {{c2py::python_typename<const typename triqs::atom_diag::atom_diag<false>::many_body_op_t &>(),
     c2py::python_typename<const typename triqs::atom_diag::atom_diag<true>::many_body_op_t &>()},
    {c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()}},
   {c2py::python_typename<std::vector<std::vector<double>>>()});
static const auto _c2py_doc_26 = _c2py_fun_26.doc(
   R"DOC(
Compute values of a given quantum number for all eigenstates

This function is similar to :ref:`quantum_number_eigenvalues()` except it checks that
the quantum number operator is diagonal in the eigenbasis.

Parameters
----------
op : {par_0}
   Observable operator; supposed to be a quantum number (if not -> exception).
atom : {par_1}
   Solved diagonalization problem.

Returns
-------
{ret_0}
   The eigenvalues by block
)DOC",
   {{c2py::python_typename<const typename triqs::atom_diag::atom_diag<false>::many_body_op_t &>(),
     c2py::python_typename<const typename triqs::atom_diag::atom_diag<true>::many_body_op_t &>()},
    {c2py::python_typename<const triqs::atom_diag::atom_diag<0> &>(), c2py::python_typename<const triqs::atom_diag::atom_diag<1> &>()}},
   {c2py::python_typename<std::vector<std::vector<double>>>()});
static const auto _c2py_doc_27 = _c2py_fun_27.doc(
   R"DOC(
Compute Tr (op * density_matrix)

Parameters
----------
density_matrix : {par_0}
   Density matrix as a list of diagonal blocks for all invariant subspaces in `atom`.
op : {par_1}
   Operator to be averaged.
atom : {par_2}
   Solved diagonalization problem.

Returns
-------
[1] : {ret_0}
   Operator `op` averaged over the density matrix.

[2] : {ret_1}
   Operator `op` averaged over the density matrix.
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
                                        "atom_diag",                                               /* name of module */
                                        R"RAWDOC(Lightweight exact diagonalization solver)RAWDOC", /* module documentation, may be NULL */
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
