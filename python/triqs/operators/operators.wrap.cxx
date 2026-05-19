
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
using _c2py_cls_0                                            = triqs::operators::canonical_ops_t;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs.operators.operators.CanonicalOpsT";

static int synth_constructor_0(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::operators::canonical_ops_t.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_0> *)self)->_c = new _c2py_cls_0{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::operators::canonical_ops_t from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_0> *)self)->_c);
  de("dagger", self_c.dagger, false);
  de("indices", self_c.indices, false);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_0> = synth_constructor_0;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_0> =
   c2py::replace_tags(R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
dagger : {par_0}

indices : {par_1}

)DOC",
                      "par", {c2py::python_typename<bool>(), c2py::python_typename<triqs::operators::indices_t>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_0>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_0>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_0 = R"DOC()DOC";
constexpr auto _c2py_doc_member_1 = R"DOC()DOC";
static PyObject *prop_get_dict_0(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_0> *)self)->_c);
  c2py::pydict dic;
  dic["dagger"]  = self_c.dagger;
  dic["indices"] = self_c.indices;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_0>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_0::dagger, _c2py_cls_0>("dagger", _c2py_doc_member_0),
   c2py::getsetdef_from_member<&_c2py_cls_0::indices, _c2py_cls_0>("indices", _c2py_doc_member_1),
   {"__dict__", (getter)prop_get_dict_0, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_0> =
   R"DOC(The canonical operator: a dagger and some indices)DOC" + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_0>;
// --------- class _c2py_cls_1 -----------
using _c2py_cls_1                                            = triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_1>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_1> = "triqs.operators.operators.Operator";
static const auto _c2py_init_0                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_1>(),
   c2py::c_constructor<_c2py_cls_1, const triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>::scalar_t &>("x"),
   c2py::c_constructor<_c2py_cls_1, const triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>::scalar_t &,
                       triqs::operators::monomial_t>("x", "monomial")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_1>    = c2py::pyfkw_constructor<_c2py_init_0>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_1> = _c2py_init_0.doc(R"DOC()DOC");
// get_monomials
static auto const _c2py_fun_0 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_1 const &self) -> decltype(auto) { return self.get_monomials(); }, "self")};

// is_almost_zero
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_1 const &self, double precision) -> decltype(auto) { return self.is_almost_zero(precision); }, "self", "precision"_a = 1e-10)};

// is_zero
static auto const _c2py_fun_2 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_1 const &self) -> decltype(auto) { return self.is_zero(); }, "self")};

// make_canonical
static auto const _c2py_fun_3 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](bool is_dag, triqs::operators::indices_t indices) { return _c2py_cls_1::make_canonical(is_dag, indices); }, "is_dag", "indices")};

