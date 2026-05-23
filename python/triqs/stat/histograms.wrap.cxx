
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
using _c2py_cls_0                                            = triqs::stat::histogram;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs.stat.histograms.Histogram";
static const auto _c2py_init_0 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_0, int, int>("a", "b"),
                           c2py::c_constructor<_c2py_cls_0, double, double, long>("a", "b", "n_bins"), c2py::c_constructor<_c2py_cls_0>()};
template <> constexpr initproc c2py::tp_init<_c2py_cls_0> = c2py::pyfkw_constructor<_c2py_init_0>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_0> = _c2py_init_0.doc(R"DOC(
[1] Constructs a histogram over :math:`[a; b]` range with bin length equal to 1.

------

[2] Constructs a histogram over :math:`[a; b]` range with a given number of bins.

------

[3] Default constructor

------

Parameters
----------
a : {par_0}
   Left end of the sampling range
b : {par_1}
   Right end of the sampling range
n_bins : {par_2}
   Number of bins
)DOC",
                                                                    {{c2py::python_typename<int>(), c2py::python_typename<double>()},
                                                                     {c2py::python_typename<int>(), c2py::python_typename<double>()},
                                                                     {c2py::python_typename<long>()}});
// clear
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 &self) -> decltype(auto) { return self.clear(); }, "self")};

// mesh_point
static auto const _c2py_fun_1 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 const &self, int n) -> decltype(auto) { return self.mesh_point(n); }, "self", "n")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC(
Reset all histogram values to 0

Resets all data values and the total counts of accumulated and discarded points.
)DOC");
static const auto _c2py_doc_1 = _c2py_fun_1.doc(R"DOC(
Get position of bin's center

Get position of bin's center

Parameters
----------
n : {par_0}
   Bin index

Returns
-------
{ret_0}
   Position of the center, :math:`n (b - a) / (n_{bins} - 1)`
)DOC",
                                                {{c2py::python_typename<int>()}}, {c2py::python_typename<double>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {
   {"clear", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"mesh_point", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_0>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_0>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_0>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0 = R"DOC(Read-only access to the data storage

Read-only access to the data storage)DOC";
static constexpr auto prop_doc_1 = R"DOC(Get boundaries of the histogram

Get boundaries of the histogram)DOC";
static constexpr auto prop_doc_2 = R"DOC(Get number of accumulated samples

Get number of accumulated samples)DOC";
static constexpr auto prop_doc_3 = R"DOC(Get number of discarded samples

Get number of discarded samples)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_0>[] = {

   {"data", c2py::getter_from_method<c2py::castmc<>(&triqs::stat::histogram::data)>, nullptr, prop_doc_0, nullptr},
   {"limits", c2py::getter_from_method<c2py::castmc<>(&triqs::stat::histogram::limits)>, nullptr, prop_doc_1, nullptr},
   {"n_data_pts", c2py::getter_from_method<c2py::castmc<>(&triqs::stat::histogram::n_data_pts)>, nullptr, prop_doc_2, nullptr},
   {"n_lost_pts", c2py::getter_from_method<c2py::castmc<>(&triqs::stat::histogram::n_lost_pts)>, nullptr, prop_doc_3, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_0> = {c2py::tpxx_size<_c2py_cls_0>, nullptr, nullptr};

template <> struct c2py::arithmetic<_c2py_cls_0, c2py::OpName::Add> : std::tuple<std::pair<triqs::stat::histogram, triqs::stat::histogram>> {};

template <> struct c2py::arithmetic<_c2py_cls_0, c2py::OpName::LShift> : std::tuple<std::pair<triqs::stat::histogram, double>> {};

template <> constexpr PyNumberMethods *c2py::tp_as_number<_c2py_cls_0> = &c2py::tp_as_number_impl<_c2py_cls_0>;

template <>
const std::string c2py::tp_doc<_c2py_cls_0> = R"DOC(Statistical histogram

This class serves to sample a continuous random variable, and to 'bin' it.
It divides a given range of real values into a series of equal intervals,
and counts amounts of samples falling into each interval.
The histogram keeps track of the total number of the sampled values, as well
as of the lost samples that lie outside the chosen range.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_0>;

// ==================== module functions ====================

// cdf
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{c2py::cfun([](const triqs::stat::histogram &h) { return triqs::stat::cdf(h); }, "h")};

// pdf
static auto const _c2py_fun_3 = c2py::dispatcher_f_kw_t{c2py::cfun([](const triqs::stat::histogram &h) { return triqs::stat::pdf(h); }, "h")};

static const auto _c2py_doc_2 =
   _c2py_fun_2.doc(R"DOC(
Get histogram cumulative distribution function (CDF)

Integrate and normalise histogram to get cumulative distribution function (CDF)

Parameters
----------
h : {par_0}
   Histogram to be integrated and normalized

Returns
-------
{ret_0}
   Cumulative distribution function
)DOC",
                   {{c2py::python_typename<const triqs::stat::histogram &>()}}, {c2py::python_typename<triqs::stat::histogram>()});
static const auto _c2py_doc_3 =
   _c2py_fun_3.doc(R"DOC(
Get histogram probability density function (PDF)

Normalise histogram to get probability density function (PDF)

Parameters
----------
h : {par_0}
   Histogram to be normalized

Returns
-------
{ret_0}
   Probability density function
)DOC",
                   {{c2py::python_typename<const triqs::stat::histogram &>()}}, {c2py::python_typename<triqs::stat::histogram>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"cdf", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"pdf", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {
   PyModuleDef_HEAD_INIT,
   "histograms",                                                                                  /* name of module */
   R"RAWDOC(Statistical histograms for sampling and binning continuous random variables.)RAWDOC", /* module documentation, may be NULL */
   -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
   module_methods,
   NULL,
   NULL,
   NULL,
   NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_histograms() {

  if (not c2py::check_python_version("histograms")) return NULL;

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
  _add_type(_c2py_cls_0, "Histogram");
#undef _add_type

  c2py::pyref module = c2py::pyref::module("h5.formats");
  if (not module) return nullptr;
  c2py::pyref register_class = module.attr("register_class");

  register_h5_type<_c2py_cls_0>(register_class);

  return m;
}
#endif
// CLAIR_WRAP_GEN
