
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
using _c2py_cls_0                                            = triqs::mc_tools::random_generator;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs.mc_tools.random_generator.RandomGenerator";
static const auto _c2py_init_0 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_0>(),
                           c2py::c_constructor<_c2py_cls_0, std::string, unsigned int, unsigned long>("name", "seed", "buffer_size"_a = 1000)};
template <> constexpr initproc c2py::tp_init<_c2py_cls_0> = c2py::pyfkw_constructor<_c2py_init_0>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_0> =
   _c2py_init_0.doc(R"DOC(
[1] Default constructor uses the *mt19937* engine with the default seed.

------

[2] Construct a random generator by wrapping the specified RNG and seeding it with the given seed.

The given name has to correspond to one of the supported engines. If the name does not match any of the 
supported engines, a runtime error is raised.

An empty name selects the built-in custom Mersenne Twister RNG.

------

Parameters
----------
name : {par_0}
   Name of the RNG to be used.
seed : {par_1}
   Seed for the RNG.
buffer_size : {par_2}
   Size of the buffer used to store random numbers.
)DOC",
                    {{c2py::python_typename<std::string>()}, {c2py::python_typename<unsigned int>()}, {c2py::python_typename<unsigned long>()}});
// __call__
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_0 &self) -> decltype(auto) { return self.operator()(); }, "self"),
   c2py::cmethod([](_c2py_cls_0 &self, double b) -> decltype(auto) { return self.operator()(b); }, "self", "b"),
   c2py::cmethod([](_c2py_cls_0 &self, double a, double b) -> decltype(auto) { return self.operator()(a, b); }, "self", "a", "b")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_0> = c2py::pyfkw<_c2py_fun_0>;

// name
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 const &self) -> decltype(auto) { return self.name(); }, "self")};

// preview
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 &self) -> decltype(auto) { return self.preview(); }, "self")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC(
[1] Generate a random sample from the uniform distribution defined on the interval :math:`[0, 1)`.

------

[2] Generate a random sample from the uniform distribution defined on the interval :math:`[0, b)`.

------

[3] Generate a random sample from the uniform distribution defined on the interval :math:`[a, b)`.

------

Parameters
----------
b : {par_0}
   Upper bound of the interval.
a : {par_1}
   Lower bound of the interval.

Returns
-------
[1] : {ret_0}
   Uniform random double from the interval :math:`[0, 1)`.

[2] : {ret_1}
   Uniform random double from the interval :math:`[0, b)`.

[3] : {ret_2}
   Uniform random double from the interval :math:`[a, b)`.
)DOC",
                                                {{c2py::python_typename<double>()}, {c2py::python_typename<double>()}},
                                                {c2py::python_typename<double>(), c2py::python_typename<double>(), c2py::python_typename<double>()});
static const auto _c2py_doc_1 = _c2py_fun_1.doc(R"DOC(
Get the name of the underlying RNG.
)DOC");
static const auto _c2py_doc_2 = _c2py_fun_2.doc(R"DOC(
Look ahead at the next value that will be generated when the generator is called.

Returns
-------
{ret_0}
   Uniform random double from the interval :math:`[0, 1)`.
)DOC",
                                                {}, {c2py::python_typename<double>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {
   {"name", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"preview", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_0>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_h5<_c2py_cls_0>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_h5<_c2py_cls_0>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <>
const std::string c2py::tp_doc<_c2py_cls_0> = R"DOC(Random number generator with a selectable underlying engine.

The following engine names are currently accepted by the constructor:

- *empty string* -- a built-in custom Mersenne Twister RNG
- *mt19937*
- *mt11213b*
- *lagged_fibonacci607*
- *lagged_fibonacci1279*
- *lagged_fibonacci2281*
- *lagged_fibonacci3217*
- *lagged_fibonacci4423*
- *lagged_fibonacci9689*
- *lagged_fibonacci19937*
- *lagged_fibonacci23209*
- *lagged_fibonacci44497*
- *ranlux3*

For non-empty names, the underlying engine is provided by boost.random.

For performance reasons, we use a buffer for the generated random numbers to avoid some of the costs of repeated
function calls to the RNG.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_0>;

// ==================== module functions ====================

// random_generator_names
static auto const _c2py_fun_3 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const std::string &sep) { return triqs::mc_tools::random_generator_names(sep); }, "sep"_a = " ")};

// random_generator_names_list
static auto const _c2py_fun_4 = c2py::dispatcher_f_kw_t{c2py::cfun([]() { return triqs::mc_tools::random_generator_names_list(); })};

static const auto _c2py_doc_3 = _c2py_fun_3.doc(R"DOC(
Get a string containing the names of all available RNGs.

Parameters
----------
sep : {par_0}
   Separator between the names.

Returns
-------
{ret_0}
   String containing the available RNGs separated by the given separator.
)DOC",
                                                {{c2py::python_typename<const std::string &>()}}, {c2py::python_typename<std::string>()});
static const auto _c2py_doc_4 = _c2py_fun_4.doc(R"DOC(
Get a list of all available RNG names.
)DOC");
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"random_generator_names", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {"random_generator_names_list", (PyCFunction)c2py::pyfkw<_c2py_fun_4>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_4.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {
   PyModuleDef_HEAD_INIT,
   "random_generator",                                                              /* name of module */
   R"RAWDOC(Random number generator utilities for Monte Carlo simulations.)RAWDOC", /* module documentation, may be NULL */
   -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
   module_methods,
   NULL,
   NULL,
   NULL,
   NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_random_generator() {

  if (not c2py::check_python_version("random_generator")) return NULL;

  // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_0>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
#define _add_type(T, N) c2py::add_type_object_to_main<T>(N, m, conv_table)
  _add_type(_c2py_cls_0, "RandomGenerator");
#undef _add_type

  c2py::pyref module = c2py::pyref::module("h5.formats");
  if (not module) return nullptr;
  c2py::pyref register_class = module.attr("register_class");

  register_h5_type<_c2py_cls_0>(register_class);

  return m;
}
#endif
// CLAIR_WRAP_GEN
