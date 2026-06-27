
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

using c2py::operator""_a;

// ==================== enums =====================

// ==================== module classes =====================

// ==================== module functions ====================

// block_matrix_from_op
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex> &h, const triqs::gfs::gf_struct_t &gf_struct,
      bool ignore_irrelevant) {
     return triqs::operators::utils::block_matrix_from_op<triqs::utility::real_or_complex>(h, gf_struct, ignore_irrelevant);
   },
   "h", "gf_struct", "ignore_irrelevant"_a = false)};

// dict_to_matrix
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cfun(
      [](std::map<
            std::tuple<std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>,
                       std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>>,
            triqs::utility::real_or_complex,
            std::less<std::tuple<std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                             std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>,
                                 std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                             std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>>>,
            std::allocator<std::pair<const std::tuple<std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                                                  std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>,
                                                      std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                                                  std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>>,
                                     triqs::utility::real_or_complex>>>
            d,
         triqs::gfs::gf_struct_t gf_struct) { return triqs::operators::utils::py::dict_to_matrix(d, gf_struct); },
      "d", "gf_struct"),
   c2py::cfun(
      [](std::map<
            std::tuple<std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>,
                       std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>,
                       std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>,
                       std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>>,
            triqs::utility::real_or_complex,
            std::less<std::tuple<std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                             std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>,
                                 std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                             std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>,
                                 std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                             std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>,
                                 std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                             std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>>>,
            std::allocator<std::pair<const std::tuple<std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                                                  std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>,
                                                      std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                                                  std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>,
                                                      std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                                                  std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>,
                                                      std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                                                  std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>>,
                                     triqs::utility::real_or_complex>>>
            d,
         triqs::gfs::gf_struct_t gf_struct) { return triqs::operators::utils::py::dict_to_matrix(d, gf_struct); },
      "d", "gf_struct")};

// extract_U_dict2
static auto const _c2py_fun_2 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const triqs::operators::many_body_operator &H,
                                         bool ignore_irrelevant) { return triqs::operators::utils::py::extract_U_dict2(H, ignore_irrelevant); },
                                      "H", "ignore_irrelevant"_a = false)};

// extract_U_dict4
static auto const _c2py_fun_3 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const triqs::operators::many_body_operator &H,
                                         bool ignore_irrelevant) { return triqs::operators::utils::py::extract_U_dict4(H, ignore_irrelevant); },
                                      "H", "ignore_irrelevant"_a = false)};

// extract_h_dict
static auto const _c2py_fun_4 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const triqs::operators::many_body_operator &H,
                                         bool ignore_irrelevant) { return triqs::operators::utils::py::extract_h_dict(H, ignore_irrelevant); },
                                      "H", "ignore_irrelevant"_a = false)};

// op_from_block_matrix
static auto const _c2py_fun_5 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const nda::basic_array<nda::basic_array<triqs::utility::real_or_complex, 2, nda::C_layout, 'M',
                                              nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>,
                             1, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &bl_mat,
      const triqs::gfs::gf_struct_t &gf_struct) {
     return triqs::operators::utils::op_from_block_matrix<triqs::utility::real_or_complex>(bl_mat, gf_struct);
   },
   "bl_mat", "gf_struct")};

// quadratic_terms
static auto const _c2py_fun_6 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex> &h) {
     return triqs::operators::utils::quadratic_terms<triqs::utility::real_or_complex>(h);
   },
   "h")};

// quartic_terms
static auto const _c2py_fun_7 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex> &h) {
     return triqs::operators::utils::quartic_terms<triqs::utility::real_or_complex>(h);
   },
   "h")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(
   R"DOC(
Convert a block-diagonal quadratic operator into its block-matrix representation.

Assumes that :math:`\hat{h}` has the form

.. math::

   \hat{h} = \sum_{\sigma ij} h_{\sigma ij} \hat{c}_{\sigma, i}^\dagger \hat{c}_{\sigma, j} \; ,

where the first element of each canonical operator's index is interpreted as the block label :math:`\sigma` (a
string) and the second element as the in-block integer index :math:`i`.

If a term that is not of this form is encountered, an exception is thrown unless `ignore_irrelevant` is true, in 
which case the offending term is silently skipped.

Parameters
----------
h : {par_0}
   Many-body operator :math:`\hat{h}`.
gf_struct : {par_1}
   Block structure specifying the block labels and the size of each block.
