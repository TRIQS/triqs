
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

// bisection
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](std::function<double(double)> f, double x_low, double x_high, double y_target, double precision, long max_loops, std::string x_name,
      std::string y_name, bool verbosity) {
     return triqs::experimental::utility::bisection(f, x_low, x_high, y_target, precision, max_loops, x_name, y_name, verbosity);
   },
   "f", "x_low", "x_high", "y_target", "precision", "max_loops", "x_name", "y_name", "verbosity")};

// dichotomy
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](std::function<double(double)> f, double x_low, double x_high, double y_target, double precision, long max_loops, std::string x_name,
      std::string y_name, bool verbosity) {
     return triqs::experimental::utility::dichotomy(f, x_low, x_high, y_target, precision, max_loops, x_name, y_name, verbosity);
   },
   "f", "x_low", "x_high", "y_target", "precision", "max_loops", "x_name", "y_name", "verbosity")};

// find_bounds
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](std::function<double(double)> f, double x_init, double y_value, double delta_x, double precision,
                 long max_loops) { return triqs::experimental::utility::find_bounds(f, x_init, y_value, delta_x, precision, max_loops); },
              "f", "x_init", "y_value", "delta_x", "precision", "max_loops"_a = 1000)};

// root_finder
static auto const _c2py_fun_3 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](std::string method, std::function<double(double)> f, double x_init, double y_value, double precision, double delta_x, long max_loops,
      std::string x_name, std::string y_name, bool verbosity) {
     return triqs::experimental::utility::root_finder(method, f, x_init, y_value, precision, delta_x, max_loops, x_name, y_name, verbosity);
   },
   "method", "f", "x_init", "y_value", "precision", "delta_x", "max_loops"_a = 1000, "x_name"_a = "", "y_name"_a = "", "verbosity"_a = false)};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC(
bisection algorithm

Parameters
----------
f : {par_0}
   function f(x)
x_low : {par_1}
   lower bound on f(x)
x_high : {par_2}
   upper bound on f(x)
y_target : {par_3}
   target value for f(x) = y_target
precision : {par_4}
   f(x) - y_target < precision
max_loops : {par_5}
   maximum number of attempts
x_name : {par_6}
   Name of x variable
y_name : {par_7}
   Name of y variable
verbosity : {par_8}
   Print progress

Returns
-------
{ret_0}
   x, f(x) where f(x) = y_target
)DOC",
                                                {{c2py::python_typename<std::function<double(double)>>()},
                                                 {c2py::python_typename<double>()},
                                                 {c2py::python_typename<double>()},
                                                 {c2py::python_typename<double>()},
                                                 {c2py::python_typename<double>()},
                                                 {c2py::python_typename<long>()},
                                                 {c2py::python_typename<std::string>()},
                                                 {c2py::python_typename<std::string>()},
                                                 {c2py::python_typename<bool>()}},
                                                {c2py::python_typename<std::pair<double, double>>()});
static const auto _c2py_doc_1 = _c2py_fun_1.doc(R"DOC(
dichotomy algorithm

Parameters
----------
f : {par_0}
   function f(x)
x_low : {par_1}
   lower bound on x
x_high : {par_2}
   upper bound on x
y_target : {par_3}
   target value for f(x)
precision : {par_4}
   f(x) - y_target < precision
max_loops : {par_5}
   maxmimum number of iterations
x_name : {par_6}
   Name of x variable
y_name : {par_7}
   Name of y variable
verbosity : {par_8}
   Print progress

Returns
-------
{ret_0}
   x, f(x) where f(x) = y_target
)DOC",
                                                {{c2py::python_typename<std::function<double(double)>>()},
                                                 {c2py::python_typename<double>()},
                                                 {c2py::python_typename<double>()},
                                                 {c2py::python_typename<double>()},
                                                 {c2py::python_typename<double>()},
                                                 {c2py::python_typename<long>()},
                                                 {c2py::python_typename<std::string>()},
                                                 {c2py::python_typename<std::string>()},
                                                 {c2py::python_typename<bool>()}},
                                                {c2py::python_typename<std::pair<double, double>>()});
static const auto _c2py_doc_2 = _c2py_fun_2.doc(R"DOC(
find upper and lower bounds of f(x)

Parameters
----------
f : {par_0}
   function of f(x)
x_init : {par_1}
   initial guess x
y_value : {par_2}
   target value y = f(x)
delta_x : {par_3}
   x increment
precision : {par_4}
   f(x) - y_value < precision
max_loops : {par_5}
   maximum number of loops

Returns
-------
{ret_0}
   x1, x2 where f(x1) and f(x2) bound y_value
)DOC",
                                                {{c2py::python_typename<std::function<double(double)>>()},
                                                 {c2py::python_typename<double>()},
                                                 {c2py::python_typename<double>()},
                                                 {c2py::python_typename<double>()},
                                                 {c2py::python_typename<double>()},
                                                 {c2py::python_typename<long>()}},
                                                {c2py::python_typename<std::pair<double, double>>()});
static const auto _c2py_doc_3 = _c2py_fun_3.doc(R"DOC(
Root finder :math:`f(x) = 0`.

Parameters
----------
method : {par_0}
   Root finding method (`dichtomy` or `bisection`).
f : {par_1}
   :math:`f(x) : \mathbb{R} \to \mathbb{R}`.
x_init : {par_2}
   Initial value for :math:`x`.
y_value : {par_3}
   Target value for :math:`y`.
precision : {par_4}
   Precision for algorithm.
delta_x : {par_5}
   Increment of :math:`x`.
max_loops : {par_6}
   Max number of iterations.
x_name : {par_7}
   Name of :math:`x` variable.
y_name : {par_8}
   Name of :math:`y = f(x)` variable.
verbosity : {par_9}
   Turn on/off logging.

Returns
-------
{ret_0}
   :math:`x, f(x)` where :math:`f(x) = y`.
)DOC",
                                                {{c2py::python_typename<std::string>()},
                                                 {c2py::python_typename<std::function<double(double)>>()},
                                                 {c2py::python_typename<double>()},
                                                 {c2py::python_typename<double>()},
                                                 {c2py::python_typename<double>()},
                                                 {c2py::python_typename<double>()},
                                                 {c2py::python_typename<long>()},
                                                 {c2py::python_typename<std::string>()},
                                                 {c2py::python_typename<std::string>()},
                                                 {c2py::python_typename<bool>()}},
                                                {c2py::python_typename<std::pair<double, double>>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"bisection", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"dichotomy", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"find_bounds", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"root_finder", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {
   PyModuleDef_HEAD_INIT,
   "utility",                                                                       /* name of module */
   R"RAWDOC(Experimental utility tools (root finder, adaptive integration))RAWDOC", /* module documentation, may be NULL */
   -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
   module_methods,
   NULL,
   NULL,
   NULL,
   NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_utility() {

  if (not c2py::check_python_version("utility")) return NULL;

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
