
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

// --------- class _c2py_cls_d1d99893 -----------
using _c2py_cls_d1d99893                                            = triqs::utility::tau_t;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_d1d99893>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_d1d99893> = "triqs.utility.utilities.TauT";
static const auto _c2py_init_45542d69 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_d1d99893>(), c2py::c_constructor<_c2py_cls_d1d99893, uint64_t>("n_")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_d1d99893> = c2py::pyfkw_constructor<_c2py_init_45542d69>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_d1d99893> = _c2py_init_45542d69.doc(R"DOC(
Construct from a raw grid index.
)DOC");
// beta
static auto const _c2py_fun_f70e8d99 = c2py::dispatcher_f_kw_t{c2py::cfun([]() { return _c2py_cls_d1d99893::beta(); })};

// epsilon
static auto const _c2py_fun_6724fc93 = c2py::dispatcher_f_kw_t{c2py::cfun([]() { return _c2py_cls_d1d99893::epsilon(); })};

// from_double
static auto const _c2py_fun_89d58a4d = c2py::dispatcher_f_kw_t{c2py::cfun([](double x) { return _c2py_cls_d1d99893::from_double(x); }, "x")};

// get_beta_value
static auto const _c2py_fun_c8b1741e = c2py::dispatcher_f_kw_t{c2py::cfun([]() { return _c2py_cls_d1d99893::get_beta_value(); })};

// get_n
static auto const _c2py_fun_959264c8 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_d1d99893 const &self) -> decltype(auto) { return self.get_n(); }, "self")};

// set_beta
static auto const _c2py_fun_1e0fccc8 = c2py::dispatcher_f_kw_t{c2py::cfun([](double beta) { return _c2py_cls_d1d99893::set_beta(beta); }, "beta")};

// zero
static auto const _c2py_fun_06f254b5 = c2py::dispatcher_f_kw_t{c2py::cfun([]() { return _c2py_cls_d1d99893::zero(); })};

static const auto _c2py_doc_f70e8d99 = _c2py_fun_f70e8d99.doc(R"DOC(
:math:`\tau = \beta`
)DOC");
static const auto _c2py_doc_6724fc93 = _c2py_fun_6724fc93.doc(R"DOC(
Smallest representable positive time, :math:`\epsilon = \beta / N_\mathrm{max}`.
)DOC");
static const auto _c2py_doc_89d58a4d = _c2py_fun_89d58a4d.doc(R"DOC(
Create a tau_t from a double value in :math:`[0, \beta]`.
)DOC");
static const auto _c2py_doc_c8b1741e = _c2py_fun_c8b1741e.doc(R"DOC(
Get the current inverse temperature.
)DOC");
static const auto _c2py_doc_959264c8 = _c2py_fun_959264c8.doc(R"DOC(
Access the raw grid index.
)DOC");
static const auto _c2py_doc_1e0fccc8 = _c2py_fun_1e0fccc8.doc(R"DOC(
Set the inverse temperature for all tau_t instances.
)DOC");
static const auto _c2py_doc_06f254b5 = _c2py_fun_06f254b5.doc(R"DOC(
:math:`\tau = 0`
)DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_d1d99893>[] = {
   PMDF("beta", f70e8d99, METH_STATIC),
   PMDF("epsilon", 6724fc93, METH_STATIC),
   PMDF("from_double", 89d58a4d, METH_STATIC),
   PMDF("get_beta_value", c8b1741e, METH_STATIC),
   PMDF("get_n", 959264c8),
   PMDF("set_beta", 1e0fccc8, METH_STATIC),
   PMDF("zero", 06f254b5, METH_STATIC),
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_d1d99893>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_h5<_c2py_cls_d1d99893>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_h5<_c2py_cls_d1d99893>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <>
const std::string c2py::tp_doc<_c2py_cls_d1d99893> = R"DOC(Discretized imaginary time point.

Represents a point :math:`\tau \in [0, \beta]` on a very fine integer grid.
The position is stored as a `uint64_t`, allowing exact comparisons
that would be dangerous on floating-point numbers.

The inverse temperature `beta` is stored as a static member shared by all instances.
It must be set via `set_beta()` before use.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_d1d99893>;

// ==================== module functions ====================

//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "utilities",                                                          /* name of module */
                                        R"RAWDOC(Utilities generated from C++ code in triqs/utility.)RAWDOC", /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_utilities() {

  if (not c2py::check_python_version("utilities")) return NULL;

  // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_d1d99893>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
#define _add_type(T, N) c2py::add_type_object_to_main<T>(N, m, conv_table)
  _add_type(_c2py_cls_d1d99893, "TauT");
#undef _add_type

  c2py::pyref module = c2py::pyref::module("h5.formats");
  if (not module) return nullptr;
  c2py::pyref register_class = module.attr("register_class");

  register_h5_type<_c2py_cls_d1d99893>(register_class);

  return m;
}
#endif
// CLAIR_WRAP_GEN