ignore_irrelevant : {par_2}
   If true, terms that do not match the expected form are skipped instead of triggering an 
   exception.

Returns
-------
{ret_0}
   One matrix per block, packaged as a one-dimensional array of matrices.
)DOC",
   {{c2py::python_typename<const triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex> &>()},
    {c2py::python_typename<const triqs::gfs::gf_struct_t &>()},
    {c2py::python_typename<bool>()}},
   {c2py::python_typename<nda::basic_array<
      nda::basic_array<triqs::utility::real_or_complex, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>,
      1, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});
static const auto _c2py_doc_1 = _c2py_fun_1.doc(
   R"DOC(
Convert a coefficient dictionary to a matrix, given a GF structure.

Parameters
----------
d : {par_0}
   The 2-index/4-index dictionary.
gf_struct : {par_1}
   The Green's function block structure.

Returns
-------
{ret_0}
   A real or complex matrix/rank-4 tensor (as variant).
)DOC",
   {{c2py::python_typename<
       std::map<std::tuple<std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                       std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>,
                           std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                       std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>>,
                triqs::utility::real_or_complex,
                std::less<std::tuple<std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                                 std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>,
                                     std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                                 std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>>>,
                std::allocator<std::pair<const std::tuple<std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                                                      std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>,
                                                          std::vector<std::variant<long, std::string, double, std::array<long, 3>>,
                                                                      std::allocator<std::variant<long, std::string, double, std::array<long, 3>>>>>,
                                         triqs::utility::real_or_complex>>>>()},
    {c2py::python_typename<triqs::gfs::gf_struct_t>()}},
   {c2py::python_typename<std::variant<
      nda::basic_array<double, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>,
      nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>()});
static const auto _c2py_doc_2 = _c2py_fun_2.doc(
   R"DOC(
Extract the density-density interaction :math:`\frac{1}{2} \sum_{ij} U_{ij} \hat{n}_i \hat{n}_j` from a
many-body operator.

Parameters
----------
H : {par_0}
   The many-body operator.
ignore_irrelevant : {par_1}
   If `true`, silently skip irrelevant terms.

Returns
-------
{ret_0}
   Dictionary mapping index pairs :math:`(i, j)` to coefficients :math:`U_{ij}`.
)DOC",
   {{c2py::python_typename<const triqs::operators::many_body_operator &>()}, {c2py::python_typename<bool>()}},
   {c2py::python_typename<
      std::map<std::tuple<triqs::operators::utils::py::idx_tup, triqs::operators::utils::py::idx_tup>, triqs::utility::real_or_complex>>()});
static const auto _c2py_doc_3 =
   _c2py_fun_3.doc(R"DOC(
Extract the two-body interaction part from a many-body operator.

The two body interaction is assumed to have the form

.. math::

   \frac{1}{2} \sum_{ijkl} U_{ijkl} \hat{c}^\dagger_i \hat{c}^\dagger_j \hat{c}_l \hat{c}_k \; .

Parameters
----------
H : {par_0}
   The many-body operator.
ignore_irrelevant : {par_1}
   If `true`, silently skip irrelevant terms.

Returns
-------
{ret_0}
   Dictionary mapping index quadruples :math:`(i, j, k, l)` to coefficients :math:`U_{ijkl}`.
)DOC",
                   {{c2py::python_typename<const triqs::operators::many_body_operator &>()}, {c2py::python_typename<bool>()}},
                   {c2py::python_typename<std::map<std::tuple<triqs::operators::utils::py::idx_tup, triqs::operators::utils::py::idx_tup,
                                                              triqs::operators::utils::py::idx_tup, triqs::operators::utils::py::idx_tup>,
                                                   triqs::utility::real_or_complex>>()});
static const auto _c2py_doc_4 = _c2py_fun_4.doc(
   R"DOC(
Extract the quadratic part :math:`\sum_{ij} h_{ij} \hat{c}^\dagger_i \hat{c}_j` from a many-body operator.

Parameters
----------
H : {par_0}
   The many-body operator.
ignore_irrelevant : {par_1}
   If `true`, silently skip non-quadratic terms.

Returns
-------
{ret_0}
   Dictionary mapping index pairs :math:`(i, j)` to coefficients :math:`h_{ij}`.
)DOC",
   {{c2py::python_typename<const triqs::operators::many_body_operator &>()}, {c2py::python_typename<bool>()}},
   {c2py::python_typename<
      std::map<std::tuple<triqs::operators::utils::py::idx_tup, triqs::operators::utils::py::idx_tup>, triqs::utility::real_or_complex>>()});
