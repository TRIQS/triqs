
// HAND-MAINTAINED: this file is NOT regenerable by clair-c2py. random_generator's only constructor
// takes an mpi::communicator, which has no c2py converter, so clair fails ("cannot convert argument /
// no wrapped constructor"). The constructor below is therefore wired by hand via c2py::c_maker,
// injecting the world communicator. Keep this file in sync with the C++ API manually.

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
// The C++ seeding constructor takes an mpi::communicator (no c2py converter); Python streams are
// seeded against the world communicator, so the rank is folded in automatically.
static const auto _c2py_init_0 = c2py::dispatcher_c_kw_t{c2py::c_maker<_c2py_cls_0>(
   +[](std::string name, uint64_t seed) -> _c2py_cls_0 { return _c2py_cls_0{std::move(name), seed, mpi::communicator{}}; },
   "name", "seed")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_0> = c2py::pyfkw_constructor<_c2py_init_0>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_0> = _c2py_init_0.doc(R"DOC(
Construct a random generator with an independent stream for each MPI rank.

The given name has to correspond to one of the supported engines. If the name does not match any of the
supported engines, a runtime error is raised. An empty name selects the default engine `std::mt19937_64`.

All ranks must pass the same seed; the rank of the world communicator is folded in so the ranks obtain
decorrelated streams (see triqs::mc_tools::splitmix_seed_seq).

------

Parameters
----------
name : {par_0}
   Name of the RNG to be used.
seed : {par_1}
   Seed shared by all ranks.
)DOC",
                                                                    {{c2py::python_typename<std::string>()}, {c2py::python_typename<uint64_t>()}});
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

Uses the standard 53-bit technique: the upper 53 bits of a 64-bit integer are scaled
to produce a double with full mantissa precision.

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

All supported engines produce 64-bit unsigned integers internally via the standard library.
Engines with native output smaller than 64 bits are wrapped using `std::independent_bits_engine`.

The following engines are supported (see also triqs::mc_tools::random_generator_names_list()):

- *empty string* or *mt19937_64*: uses `std::mt19937_64` (default, native 64-bit)
- *mt19937*: uses `std::mt19937` (32-bit, combined to 64-bit)
- *ranlux48*: uses `std::ranlux48` (48-bit, combined to 64-bit)
- *ranlux24*: uses `std::ranlux24` (24-bit, combined to 64-bit)
- *minstd_rand*: uses `std::minstd_rand` (31-bit, combined to 64-bit)
- *knuth_b*: uses `std::knuth_b` (31-bit, combined to 64-bit)

For performance, raw `uint64_t` values are generated in batches and stored in a buffer.
Doubles in [0, 1) are derived using the standard 53-bit technique.
Integers in [0, i) are generated using Lemire's nearly divisionless method (unbiased for all ranges).

All engines have their full state initialized through triqs::mc_tools::splitmix_seed_seq from the
seed and the MPI rank. Every rank passes the same seed and the rank (of the world communicator) is
folded in, so the Markov chains across ranks are decorrelated.

.. note::

   A random_generator is not thread-safe: a single instance must be used by one thread for
   its whole lifetime. Per-thread streams are not exposed today. In debug builds, drawing from a
   generator on a thread other than the one that created it triggers an assertion (the check is
   compiled out when `NDEBUG` is set).)DOC"
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