// make_fundamental_operator_set
static auto const _c2py_fun_4 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_1 const &self) -> decltype(auto) { return self.make_fundamental_operator_set(); }, "self")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC()DOC");
static const auto _c2py_doc_1 = _c2py_fun_1.doc(R"DOC(
Check if the operator is close to zero
)DOC");
static const auto _c2py_doc_2 = _c2py_fun_2.doc(R"DOC(
Check if the operator is identically zero
)DOC");
static const auto _c2py_doc_3 = _c2py_fun_3.doc(R"DOC()DOC");
static const auto _c2py_doc_4 = _c2py_fun_4.doc(R"DOC(
Make a minimal fundamental_operator_set with all the canonical operators of this
)DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_1>[] = {
   {"get_monomials", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"is_almost_zero", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"is_zero", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"make_canonical", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS | METH_STATIC, _c2py_doc_3.c_str()},
   {"make_fundamental_operator_set", (PyCFunction)c2py::pyfkw<_c2py_fun_4>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_4.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_1>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_1>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_1>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0 = R"DOC(Get a copy of the operator with the real parts of all monomial coefficients set to zero.)DOC";
static constexpr auto prop_doc_1 = R"DOC(Get a copy of the operator with the imaginary parts of all monomial coefficients set to zero.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_1>[] = {

   {"imag", c2py::getter_from_method<c2py::castmc<>(&triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>::imag)>, nullptr,
    prop_doc_0, nullptr},
   {"real", c2py::getter_from_method<c2py::castmc<>(&triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>::real)>, nullptr,
    prop_doc_1, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
struct c2py::arithmetic<_c2py_cls_1, c2py::OpName::Add>
   : std::tuple<std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>::scalar_t>,
                std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>::scalar_t,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>>,
                std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>>> {};

template <>
struct c2py::arithmetic<_c2py_cls_1, c2py::OpName::Sub>
   : std::tuple<std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>::scalar_t>,
                std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>::scalar_t,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>>,
                std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>>> {};

template <>
struct c2py::arithmetic<_c2py_cls_1, c2py::OpName::Mul>
   : std::tuple<std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>::scalar_t>,
                std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>::scalar_t,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>>,
                std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>>> {};

template <>
struct c2py::arithmetic<_c2py_cls_1, c2py::OpName::Div>
   : std::tuple<std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>::scalar_t>> {};

template <>
struct c2py::arithmetic<_c2py_cls_1, c2py::OpName::Neg> : std::tuple<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>> {
};

template <>
struct c2py::arithmetic<_c2py_cls_1, c2py::OpName::IAdd>
   : std::tuple<std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>::scalar_t>,
                std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>>> {};

template <>
struct c2py::arithmetic<_c2py_cls_1, c2py::OpName::ISub>
   : std::tuple<std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>::scalar_t>,
                std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>>> {};

template <>
struct c2py::arithmetic<_c2py_cls_1, c2py::OpName::IMul>
   : std::tuple<std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>::scalar_t>,
                std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>>> {};

template <>
struct c2py::arithmetic<_c2py_cls_1, c2py::OpName::IDiv>
   : std::tuple<std::pair<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>,
                          triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>::scalar_t>> {};

template <> constexpr PyNumberMethods *c2py::tp_as_number<_c2py_cls_1> = &c2py::tp_as_number_impl<_c2py_cls_1>;

template <>
const std::string c2py::tp_doc<_c2py_cls_1> = R"DOC(The generic class)DOC" + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_1>;

// ==================== module functions ====================

// c
static auto const _c2py_fun_5 = c2py::dispatcher_f_kw_t{
   c2py::cfun([]() { return triqs::operators::c<triqs::utility::real_or_complex>(); }),
   c2py::cfun(
      [](std::variant<long, std::string, double, std::array<long, 3>> indices) {
        return triqs::operators::c<triqs::utility::real_or_complex, std::variant<long, std::string, double, std::array<long, 3>>>(indices);
      },
      "indices"),
   c2py::cfun(
      [](std::variant<long, std::string, double, std::array<long, 3>> indices0,
         std::variant<long, std::string, double, std::array<long, 3>> indices1) {
        return triqs::operators::c<triqs::utility::real_or_complex, std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::variant<long, std::string, double, std::array<long, 3>>>(indices0, indices1);
      },
      "indices0", "indices1"),
   c2py::cfun(
      [](std::variant<long, std::string, double, std::array<long, 3>> indices0, std::variant<long, std::string, double, std::array<long, 3>> indices1,
         std::variant<long, std::string, double, std::array<long, 3>> indices2) {
        return triqs::operators::c<triqs::utility::real_or_complex, std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::variant<long, std::string, double, std::array<long, 3>>>(indices0, indices1, indices2);
      },
      "indices0", "indices1", "indices2"),
   c2py::cfun(
      [](std::variant<long, std::string, double, std::array<long, 3>> indices0, std::variant<long, std::string, double, std::array<long, 3>> indices1,
         std::variant<long, std::string, double, std::array<long, 3>> indices2,
         std::variant<long, std::string, double, std::array<long, 3>> indices3) {
        return triqs::operators::c<triqs::utility::real_or_complex, std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::variant<long, std::string, double, std::array<long, 3>>>(indices0, indices1, indices2, indices3);
      },
      "indices0", "indices1", "indices2", "indices3")};

// c_dag
static auto const _c2py_fun_6 = c2py::dispatcher_f_kw_t{
   c2py::cfun([]() { return triqs::operators::c_dag<triqs::utility::real_or_complex>(); }),
   c2py::cfun(
      [](std::variant<long, std::string, double, std::array<long, 3>> indices) {
        return triqs::operators::c_dag<triqs::utility::real_or_complex, std::variant<long, std::string, double, std::array<long, 3>>>(indices);
      },
      "indices"),
   c2py::cfun(
      [](std::variant<long, std::string, double, std::array<long, 3>> indices0,
         std::variant<long, std::string, double, std::array<long, 3>> indices1) {
        return triqs::operators::c_dag<triqs::utility::real_or_complex, std::variant<long, std::string, double, std::array<long, 3>>,
                                       std::variant<long, std::string, double, std::array<long, 3>>>(indices0, indices1);
      },
      "indices0", "indices1"),
   c2py::cfun(
      [](std::variant<long, std::string, double, std::array<long, 3>> indices0, std::variant<long, std::string, double, std::array<long, 3>> indices1,
         std::variant<long, std::string, double, std::array<long, 3>> indices2) {
        return triqs::operators::c_dag<triqs::utility::real_or_complex, std::variant<long, std::string, double, std::array<long, 3>>,
                                       std::variant<long, std::string, double, std::array<long, 3>>,
                                       std::variant<long, std::string, double, std::array<long, 3>>>(indices0, indices1, indices2);
      },
      "indices0", "indices1", "indices2"),
   c2py::cfun(
      [](std::variant<long, std::string, double, std::array<long, 3>> indices0, std::variant<long, std::string, double, std::array<long, 3>> indices1,
         std::variant<long, std::string, double, std::array<long, 3>> indices2,
         std::variant<long, std::string, double, std::array<long, 3>> indices3) {
        return triqs::operators::c_dag<triqs::utility::real_or_complex, std::variant<long, std::string, double, std::array<long, 3>>,
                                       std::variant<long, std::string, double, std::array<long, 3>>,
                                       std::variant<long, std::string, double, std::array<long, 3>>,
                                       std::variant<long, std::string, double, std::array<long, 3>>>(indices0, indices1, indices2, indices3);
      },
      "indices0", "indices1", "indices2", "indices3")};

// dagger
static auto const _c2py_fun_7 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex> &op) { return dagger(op); }, "op")};