static const auto _c2py_doc_5 = _c2py_fun_5.doc(
   R"DOC(
Build a block-diagonal quadratic operator from its block-matrix representation.

Given the block matrices :math:`h_{\sigma ij}` and the block structure, returns

.. math::

   \hat{h} = \sum_{\sigma ij} h_{\sigma ij} \hat{c}_{\sigma, i}^\dagger \hat{c}_{\sigma, j} \; .

Parameters
----------
bl_mat : {par_0}
   One matrix :math:`h_{\sigma ij}` per block :math:`\sigma`, packaged as a one-dimensional array of
   matrices.
gf_struct : {par_1}
   Block structure specifying the block labels and the size of each block.

Returns
-------
{ret_0}
   Many-body operator :math:`\hat{h}`.
)DOC",
   {{c2py::python_typename<const nda::basic_array<
       nda::basic_array<triqs::utility::real_or_complex, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>,
       1, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()},
    {c2py::python_typename<const triqs::gfs::gf_struct_t &>()}},
   {c2py::python_typename<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>>()});
static const auto _c2py_doc_6 =
   _c2py_fun_6.doc(R"DOC(
Keep only quadratic terms of a many-body operator :math:`\hat{h}`.

Parameters
----------
h : {par_0}
   Many-body operator :math:`\hat{h}`.

Returns
-------
{ret_0}
   Many-body operator containing only the quadratic terms.
)DOC",
                   {{c2py::python_typename<const triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex> &>()}},
                   {c2py::python_typename<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>>()});
static const auto _c2py_doc_7 =
   _c2py_fun_7.doc(R"DOC(
Keep only quartic terms of a many-body operator :math:`\hat{h}`.

Parameters
----------
h : {par_0}
   Many-body operator :math:`\hat{h}`.

Returns
-------
{ret_0}
   Many-body operator containing only the quartic terms.
)DOC",
                   {{c2py::python_typename<const triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex> &>()}},
                   {c2py::python_typename<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"block_matrix_from_op", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"dict_to_matrix", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"extract_U_dict2", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"extract_U_dict4", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {"extract_h_dict", (PyCFunction)c2py::pyfkw<_c2py_fun_4>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_4.c_str()},
   {"op_from_block_matrix", (PyCFunction)c2py::pyfkw<_c2py_fun_5>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_5.c_str()},
   {"quadratic_terms", (PyCFunction)c2py::pyfkw<_c2py_fun_6>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_6.c_str()},
   {"quartic_terms", (PyCFunction)c2py::pyfkw<_c2py_fun_7>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_7.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "extractors", /* name of module */
                                        R"RAWDOC(Extract coefficients (matrices and tensors) from many-body operators.

Given a :class:`~triqs.operators.operators.Operator`, the ``extract_*`` functions recover 
the coefficient dictionaries of a normally ordered operator that matches a specific structural pattern:

- :func:`extract_h_dict` — quadratic part :math:`\sum_{ij} h_{ij} \hat{c}_i^\dagger \hat{c}_j`,
- :func:`extract_U_dict2` — density-density interaction :math:`\frac{1}{2} \sum_{ij} U_{ij} \hat{n}_i \hat{n}_j`,
- :func:`extract_U_dict4` — general two-body interaction
  :math:`\frac{1}{2} \sum_{ijkl} U_{ijkl} \hat{c}_i^\dagger \hat{c}_j^\dagger \hat{c}_l \hat{c}_k`.

The returned dictionaries map tuples of single-particle indices to the corresponding coefficient, and can be converted
to dense arrays via :func:`dict_to_matrix` (given a Green's-function block structure). The companion functions
:func:`quadratic_terms`, :func:`quartic_terms`, :func:`block_matrix_from_op` and :func:`op_from_block_matrix` filter and
round-trip block-diagonal quadratic operators between the operator and matrix representations.
)RAWDOC",                                             /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_extractors() {

  if (not c2py::check_python_version("extractors")) return NULL;

  // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
#define _add_type(T, N) c2py::add_type_object_to_main<T>(N, m, conv_table)

#undef _add_type

  return m;
}
#endif
// CLAIR_WRAP_GEN