// is_op_hermitian
static auto const _c2py_fun_8 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex> &op,
                 double tolerance) { return triqs::operators::is_op_hermitian<triqs::utility::real_or_complex>(op, tolerance); },
              "op", "tolerance"_a = 0.0)};

// n
static auto const _c2py_fun_9 = c2py::dispatcher_f_kw_t{
   c2py::cfun([]() { return triqs::operators::n<triqs::utility::real_or_complex>(); }),
   c2py::cfun(
      [](std::variant<long, std::string, double, std::array<long, 3>> indices) {
        return triqs::operators::n<triqs::utility::real_or_complex, std::variant<long, std::string, double, std::array<long, 3>>>(indices);
      },
      "indices"),
   c2py::cfun(
      [](std::variant<long, std::string, double, std::array<long, 3>> indices0,
         std::variant<long, std::string, double, std::array<long, 3>> indices1) {
        return triqs::operators::n<triqs::utility::real_or_complex, std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::variant<long, std::string, double, std::array<long, 3>>>(indices0, indices1);
      },
      "indices0", "indices1"),
   c2py::cfun(
      [](std::variant<long, std::string, double, std::array<long, 3>> indices0, std::variant<long, std::string, double, std::array<long, 3>> indices1,
         std::variant<long, std::string, double, std::array<long, 3>> indices2) {
        return triqs::operators::n<triqs::utility::real_or_complex, std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::variant<long, std::string, double, std::array<long, 3>>>(indices0, indices1, indices2);
      },
      "indices0", "indices1", "indices2"),
   c2py::cfun(
      [](std::variant<long, std::string, double, std::array<long, 3>> indices0, std::variant<long, std::string, double, std::array<long, 3>> indices1,
         std::variant<long, std::string, double, std::array<long, 3>> indices2,
         std::variant<long, std::string, double, std::array<long, 3>> indices3) {
        return triqs::operators::n<triqs::utility::real_or_complex, std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::variant<long, std::string, double, std::array<long, 3>>,
                                   std::variant<long, std::string, double, std::array<long, 3>>>(indices0, indices1, indices2, indices3);
      },
      "indices0", "indices1", "indices2", "indices3")};

static const auto _c2py_doc_5 = _c2py_fun_5.doc(R"DOC()DOC");
static const auto _c2py_doc_6 = _c2py_fun_6.doc(R"DOC()DOC");
static const auto _c2py_doc_7 = _c2py_fun_7.doc(R"DOC()DOC");
static const auto _c2py_doc_8 = _c2py_fun_8.doc(R"DOC()DOC");
static const auto _c2py_doc_9 = _c2py_fun_9.doc(R"DOC()DOC");
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"c", (PyCFunction)c2py::pyfkw<_c2py_fun_5>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_5.c_str()},
   {"c_dag", (PyCFunction)c2py::pyfkw<_c2py_fun_6>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_6.c_str()},
   {"dagger", (PyCFunction)c2py::pyfkw<_c2py_fun_7>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_7.c_str()},
   {"is_op_hermitian", (PyCFunction)c2py::pyfkw<_c2py_fun_8>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_8.c_str()},
   {"n", (PyCFunction)c2py::pyfkw<_c2py_fun_9>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_9.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "operators",       /* name of module */
                                        R"RAWDOC()RAWDOC", /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_operators() {

  if (not c2py::check_python_version("operators")) return NULL;

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
  _add_type(_c2py_cls_0, "CanonicalOpsT");
  _add_type(_c2py_cls_1, "Operator");
#undef _add_type

  c2py::pyref module = c2py::pyref::module("h5.formats");
  if (not module) return nullptr;
  c2py::pyref register_class = module.attr("register_class");

  register_h5_type<_c2py_cls_1>(register_class);

  return m;
}
#endif
// CLAIR_WRAP_GEN
