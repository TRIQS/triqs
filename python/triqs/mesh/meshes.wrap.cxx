
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

template <> constexpr bool c2py::is_wrapped<cppdlr::statistic_t> = true;
template <>
const std::map<cppdlr::statistic_t, str_t> c2py::enum_to_string<cppdlr::statistic_t> = {{cppdlr::statistic_t::Boson, "Boson"},
                                                                                        {cppdlr::statistic_t::Fermion, "Fermion"}};
template <> constexpr bool c2py::is_wrapped<triqs::mesh::statistic_enum>             = true;
template <>
const std::map<triqs::mesh::statistic_enum, str_t> c2py::enum_to_string<triqs::mesh::statistic_enum> = {
   {triqs::mesh::statistic_enum::Boson, "Boson"},
   {triqs::mesh::statistic_enum::Fermion, "Fermion"}};
template <> constexpr bool c2py::is_wrapped<triqs::mesh::channel_enum> = true;
template <>
const std::map<triqs::mesh::channel_enum, str_t> c2py::enum_to_string<triqs::mesh::channel_enum> = {{triqs::mesh::channel_enum::PP, "PP"},
                                                                                                    {triqs::mesh::channel_enum::PH, "PH"}};
template <> constexpr bool c2py::is_wrapped<triqs::mesh::imfreq::option>                         = true;
template <>
const std::map<triqs::mesh::imfreq::option, str_t> c2py::enum_to_string<triqs::mesh::imfreq::option> = {
   {triqs::mesh::imfreq::option::all_frequencies, "all_frequencies"},
   {triqs::mesh::imfreq::option::positive_frequencies_only, "positive_frequencies_only"}};

// ==================== module classes =====================

// --------- class _c2py_cls_0 -----------
using _c2py_cls_0                                            = triqs::mesh::imtime;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs.mesh.meshes.MeshImTime";
static const auto _c2py_init_0 = c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_0, double, triqs::mesh::statistic_enum, long>(
                                                            "beta"_a = 1.0, "statistic"_a = triqs::mesh::Fermion, "n_tau"_a = 0)}
                                    .with_deprecated_params({{"S", "statistic"}, {"n_max", "n_tau"}});
template <> constexpr initproc c2py::tp_init<_c2py_cls_0> = c2py::pyfkw_constructor<_c2py_init_0>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_0> =
   _c2py_init_0.doc(R"DOC(
Construct an imaginary time mesh on the interval :math:`[0, \beta]` with :math:`N \geq 0` equally spaced
mesh points and the given particle statistics.

Parameters
----------
beta : {par_0}
   Inverse temperature :math:`\beta > 0`.
statistic : {par_1}
   Particle statistics.
n_tau : {par_2}
   Size of the mesh.
)DOC",
                    {{c2py::python_typename<double>()}, {c2py::python_typename<triqs::mesh::statistic_enum>()}, {c2py::python_typename<long>()}});
// __call__
static auto const _c2py_fun_0 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 const &self, long n) -> decltype(auto) { return self.operator()(n); }, "self", "n")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_0> = c2py::pyfkw<_c2py_fun_0>;

// copy
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy<triqs::mesh::imtime>, "m")};

// copy_from
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy_from<triqs::mesh::imtime>, "m1", "m2")};

// is_index_valid
static auto const _c2py_fun_3 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 const &self, long n) -> decltype(auto) { return self.is_index_valid(n); }, "self", "n")};

// to_data_index
static auto const _c2py_fun_4 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 const &self, long n) -> decltype(auto) { return self.to_data_index(n); }, "self", "n")};

// to_index
static auto const _c2py_fun_5 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 const &self, long d) -> decltype(auto) { return self.to_index(d); }, "self", "d")};

// to_value
static auto const _c2py_fun_6 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 const &self, long n) -> decltype(auto) { return self.to_value(n); }, "self", "n")};

// values
static auto const _c2py_fun_7 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::values<triqs::mesh::imtime>, "m")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC(
Function call operator to access a mesh point by its index :math:`n \in \{0, 1, \ldots, N-1\}`.

Parameters
----------
n : {par_0}
   Index :math:`n` of the mesh point.

Returns
-------
{ret_0}
   mesh_point_t with the index :math:`n`, data index :math:`d(n) = n`, hash value of the current mesh and
   value :math:`m(n) = a + n \cdot \Delta`.
)DOC",
                                                {{c2py::python_typename<long>()}},
                                                {c2py::python_typename<triqs::mesh::detail::linear<triqs::mesh::imtime, double>::mesh_point_t>()});
static const auto _c2py_doc_1 =
   _c2py_fun_1.doc(R"DOC(
Get a copy of a mesh (for Python bindings).

Parameters
----------
m : {par_0}
   The mesh object to copy.

Returns
-------
{ret_0}
   Copy of the given mesh.
)DOC",
                   {{c2py::python_typename<const triqs::mesh::imtime &>()}}, {c2py::python_typename<triqs::mesh::imtime>()});
static const auto _c2py_doc_2 =
   _c2py_fun_2.doc(R"DOC(
Copy one mesh into another (for Python bindings).

Simply calls the copy assignment operator of the mesh.

Parameters
----------
m1 : {par_0}
   The mesh object to copy into.
m2 : {par_1}
   The mesh object to copy from.
)DOC",
                   {{c2py::python_typename<triqs::mesh::imtime &>()}, {c2py::python_typename<const triqs::mesh::imtime &>()}});
static const auto _c2py_doc_3 = _c2py_fun_3.doc(R"DOC(
Check if an index :math:`n` is valid.

Parameters
----------
n : {par_0}
   Index :math:`n` to check.

Returns
-------
{ret_0}
   True if :math:`0 \leq n < N`, false otherwise.
)DOC",
                                                {{c2py::python_typename<long>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_4 = _c2py_fun_4.doc(R"DOC(
Map an index :math:`n \in \{0, 1, \ldots, N-1\}` to its corresponding data index :math:`d(n)`.

Parameters
----------
n : {par_0}
   Index :math:`n` to map.

Returns
-------
{ret_0}
   Data index :math:`d(n) = n`.
)DOC",
                                                {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_5 = _c2py_fun_5.doc(R"DOC(
Map a data index :math:`d \in \{0, 1, \ldots, N-1\}` to the corresponding index :math:`n(d)`.

Parameters
----------
d : {par_0}
   Data index :math:`d` to map.

Returns
-------
{ret_0}
   Index :math:`n(d) = d`.
)DOC",
                                                {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_6 = _c2py_fun_6.doc(R"DOC(
Map an index :math:`n \in \{0, 1, \ldots, N-1\}` to its corresponding value :math:`m(n)`.

Parameters
----------
n : {par_0}
   Index :math:`n` to map.

Returns
-------
{ret_0}
   Value of the mesh point :math:`m(n) = a + n \cdot \Delta`.
)DOC",
                                                {{c2py::python_typename<long>()}}, {c2py::python_typename<double>()});
static const auto _c2py_doc_7 = _c2py_fun_7.doc(
   R"DOC(
Get the values of all mesh points in a mesh.

Parameters
----------
m : {par_0}
   A mesh object.

Returns
-------
{ret_0}
   Array containing the values of all mesh points.
)DOC",
   {{c2py::python_typename<const triqs::mesh::imtime &>()}},
   {c2py::python_typename<nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {
   {"copy", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"copy_from", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"is_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {"to_data_index", (PyCFunction)c2py::pyfkw<_c2py_fun_4>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_4.c_str()},
   {"to_index", (PyCFunction)c2py::pyfkw<_c2py_fun_5>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_5.c_str()},
   {"to_value", (PyCFunction)c2py::pyfkw<_c2py_fun_6>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_6.c_str()},
   {"values", (PyCFunction)c2py::pyfkw<_c2py_fun_7>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_7.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_0>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_0>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_0>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0 = R"DOC(Get the inverse temperature :math:`\beta`.)DOC";
static constexpr auto prop_doc_1 = R"DOC(Get the step size :math:`\Delta` of the mesh, i.e. the distance between two consecutive mesh points.)DOC";
static constexpr auto prop_doc_2 = R"DOC(Get the inverse of the step size of the mesh, i.e. :math:`1 / \Delta`.)DOC";
static constexpr auto prop_doc_3 = R"DOC(Get the first index of the mesh, i.e. :math:`0`.)DOC";
static constexpr auto prop_doc_4 = R"DOC(Get the last index of the mesh, i.e. :math:`N - 1`.)DOC";
static constexpr auto prop_doc_5 = R"DOC(Get the hash value of the mesh.)DOC";
static constexpr auto prop_doc_6 = R"DOC(Get the particle statistics.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_0>[] = {

   {"beta", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::imtime::beta)>, nullptr, prop_doc_0, nullptr},
   {"delta", c2py::getter_from_method_B<triqs::mesh::imtime, c2py::castmc<>(&triqs::mesh::detail::linear<triqs::mesh::imtime, double>::delta)>,
    nullptr, prop_doc_1, nullptr},
   {"delta_inv",
    c2py::getter_from_method_B<triqs::mesh::imtime, c2py::castmc<>(&triqs::mesh::detail::linear<triqs::mesh::imtime, double>::delta_inv)>, nullptr,
    prop_doc_2, nullptr},
   {"first_index",
    c2py::getter_from_method_B<triqs::mesh::imtime, c2py::castmc<>(&triqs::mesh::detail::linear<triqs::mesh::imtime, double>::first_index)>, nullptr,
    prop_doc_3, nullptr},
   {"last_index",
    c2py::getter_from_method_B<triqs::mesh::imtime, c2py::castmc<>(&triqs::mesh::detail::linear<triqs::mesh::imtime, double>::last_index)>, nullptr,
    prop_doc_4, nullptr},
   {"mesh_hash",
    c2py::getter_from_method_B<triqs::mesh::imtime, c2py::castmc<>(&triqs::mesh::detail::linear<triqs::mesh::imtime, double>::mesh_hash)>, nullptr,
    prop_doc_5, nullptr},
   {"statistic", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::imtime::statistic)>, nullptr, prop_doc_6, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_0(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_0, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_0> = {c2py::tpxx_size<_c2py_cls_0>, getitem_0, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_0> = R"DOC(Imaginary time mesh type.

An imaginary time mesh is defined by its size :math:`N \geq 0`, an inverse temperature :math:`\beta > 0`
and its particle statistics. It contains :math:`N` equally spaced mesh points on the interval :math:`[0, \beta]`
such that the distance between two consecutive mesh points (step size) is constant.

An imaginary time mesh has the following properties:

- Each mesh point is identified by a unique index :math:`n \in \{0, 1, \ldots, N-1\}`.
- An index :math:`n` is mapped to the corresponding data index :math:`d` by the identity function :math:`d(n) = n`
  and vice versa.
- An index :math:`n` is mapped to the corresponding value :math:`\tau` by the linear function
  :math:`\tau(n) = n \cdot \Delta` such that :math:`\tau(0) = 0` and :math:`\tau(N-1) = \beta`. The step size of
  the mesh is :math:`\Delta = \frac{\beta}{N - 1}` for :math:`N > 1`, otherwise it is undefined. For implementation
  purposes, we set :math:`\Delta = 0` and :math:`\Delta^{-1} = 0` for :math:`N = 0` and :math:`\Delta = 0` and
  :math:`\Delta^{-1} = \infty` for :math:`N = 1`.
- An arbitrary value :math:`\tau \in [0, \beta]` is mapped to the closest mesh point with index :math:`n` by the
  function :math:`n(\tau) = \left\lfloor \frac{\tau}{\Delta} + 0.5 \right\rfloor`.

Green's function containers that are based on an imaginary time mesh store the function values at the discrete time
points :math:`\tau(n)`, i.e. :math:`f_n = f(\tau(n))`, and use linear interpolation to evaluate the function at an
arbitrary imaginary time :math:`\tau \in [0, \beta]`.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_0>;
// --------- class _c2py_cls_1 -----------
using _c2py_cls_1                                            = triqs::mesh::energy_t;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_1>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_1> = "triqs.mesh.meshes.EnergyT";

static int synth_constructor_0(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::mesh::energy_t.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_1> *)self)->_c = new _c2py_cls_1{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError, ("Error in constructing triqs::mesh::energy_t from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_1> *)self)->_c);
  de("value", self_c.value, true);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_1> = synth_constructor_0;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_1> = c2py::replace_tags(R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
value : {par_0}, default=0

)DOC",
                                                                      "par", {c2py::python_typename<double>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_1>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_0 = R"DOC(Energy value.)DOC";
static PyObject *prop_get_dict_0(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_1> *)self)->_c);
  c2py::pydict dic;
  dic["value"] = self_c.value;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_1>[] = {c2py::getsetdef_from_member<&_c2py_cls_1::value, _c2py_cls_1>("value", _c2py_doc_member_0),
                                                        {"__dict__", (getter)prop_get_dict_0, nullptr, "", nullptr},
                                                        {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_1> = R"DOC(Represents an energy value to distinguish constructors in imaginary-frequency mesh.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_1>;
// --------- class _c2py_cls_2 -----------
using _c2py_cls_2                                            = triqs::mesh::imfreq;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_2>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_2> = "triqs.mesh.meshes.MeshImFreq";
static const auto _c2py_init_1 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_2>(),
                           c2py::c_constructor<_c2py_cls_2, double, triqs::mesh::statistic_enum, long, triqs::mesh::imfreq::option>(
                              "beta", "statistic", "n_iw"_a = 1025, "opt"_a = triqs::mesh::imfreq::option::all_frequencies)}
      .with_deprecated_params({{"S", "statistic"}, {"n_max", "n_iw"}});
template <> constexpr initproc c2py::tp_init<_c2py_cls_2> = c2py::pyfkw_constructor<_c2py_init_1>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_2> = _c2py_init_1.doc(R"DOC(
[1] Default constructor constructs an empty mesh.

------

[2] Construct an imaginary frequency mesh with :math:`N_{i\omega_n} \geq 0` positive Matsubara frequencies.

------

Parameters
----------
beta : {par_0}
   Inverse temperature :math:`\beta > 0`.
statistic : {par_1}
   Particle statistics.
n_iw : {par_2}
   Number of positive Matsubara frequencies, i.e. :math:`N_{i\omega_n}`.
opt : {par_3}
   Whether to use all or only the positive frequencies.
)DOC",
                                                                    {{c2py::python_typename<double>()},
                                                                     {c2py::python_typename<triqs::mesh::statistic_enum>()},
                                                                     {c2py::python_typename<long>()},
                                                                     {c2py::python_typename<triqs::mesh::imfreq::option>()}});
// __call__
static auto const _c2py_fun_8 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_2 const &self, long n) -> decltype(auto) { return self.operator()(n); }, "self", "n")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_2> = c2py::pyfkw<_c2py_fun_8>;

// copy
static auto const _c2py_fun_9 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy<triqs::mesh::imfreq>, "m")};

// copy_from
static auto const _c2py_fun_10 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy_from<triqs::mesh::imfreq>, "m1", "m2")};

// is_index_valid
static auto const _c2py_fun_11 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_2 const &self, long n) -> decltype(auto) { return self.is_index_valid(n); }, "self", "n")};

// set_tail_fit_parameters
static auto const _c2py_fun_12 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_2 const &self, double tail_fraction, int n_tail_max, std::optional<int> expansion_order) -> decltype(auto) {
     return self.set_tail_fit_parameters(tail_fraction, n_tail_max, expansion_order);
   },
   "self", "tail_fraction", "n_tail_max"_a = triqs::mesh::tail_fitter::default_n_tail_max, "expansion_order"_a = std::optional<int>{})};

// to_data_index
static auto const _c2py_fun_13 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_2 const &self, long n) -> decltype(auto) { return self.to_data_index(n); }, "self", "n"),
   c2py::cmethod([](_c2py_cls_2 const &self, const triqs::mesh::matsubara_freq &iw) -> decltype(auto) { return self.to_data_index(iw); }, "self",
                 "iw")};

// to_index
static auto const _c2py_fun_14 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_2 const &self, long d) -> decltype(auto) { return self.to_index(d); }, "self", "d")};

// to_value
static auto const _c2py_fun_15 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_2 const &self, long n) -> decltype(auto) { return self.to_value(n); }, "self", "n")};

// values
static auto const _c2py_fun_16 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::values<triqs::mesh::imfreq>, "m")};

static const auto _c2py_doc_8 = _c2py_fun_8.doc(R"DOC(
Function call operator to access a mesh point by its Matsubara index :math:`n \in \{ n_{\text{min}}, \dots,
n_{\text{max}} \}`.

Parameters
----------
n : {par_0}
   Matsubara index :math:`n` of the mesh point.

Returns
-------
{ret_0}
   mesh_point_t with the Matsubara index :math:`n`, data index :math:`d(n) = n - n_{\text{min}}` and same
   :math:`\beta`, particle statistics and hash value as the current mesh.
)DOC",
                                                {{c2py::python_typename<long>()}}, {c2py::python_typename<triqs::mesh::imfreq::mesh_point_t>()});
static const auto _c2py_doc_9 =
   _c2py_fun_9.doc(R"DOC(
Get a copy of a mesh (for Python bindings).

Parameters
----------
m : {par_0}
   The mesh object to copy.

Returns
-------
{ret_0}
   Copy of the given mesh.
)DOC",
                   {{c2py::python_typename<const triqs::mesh::imfreq &>()}}, {c2py::python_typename<triqs::mesh::imfreq>()});
static const auto _c2py_doc_10 =
   _c2py_fun_10.doc(R"DOC(
Copy one mesh into another (for Python bindings).

Simply calls the copy assignment operator of the mesh.

Parameters
----------
m1 : {par_0}
   The mesh object to copy into.
m2 : {par_1}
   The mesh object to copy from.
)DOC",
                    {{c2py::python_typename<triqs::mesh::imfreq &>()}, {c2py::python_typename<const triqs::mesh::imfreq &>()}});
static const auto _c2py_doc_11 = _c2py_fun_11.doc(R"DOC(
Check if a Matsubara index :math:`n` is valid.

Parameters
----------
n : {par_0}
   Matsubara index :math:`n` to check.

Returns
-------
{ret_0}
   True if :math:`n_\text{min} \leq n \leq n_\text{max}`, false otherwise.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_12 =
   _c2py_fun_12.doc(R"DOC(
Set the pointer to a new tail-fitter object constructed with the given parameters.

Parameters
----------
tail_fraction : {par_0}
   Fraction of the mesh to consider in the tail fit (:math:`0 < r \leq 1`).
n_tail_max : {par_1}
   Maximum number of points to use in the tail fit (:math:`p_\text{max} > 0`).
expansion_order : {par_2}
   Optional expansion order :math:`q \leq q_{\text{max}} = 9`. If not set, it will be
   adjusted automatically.
)DOC",
                    {{c2py::python_typename<double>()}, {c2py::python_typename<int>()}, {c2py::python_typename<std::optional<int>>()}});
static const auto _c2py_doc_13 = _c2py_fun_13.doc(R"DOC(
[1] Map a Matsubara index :math:`n \in \{ n_{\text{min}}, \dots, n_{\text{max}} \}` to its corresponding data
index :math:`d(n)`.

------

[2] Map a Matsubara frequency :math:`i\omega_n` to its data index :math:`d(i\omega_n)`.

------

Parameters
----------
n : {par_0}
   Matsubara index :math:`n` to map.
iw : {par_1}
   Matsubara frequency to map.

Returns
-------
[1] : {ret_0}
   Data index :math:`d(n) = n - n_{\text{min}}`.

[2] : {ret_1}
   Data index :math:`d(i\omega_n) = n - n_{\text{min}}`.
)DOC",
                                                  {{c2py::python_typename<long>()}, {c2py::python_typename<const triqs::mesh::matsubara_freq &>()}},
                                                  {c2py::python_typename<long>(), c2py::python_typename<long>()});
static const auto _c2py_doc_14 = _c2py_fun_14.doc(R"DOC(
Map a data index :math:`d \in \{0, 1, \ldots, N-1\}` to the corresponding Matsubara index :math:`n(d)`.

Parameters
----------
d : {par_0}
   Data index :math:`d` to map.

Returns
-------
{ret_0}
   Matsubara index :math:`n(d) = d + n_{\text{min}}`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_15 = _c2py_fun_15.doc(R"DOC(
Map a Matsubara index :math:`n \in \{ n_{\text{min}}, \dots, n_{\text{max}} \}` to its corresponding
Matsubara frequency :math:`i\omega_n`.

Parameters
----------
n : {par_0}
   Matsubara index :math:`n` to map.

Returns
-------
{ret_0}
   Matsubara frequency :math:`i\omega_n`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<triqs::mesh::matsubara_freq>()});
static const auto _c2py_doc_16 = _c2py_fun_16.doc(
   R"DOC(
Get the values of all mesh points in a mesh.

Parameters
----------
m : {par_0}
   A mesh object.

Returns
-------
{ret_0}
   Array containing the values of all mesh points.
)DOC",
   {{c2py::python_typename<const triqs::mesh::imfreq &>()}},
   {c2py::python_typename<
      nda::basic_array<triqs::mesh::matsubara_freq, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_2>[] = {
   {"copy", (PyCFunction)c2py::pyfkw<_c2py_fun_9>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_9.c_str()},
   {"copy_from", (PyCFunction)c2py::pyfkw<_c2py_fun_10>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_10.c_str()},
   {"is_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_11>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_11.c_str()},
   {"set_tail_fit_parameters", (PyCFunction)c2py::pyfkw<_c2py_fun_12>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_12.c_str()},
   {"to_data_index", (PyCFunction)c2py::pyfkw<_c2py_fun_13>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_13.c_str()},
   {"to_index", (PyCFunction)c2py::pyfkw<_c2py_fun_14>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_14.c_str()},
   {"to_value", (PyCFunction)c2py::pyfkw<_c2py_fun_15>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_15.c_str()},
   {"values", (PyCFunction)c2py::pyfkw<_c2py_fun_16>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_16.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_2>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_2>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_2>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_7  = R"DOC(Get the inverse temperature :math:`\beta`.)DOC";
static constexpr auto prop_doc_8  = R"DOC(Get the first Matsubara index, i.e. :math:`n_{\text{min}}`.)DOC";
static constexpr auto prop_doc_9  = R"DOC(Get a new mesh with the same :math:`\beta`, particle statistics and :math:`N_{i\omega_n}` but only
positive frequencies.)DOC";
static constexpr auto prop_doc_10 = R"DOC(Get the last Matsubara index, i.e. :math:`n_{\text{max}}`.)DOC";
static constexpr auto prop_doc_11 = R"DOC(Get the hash value of the mesh.)DOC";
static constexpr auto prop_doc_12 = R"DOC(Get the number of positive Matsubara frequencies :math:`N_{i\omega_n}`.)DOC";
static constexpr auto prop_doc_13 = R"DOC(Is the mesh restricted to positive Matsubara frequencies?)DOC";
static constexpr auto prop_doc_14 = R"DOC(Get the particle statistics.)DOC";
static constexpr auto prop_doc_15 = R"DOC(Get the complex value of the largest positive Matsubara frequency in the mesh.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_2>[] = {

   {"beta", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::imfreq::beta)>, nullptr, prop_doc_7, nullptr},
   {"first_index", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::imfreq::first_index)>, nullptr, prop_doc_8, nullptr},
   {"get_positive_freq", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::imfreq::get_positive_freq)>, nullptr, prop_doc_9, nullptr},
   {"last_index", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::imfreq::last_index)>, nullptr, prop_doc_10, nullptr},
   {"mesh_hash", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::imfreq::mesh_hash)>, nullptr, prop_doc_11, nullptr},
   {"n_iw", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::imfreq::n_iw)>, nullptr, prop_doc_12, nullptr},
   {"positive_only", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::imfreq::positive_only)>, nullptr, prop_doc_13, nullptr},
   {"statistic", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::imfreq::statistic)>, nullptr, prop_doc_14, nullptr},
   {"w_max", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::imfreq::w_max)>, nullptr, prop_doc_15, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_1(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_2, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_2> = {c2py::tpxx_size<_c2py_cls_2>, getitem_1, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_2> = R"DOC(Imaginary frequency mesh type.

An imaginary frequency mesh is defined by the number of positive frequencies :math:`N_{i\omega_n} \geq 0`,
the inverse temperature :math:`\beta > 0`, its particle statistics and an option (`all_frequencies` vs
`positive_frequencies_only`).

An imaginary frequency mesh has the following properties:

- Each mesh point is identified by a unique index
  :math:`n \in \{ n_{\text{min}}, n_{\text{min}} + 1, \dots, n_{\text{max}} - 1, n_{\text{max}} \}`, where
  :math:`n_{\text{max}} = N_{i\omega_n} - 1` and
  

.. math::

   n_{\text{min}} =
   \begin{cases}
   -N_{i\omega_n} & \text{if fermionic statistics and all frequencies} \\
   -N_{i\omega_n} + 1 & \text{if bosonic statistics and all frequencies} \\
   0 & \text{if positive frequencies only}
   \end{cases}
   \; .

- The size of the mesh is :math:`N = n_{\text{max}} - n_{\text{min}} + 1`.
- An index :math:`n` is mapped to the corresponding data index :math:`d` by the function
  :math:`d(n) = n - n_{\text{min}}`. The inverse map is :math:`n(d) = d + n_{\text{min}}`.
- An index :math:`n` is mapped to the corresponding value :math:`i\omega_n \equiv i\omega(n)` by the function
  

.. math::

   i\omega_n =
   \begin{cases}
   i\frac{2n \pi}{\beta} & \text{if fermionic statistics} \\
   i\frac{(2n + 1) \pi}{\beta} & \text{if bosonic statistics}
   \end{cases}
   \; .

Green's function containers that are based on an imaginary frequency mesh store the function values at the discrete
frequency points :math:`i\omega_n`, i.e. :math:`f_n = f(i\omega(n))`, and to evaluate the function at a specific
Matsubara frequency :math:`i\omega_n` with :math:`n_{\text{min}} \leq n \leq n_{\text{max}}` the container simply
returns the corresponding :math:`f_n`.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_2>;
// --------- class _c2py_cls_3 -----------
using _c2py_cls_3                                            = triqs::mesh::dlr;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_3>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_3> = "triqs.mesh.meshes.MeshDLR";
static const auto _c2py_init_2 = c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_3>(),
                                                         c2py::c_constructor<_c2py_cls_3, double, triqs::mesh::statistic_enum, double, double, bool>(
                                                            "beta", "statistic", "w_max", "eps", "symmetrize"_a = true),
                                                         c2py::c_constructor<_c2py_cls_3, const triqs::mesh::dlr_imtime &>("m"),
                                                         c2py::c_constructor<_c2py_cls_3, const triqs::mesh::dlr_imfreq &>("m")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_3> = c2py::pyfkw_constructor<_c2py_init_2>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_3> =
   _c2py_init_2.doc(R"DOC(
[1] Default constructor constructs an empty mesh.

------

[2] Construct a DLR mesh with a given energy cutoff :math:`\omega_{\text{max}}` and error tolerance :math:`\epsilon`
.

It builds the DLR frequencies :math:`\omega_l` from :math:`\Lambda = \omega_{\text{max}} \beta` and the
error tolerance :math:`\epsilon`, then constructs the imaginary-time and imaginary-frequency DLR operator tables
from them.

------

[3, 4] Construct a DLR mesh from another DLR type mesh.

------

Parameters
----------
beta : {par_0}
   Inverse temperature :math:`\beta > 0`.
statistic : {par_1}
   Particle statistics.
w_max : {par_2}
   DLR energy cutoff :math:`\omega_{\text{max}} = \Lambda / \beta`.
eps : {par_3}
   Error tolerance :math:`\epsilon`.
symmetrize : {par_4}
   Whether to choose the DLR frequencies symmetrically around :math:`\omega = 0`.
m : {par_5}
   Other mesh.
)DOC",
                    {{c2py::python_typename<double>()},
                     {c2py::python_typename<triqs::mesh::statistic_enum>()},
                     {c2py::python_typename<double>()},
                     {c2py::python_typename<double>()},
                     {c2py::python_typename<bool>()},
                     {c2py::python_typename<const triqs::mesh::dlr_imtime &>(), c2py::python_typename<const triqs::mesh::dlr_imfreq &>()}});
// __call__
static auto const _c2py_fun_17 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_3 const &self, long l) -> decltype(auto) { return self.operator()(l); }, "self", "l")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_3> = c2py::pyfkw<_c2py_fun_17>;

// copy
static auto const _c2py_fun_18 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy<triqs::mesh::dlr>, "m")};

// copy_from
static auto const _c2py_fun_19 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy_from<triqs::mesh::dlr>, "m1", "m2")};

// is_index_valid
static auto const _c2py_fun_20 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_3 const &self, long l) -> decltype(auto) { return self.is_index_valid(l); }, "self", "l")};

// to_data_index
static auto const _c2py_fun_21 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_3 const &self, long l) -> decltype(auto) { return self.to_data_index(l); }, "self", "l")};

// to_index
static auto const _c2py_fun_22 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_3 const &self, long d) -> decltype(auto) { return self.to_index(d); }, "self", "d")};

// to_value
static auto const _c2py_fun_23 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_3 const &self, long l) -> decltype(auto) { return self.to_value(l); }, "self", "l")};

// values
static auto const _c2py_fun_24 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::values<triqs::mesh::dlr>, "m")};

static const auto _c2py_doc_17 = _c2py_fun_17.doc(R"DOC(
Function call operator to access a mesh point by its index :math:`l \in \{0, 1, \ldots, N-1\}`.

Parameters
----------
l : {par_0}
   Index :math:`l` of the mesh point.

Returns
-------
{ret_0}
   mesh_point_t with the index :math:`l`, data index :math:`d(l) = l`, hash value of the current mesh and
   the DLR frequency :math:`\omega_l` as its value.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<triqs::mesh::dlr::mesh_point_t>()});
static const auto _c2py_doc_18 = _c2py_fun_18.doc(R"DOC(
Get a copy of a mesh (for Python bindings).

Parameters
----------
m : {par_0}
   The mesh object to copy.

Returns
-------
{ret_0}
   Copy of the given mesh.
)DOC",
                                                  {{c2py::python_typename<const triqs::mesh::dlr &>()}}, {c2py::python_typename<triqs::mesh::dlr>()});
static const auto _c2py_doc_19 =
   _c2py_fun_19.doc(R"DOC(
Copy one mesh into another (for Python bindings).

Simply calls the copy assignment operator of the mesh.

Parameters
----------
m1 : {par_0}
   The mesh object to copy into.
m2 : {par_1}
   The mesh object to copy from.
)DOC",
                    {{c2py::python_typename<triqs::mesh::dlr &>()}, {c2py::python_typename<const triqs::mesh::dlr &>()}});
static const auto _c2py_doc_20 = _c2py_fun_20.doc(R"DOC(
Check if an index :math:`l` is valid.

Parameters
----------
l : {par_0}
   Index :math:`l` to check.

Returns
-------
{ret_0}
   True if :math:`0 \leq l < N`, false otherwise.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_21 = _c2py_fun_21.doc(R"DOC(
Map an index :math:`l \in \{0, 1, \ldots, N-1\}` to its corresponding data index :math:`d(l)`.

Parameters
----------
l : {par_0}
   Index :math:`l` to map.

Returns
-------
{ret_0}
   Data index :math:`d(l) = l`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_22 = _c2py_fun_22.doc(R"DOC(
Map a data index :math:`d \in \{0, 1, \ldots, N-1\}` to the corresponding index :math:`l(d)`.

Parameters
----------
d : {par_0}
   Data index :math:`d` to map.

Returns
-------
{ret_0}
   Index :math:`l(d) = d`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_23 = _c2py_fun_23.doc(R"DOC(
Map an index :math:`l \in \{0, 1, \ldots, N-1\}` to its corresponding value :math:`\omega_l`.

Parameters
----------
l : {par_0}
   Index :math:`l` to map.

Returns
-------
{ret_0}
   Value of the l-th DLR frequency :math:`\omega_l`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<double>()});
static const auto _c2py_doc_24 = _c2py_fun_24.doc(
   R"DOC(
Get the values of all mesh points in a mesh.

Parameters
----------
m : {par_0}
   A mesh object.

Returns
-------
{ret_0}
   Array containing the values of all mesh points.
)DOC",
   {{c2py::python_typename<const triqs::mesh::dlr &>()}},
   {c2py::python_typename<nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_3>[] = {
   {"copy", (PyCFunction)c2py::pyfkw<_c2py_fun_18>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_18.c_str()},
   {"copy_from", (PyCFunction)c2py::pyfkw<_c2py_fun_19>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_19.c_str()},
   {"is_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_20>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_20.c_str()},
   {"to_data_index", (PyCFunction)c2py::pyfkw<_c2py_fun_21>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_21.c_str()},
   {"to_index", (PyCFunction)c2py::pyfkw<_c2py_fun_22>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_22.c_str()},
   {"to_value", (PyCFunction)c2py::pyfkw<_c2py_fun_23>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_23.c_str()},
   {"values", (PyCFunction)c2py::pyfkw<_c2py_fun_24>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_24.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_3>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_3>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_3>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_16 = R"DOC(Get the inverse temperature :math:`\beta`.)DOC";
static constexpr auto prop_doc_17 = R"DOC(Get the array of DLR frequencies :math:`\omega_l`.)DOC";
static constexpr auto prop_doc_18 = R"DOC(Get the DLR error tolerance :math:`\epsilon`.)DOC";
static constexpr auto prop_doc_19 = R"DOC(Get the hash value of the mesh.)DOC";
static constexpr auto prop_doc_20 = R"DOC(Get the particle statistics.)DOC";
static constexpr auto prop_doc_21 = R"DOC(Is the mesh symmetric around :math:`\omega = 0`?)DOC";
static constexpr auto prop_doc_22 = R"DOC(Get the DLR energy cutoff :math:`\omega_{\text{max}} = \Lambda / \beta`.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_3>[] = {

   {"beta", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr::beta)>, nullptr, prop_doc_16, nullptr},
   {"dlr_freq", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr::dlr_freq)>, nullptr, prop_doc_17, nullptr},
   {"eps", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr::eps)>, nullptr, prop_doc_18, nullptr},
   {"mesh_hash", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr::mesh_hash)>, nullptr, prop_doc_19, nullptr},
   {"statistic", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr::statistic)>, nullptr, prop_doc_20, nullptr},
   {"symmetrize", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr::symmetrize)>, nullptr, prop_doc_21, nullptr},
   {"w_max", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr::w_max)>, nullptr, prop_doc_22, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_2(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_3, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_3> = {c2py::tpxx_size<_c2py_cls_3>, getitem_2, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_3> = R"DOC(Discrete Lehmann representation (DLR) mesh type.

A DLR mesh is defined by the inverse temperature :math:`\beta > 0`, the particle statistics, a DLR energy
cutoff :math:`\omega_{\text{max}}`, an error tolerance :math:`\epsilon` and a boolean flag specifying if the mesh
should be symmetric around :math:`\omega = 0`.

A DLR mesh has the following properties:

- Each mesh point is identified by a unique index :math:`l \in \{0, 1, \ldots, N-1\}`.
- The size of the mesh :math:`N` depends on :math:`\beta` and the choice of :math:`\omega_{\text{max}}` and
  :math:`\epsilon`. It is equal to the DLR rank :math:`r` and the number of DLR basis functions
  :math:`K(\tau, \omega_l)` or :math:`K(i\omega_n, \omega_l)`.
- An index :math:`l` is mapped to the corresponding data index :math:`d` by the identity function :math:`d(l) = l`
  and vice versa.
- An index :math:`l` is mapped to the corresponding value :math:`\omega_l`, where :math:`\omega_l` is the l-th DLR
  frequency.

Green's function containers that are based on a DLR mesh store the coefficients :math:`f_l` of the discrete Lehmann
representation of a function :math:`f(\tau)` or :math:`f(i\omega_n)`. To evaluate the function at an arbitrary
imaginary time :math:`\tau \in [0, \beta]` or at a specific Matsubara frequency :math:`i\omega_n`, the GF container
calculates the DLR approximation of the function via

.. math::

   f(\tau) \approx \sum_{l=0}^{N-1} \frac{e^{-\omega_l \tau}}{1 + e^{-\omega_l \beta}} f_l \; ,

or

.. math::

   f(i\omega_n) \approx \sum_{l=0}^{N-1} \frac{1}{i\omega_n + \omega_l} f_l \; .)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_3>;
// --------- class _c2py_cls_4 -----------
using _c2py_cls_4                                            = triqs::mesh::dlr_imtime;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_4>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_4> = "triqs.mesh.meshes.MeshDLRImTime";
static const auto _c2py_init_3 = c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_4>(),
                                                         c2py::c_constructor<_c2py_cls_4, double, triqs::mesh::statistic_enum, double, double, bool>(
                                                            "beta", "statistic", "w_max", "eps", "symmetrize"_a = true),
                                                         c2py::c_constructor<_c2py_cls_4, const triqs::mesh::dlr_imfreq &>("m"),
                                                         c2py::c_constructor<_c2py_cls_4, const triqs::mesh::dlr &>("m")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_4> = c2py::pyfkw_constructor<_c2py_init_3>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_4> =
   _c2py_init_3.doc(R"DOC(
[1] Default constructor constructs an empty mesh.

------

[2] Construct an imaginary time DLR mesh with a given energy cutoff :math:`\omega_{\text{max}}` and error
tolerance :math:`\epsilon`.

It builds the DLR frequencies :math:`\omega_l` from :math:`\Lambda = \omega_{\text{max}} \beta` and the
error tolerance :math:`\epsilon`, then constructs the imaginary-time and imaginary-frequency DLR operator tables
from them.

------

[3, 4] Construct an imaginary time DLR mesh from another DLR type mesh.

------

Parameters
----------
beta : {par_0}
   Inverse temperature :math:`\beta > 0`.
statistic : {par_1}
   Particle statistics.
w_max : {par_2}
   DLR energy cutoff :math:`\omega_{\text{max}} = \Lambda / \beta`.
eps : {par_3}
   Error tolerance :math:`\epsilon`.
symmetrize : {par_4}
   Whether to choose the imaginary time points symmetrically around :math:`\tau = \beta / 2`.
m : {par_5}
   Other mesh.
)DOC",
                    {{c2py::python_typename<double>()},
                     {c2py::python_typename<triqs::mesh::statistic_enum>()},
                     {c2py::python_typename<double>()},
                     {c2py::python_typename<double>()},
                     {c2py::python_typename<bool>()},
                     {c2py::python_typename<const triqs::mesh::dlr_imfreq &>(), c2py::python_typename<const triqs::mesh::dlr &>()}});
// __call__
static auto const _c2py_fun_25 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_4 const &self, long l) -> decltype(auto) { return self.operator()(l); }, "self", "l")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_4> = c2py::pyfkw<_c2py_fun_25>;

// copy
static auto const _c2py_fun_26 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy<triqs::mesh::dlr_imtime>, "m")};

// copy_from
static auto const _c2py_fun_27 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy_from<triqs::mesh::dlr_imtime>, "m1", "m2")};

// is_index_valid
static auto const _c2py_fun_28 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_4 const &self, long l) -> decltype(auto) { return self.is_index_valid(l); }, "self", "l")};

// to_data_index
static auto const _c2py_fun_29 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_4 const &self, long l) -> decltype(auto) { return self.to_data_index(l); }, "self", "l")};

// to_index
static auto const _c2py_fun_30 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_4 const &self, long d) -> decltype(auto) { return self.to_index(d); }, "self", "d")};

// to_value
static auto const _c2py_fun_31 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_4 const &self, long l) -> decltype(auto) { return self.to_value(l); }, "self", "l")};

// values
static auto const _c2py_fun_32 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::values<triqs::mesh::dlr_imtime>, "m")};

static const auto _c2py_doc_25 =
   _c2py_fun_25.doc(R"DOC(
Function call operator to access a mesh point by its index :math:`l \in \{0, 1, \ldots, N-1\}`.

Parameters
----------
l : {par_0}
   Index :math:`l` of the mesh point.

Returns
-------
{ret_0}
   mesh_point_t with the index :math:`l`, data index :math:`d(l) = l`, hash value of the current mesh and
   the imaginary time node :math:`\tau_l` as its value.
)DOC",
                    {{c2py::python_typename<long>()}}, {c2py::python_typename<triqs::mesh::dlr_imtime::mesh_point_t>()});
static const auto _c2py_doc_26 =
   _c2py_fun_26.doc(R"DOC(
Get a copy of a mesh (for Python bindings).

Parameters
----------
m : {par_0}
   The mesh object to copy.

Returns
-------
{ret_0}
   Copy of the given mesh.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::dlr_imtime &>()}}, {c2py::python_typename<triqs::mesh::dlr_imtime>()});
static const auto _c2py_doc_27 =
   _c2py_fun_27.doc(R"DOC(
Copy one mesh into another (for Python bindings).

Simply calls the copy assignment operator of the mesh.

Parameters
----------
m1 : {par_0}
   The mesh object to copy into.
m2 : {par_1}
   The mesh object to copy from.
)DOC",
                    {{c2py::python_typename<triqs::mesh::dlr_imtime &>()}, {c2py::python_typename<const triqs::mesh::dlr_imtime &>()}});
static const auto _c2py_doc_28 = _c2py_fun_28.doc(R"DOC(
Check if an index :math:`l` is valid.

Parameters
----------
l : {par_0}
   Index :math:`l` to check.

Returns
-------
{ret_0}
   True if :math:`0 \leq l < N`, false otherwise.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_29 = _c2py_fun_29.doc(R"DOC(
Map an index :math:`l \in \{0, 1, \ldots, N-1\}` to its corresponding data index :math:`d(l)`.

Parameters
----------
l : {par_0}
   Index :math:`l` to map.

Returns
-------
{ret_0}
   Data index :math:`d(l) = l`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_30 = _c2py_fun_30.doc(R"DOC(
Map a data index :math:`d \in \{0, 1, \ldots, N-1\}` to the corresponding index :math:`l(d)`.

Parameters
----------
d : {par_0}
   Data index :math:`d` to map.

Returns
-------
{ret_0}
   Index :math:`l(d) = d`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_31 = _c2py_fun_31.doc(R"DOC(
Map an index :math:`l \in \{0, 1, \ldots, N-1\}` to its corresponding value :math:`\tau_l`.

Parameters
----------
l : {par_0}
   Index :math:`l` to map.

Returns
-------
{ret_0}
   Value of the l-th DLR interpolation node in imaginary time space, i.e. :math:`\tau_l` .
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<double>()});
static const auto _c2py_doc_32 = _c2py_fun_32.doc(
   R"DOC(
Get the values of all mesh points in a mesh.

Parameters
----------
m : {par_0}
   A mesh object.

Returns
-------
{ret_0}
   Array containing the values of all mesh points.
)DOC",
   {{c2py::python_typename<const triqs::mesh::dlr_imtime &>()}},
   {c2py::python_typename<nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_4>[] = {
   {"copy", (PyCFunction)c2py::pyfkw<_c2py_fun_26>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_26.c_str()},
   {"copy_from", (PyCFunction)c2py::pyfkw<_c2py_fun_27>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_27.c_str()},
   {"is_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_28>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_28.c_str()},
   {"to_data_index", (PyCFunction)c2py::pyfkw<_c2py_fun_29>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_29.c_str()},
   {"to_index", (PyCFunction)c2py::pyfkw<_c2py_fun_30>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_30.c_str()},
   {"to_value", (PyCFunction)c2py::pyfkw<_c2py_fun_31>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_31.c_str()},
   {"values", (PyCFunction)c2py::pyfkw<_c2py_fun_32>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_32.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_4>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_4>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_4>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_23 = R"DOC(Get the inverse temperature :math:`\beta`.)DOC";
static constexpr auto prop_doc_24 = R"DOC(Get the array of DLR frequencies :math:`\omega_l`.)DOC";
static constexpr auto prop_doc_25 = R"DOC(Get the DLR error tolerance :math:`\epsilon`.)DOC";
static constexpr auto prop_doc_26 = R"DOC(Get the hash value of the mesh.)DOC";
static constexpr auto prop_doc_27 = R"DOC(Get the particle statistics.)DOC";
static constexpr auto prop_doc_28 = R"DOC(Is the mesh symmetric around :math:`\tau = \beta / 2`?)DOC";
static constexpr auto prop_doc_29 = R"DOC(Get the DLR energy cutoff :math:`\omega_{\text{max}} = \Lambda / \beta`.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_4>[] = {

   {"beta", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr_imtime::beta)>, nullptr, prop_doc_23, nullptr},
   {"dlr_freq", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr_imtime::dlr_freq)>, nullptr, prop_doc_24, nullptr},
   {"eps", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr_imtime::eps)>, nullptr, prop_doc_25, nullptr},
   {"mesh_hash", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr_imtime::mesh_hash)>, nullptr, prop_doc_26, nullptr},
   {"statistic", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr_imtime::statistic)>, nullptr, prop_doc_27, nullptr},
   {"symmetrize", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr_imtime::symmetrize)>, nullptr, prop_doc_28, nullptr},
   {"w_max", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr_imtime::w_max)>, nullptr, prop_doc_29, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_3(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_4, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_4> = {c2py::tpxx_size<_c2py_cls_4>, getitem_3, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_4> = R"DOC(Imaginary time discrete Lehmann representation (DLR) mesh type.

An imaginary time DLR mesh is defined by the inverse temperature :math:`\beta > 0`, the particle
statistics, a DLR energy cutoff :math:`\omega_{\text{max}}`, an error tolerance :math:`\epsilon` and a boolean flag
specifying if the mesh should be symmetric around :math:`\tau = \beta / 2` (a symmetric mesh enforces the DLR rank
to be even for Fermions and odd for Bosons).

An imaginary time DLR mesh has the following properties:

- Each mesh point is identified by a unique index :math:`l \in \{0, 1, \ldots, N-1\}`.
- The size of the mesh :math:`N` depends on :math:`\beta` and the choice of :math:`\omega_{\text{max}}` and
  :math:`\epsilon`. It is equal to the DLR rank :math:`r` and the number of DLR basis functions
  :math:`K(\tau, \omega_l)`.
- An index :math:`l` is mapped to the corresponding data index :math:`d` by the identity function :math:`d(l) = l`
  and vice versa.
- An index :math:`l` is mapped to the corresponding value :math:`\tau_l`, where :math:`\tau_l` is the l-th DLR
  interpolation node in imaginary time space.

Green's function containers that are based on an imaginary time DLR mesh store the function values at the discrete
time points :math:`\tau_l`, i.e. :math:`f_l = f(\tau_l)`. In contrast to DLR and imaginary-time meshes, the GF
container cannot evaluate the function at an arbitrary imaginary time :math:`\tau \in [0, \beta]`
(evaluation at arbitrary points is intentionally unsupported).)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_4>;
// --------- class _c2py_cls_5 -----------
using _c2py_cls_5                                            = triqs::mesh::dlr_imfreq;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_5>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_5> = "triqs.mesh.meshes.MeshDLRImFreq";
static const auto _c2py_init_4 = c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_5>(),
                                                         c2py::c_constructor<_c2py_cls_5, double, triqs::mesh::statistic_enum, double, double, bool>(
                                                            "beta", "statistic", "w_max", "eps", "symmetrize"_a = true),
                                                         c2py::c_constructor<_c2py_cls_5, const triqs::mesh::dlr_imtime &>("m"),
                                                         c2py::c_constructor<_c2py_cls_5, const triqs::mesh::dlr &>("m")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_5> = c2py::pyfkw_constructor<_c2py_init_4>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_5> =
   _c2py_init_4.doc(R"DOC(
[1] Default constructor constructs an empty mesh.

------

[2] Construct an imaginary frequency DLR mesh with a given energy cutoff :math:`\omega_{\text{max}}` and error
tolerance :math:`\epsilon`.

It builds the DLR frequencies :math:`\omega_l` from :math:`\Lambda = \omega_{\text{max}} \beta` and the
error tolerance :math:`\epsilon`, then constructs the imaginary-time and imaginary-frequency DLR operator tables
from them.

------

[3, 4] Construct an imaginary frequency DLR mesh from another DLR type mesh.

------

Parameters
----------
beta : {par_0}
   Inverse temperature :math:`\beta > 0`.
statistic : {par_1}
   Particle statistics.
w_max : {par_2}
   DLR energy cutoff :math:`\omega_{\text{max}} = \Lambda / \beta`.
eps : {par_3}
   Error tolerance :math:`\epsilon`.
symmetrize : {par_4}
   Whether to choose the Matsubara frequencies symmetrically around :math:`i\omega_n = 0`.
m : {par_5}
   Other mesh.
)DOC",
                    {{c2py::python_typename<double>()},
                     {c2py::python_typename<triqs::mesh::statistic_enum>()},
                     {c2py::python_typename<double>()},
                     {c2py::python_typename<double>()},
                     {c2py::python_typename<bool>()},
                     {c2py::python_typename<const triqs::mesh::dlr_imtime &>(), c2py::python_typename<const triqs::mesh::dlr &>()}});
// __call__
static auto const _c2py_fun_33 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_5 const &self, long l) -> decltype(auto) { return self.operator()(l); }, "self", "l")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_5> = c2py::pyfkw<_c2py_fun_33>;

// copy
static auto const _c2py_fun_34 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy<triqs::mesh::dlr_imfreq>, "m")};

// copy_from
static auto const _c2py_fun_35 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy_from<triqs::mesh::dlr_imfreq>, "m1", "m2")};

// is_index_valid
static auto const _c2py_fun_36 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_5 const &self, long l) -> decltype(auto) { return self.is_index_valid(l); }, "self", "l")};

// to_data_index
static auto const _c2py_fun_37 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_5 const &self, long l) -> decltype(auto) { return self.to_data_index(l); }, "self", "l")};

// to_index
static auto const _c2py_fun_38 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_5 const &self, long d) -> decltype(auto) { return self.to_index(d); }, "self", "d")};

// to_value
static auto const _c2py_fun_39 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_5 const &self, long l) -> decltype(auto) { return self.to_value(l); }, "self", "l")};

// values
static auto const _c2py_fun_40 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::values<triqs::mesh::dlr_imfreq>, "m")};

static const auto _c2py_doc_33 =
   _c2py_fun_33.doc(R"DOC(
Function call operator to access a mesh point by its index :math:`l \in \{0, 1, \ldots, N-1\}`.

Parameters
----------
l : {par_0}
   Index :math:`l` of the mesh point.

Returns
-------
{ret_0}
   mesh_point_t with the Matsubara index :math:`n_l`, data index :math:`d(l) = l` and same :math:`\beta`,
   particle statistics and hash value as the current mesh.
)DOC",
                    {{c2py::python_typename<long>()}}, {c2py::python_typename<triqs::mesh::dlr_imfreq::mesh_point_t>()});
static const auto _c2py_doc_34 =
   _c2py_fun_34.doc(R"DOC(
Get a copy of a mesh (for Python bindings).

Parameters
----------
m : {par_0}
   The mesh object to copy.

Returns
-------
{ret_0}
   Copy of the given mesh.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::dlr_imfreq &>()}}, {c2py::python_typename<triqs::mesh::dlr_imfreq>()});
static const auto _c2py_doc_35 =
   _c2py_fun_35.doc(R"DOC(
Copy one mesh into another (for Python bindings).

Simply calls the copy assignment operator of the mesh.

Parameters
----------
m1 : {par_0}
   The mesh object to copy into.
m2 : {par_1}
   The mesh object to copy from.
)DOC",
                    {{c2py::python_typename<triqs::mesh::dlr_imfreq &>()}, {c2py::python_typename<const triqs::mesh::dlr_imfreq &>()}});
static const auto _c2py_doc_36 = _c2py_fun_36.doc(R"DOC(
Check if an index :math:`l` is valid.

Parameters
----------
l : {par_0}
   Index :math:`l` to check.

Returns
-------
{ret_0}
   True if :math:`0 \leq l < N`, false otherwise.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_37 = _c2py_fun_37.doc(R"DOC(
Map an index :math:`l \in \{0, 1, \ldots, N-1\}` to its corresponding data index :math:`d(l)`.

Parameters
----------
l : {par_0}
   Index :math:`l` to map.

Returns
-------
{ret_0}
   Data index :math:`d(l) = l`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_38 = _c2py_fun_38.doc(R"DOC(
Map a data index :math:`d \in \{0, 1, \ldots, N-1\}` to the corresponding index :math:`l(d)`.

Parameters
----------
d : {par_0}
   Data index :math:`d` to map.

Returns
-------
{ret_0}
   Index :math:`l(d) = d`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_39 = _c2py_fun_39.doc(R"DOC(
Map an index :math:`l \in \{0, 1, \ldots, N-1\}` to its corresponding Matsubara frequency
:math:`i\omega_{n_l}`.

Parameters
----------
l : {par_0}
   Index :math:`l` to map.

Returns
-------
{ret_0}
   Matsubara frequency :math:`i\omega_{n_l}`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<triqs::mesh::matsubara_freq>()});
static const auto _c2py_doc_40 = _c2py_fun_40.doc(
   R"DOC(
Get the values of all mesh points in a mesh.

Parameters
----------
m : {par_0}
   A mesh object.

Returns
-------
{ret_0}
   Array containing the values of all mesh points.
)DOC",
   {{c2py::python_typename<const triqs::mesh::dlr_imfreq &>()}},
   {c2py::python_typename<
      nda::basic_array<triqs::mesh::matsubara_freq, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_5>[] = {
   {"copy", (PyCFunction)c2py::pyfkw<_c2py_fun_34>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_34.c_str()},
   {"copy_from", (PyCFunction)c2py::pyfkw<_c2py_fun_35>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_35.c_str()},
   {"is_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_36>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_36.c_str()},
   {"to_data_index", (PyCFunction)c2py::pyfkw<_c2py_fun_37>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_37.c_str()},
   {"to_index", (PyCFunction)c2py::pyfkw<_c2py_fun_38>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_38.c_str()},
   {"to_value", (PyCFunction)c2py::pyfkw<_c2py_fun_39>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_39.c_str()},
   {"values", (PyCFunction)c2py::pyfkw<_c2py_fun_40>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_40.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_5>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_5>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_5>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_30 = R"DOC(Get the inverse temperature :math:`\beta`.)DOC";
static constexpr auto prop_doc_31 = R"DOC(Get the array of DLR frequencies :math:`\omega_l`.)DOC";
static constexpr auto prop_doc_32 = R"DOC(Get the DLR error tolerance :math:`\epsilon`.)DOC";
static constexpr auto prop_doc_33 = R"DOC(Get the hash value of the mesh.)DOC";
static constexpr auto prop_doc_34 = R"DOC(Get a pair containing the smallest and largest Matsubara frequency in the mesh.)DOC";
static constexpr auto prop_doc_35 = R"DOC(Get the particle statistics.)DOC";
static constexpr auto prop_doc_36 = R"DOC(Is the mesh symmetric around :math:`i\omega_n = 0`?)DOC";
static constexpr auto prop_doc_37 = R"DOC(Get the DLR energy cutoff :math:`\omega_{\text{max}} = \Lambda / \beta`.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_5>[] = {

   {"beta", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr_imfreq::beta)>, nullptr, prop_doc_30, nullptr},
   {"dlr_freq", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr_imfreq::dlr_freq)>, nullptr, prop_doc_31, nullptr},
   {"eps", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr_imfreq::eps)>, nullptr, prop_doc_32, nullptr},
   {"mesh_hash", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr_imfreq::mesh_hash)>, nullptr, prop_doc_33, nullptr},
   {"min_max_frequencies", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr_imfreq::min_max_frequencies)>, nullptr, prop_doc_34, nullptr},
   {"statistic", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr_imfreq::statistic)>, nullptr, prop_doc_35, nullptr},
   {"symmetrize", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr_imfreq::symmetrize)>, nullptr, prop_doc_36, nullptr},
   {"w_max", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr_imfreq::w_max)>, nullptr, prop_doc_37, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_4(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_5, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_5> = {c2py::tpxx_size<_c2py_cls_5>, getitem_4, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_5> = R"DOC(Imaginary frequency discrete Lehmann representation (DLR) mesh type.

An imaginary frequency DLR mesh is defined by the inverse temperature :math:`\beta > 0`, the particle
statistics, a DLR energy cutoff :math:`\omega_{\text{max}}`, an error tolerance :math:`\epsilon` and a boolean flag
specifying if the mesh should be symmetric around :math:`i\omega_n = 0` (a symmetric mesh enforces the DLR rank to
be even for Fermions and odd for Bosons).

An imaginary frequency DLR mesh has the following properties:

- Each mesh point is identified by a unique index :math:`l \in \{0, 1, \ldots, N-1\}`.
- The size of the mesh :math:`N` depends on :math:`\beta` and the choice of :math:`\omega_{\text{max}}` and
  :math:`\epsilon`. It is equal to the DLR rank :math:`r` and the number of DLR basis functions
  :math:`K(i\omega_n, \omega_l)`.
- An index :math:`l` is mapped to the corresponding data index :math:`d` by the identity function :math:`d(l) = l`
  and vice versa.
- An index :math:`l` is mapped to the corresponding value :math:`i\omega_{n_l}`, where :math:`i\omega_{n_l}` is the
  l-th DLR interpolation node in imaginary frequency space.

.. note::

   The index :math:`l`, which is used to access a certain mesh point, is different from the Matsubara index
   :math:`n_l` that is stored in the mesh point object. That means, if `m` is an instance of imaginary-frequency DLR
   mesh, then `m(l).index() != l` in general.

Green's function containers that are based on an imaginary frequency DLR mesh store the function values at the
discrete frequency points :math:`i\omega_{n_l}`, i.e. :math:`f_l = f(i\omega_{n_l})`. In contrast to DLR and
imaginary-frequency meshes, the GF container cannot evaluate the function at an arbitrary Matsubara frequency
:math:`i\omega_n` (evaluation at arbitrary points is intentionally unsupported).)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_5>;
// --------- class _c2py_cls_6 -----------
using _c2py_cls_6                                            = triqs::mesh::retime;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_6>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_6> = "triqs.mesh.meshes.MeshReTime";
static const auto _c2py_init_5 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_6, double, double, long>("t_min"_a = 0.0, "t_max"_a = 0.0, "n_t"_a = 0),
                           c2py::c_constructor<_c2py_cls_6, std::pair<double, double>, int>("window", "n_t")}
      .with_deprecated_params({{"n_max", "n_t"}});
template <> constexpr initproc c2py::tp_init<_c2py_cls_6> = c2py::pyfkw_constructor<_c2py_init_5>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_6> = _c2py_init_5.doc(R"DOC(
Construct a real time mesh on the interval :math:`[t_{\mathrm{min}}, t_{\mathrm{max}}]` with :math:`N \geq 0`
equally spaced mesh points.

Parameters
----------
t_min : {par_0}
   Lower bound :math:`t_{\mathrm{min}}` of the time interval.
t_max : {par_1}
   Upper bound :math:`t_{\mathrm{max}}` of the time interval.
n_t : {par_2}
   Size of the mesh.
window : {par_3}
   Pair containing the lower and upper bounds of the time interval.
)DOC",
                                                                    {{c2py::python_typename<double>()},
                                                                     {c2py::python_typename<double>()},
                                                                     {c2py::python_typename<long>(), c2py::python_typename<int>()},
                                                                     {c2py::python_typename<std::pair<double, double>>()}});
// __call__
static auto const _c2py_fun_41 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_6 const &self, long n) -> decltype(auto) { return self.operator()(n); }, "self", "n")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_6> = c2py::pyfkw<_c2py_fun_41>;

// copy
static auto const _c2py_fun_42 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy<triqs::mesh::retime>, "m")};

// copy_from
static auto const _c2py_fun_43 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy_from<triqs::mesh::retime>, "m1", "m2")};

// is_index_valid
static auto const _c2py_fun_44 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_6 const &self, long n) -> decltype(auto) { return self.is_index_valid(n); }, "self", "n")};

// to_data_index
static auto const _c2py_fun_45 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_6 const &self, long n) -> decltype(auto) { return self.to_data_index(n); }, "self", "n")};

// to_index
static auto const _c2py_fun_46 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_6 const &self, long d) -> decltype(auto) { return self.to_index(d); }, "self", "d")};

// to_value
static auto const _c2py_fun_47 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_6 const &self, long n) -> decltype(auto) { return self.to_value(n); }, "self", "n")};

// values
static auto const _c2py_fun_48 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::values<triqs::mesh::retime>, "m")};

static const auto _c2py_doc_41 = _c2py_fun_41.doc(R"DOC(
Function call operator to access a mesh point by its index :math:`n \in \{0, 1, \ldots, N-1\}`.

Parameters
----------
n : {par_0}
   Index :math:`n` of the mesh point.

Returns
-------
{ret_0}
   mesh_point_t with the index :math:`n`, data index :math:`d(n) = n`, hash value of the current mesh and
   value :math:`m(n) = a + n \cdot \Delta`.
)DOC",
                                                  {{c2py::python_typename<long>()}},
                                                  {c2py::python_typename<triqs::mesh::detail::linear<triqs::mesh::retime, double>::mesh_point_t>()});
static const auto _c2py_doc_42 =
   _c2py_fun_42.doc(R"DOC(
Get a copy of a mesh (for Python bindings).

Parameters
----------
m : {par_0}
   The mesh object to copy.

Returns
-------
{ret_0}
   Copy of the given mesh.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::retime &>()}}, {c2py::python_typename<triqs::mesh::retime>()});
static const auto _c2py_doc_43 =
   _c2py_fun_43.doc(R"DOC(
Copy one mesh into another (for Python bindings).

Simply calls the copy assignment operator of the mesh.

Parameters
----------
m1 : {par_0}
   The mesh object to copy into.
m2 : {par_1}
   The mesh object to copy from.
)DOC",
                    {{c2py::python_typename<triqs::mesh::retime &>()}, {c2py::python_typename<const triqs::mesh::retime &>()}});
static const auto _c2py_doc_44 = _c2py_fun_44.doc(R"DOC(
Check if an index :math:`n` is valid.

Parameters
----------
n : {par_0}
   Index :math:`n` to check.

Returns
-------
{ret_0}
   True if :math:`0 \leq n < N`, false otherwise.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_45 = _c2py_fun_45.doc(R"DOC(
Map an index :math:`n \in \{0, 1, \ldots, N-1\}` to its corresponding data index :math:`d(n)`.

Parameters
----------
n : {par_0}
   Index :math:`n` to map.

Returns
-------
{ret_0}
   Data index :math:`d(n) = n`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_46 = _c2py_fun_46.doc(R"DOC(
Map a data index :math:`d \in \{0, 1, \ldots, N-1\}` to the corresponding index :math:`n(d)`.

Parameters
----------
d : {par_0}
   Data index :math:`d` to map.

Returns
-------
{ret_0}
   Index :math:`n(d) = d`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_47 = _c2py_fun_47.doc(R"DOC(
Map an index :math:`n \in \{0, 1, \ldots, N-1\}` to its corresponding value :math:`m(n)`.

Parameters
----------
n : {par_0}
   Index :math:`n` to map.

Returns
-------
{ret_0}
   Value of the mesh point :math:`m(n) = a + n \cdot \Delta`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<double>()});
static const auto _c2py_doc_48 = _c2py_fun_48.doc(
   R"DOC(
Get the values of all mesh points in a mesh.

Parameters
----------
m : {par_0}
   A mesh object.

Returns
-------
{ret_0}
   Array containing the values of all mesh points.
)DOC",
   {{c2py::python_typename<const triqs::mesh::retime &>()}},
   {c2py::python_typename<nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_6>[] = {
   {"copy", (PyCFunction)c2py::pyfkw<_c2py_fun_42>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_42.c_str()},
   {"copy_from", (PyCFunction)c2py::pyfkw<_c2py_fun_43>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_43.c_str()},
   {"is_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_44>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_44.c_str()},
   {"to_data_index", (PyCFunction)c2py::pyfkw<_c2py_fun_45>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_45.c_str()},
   {"to_index", (PyCFunction)c2py::pyfkw<_c2py_fun_46>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_46.c_str()},
   {"to_value", (PyCFunction)c2py::pyfkw<_c2py_fun_47>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_47.c_str()},
   {"values", (PyCFunction)c2py::pyfkw<_c2py_fun_48>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_48.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_6>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_6>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_6>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_38 = R"DOC(Get the step size :math:`\Delta` of the mesh, i.e. the distance between two consecutive mesh points.)DOC";
static constexpr auto prop_doc_39 = R"DOC(Get the inverse of the step size of the mesh, i.e. :math:`1 / \Delta`.)DOC";
static constexpr auto prop_doc_40 = R"DOC(Get the first index of the mesh, i.e. :math:`0`.)DOC";
static constexpr auto prop_doc_41 = R"DOC(Get the last index of the mesh, i.e. :math:`N - 1`.)DOC";
static constexpr auto prop_doc_42 = R"DOC(Get the hash value of the mesh.)DOC";
static constexpr auto prop_doc_43 = R"DOC(Get the upper bound of the interval :math:`t_{\mathrm{max}}`, i.e. the value of the last mesh point.)DOC";
static constexpr auto prop_doc_44 = R"DOC(Get the lower bound of the interval :math:`t_{\mathrm{min}}`, i.e. the value of the first mesh point.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_6>[] = {

   {"delta", c2py::getter_from_method_B<triqs::mesh::retime, c2py::castmc<>(&triqs::mesh::detail::linear<triqs::mesh::retime, double>::delta)>,
    nullptr, prop_doc_38, nullptr},
   {"delta_inv",
    c2py::getter_from_method_B<triqs::mesh::retime, c2py::castmc<>(&triqs::mesh::detail::linear<triqs::mesh::retime, double>::delta_inv)>, nullptr,
    prop_doc_39, nullptr},
   {"first_index",
    c2py::getter_from_method_B<triqs::mesh::retime, c2py::castmc<>(&triqs::mesh::detail::linear<triqs::mesh::retime, double>::first_index)>, nullptr,
    prop_doc_40, nullptr},
   {"last_index",
    c2py::getter_from_method_B<triqs::mesh::retime, c2py::castmc<>(&triqs::mesh::detail::linear<triqs::mesh::retime, double>::last_index)>, nullptr,
    prop_doc_41, nullptr},
   {"mesh_hash",
    c2py::getter_from_method_B<triqs::mesh::retime, c2py::castmc<>(&triqs::mesh::detail::linear<triqs::mesh::retime, double>::mesh_hash)>, nullptr,
    prop_doc_42, nullptr},
   {"t_max", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::retime::t_max)>, nullptr, prop_doc_43, nullptr},
   {"t_min", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::retime::t_min)>, nullptr, prop_doc_44, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_5(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_6, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_6> = {c2py::tpxx_size<_c2py_cls_6>, getitem_5, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_6> = R"DOC(Real time mesh type.

A real time mesh is defined by its size :math:`N \geq 0` and a time interval :math:`[t_{\mathrm{min}},
t_{\mathrm{max}}]`. It contains :math:`N` equally spaced mesh points on the interval :math:`[t_{\mathrm{min}},
t_{\mathrm{max}}]` such that the distance between two consecutive mesh points (step size) is constant.

A real time mesh has the following properties:

- Each mesh point is identified by a unique index :math:`n \in \{0, 1, \ldots, N-1\}`.
- An index :math:`n` is mapped to the corresponding data index :math:`d` by the identity function :math:`d(n) = n`
  and vice versa.
- An index :math:`n` is mapped to the corresponding value :math:`t` by the linear function
  :math:`t(n) = t_{\mathrm{min}} + n \cdot \Delta` such that :math:`t(0) = t_{\mathrm{min}}` and
  :math:`t(N - 1) = t_{\mathrm{max}}`. The step size of
  the mesh is :math:`\Delta = \frac{t_{\mathrm{max}} - t_{\mathrm{min}}}{N - 1}` for :math:`N > 1`, otherwise it is
  undefined. For implementation purposes, we set :math:`\Delta = 0` and :math:`\Delta^{-1} = 0` for :math:`N = 0`
  and :math:`\Delta = 0` and :math:`\Delta^{-1} = \infty` for :math:`N = 1`.
- An arbitrary value :math:`t \in [t_{\mathrm{min}}, t_{\mathrm{max}}]` is mapped to the closest mesh point with
  index :math:`n` by the function
  :math:`n(t) = \left\lfloor \frac{t - t_{\mathrm{min}}}{\Delta} + 0.5 \right\rfloor`.

Green's function containers that are based on a real time mesh store the function values at the discrete time
points :math:`t(n)`, i.e. :math:`f_n = f(t(n))`, and use linear interpolation to evaluate the function at an
arbitrary time :math:`t \in [t_{\mathrm{min}}, t_{\mathrm{max}}]`.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_6>;
// --------- class _c2py_cls_7 -----------
using _c2py_cls_7                                            = triqs::mesh::refreq;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_7>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_7> = "triqs.mesh.meshes.MeshReFreq";
static const auto _c2py_init_6 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_7, double, double, long>("w_min"_a = 0.0, "w_max"_a = 0.0, "n_w"_a = 0),
                           c2py::c_constructor<_c2py_cls_7, std::pair<double, double>, int>("window", "n_w")}
      .with_deprecated_params({{"n_max", "n_w"}});
template <> constexpr initproc c2py::tp_init<_c2py_cls_7> = c2py::pyfkw_constructor<_c2py_init_6>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_7> = _c2py_init_6.doc(R"DOC(
Construct a real frequency mesh on the interval :math:`[\omega_{\text{min}}, \omega_{\text{max}}]` with
:math:`N \geq 0` equally spaced mesh points.

Parameters
----------
w_min : {par_0}
   Lower bound :math:`\omega_{\mathrm{min}}` of the frequency interval.
w_max : {par_1}
   Upper bound :math:`\omega_{\mathrm{max}}` of the frequency interval.
n_w : {par_2}
   Size of the mesh.
window : {par_3}
   Pair containing the lower and upper bounds of the frequency interval.
)DOC",
                                                                    {{c2py::python_typename<double>()},
                                                                     {c2py::python_typename<double>()},
                                                                     {c2py::python_typename<long>(), c2py::python_typename<int>()},
                                                                     {c2py::python_typename<std::pair<double, double>>()}});
// __call__
static auto const _c2py_fun_49 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_7 const &self, long n) -> decltype(auto) { return self.operator()(n); }, "self", "n")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_7> = c2py::pyfkw<_c2py_fun_49>;

// copy
static auto const _c2py_fun_50 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy<triqs::mesh::refreq>, "m")};

// copy_from
static auto const _c2py_fun_51 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy_from<triqs::mesh::refreq>, "m1", "m2")};

// is_index_valid
static auto const _c2py_fun_52 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_7 const &self, long n) -> decltype(auto) { return self.is_index_valid(n); }, "self", "n")};

// set_tail_fit_parameters
static auto const _c2py_fun_53 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_7 const &self, double tail_fraction, int n_tail_max, std::optional<int> expansion_order) -> decltype(auto) {
     return self.set_tail_fit_parameters(tail_fraction, n_tail_max, expansion_order);
   },
   "self", "tail_fraction", "n_tail_max"_a = triqs::mesh::tail_fitter::default_n_tail_max, "expansion_order"_a = std::optional<int>{})};

// to_data_index
static auto const _c2py_fun_54 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_7 const &self, long n) -> decltype(auto) { return self.to_data_index(n); }, "self", "n")};

// to_index
static auto const _c2py_fun_55 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_7 const &self, long d) -> decltype(auto) { return self.to_index(d); }, "self", "d")};

// to_value
static auto const _c2py_fun_56 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_7 const &self, long n) -> decltype(auto) { return self.to_value(n); }, "self", "n")};

// values
static auto const _c2py_fun_57 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::values<triqs::mesh::refreq>, "m")};

static const auto _c2py_doc_49 = _c2py_fun_49.doc(R"DOC(
Function call operator to access a mesh point by its index :math:`n \in \{0, 1, \ldots, N-1\}`.

Parameters
----------
n : {par_0}
   Index :math:`n` of the mesh point.

Returns
-------
{ret_0}
   mesh_point_t with the index :math:`n`, data index :math:`d(n) = n`, hash value of the current mesh and
   value :math:`m(n) = a + n \cdot \Delta`.
)DOC",
                                                  {{c2py::python_typename<long>()}},
                                                  {c2py::python_typename<triqs::mesh::detail::linear<triqs::mesh::refreq, double>::mesh_point_t>()});
static const auto _c2py_doc_50 =
   _c2py_fun_50.doc(R"DOC(
Get a copy of a mesh (for Python bindings).

Parameters
----------
m : {par_0}
   The mesh object to copy.

Returns
-------
{ret_0}
   Copy of the given mesh.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::refreq &>()}}, {c2py::python_typename<triqs::mesh::refreq>()});
static const auto _c2py_doc_51 =
   _c2py_fun_51.doc(R"DOC(
Copy one mesh into another (for Python bindings).

Simply calls the copy assignment operator of the mesh.

Parameters
----------
m1 : {par_0}
   The mesh object to copy into.
m2 : {par_1}
   The mesh object to copy from.
)DOC",
                    {{c2py::python_typename<triqs::mesh::refreq &>()}, {c2py::python_typename<const triqs::mesh::refreq &>()}});
static const auto _c2py_doc_52 = _c2py_fun_52.doc(R"DOC(
Check if an index :math:`n` is valid.

Parameters
----------
n : {par_0}
   Index :math:`n` to check.

Returns
-------
{ret_0}
   True if :math:`0 \leq n < N`, false otherwise.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_53 =
   _c2py_fun_53.doc(R"DOC(
Set the pointer to a new tail-fitter object constructed with the given parameters.

Parameters
----------
tail_fraction : {par_0}
   Fraction of the mesh to consider in the tail fit (:math:`0 < r \leq 1`).
n_tail_max : {par_1}
   Maximum number of points to use in the tail fit (:math:`p_\text{max} > 0`).
expansion_order : {par_2}
   Optional expansion order :math:`q \leq q_{\text{max}} = 9`. If not set, it will be
   adjusted automatically.
)DOC",
                    {{c2py::python_typename<double>()}, {c2py::python_typename<int>()}, {c2py::python_typename<std::optional<int>>()}});
static const auto _c2py_doc_54 = _c2py_fun_54.doc(R"DOC(
Map an index :math:`n \in \{0, 1, \ldots, N-1\}` to its corresponding data index :math:`d(n)`.

Parameters
----------
n : {par_0}
   Index :math:`n` to map.

Returns
-------
{ret_0}
   Data index :math:`d(n) = n`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_55 = _c2py_fun_55.doc(R"DOC(
Map a data index :math:`d \in \{0, 1, \ldots, N-1\}` to the corresponding index :math:`n(d)`.

Parameters
----------
d : {par_0}
   Data index :math:`d` to map.

Returns
-------
{ret_0}
   Index :math:`n(d) = d`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_56 = _c2py_fun_56.doc(R"DOC(
Map an index :math:`n \in \{0, 1, \ldots, N-1\}` to its corresponding value :math:`m(n)`.

Parameters
----------
n : {par_0}
   Index :math:`n` to map.

Returns
-------
{ret_0}
   Value of the mesh point :math:`m(n) = a + n \cdot \Delta`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<double>()});
static const auto _c2py_doc_57 = _c2py_fun_57.doc(
   R"DOC(
Get the values of all mesh points in a mesh.

Parameters
----------
m : {par_0}
   A mesh object.

Returns
-------
{ret_0}
   Array containing the values of all mesh points.
)DOC",
   {{c2py::python_typename<const triqs::mesh::refreq &>()}},
   {c2py::python_typename<nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_7>[] = {
   {"copy", (PyCFunction)c2py::pyfkw<_c2py_fun_50>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_50.c_str()},
   {"copy_from", (PyCFunction)c2py::pyfkw<_c2py_fun_51>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_51.c_str()},
   {"is_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_52>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_52.c_str()},
   {"set_tail_fit_parameters", (PyCFunction)c2py::pyfkw<_c2py_fun_53>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_53.c_str()},
   {"to_data_index", (PyCFunction)c2py::pyfkw<_c2py_fun_54>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_54.c_str()},
   {"to_index", (PyCFunction)c2py::pyfkw<_c2py_fun_55>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_55.c_str()},
   {"to_value", (PyCFunction)c2py::pyfkw<_c2py_fun_56>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_56.c_str()},
   {"values", (PyCFunction)c2py::pyfkw<_c2py_fun_57>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_57.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_7>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_7>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_7>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_45 = R"DOC(Get the step size :math:`\Delta` of the mesh, i.e. the distance between two consecutive mesh points.)DOC";
static constexpr auto prop_doc_46 = R"DOC(Get the inverse of the step size of the mesh, i.e. :math:`1 / \Delta`.)DOC";
static constexpr auto prop_doc_47 = R"DOC(Get the first index of the mesh, i.e. :math:`0`.)DOC";
static constexpr auto prop_doc_48 = R"DOC(Get the last index of the mesh, i.e. :math:`N - 1`.)DOC";
static constexpr auto prop_doc_49 = R"DOC(Get the hash value of the mesh.)DOC";
static constexpr auto prop_doc_50 = R"DOC(Is the mesh restricted to positive frequencies?)DOC";
static constexpr auto prop_doc_51 =
   R"DOC(Get the upper bound of the interval :math:`\omega_{\text{max}}`, i.e. the value of the last mesh point.)DOC";
static constexpr auto prop_doc_52 =
   R"DOC(Get the lower bound of the interval :math:`\omega_{\text{min}}`, i.e. the value of the first mesh point.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_7>[] = {

   {"delta", c2py::getter_from_method_B<triqs::mesh::refreq, c2py::castmc<>(&triqs::mesh::detail::linear<triqs::mesh::refreq, double>::delta)>,
    nullptr, prop_doc_45, nullptr},
   {"delta_inv",
    c2py::getter_from_method_B<triqs::mesh::refreq, c2py::castmc<>(&triqs::mesh::detail::linear<triqs::mesh::refreq, double>::delta_inv)>, nullptr,
    prop_doc_46, nullptr},
   {"first_index",
    c2py::getter_from_method_B<triqs::mesh::refreq, c2py::castmc<>(&triqs::mesh::detail::linear<triqs::mesh::refreq, double>::first_index)>, nullptr,
    prop_doc_47, nullptr},
   {"last_index",
    c2py::getter_from_method_B<triqs::mesh::refreq, c2py::castmc<>(&triqs::mesh::detail::linear<triqs::mesh::refreq, double>::last_index)>, nullptr,
    prop_doc_48, nullptr},
   {"mesh_hash",
    c2py::getter_from_method_B<triqs::mesh::refreq, c2py::castmc<>(&triqs::mesh::detail::linear<triqs::mesh::refreq, double>::mesh_hash)>, nullptr,
    prop_doc_49, nullptr},
   {"positive_only", c2py::getter_from_method<c2py::cast<>(&triqs::mesh::refreq::positive_only)>, nullptr, prop_doc_50, nullptr},
   {"w_max", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::refreq::w_max)>, nullptr, prop_doc_51, nullptr},
   {"w_min", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::refreq::w_min)>, nullptr, prop_doc_52, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_6(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_7, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_7> = {c2py::tpxx_size<_c2py_cls_7>, getitem_6, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_7> = R"DOC(Real frequency mesh type.

A real frequency mesh is defined by its size :math:`N \geq 0` and a frequency interval
:math:`[\omega_{\mathrm{min}}, \omega_{\mathrm{max}}]`. It contains :math:`N` equally spaced mesh points on the
interval :math:`[\omega_{\mathrm{min}}, \omega_{\mathrm{max}}]` such that the distance between two consecutive mesh
points (step size) is constant.

A real frequency mesh has the following properties:

- Each mesh point is identified by a unique index :math:`n \in \{0, 1, \ldots, N-1\}`.
- An index :math:`n` is mapped to the corresponding data index :math:`d` by the identity function :math:`d(n) = n`
  and vice versa.
- An index :math:`n` is mapped to the corresponding value :math:`\omega` by the linear function
  :math:`\omega(n) = \omega_{\mathrm{min}} + n \cdot \Delta` such that :math:`\omega(0) = \omega_{\mathrm{min}}`
  and :math:`\omega(N - 1) = \omega_{\mathrm{max}}`. The step size of the mesh is
  :math:`\Delta = \frac{\omega_{\mathrm{max}} - \omega_{\mathrm{min}}}{N - 1}` for :math:`N > 1`, otherwise it is
  undefined. For implementation purposes, we set :math:`\Delta = 0` and :math:`\Delta^{-1} = 0` for :math:`N = 0`
  and :math:`\Delta = 0` and :math:`\Delta^{-1} = \infty` for :math:`N = 1`.
- An arbitrary value :math:`\omega \in [\omega_{\mathrm{min}}, \omega_{\mathrm{max}}]` is mapped to the closest
  mesh point with index :math:`n` by the function
  :math:`n(\omega) = \left\lfloor \frac{\omega - \omega_{\mathrm{min}}}{\Delta} + 0.5 \right\rfloor`.

Green's function containers that are based on a real frequency mesh store the function values at the discrete
frequency points :math:`\omega(n)`, i.e. :math:`f_n = f(\omega(n))`, and use linear interpolation to evaluate the
function at an arbitrary frequency :math:`\omega \in [\omega_{\mathrm{min}}, \omega_{\mathrm{max}}]`.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_7>;
// --------- class _c2py_cls_8 -----------
using _c2py_cls_8                                            = triqs::mesh::cyclat;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_8>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_8> = "triqs.mesh.meshes.MeshCycLat";
static const auto _c2py_init_7 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_8, const triqs::lattice::bravais_lattice &, const std::array<long, 3> &>("bl", "dims"),
                           c2py::c_constructor<_c2py_cls_8, const triqs::lattice::bravais_lattice &, long>("bl", "L"),
                           c2py::c_constructor<_c2py_cls_8, long, long, long>("L1"_a = 1, "L2"_a = 1, "L3"_a = 1)}
      .with_deprecated_params({{"lattice", "bl"}});
template <> constexpr initproc c2py::tp_init<_c2py_cls_8> = c2py::pyfkw_constructor<_c2py_init_7>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_8> = _c2py_init_7.doc(R"DOC(
[1] Construct a cyclic lattice mesh on a Bravais lattice with the given supercell dimensions.

------

[2] Construct a cyclic lattice mesh on a Bravais lattice with a cubic supercell.

------

[3] Construct a cyclic lattice mesh on a cubic Bravais lattice with :math:`a = 1` and the given supercell
dimensions.

------

Parameters
----------
bl : {par_0}
   Bravais lattice object representing the underlying Bravais lattice.
dims : {par_1}
   Number of unit cells in the supercell along each of the three dimensions, i.e. :math:`(N_1, N_2, N_3)`
   .
L : {par_2}
   Number of unit cells in the supercell along each of the three dimensions.
L1 : {par_3}
   Number of unit cells in the supercell along the first dimension.
L2 : {par_4}
   Number of unit cells in the supercell along the second dimension.
L3 : {par_5}
   Number of unit cells in the supercell along the third dimension.
)DOC",
                                                                    {{c2py::python_typename<const triqs::lattice::bravais_lattice &>()},
                                                                     {c2py::python_typename<const std::array<long, 3> &>()},
                                                                     {c2py::python_typename<long>()},
                                                                     {c2py::python_typename<long>()},
                                                                     {c2py::python_typename<long>()},
                                                                     {c2py::python_typename<long>()}});
// __call__
static auto const _c2py_fun_58 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_8 const &self, const triqs::mesh::cyclat::index_t &n) -> decltype(auto) { return self.operator()(n); }, "self", "n")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_8> = c2py::pyfkw<_c2py_fun_58>;

// copy
static auto const _c2py_fun_59 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy<triqs::mesh::cyclat>, "m")};

// copy_from
static auto const _c2py_fun_60 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy_from<triqs::mesh::cyclat>, "m1", "m2")};

// index_modulo
static auto const _c2py_fun_61 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_8 const &self, const triqs::mesh::cyclat::index_t &n_tilde) -> decltype(auto) { return self.index_modulo(n_tilde); },
                 "self", "n_tilde")};

// is_index_valid
static auto const _c2py_fun_62 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_8 const &self, const triqs::mesh::cyclat::index_t &n) -> decltype(auto) { return self.is_index_valid(n); }, "self", "n")};

// to_data_index
static auto const _c2py_fun_63 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_8 const &self, const triqs::mesh::cyclat::index_t &n) -> decltype(auto) { return self.to_data_index(n); }, "self", "n")};

// to_index
static auto const _c2py_fun_64 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_8 const &self, long d) -> decltype(auto) { return self.to_index(d); }, "self", "d")};

// to_value
static auto const _c2py_fun_65 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_8 const &self, const triqs::mesh::cyclat::index_t &n) -> decltype(auto) { return self.to_value(n); }, "self", "n")};

// values
static auto const _c2py_fun_66 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::values<triqs::mesh::cyclat>, "m")};

static const auto _c2py_doc_58 =
   _c2py_fun_58.doc(R"DOC(
Function call operator to access a mesh point by its index :math:`\mathbf{n}`.

Parameters
----------
n : {par_0}
   Index :math:`\mathbf{n}` of the mesh point.

Returns
-------
{ret_0}
   mesh_point_t with the index :math:`\mathbf{n}`, data index :math:`d(\mathbf{n}) = d(\mathbf{n}) = n_3 + N_3
   (n_2 + N_2 n_1)` and the hash value and underlying Bravais lattice of the current mesh.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::cyclat::index_t &>()}}, {c2py::python_typename<triqs::mesh::cyclat::mesh_point_t>()});
static const auto _c2py_doc_59 =
   _c2py_fun_59.doc(R"DOC(
Get a copy of a mesh (for Python bindings).

Parameters
----------
m : {par_0}
   The mesh object to copy.

Returns
-------
{ret_0}
   Copy of the given mesh.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::cyclat &>()}}, {c2py::python_typename<triqs::mesh::cyclat>()});
static const auto _c2py_doc_60 =
   _c2py_fun_60.doc(R"DOC(
Copy one mesh into another (for Python bindings).

Simply calls the copy assignment operator of the mesh.

Parameters
----------
m1 : {par_0}
   The mesh object to copy into.
m2 : {par_1}
   The mesh object to copy from.
)DOC",
                    {{c2py::python_typename<triqs::mesh::cyclat &>()}, {c2py::python_typename<const triqs::mesh::cyclat &>()}});
static const auto _c2py_doc_61 =
   _c2py_fun_61.doc(R"DOC(
Map an arbitrary index :math:`\tilde{\mathbf{n}}` to the unique index :math:`\mathbf{n}` in the supercell.

Parameters
----------
n_tilde : {par_0}
   Index :math:`\tilde{\mathbf{n}}` to map back to the supercell.

Returns
-------
{ret_0}
   Corresponding index :math:`\mathbf{n}` in the supercell such that :math:`\tilde{\mathbf{n}} = \mathbf{n} +
   \mathbf{N} \mathbf{m}`.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::cyclat::index_t &>()}}, {c2py::python_typename<triqs::mesh::cyclat::index_t>()});
static const auto _c2py_doc_62 = _c2py_fun_62.doc(R"DOC(
Check if an index :math:`\mathbf{n}` is valid, i.e. corresponds to a unit cell/lattice point in the
supercell.

Parameters
----------
n : {par_0}
   Index :math:`\mathbf{n}` to check.

Returns
-------
{ret_0}
   True if :math:`0 \leq n_i < N_i \; \forall i`, false otherwise.
)DOC",
                                                  {{c2py::python_typename<const triqs::mesh::cyclat::index_t &>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_63 = _c2py_fun_63.doc(R"DOC(
Map an index :math:`\mathbf{n}` to its corresponding data index :math:`d(\mathbf{n})`.

Parameters
----------
n : {par_0}
   Index :math:`\mathbf{n}` to map.

Returns
-------
{ret_0}
   Data index :math:`d(\mathbf{n}) = n_3 + N_3 (n_2 + N_2 n_1)`.
)DOC",
                                                  {{c2py::python_typename<const triqs::mesh::cyclat::index_t &>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_64 = _c2py_fun_64.doc(R"DOC(
Map a data index :math:`d \in \{0, 1, \ldots, N-1\}` to the corresponding index :math:`\mathbf{n}(d)`
.

Parameters
----------
d : {par_0}
   Data index :math:`d` to map.

Returns
-------
{ret_0}
   Index :math:`\mathbf{n}(d) = (\lfloor d / s_1 \rfloor, \lfloor (d \mod s_1) / s_2 \rfloor, (d \mod s_1)
   \mod s_2 )`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<triqs::mesh::cyclat::index_t>()});
static const auto _c2py_doc_65 =
   _c2py_fun_65.doc(R"DOC(
Map an index :math:`\mathbf{n}` to its corresponding lattice point :math:`\mathbf{R}^{\mathbf{n}}`.

Parameters
----------
n : {par_0}
   Index :math:`\mathbf{n}` to map.

Returns
-------
{ret_0}
   Bravais lattice point :math:`\mathbf{R}^{\mathbf{n}}`.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::cyclat::index_t &>()}}, {c2py::python_typename<triqs::mesh::cyclat::value_t>()});
static const auto _c2py_doc_66 =
   _c2py_fun_66.doc(R"DOC(
Get the values of all mesh points in a mesh.

Parameters
----------
m : {par_0}
   A mesh object.

Returns
-------
{ret_0}
   Array containing the values of all mesh points.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::cyclat &>()}},
                    {c2py::python_typename<nda::basic_array<triqs::lattice::bravais_lattice::point_t, 1, nda::C_layout, 'V',
                                                            nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_8>[] = {
   {"copy", (PyCFunction)c2py::pyfkw<_c2py_fun_59>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_59.c_str()},
   {"copy_from", (PyCFunction)c2py::pyfkw<_c2py_fun_60>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_60.c_str()},
   {"index_modulo", (PyCFunction)c2py::pyfkw<_c2py_fun_61>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_61.c_str()},
   {"is_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_62>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_62.c_str()},
   {"to_data_index", (PyCFunction)c2py::pyfkw<_c2py_fun_63>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_63.c_str()},
   {"to_index", (PyCFunction)c2py::pyfkw<_c2py_fun_64>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_64.c_str()},
   {"to_value", (PyCFunction)c2py::pyfkw<_c2py_fun_65>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_65.c_str()},
   {"values", (PyCFunction)c2py::pyfkw<_c2py_fun_66>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_66.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_8>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_8>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_8>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_53 = R"DOC(Get the number of unit cells in each of the three dimensions.)DOC";
static constexpr auto prop_doc_54 = R"DOC(Get the underlying Bravais lattice.)DOC";
static constexpr auto prop_doc_55 = R"DOC(Get the hash value of the mesh.)DOC";
static constexpr auto prop_doc_56 = R"DOC(Get the matrix :math:`\mathbf{A}^T` containing the basis vectors of the Bravais lattice in its rows.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_8>[] = {

   {"dims", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::cyclat::dims)>, nullptr, prop_doc_53, nullptr},
   {"lattice", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::cyclat::lattice)>, nullptr, prop_doc_54, nullptr},
   {"mesh_hash", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::cyclat::mesh_hash)>, nullptr, prop_doc_55, nullptr},
   {"units", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::cyclat::units)>, nullptr, prop_doc_56, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_7(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_8, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_8> = {c2py::tpxx_size<_c2py_cls_8>, getitem_7, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_8> = R"DOC(Cyclic lattice mesh type for Bravais lattices with Born-von Karman periodic boundary conditions.

A cyclic lattice mesh is defined by an underlying Bravais lattice and the number of unit cells in each of
the three dimensions, :math:`N_1`, :math:`N_2` and :math:`N_3`, which define the supercell and the periodic
boundary conditions (PBC) :math:`f(\mathbf{R}^{\mathbf{n}}) = f(\mathbf{R}^{\mathbf{n}} + \mathbf{T}^{\mathbf{m}})`.
Here, :math:`\mathbf{T}^{\mathbf{m}} = \sum_{i=1}^3 \mathbf{a}_i N_i m_i = \tilde{\mathbf{A}} \mathbf{m}` is a
supercell translation vector, :math:`\mathbf{a}_i` is a Bravais lattice basis vector in direction :math:`i` and
:math:`\mathbf{m} = (m_1, m_2, m_3)` with :math:`m_i \in \mathbb{Z}`.

It has the following properties:

- Each mesh point is identified by

  - a unique index in the supercell :math:`\mathbf{n} = (n_1, n_2, n_3)`, where :math:`0 \leq n_i < N_i`, and
  - an infinite set of indices due to the periodic boundary conditions, i.e.
    :math:`\{ \tilde{\mathbf{n}} = \mathbf{n} + \mathbf{N} \mathbf{m} : \mathbf{N} \mathbf{m} = (N_1 m_1, N_2 m_2, N_3 m_3) \in \mathbb{Z}^3 \}`.

- The size of the mesh is :math:`N = N_1 \, N_2 \, N_3`, i.e. the total number of unit cells in the supercell.
- An index :math:`\mathbf{n}` is mapped to the corresponding data index :math:`d` by the function
  :math:`d(\mathbf{n}) = n_3 + N_3 (n_2 + N_2 n_1) = n_3 + n_2 N_3 + n_1 N_2 N_3`. The inverse map is
  :math:`\mathbf{n}(d) = (\lfloor d / s_1 \rfloor, \lfloor (d \mod s_1) / s_2 \rfloor, (d \mod s_1) \mod s_2 )`,
  where :math:`s_1 = N_2 N_3` and :math:`s_2 = N_3`.
- An index :math:`\mathbf{n}` is mapped to the corresponding value (lattice point)
  :math:`\mathbf{R}(\mathbf{n}) \equiv \mathbf{R}^{\mathbf{n}} = \sum_{i=1}^3 \mathbf{a}_i n_i = \mathbf{A} \mathbf{n}`,
  where :math:`\mathbf{a}_i` is the Bravais lattice basis vectors in direction :math:`i`. The inverse map is then
  :math:`\mathbf{n}(\mathbf{R}^{\mathbf{n}}) = \mathbf{A}^{-1} \mathbf{R}^{\mathbf{n}} = \mathbf{n}`.

Green's function containers that are based on a cyclic lattice mesh store the function values at the discrete
lattice points :math:`\mathbf{R}^{\mathbf{n}}`, i.e. :math:`f_{\mathbf{n}} = f(\mathbf{R}^{\mathbf{n}})`. Because
of the PBC, the container only has to store values for indices with :math:`0 \leq n_i < N_i`. To evaluate the
function at an arbitrary lattice point, it is first mapped to the supercell using the PBC and then the
corresponding function value is returned.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_8>;
// --------- class _c2py_cls_9 -----------
using _c2py_cls_9                                            = triqs::mesh::brzone;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_9>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_9> = "triqs.mesh.meshes.MeshBrZone";
static const auto _c2py_init_8 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_9>(),
                           c2py::c_constructor<_c2py_cls_9, const triqs::lattice::brillouin_zone &, const std::array<long, 3> &>("bz", "dims"),
                           c2py::c_constructor<_c2py_cls_9, const triqs::lattice::brillouin_zone &, long>("bz", "n_k")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_9> = c2py::pyfkw_constructor<_c2py_init_8>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_9> = _c2py_init_8.doc(R"DOC(
[1] Default constructor constructs an empty mesh.

------

[2] Construct a Brillouin zone mesh with the given number of mesh points.

------

[3] Construct a Brillouin zone mesh with the same number of mesh points in each direction.

------

Parameters
----------
bz : {par_0}
   Brillouin zone object representing the underlying BZ.
dims : {par_1}
   Number of mesh points along each of the three dimensions, i.e. :math:`(N_1, N_2, N_3)`.
n_k : {par_2}
   Number of mesh points along each of the three dimensions.
)DOC",
                                                                    {{c2py::python_typename<const triqs::lattice::brillouin_zone &>()},
                                                                     {c2py::python_typename<const std::array<long, 3> &>()},
                                                                     {c2py::python_typename<long>()}});
// __call__
static auto const _c2py_fun_67 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_9 const &self, const triqs::mesh::brzone::index_t &n) -> decltype(auto) { return self.operator()(n); }, "self", "n")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_9> = c2py::pyfkw<_c2py_fun_67>;

// closest_index
static auto const _c2py_fun_68 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_9 const &self,
      const nda::basic_array_view<const double, 1, nda::C_stride_layout, 'V', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>> &k)
      -> decltype(auto) { return self.template closest_index<nda::basic_array_view<const double, 1, nda::C_stride_layout, 'V'>>(k); },
   "self", "k")};

// copy
static auto const _c2py_fun_69 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy<triqs::mesh::brzone>, "m")};

// copy_from
static auto const _c2py_fun_70 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy_from<triqs::mesh::brzone>, "m1", "m2")};

// index_modulo
static auto const _c2py_fun_71 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_9 const &self, const triqs::mesh::brzone::index_t &n_tilde) -> decltype(auto) { return self.index_modulo(n_tilde); },
                 "self", "n_tilde")};

// is_index_valid
static auto const _c2py_fun_72 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_9 const &self, const triqs::mesh::brzone::index_t &n) -> decltype(auto) { return self.is_index_valid(n); }, "self", "n")};

// to_data_index
static auto const _c2py_fun_73 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_9 const &self, const triqs::mesh::brzone::index_t &n) -> decltype(auto) { return self.to_data_index(n); }, "self", "n")};

// to_index
static auto const _c2py_fun_74 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_9 const &self, long d) -> decltype(auto) { return self.to_index(d); }, "self", "d")};

// to_value
static auto const _c2py_fun_75 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_9 const &self, const triqs::mesh::brzone::index_t &n) -> decltype(auto) { return self.to_value(n); }, "self", "n")};

// values
static auto const _c2py_fun_76 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::values<triqs::mesh::brzone>, "m")};

static const auto _c2py_doc_67 =
   _c2py_fun_67.doc(R"DOC(
Function call operator to access a mesh point by its index :math:`\mathbf{n}`.

Parameters
----------
n : {par_0}
   Index :math:`\mathbf{n}` of the mesh point.

Returns
-------
{ret_0}
   mesh_point_t with the index :math:`\mathbf{n}`, data index :math:`d(\mathbf{n}) = d(\mathbf{n}) = n_3 + N_3
   (n_2 + N_2 n_1)` and a pointer to the current mesh.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::brzone::index_t &>()}}, {c2py::python_typename<triqs::mesh::brzone::mesh_point_t>()});
static const auto _c2py_doc_68 =
   _c2py_fun_68.doc(R"DOC(
Map a given :math:`\mathbf{k}`-vector or expression to the closest :math:`\mathbf{k}^{\mathbf{n}}` in the
first BZ and return its index :math:`\mathbf{n}`.

Parameters
----------
k : {par_0}
   :math:`\mathbf{k}`-vector or expression to map.

Returns
-------
{ret_0}
   Index :math:`\mathbf{n}` of the closest :math:`\mathbf{k}^{\mathbf{n}}` in the first BZ.
)DOC",
                    {{c2py::python_typename<const nda::basic_array_view<const double, 1, nda::C_stride_layout, 'V', nda::default_accessor,
                                                                        nda::borrowed<nda::mem::AddressSpace::Host>> &>()}},
                    {c2py::python_typename<triqs::mesh::brzone::index_t>()});
static const auto _c2py_doc_69 =
   _c2py_fun_69.doc(R"DOC(
Get a copy of a mesh (for Python bindings).

Parameters
----------
m : {par_0}
   The mesh object to copy.

Returns
-------
{ret_0}
   Copy of the given mesh.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::brzone &>()}}, {c2py::python_typename<triqs::mesh::brzone>()});
static const auto _c2py_doc_70 =
   _c2py_fun_70.doc(R"DOC(
Copy one mesh into another (for Python bindings).

Simply calls the copy assignment operator of the mesh.

Parameters
----------
m1 : {par_0}
   The mesh object to copy into.
m2 : {par_1}
   The mesh object to copy from.
)DOC",
                    {{c2py::python_typename<triqs::mesh::brzone &>()}, {c2py::python_typename<const triqs::mesh::brzone &>()}});
static const auto _c2py_doc_71 =
   _c2py_fun_71.doc(R"DOC(
Map an arbitrary index :math:`\tilde{\mathbf{n}}` to the unique index :math:`\mathbf{n}` in the first BZ.

Parameters
----------
n_tilde : {par_0}
   Index :math:`\tilde{\mathbf{n}}` to map back to the first BZ.

Returns
-------
{ret_0}
   Corresponding index :math:`\mathbf{n}` in the first BZ such that :math:`\tilde{\mathbf{n}} = \mathbf{n} +
   \mathbf{N} \mathbf{m}`.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::brzone::index_t &>()}}, {c2py::python_typename<triqs::mesh::brzone::index_t>()});
static const auto _c2py_doc_72 = _c2py_fun_72.doc(R"DOC(
Check if an index :math:`\mathbf{n}` is valid, i.e. corresponds to a :math:`\mathbf{k}^\mathbf{n}` in the
first BZ.

Parameters
----------
n : {par_0}
   Index :math:`\mathbf{n}` to check.

Returns
-------
{ret_0}
   True if :math:`0 \leq n_i < N_i \; \forall i`, false otherwise.
)DOC",
                                                  {{c2py::python_typename<const triqs::mesh::brzone::index_t &>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_73 = _c2py_fun_73.doc(R"DOC(
Map an index :math:`\mathbf{n}` to its corresponding data index :math:`d(\mathbf{n})`.

Parameters
----------
n : {par_0}
   Index :math:`\mathbf{n}` to map.

Returns
-------
{ret_0}
   Data index :math:`d(\mathbf{n}) = n_3 + N_3 (n_2 + N_2 n_1)`.
)DOC",
                                                  {{c2py::python_typename<const triqs::mesh::brzone::index_t &>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_74 = _c2py_fun_74.doc(R"DOC(
Map a data index :math:`d \in \{0, 1, \ldots, N-1\}` to the corresponding index :math:`\mathbf{n}(d)`.

Parameters
----------
d : {par_0}
   Data index :math:`d` to map.

Returns
-------
{ret_0}
   Index :math:`\mathbf{n}(d) = (\lfloor d / s_1 \rfloor, \lfloor (d \mod s_1) / s_2 \rfloor, (d \mod s_1)
   \mod s_2 )`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<triqs::mesh::brzone::index_t>()});
static const auto _c2py_doc_75 =
   _c2py_fun_75.doc(R"DOC(
Map an index :math:`\mathbf{n}` to its corresponding :math:`\mathbf{k}`-point :math:`\mathbf{k}^{\mathbf{n}}`
.

Parameters
----------
n : {par_0}
   Index :math:`\mathbf{n}` to map.

Returns
-------
{ret_0}
   :math:`\mathbf{k}`-point :math:`\mathbf{k}^{\mathbf{n}}`.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::brzone::index_t &>()}}, {c2py::python_typename<triqs::mesh::brzone::value_t>()});
static const auto _c2py_doc_76 = _c2py_fun_76.doc(
   R"DOC(
Get the values of all mesh points in a mesh.

Parameters
----------
m : {par_0}
   A mesh object.

Returns
-------
{ret_0}
   Array containing the values of all mesh points.
)DOC",
   {{c2py::python_typename<const triqs::mesh::brzone &>()}},
   {c2py::python_typename<
      nda::basic_array<nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>, 1,
                       nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_9>[] = {
   {"closest_index", (PyCFunction)c2py::pyfkw<_c2py_fun_68>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_68.c_str()},
   {"copy", (PyCFunction)c2py::pyfkw<_c2py_fun_69>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_69.c_str()},
   {"copy_from", (PyCFunction)c2py::pyfkw<_c2py_fun_70>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_70.c_str()},
   {"index_modulo", (PyCFunction)c2py::pyfkw<_c2py_fun_71>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_71.c_str()},
   {"is_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_72>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_72.c_str()},
   {"to_data_index", (PyCFunction)c2py::pyfkw<_c2py_fun_73>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_73.c_str()},
   {"to_index", (PyCFunction)c2py::pyfkw<_c2py_fun_74>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_74.c_str()},
   {"to_value", (PyCFunction)c2py::pyfkw<_c2py_fun_75>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_75.c_str()},
   {"values", (PyCFunction)c2py::pyfkw<_c2py_fun_76>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_76.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_9>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_9>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_9>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_57 = R"DOC(Get the underlying Brillouin zone.)DOC";
static constexpr auto prop_doc_58 = R"DOC(Get the number of mesh points in each of the three dimensions.)DOC";
static constexpr auto prop_doc_59 = R"DOC(Get the hash value of the mesh.)DOC";
static constexpr auto prop_doc_60 =
   R"DOC(Get the matrix :math:`\tilde{\mathbf{B}}^T` containing the scaled reciprocal basis vectors in its rows.)DOC";
static constexpr auto prop_doc_61 = R"DOC(Get the matrix :math:`\left( \tilde{\mathbf{B}}^T \right)^{-1}`.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_9>[] = {

   {"bz", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::brzone::bz)>, nullptr, prop_doc_57, nullptr},
   {"dims", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::brzone::dims)>, nullptr, prop_doc_58, nullptr},
   {"mesh_hash", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::brzone::mesh_hash)>, nullptr, prop_doc_59, nullptr},
   {"units", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::brzone::units)>, nullptr, prop_doc_60, nullptr},
   {"units_inv", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::brzone::units_inv)>, nullptr, prop_doc_61, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_8(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_9, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_9> = {c2py::tpxx_size<_c2py_cls_9>, getitem_8, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_9> = R"DOC(Brillouin zone mesh type.

A Brillouin zone (BZ) mesh is defined by an underlying Brillouin zone and the number of mesh points in the
primitive reciprocal unit cell along each of the three dimensions, :math:`N_1`, :math:`N_2` and :math:`N_3`. The
periodic boundary conditions (PBC) in real space (see cyclic-lattice mesh) translate in reciprocal space to
:math:`f(\mathbf{k}) = f(\mathbf{k} + \mathbf{G}^{\mathbf{m}})`, where :math:`\mathbf{G}^{\mathbf{m}} = \sum_{i=1}^3
\mathbf{b}_i m_i = \mathbf{B} \mathbf{m}` is a reciprocal lattice vector, :math:`\mathbf{b}_i` is a reciprocal
lattice basis vector in direction :math:`i` and :math:`\mathbf{m} = (m_1, m_2, m_3)` with :math:`m_i \in
\mathbb{Z}`.

It has the following properties:

- Each mesh point is identified by

  - a unique index in the first BZ :math:`\mathbf{n} = (n_1, n_2, n_3)`, where :math:`0 \leq n_i < N_i`, and
  - an infinite set of indices due to the periodic boundary conditions, i.e.
    :math:`\{ \tilde{\mathbf{n}} = \mathbf{n} + \mathbf{N} \mathbf{m} : \mathbf{N} \mathbf{m} = (N_1 m_1, N_2 m_2, N_3 m_3) \in \mathbb{Z}^3 \}`.

- The size of the mesh is :math:`N = N_1 \, N_2 \, N_3`, i.e. the total number of mesh points in the BZ.
- An index :math:`\mathbf{n}` is mapped to the corresponding data index :math:`d` by the function
  :math:`d(\mathbf{n}) = n_3 + N_3 (n_2 + N_2 n_1) = n_3 + n_2 N_3 + n_1 N_2 N_3`. The inverse map is
  :math:`\mathbf{n}(d) = (\lfloor d / s_1 \rfloor, \lfloor (d \mod s_1) / s_2 \rfloor, (d \mod s_1) \mod s_2 )`,
  where :math:`s_1 = N_2 N_3` and :math:`s_2 = N_3`.
- An index :math:`\mathbf{n}` is mapped to the corresponding value (:math:`\mathbf{k}`-vector)
  :math:`\mathbf{k}(\mathbf{n}) \equiv \mathbf{k}^{\mathbf{n}} = \sum_{i=1}^3 \mathbf{b}_i n_i / N_i = \tilde{\mathbf{B}} \mathbf{n}`,
  where :math:`\mathbf{b}_i` is the reciprocal lattice basis vector in direction :math:`i`. The inverse map is then
  :math:`\mathbf{n}(\mathbf{k}^{\mathbf{n}}) = \tilde{\mathbf{B}}^{-1} \mathbf{k}^{\mathbf{n}} = \mathbf{n}`.

Green's function containers that are based on a BZ mesh store the function values at the discrete :math:`\mathbf{k}`
-points :math:`\mathbf{k}^{\mathbf{n}}`, i.e. :math:`f_{\mathbf{n}} = f(\mathbf{k}^{\mathbf{n}})`.
Because of the PBC, the container only has to store values for indices with :math:`0 \leq n_i < N_i`. To evaluate
the function

- at an arbitrary :math:`\mathbf{k}`-vector, :math:`\mathbf{k}` is first mapped to the BZ using PBC and then
  trilinear interpolation is performed.
- at one of the :math:`\mathbf{k}^{\mathbf{n}}`, it is first mapped to the BZ using PBC and then the
  corresponding function value is returned.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_9>;
// --------- class _c2py_cls_10 -----------
using _c2py_cls_10                                            = triqs::mesh::chebyshev;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_10>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_10> = "triqs.mesh.meshes.MeshChebyshev";
static const auto _c2py_init_9                                = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_10>(), c2py::c_constructor<_c2py_cls_10, double, triqs::mesh::statistic_enum, long>("beta", "stat", "N")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_10> = c2py::pyfkw_constructor<_c2py_init_9>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_10> =
   _c2py_init_9.doc(R"DOC(
[1] Default constructor constructs an empty mesh.

------

[2] Construct a Chebyshev mesh on :math:`[0, \beta]` with :math:`N` collocation points.

------

Parameters
----------
beta : {par_0}
   Inverse temperature :math:`\beta > 0`.
stat : {par_1}
   Particle statistics.
N : {par_2}
   Number of Chebyshev points :math:`N > 0`.
)DOC",
                    {{c2py::python_typename<double>()}, {c2py::python_typename<triqs::mesh::statistic_enum>()}, {c2py::python_typename<long>()}});
// __call__
static auto const _c2py_fun_77 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_10 const &self, long n) -> decltype(auto) { return self.operator()(n); }, "self", "n")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_10> = c2py::pyfkw<_c2py_fun_77>;

// copy
static auto const _c2py_fun_78 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy<triqs::mesh::chebyshev>, "m")};

// copy_from
static auto const _c2py_fun_79 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy_from<triqs::mesh::chebyshev>, "m1", "m2")};

// is_index_valid
static auto const _c2py_fun_80 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_10 const &self, long n) -> decltype(auto) { return self.is_index_valid(n); }, "self", "n")};

// to_data_index
static auto const _c2py_fun_81 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_10 const &self, long n) -> decltype(auto) { return self.to_data_index(n); }, "self", "n")};

// to_index
static auto const _c2py_fun_82 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_10 const &self, long d) -> decltype(auto) { return self.to_index(d); }, "self", "d")};

// to_value
static auto const _c2py_fun_83 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_10 const &self, long n) -> decltype(auto) { return self.to_value(n); }, "self", "n")};

// values
static auto const _c2py_fun_84 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::values<triqs::mesh::chebyshev>, "m")};

static const auto _c2py_doc_77 = _c2py_fun_77.doc(R"DOC(
Function call operator to access a mesh point by its index :math:`n \in \{0, 1, \ldots, N-1\}`.

Parameters
----------
n : {par_0}
   Index :math:`n` of the mesh point.

Returns
-------
{ret_0}
   mesh_point_t with the index :math:`n`, data index :math:`d(n) = n`, hash value and value :math:`\tau_n`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<triqs::mesh::chebyshev::mesh_point_t>()});
static const auto _c2py_doc_78 =
   _c2py_fun_78.doc(R"DOC(
Get a copy of a mesh (for Python bindings).

Parameters
----------
m : {par_0}
   The mesh object to copy.

Returns
-------
{ret_0}
   Copy of the given mesh.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::chebyshev &>()}}, {c2py::python_typename<triqs::mesh::chebyshev>()});
static const auto _c2py_doc_79 =
   _c2py_fun_79.doc(R"DOC(
Copy one mesh into another (for Python bindings).

Simply calls the copy assignment operator of the mesh.

Parameters
----------
m1 : {par_0}
   The mesh object to copy into.
m2 : {par_1}
   The mesh object to copy from.
)DOC",
                    {{c2py::python_typename<triqs::mesh::chebyshev &>()}, {c2py::python_typename<const triqs::mesh::chebyshev &>()}});
static const auto _c2py_doc_80 = _c2py_fun_80.doc(R"DOC(
Check if an index :math:`n` is valid.

Parameters
----------
n : {par_0}
   Index :math:`n` to check.

Returns
-------
{ret_0}
   True if :math:`0 \leq n < N`, false otherwise.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_81 = _c2py_fun_81.doc(R"DOC(
Map an index :math:`n \in \{0, 1, \ldots, N-1\}` to its corresponding data index :math:`d(n)`.

Parameters
----------
n : {par_0}
   Index :math:`n` to map.

Returns
-------
{ret_0}
   Data index :math:`d(n) = n`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_82 = _c2py_fun_82.doc(R"DOC(
Map a data index :math:`d \in \{0, 1, \ldots, N-1\}` to the corresponding index :math:`n(d)`.

Parameters
----------
d : {par_0}
   Data index :math:`d` to map.

Returns
-------
{ret_0}
   Index :math:`n(d) = d`.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_83 = _c2py_fun_83.doc(R"DOC(
Map an index :math:`n` to its corresponding value :math:`\tau_n`.

Parameters
----------
n : {par_0}
   Index :math:`n` to map.

Returns
-------
{ret_0}
   Value :math:`\tau_n` (Chebyshev point scaled to :math:`[0, \beta]`).
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<double>()});
static const auto _c2py_doc_84 = _c2py_fun_84.doc(
   R"DOC(
Get the values of all mesh points in a mesh.

Parameters
----------
m : {par_0}
   A mesh object.

Returns
-------
{ret_0}
   Array containing the values of all mesh points.
)DOC",
   {{c2py::python_typename<const triqs::mesh::chebyshev &>()}},
   {c2py::python_typename<nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_10>[] = {
   {"copy", (PyCFunction)c2py::pyfkw<_c2py_fun_78>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_78.c_str()},
   {"copy_from", (PyCFunction)c2py::pyfkw<_c2py_fun_79>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_79.c_str()},
   {"is_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_80>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_80.c_str()},
   {"to_data_index", (PyCFunction)c2py::pyfkw<_c2py_fun_81>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_81.c_str()},
   {"to_index", (PyCFunction)c2py::pyfkw<_c2py_fun_82>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_82.c_str()},
   {"to_value", (PyCFunction)c2py::pyfkw<_c2py_fun_83>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_83.c_str()},
   {"values", (PyCFunction)c2py::pyfkw<_c2py_fun_84>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_84.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_10>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_10>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_10>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_62 = R"DOC(Get the inverse temperature :math:`\beta`.)DOC";
static constexpr auto prop_doc_63 = R"DOC(Get the precomputed :math:`2 / \beta` for fast tau -> [-1, 1] mapping.)DOC";
static constexpr auto prop_doc_64 = R"DOC(Get the hash value of the mesh.)DOC";
static constexpr auto prop_doc_65 = R"DOC(Access to Chebyshev points scaled to [0, beta].)DOC";
static constexpr auto prop_doc_66 = R"DOC(Access to Chebyshev points on [-1, 1].)DOC";
static constexpr auto prop_doc_67 = R"DOC(Get the particle statistics.)DOC";
static constexpr auto prop_doc_68 = R"DOC(Access to barycentric weights.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_10>[] = {

   {"beta", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::chebyshev::beta)>, nullptr, prop_doc_62, nullptr},
   {"inv_beta_x2", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::chebyshev::inv_beta_x2)>, nullptr, prop_doc_63, nullptr},
   {"mesh_hash", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::chebyshev::mesh_hash)>, nullptr, prop_doc_64, nullptr},
   {"points", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::chebyshev::points)>, nullptr, prop_doc_65, nullptr},
   {"points_standard", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::chebyshev::points_standard)>, nullptr, prop_doc_66, nullptr},
   {"statistic", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::chebyshev::statistic)>, nullptr, prop_doc_67, nullptr},
   {"weights", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::chebyshev::weights)>, nullptr, prop_doc_68, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_9(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_10, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_10> = {c2py::tpxx_size<_c2py_cls_10>, getitem_9, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_10> = R"DOC(Chebyshev imaginary time mesh type.

A Chebyshev imaginary time mesh is defined by its size :math:`N \geq 0`, an inverse temperature :math:`\beta
> 0` and its particle statistics. It stores function values at Chebyshev collocation points and uses
barycentric interpolation for evaluation at arbitrary imaginary times, providing exponential convergence for smooth
functions.

A Chebyshev imaginary time mesh has the following properties:

- Each mesh point is identified by a unique index :math:`n \in \{0, 1, \ldots, N-1\}`.
- An index :math:`n` is mapped to the corresponding data index :math:`d` by the identity function :math:`d(n) = n`
  and vice versa.
- An index :math:`n` is mapped to the corresponding value :math:`\tau` by the function
  :math:`\tau(n) = \frac{\beta}{2} (x_n + 1)` where :math:`x_n = \cos\left(\frac{(2n + 1) \pi}{2N}\right)` is the
  Chebyshev point of the first kind.

Green's function containers that are based on a Chebyshev imaginary time mesh store the function values at the
discrete time points :math:`\tau(n)`, i.e. :math:`f_n = f(\tau(n))`, and use barycentric interpolation to evaluate
the function at an arbitrary imaginary time :math:`\tau \in [0, \beta]`:

.. math::

   f(\tau) \approx \frac{\sum_{n=0}^{N-1} \frac{w_n}{x - x_n} f_n}{\sum_{n=0}^{N-1} \frac{w_n}{x - x_n}}

where :math:`x = 2\tau/\beta - 1` is the scaled coordinate and :math:`w_n` are the barycentric weights.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_10>;
// --------- class _c2py_cls_11 -----------
using _c2py_cls_11                                            = triqs::mesh::dlr2d;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_11>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_11> = "triqs.mesh.meshes.MeshDLR2D";
static const auto _c2py_init_10 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_11>(),
                           c2py::c_constructor<_c2py_cls_11, double, double, double, triqs::mesh::channel_enum, bool, bool>(
                              "b", "wmax", "epsilon", "channel", "compressgrid"_a = false, "compressbasis"_a = true),
                           c2py::c_constructor<_c2py_cls_11, const triqs::mesh::dlr2d_imfreq &>("m")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_11> = c2py::pyfkw_constructor<_c2py_init_10>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_11> = _c2py_init_10.doc(R"DOC(
[1] Default constructor constructs an empty mesh.

------

[2] Construct a 2D DLR mesh with a given energy cutoff :math:`\omega_{\text{max}}` and error tolerance :math:`\epsilon`
.

------

[3] Construct a 2D DLR coefficient mesh from a 2D DLR imfreq mesh.

------

Parameters
----------
b : {par_0}
   Inverse temperature :math:`\beta > 0`.
wmax : {par_1}
   DLR energy cutoff :math:`\omega_{\text{max}} = \Lambda / \beta`.
epsilon : {par_2}
   Error tolerance :math:`\epsilon`.
channel : {par_3}
   Channel: PP (particle-particle) or PH (particle-hole).
compressgrid : {par_4}
   Whether to compress the imaginary frequency grid (default: false).
compressbasis : {par_5}
   Whether to compress the real frequency basis (default: true).
m : {par_6}
   triqs::mesh::dlr2d_imfreq mesh.
)DOC",
                                                                      {{c2py::python_typename<double>()},
                                                                       {c2py::python_typename<double>()},
                                                                       {c2py::python_typename<double>()},
                                                                       {c2py::python_typename<triqs::mesh::channel_enum>()},
                                                                       {c2py::python_typename<bool>()},
                                                                       {c2py::python_typename<bool>()},
                                                                       {c2py::python_typename<const triqs::mesh::dlr2d_imfreq &>()}});
// __call__
static auto const _c2py_fun_85 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_11 const &self, const triqs::mesh::dlr2d::index_t &idx) -> decltype(auto) { return self.operator()(idx); }, "self", "idx")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_11> = c2py::pyfkw<_c2py_fun_85>;

// copy
static auto const _c2py_fun_86 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy<triqs::mesh::dlr2d>, "m")};

// copy_from
static auto const _c2py_fun_87 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy_from<triqs::mesh::dlr2d>, "m1", "m2")};

// is_data_index_valid
static auto const _c2py_fun_88 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_11 const &self, long d) -> decltype(auto) { return self.is_data_index_valid(d); }, "self", "d")};

// is_index_valid
static auto const _c2py_fun_89 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_11 const &self, const triqs::mesh::dlr2d::index_t &idx) -> decltype(auto) { return self.is_index_valid(idx); }, "self", "idx")};

// to_data_index
static auto const _c2py_fun_90 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_11 const &self, long d) -> decltype(auto) { return self.to_data_index(d); }, "self", "d"),
   c2py::cmethod([](_c2py_cls_11 const &self, const triqs::mesh::dlr2d::index_t &idx) -> decltype(auto) { return self.to_data_index(idx); }, "self",
                 "idx")};

// to_index
static auto const _c2py_fun_91 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_11 const &self, long d) -> decltype(auto) { return self.to_index(d); }, "self", "d")};

// to_value
static auto const _c2py_fun_92 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_11 const &self, long d) -> decltype(auto) { return self.to_value(d); }, "self", "d"),
   c2py::cmethod([](_c2py_cls_11 const &self, const triqs::mesh::dlr2d::index_t &idx) -> decltype(auto) { return self.to_value(idx); }, "self",
                 "idx")};

static const auto _c2py_doc_85 =
   _c2py_fun_85.doc(R"DOC(
Function call operator to access a mesh point by its index {term_type, k, l}.

Parameters
----------
idx : {par_0}
   Index of the mesh point.

Returns
-------
{ret_0}
   mesh_point_t with the index, data index, hash value and DLR frequency pair.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::dlr2d::index_t &>()}}, {c2py::python_typename<triqs::mesh::dlr2d::mesh_point_t>()});
static const auto _c2py_doc_86 =
   _c2py_fun_86.doc(R"DOC(
Get a copy of a mesh (for Python bindings).

Parameters
----------
m : {par_0}
   The mesh object to copy.

Returns
-------
{ret_0}
   Copy of the given mesh.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::dlr2d &>()}}, {c2py::python_typename<triqs::mesh::dlr2d>()});
static const auto _c2py_doc_87 =
   _c2py_fun_87.doc(R"DOC(
Copy one mesh into another (for Python bindings).

Simply calls the copy assignment operator of the mesh.

Parameters
----------
m1 : {par_0}
   The mesh object to copy into.
m2 : {par_1}
   The mesh object to copy from.
)DOC",
                    {{c2py::python_typename<triqs::mesh::dlr2d &>()}, {c2py::python_typename<const triqs::mesh::dlr2d &>()}});
static const auto _c2py_doc_88 = _c2py_fun_88.doc(R"DOC(
Check if a data index is valid.

Parameters
----------
d : {par_0}
   Data index to check.

Returns
-------
{ret_0}
   True if :math:`0 \leq d < N`, false otherwise.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_89 = _c2py_fun_89.doc(R"DOC(
Check if an index {term_type, k, l} is valid.

Parameters
----------
idx : {par_0}
   Index to check.

Returns
-------
{ret_0}
   True if term_type is in [0,3] and k,l are in [0, rank), false otherwise.
)DOC",
                                                  {{c2py::python_typename<const triqs::mesh::dlr2d::index_t &>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_90 = _c2py_fun_90.doc(R"DOC(
[1] Identity conversion for data index (required by GF framework).

------

[2] Map an index {term_type, k, l} to its corresponding data index.

------

Parameters
----------
d : {par_0}
   Data index.
idx : {par_1}
   Index to map.

Returns
-------
[1] : {ret_0}
   The same data index.

[2] : {ret_1}
   Data index (linear index into dlr2d_rf).
)DOC",
                                                  {{c2py::python_typename<long>()}, {c2py::python_typename<const triqs::mesh::dlr2d::index_t &>()}},
                                                  {c2py::python_typename<long>(), c2py::python_typename<long>()});
static const auto _c2py_doc_91 = _c2py_fun_91.doc(R"DOC(
Map a data index to the corresponding index {term_type, k, l}.

Parameters
----------
d : {par_0}
   Data index to map.

Returns
-------
{ret_0}
   Index {term_type, k_index, l_index}.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<triqs::mesh::dlr2d::index_t>()});
static const auto _c2py_doc_92 = _c2py_fun_92.doc(R"DOC(
[1] Map a data index to its corresponding value (DLR frequency pair).

------

[2] Map an index {term_type, k, l} to its corresponding value (DLR frequency pair).

------

Parameters
----------
d : {par_0}
   Data index to map.
idx : {par_1}
   Index to map.

Returns
-------
{ret_0}
   Pair of DLR frequencies :math:`(\omega_i, \omega_j)`.
)DOC",
                                                  {{c2py::python_typename<long>()}, {c2py::python_typename<const triqs::mesh::dlr2d::index_t &>()}},
                                                  {c2py::python_typename<triqs::mesh::dlr2d::value_t>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_11>[] = {
   {"copy", (PyCFunction)c2py::pyfkw<_c2py_fun_86>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_86.c_str()},
   {"copy_from", (PyCFunction)c2py::pyfkw<_c2py_fun_87>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_87.c_str()},
   {"is_data_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_88>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_88.c_str()},
   {"is_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_89>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_89.c_str()},
   {"to_data_index", (PyCFunction)c2py::pyfkw<_c2py_fun_90>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_90.c_str()},
   {"to_index", (PyCFunction)c2py::pyfkw<_c2py_fun_91>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_91.c_str()},
   {"to_value", (PyCFunction)c2py::pyfkw<_c2py_fun_92>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_92.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_11>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_11>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_11>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_69 = R"DOC(Get the inverse temperature :math:`\beta`.)DOC";
static constexpr auto prop_doc_70 = R"DOC(Get the channel: PP (particle-particle) or PH (particle-hole).)DOC";
static constexpr auto prop_doc_71 = R"DOC(Is the real frequency basis compressed?)DOC";
static constexpr auto prop_doc_72 = R"DOC(Is the imaginary frequency grid compressed?)DOC";
static constexpr auto prop_doc_73 = R"DOC(Get the 2D DLR imaginary frequency grid indices.)DOC";
static constexpr auto prop_doc_74 = R"DOC(Get the 2D DLR real frequency grid indices.)DOC";
static constexpr auto prop_doc_75 = R"DOC(Get the `nda::vector` of 1D DLR frequencies :math:`\omega_l`.)DOC";
static constexpr auto prop_doc_76 = R"DOC(Get the DLR error tolerance :math:`\epsilon`.)DOC";
static constexpr auto prop_doc_77 = R"DOC(Get the hash value of the mesh.)DOC";
static constexpr auto prop_doc_78 = R"DOC(Get the 1D DLR rank (number of 1D DLR frequencies).)DOC";
static constexpr auto prop_doc_79 = R"DOC(Get the particle statistics (always fermionic for 2D DLR).)DOC";
static constexpr auto prop_doc_80 = R"DOC(Get the DLR energy cutoff :math:`\omega_{\text{max}} = \Lambda / \beta`.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_11>[] = {

   {"beta", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d::beta)>, nullptr, prop_doc_69, nullptr},
   {"channel", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d::channel)>, nullptr, prop_doc_70, nullptr},
   {"compressbasis", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d::compressbasis)>, nullptr, prop_doc_71, nullptr},
   {"compressgrid", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d::compressgrid)>, nullptr, prop_doc_72, nullptr},
   {"dlr2d_if", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d::dlr2d_if)>, nullptr, prop_doc_73, nullptr},
   {"dlr2d_rf", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d::dlr2d_rf)>, nullptr, prop_doc_74, nullptr},
   {"dlr_rf", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d::dlr_rf)>, nullptr, prop_doc_75, nullptr},
   {"eps", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d::eps)>, nullptr, prop_doc_76, nullptr},
   {"mesh_hash", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d::mesh_hash)>, nullptr, prop_doc_77, nullptr},
   {"rank", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d::rank)>, nullptr, prop_doc_78, nullptr},
   {"statistic", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d::statistic)>, nullptr, prop_doc_79, nullptr},
   {"w_max", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d::w_max)>, nullptr, prop_doc_80, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_10(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_11, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_11> = {c2py::tpxx_size<_c2py_cls_11>, getitem_10, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_11> = R"DOC(2D Discrete Lehmann representation (DLR) coefficient mesh type.

A 2D DLR mesh is designed for representing three-point functions
using the discrete Lehmann representation. It stores the DLR expansion
coefficients for two-frequency dependent quantities.

A 2D DLR mesh has the following properties:

- Each mesh point is identified by a unique linear index :math:`l \in \{0, 1, \ldots, N-1\}`.
- The size of the mesh :math:`N` depends on the DLR rank and the compression settings.
- An index :math:`l` is mapped to the corresponding data index :math:`d` by the identity function :math:`d(l) = l`
and vice versa.
- An index :math:`l` is mapped to a pair of DLR frequencies :math:`(\omega_i, \omega_j)`.

::

   #include <fmt/base.h>
   #include <triqs/mesh.hpp>
   
   int main() {
     // initialize a 2D DLR mesh with beta = 10, w_max = 1.0 and eps = 1e-10
     triqs::mesh::dlr2d m{10, 1.0, 1e-10};
   
     // print mesh information
     fmt::println("Mesh size: {}, Rank: {}", m.size(), m.rank());
   })DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_11>;
// --------- class _c2py_cls_12 -----------
using _c2py_cls_12                                            = triqs::mesh::dlr2d_imfreq;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_12>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_12> = "triqs.mesh.meshes.MeshDLR2DImFreq";
static const auto _c2py_init_11 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_12>(),
                           c2py::c_constructor<_c2py_cls_12, double, double, double, triqs::mesh::channel_enum, bool, bool>(
                              "b", "wmax", "epsilon", "channel", "compressgrid"_a = false, "compressbasis"_a = true),
                           c2py::c_constructor<_c2py_cls_12, const triqs::mesh::dlr2d &>("m")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_12> = c2py::pyfkw_constructor<_c2py_init_11>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_12> = _c2py_init_11.doc(R"DOC(
[1] Default constructor constructs an empty mesh.

------

[2] Construct a 2D DLR imfreq mesh with a given energy cutoff :math:`\omega_{\text{max}}` and error tolerance :math:`\epsilon`
.

------

[3] Construct a 2D DLR imfreq mesh from a 2D DLR coefficient mesh.

------

Parameters
----------
b : {par_0}
   Inverse temperature :math:`\beta > 0`.
wmax : {par_1}
   DLR energy cutoff :math:`\omega_{\text{max}} = \Lambda / \beta`.
epsilon : {par_2}
   Error tolerance :math:`\epsilon`.
channel : {par_3}
   Channel: PP (particle-particle) or PH (particle-hole).
compressgrid : {par_4}
   Whether to compress the imaginary frequency grid (default: false).
compressbasis : {par_5}
   Whether to compress the real frequency basis (default: true).
m : {par_6}
   triqs::mesh::dlr2d mesh.
)DOC",
                                                                      {{c2py::python_typename<double>()},
                                                                       {c2py::python_typename<double>()},
                                                                       {c2py::python_typename<double>()},
                                                                       {c2py::python_typename<triqs::mesh::channel_enum>()},
                                                                       {c2py::python_typename<bool>()},
                                                                       {c2py::python_typename<bool>()},
                                                                       {c2py::python_typename<const triqs::mesh::dlr2d &>()}});
// __call__
static auto const _c2py_fun_93 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_12 const &self, triqs::mesh::dlr2d_imfreq::index_t idx) -> decltype(auto) { return self.operator()(idx); }, "self", "idx")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_12> = c2py::pyfkw<_c2py_fun_93>;

// copy
static auto const _c2py_fun_94 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy<triqs::mesh::dlr2d_imfreq>, "m")};

// copy_from
static auto const _c2py_fun_95 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy_from<triqs::mesh::dlr2d_imfreq>, "m1", "m2")};

// is_data_index_valid
static auto const _c2py_fun_96 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_12 const &self, long d) -> decltype(auto) { return self.is_data_index_valid(d); }, "self", "d")};

// is_index_valid
static auto const _c2py_fun_97 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_12 const &self, triqs::mesh::dlr2d_imfreq::index_t idx) -> decltype(auto) { return self.is_index_valid(idx); }, "self", "idx")};

// to_data_index
static auto const _c2py_fun_98 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_12 const &self, triqs::mesh::dlr2d_imfreq::index_t idx) -> decltype(auto) { return self.to_data_index(idx); }, "self", "idx")};

// to_index
static auto const _c2py_fun_99 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_12 const &self, long d) -> decltype(auto) { return self.to_index(d); }, "self", "d")};

// to_value
static auto const _c2py_fun_100 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_12 const &self, triqs::mesh::dlr2d_imfreq::index_t idx) -> decltype(auto) { return self.to_value(idx); }, "self", "idx")};

static const auto _c2py_doc_93 = _c2py_fun_93.doc(R"DOC(
Function call operator to access a mesh point by its index (pair of Matsubara frequency indices).

Parameters
----------
idx : {par_0}
   Index (pair of Matsubara frequency indices).

Returns
-------
{ret_0}
   mesh_point_t with the data index, hash value, and Matsubara frequency pair.
)DOC",
                                                  {{c2py::python_typename<triqs::mesh::dlr2d_imfreq::index_t>()}},
                                                  {c2py::python_typename<triqs::mesh::dlr2d_imfreq::mesh_point_t>()});
static const auto _c2py_doc_94 =
   _c2py_fun_94.doc(R"DOC(
Get a copy of a mesh (for Python bindings).

Parameters
----------
m : {par_0}
   The mesh object to copy.

Returns
-------
{ret_0}
   Copy of the given mesh.
)DOC",
                    {{c2py::python_typename<const triqs::mesh::dlr2d_imfreq &>()}}, {c2py::python_typename<triqs::mesh::dlr2d_imfreq>()});
static const auto _c2py_doc_95 =
   _c2py_fun_95.doc(R"DOC(
Copy one mesh into another (for Python bindings).

Simply calls the copy assignment operator of the mesh.

Parameters
----------
m1 : {par_0}
   The mesh object to copy into.
m2 : {par_1}
   The mesh object to copy from.
)DOC",
                    {{c2py::python_typename<triqs::mesh::dlr2d_imfreq &>()}, {c2py::python_typename<const triqs::mesh::dlr2d_imfreq &>()}});
static const auto _c2py_doc_96 = _c2py_fun_96.doc(R"DOC(
Check if a data index :math:`d` is valid.

Parameters
----------
d : {par_0}
   Data index :math:`d` to check.

Returns
-------
{ret_0}
   True if :math:`0 \leq d < N`, false otherwise.
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_97 = _c2py_fun_97.doc(R"DOC(
Check if an index (pair of Matsubara frequency indices) is valid.

Parameters
----------
idx : {par_0}
   Index (pair of Matsubara frequency indices) to check.

Returns
-------
{ret_0}
   True if the index is in the mesh, false otherwise.
)DOC",
                                                  {{c2py::python_typename<triqs::mesh::dlr2d_imfreq::index_t>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_98 = _c2py_fun_98.doc(R"DOC(
Map an index (pair of Matsubara frequency indices) to its corresponding data index.

Parameters
----------
idx : {par_0}
   Index (pair of Matsubara frequency indices).

Returns
-------
{ret_0}
   Data index.
)DOC",
                                                  {{c2py::python_typename<triqs::mesh::dlr2d_imfreq::index_t>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_99 = _c2py_fun_99.doc(R"DOC(
Map a data index to the corresponding index (pair of Matsubara frequency indices).

Parameters
----------
d : {par_0}
   Data index.

Returns
-------
{ret_0}
   Index (pair of Matsubara frequency indices).
)DOC",
                                                  {{c2py::python_typename<long>()}}, {c2py::python_typename<triqs::mesh::dlr2d_imfreq::index_t>()});
static const auto _c2py_doc_100 =
   _c2py_fun_100.doc(R"DOC(
Map an index (pair of Matsubara frequency indices) to its corresponding value (pair of Matsubara frequencies).

Parameters
----------
idx : {par_0}
   Index (pair of Matsubara frequency indices) to map.

Returns
-------
{ret_0}
   Pair of Matsubara frequencies.
)DOC",
                     {{c2py::python_typename<triqs::mesh::dlr2d_imfreq::index_t>()}}, {c2py::python_typename<triqs::mesh::dlr2d_imfreq::value_t>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_12>[] = {
   {"copy", (PyCFunction)c2py::pyfkw<_c2py_fun_94>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_94.c_str()},
   {"copy_from", (PyCFunction)c2py::pyfkw<_c2py_fun_95>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_95.c_str()},
   {"is_data_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_96>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_96.c_str()},
   {"is_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_97>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_97.c_str()},
   {"to_data_index", (PyCFunction)c2py::pyfkw<_c2py_fun_98>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_98.c_str()},
   {"to_index", (PyCFunction)c2py::pyfkw<_c2py_fun_99>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_99.c_str()},
   {"to_value", (PyCFunction)c2py::pyfkw<_c2py_fun_100>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_100.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_12>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_12>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_12>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_81 = R"DOC(Get the inverse temperature :math:`\beta`.)DOC";
static constexpr auto prop_doc_82 = R"DOC(Get the channel: PP (particle-particle) or PH (particle-hole).)DOC";
static constexpr auto prop_doc_83 = R"DOC(Is the real frequency basis compressed?)DOC";
static constexpr auto prop_doc_84 = R"DOC(Is the imaginary frequency grid compressed?)DOC";
static constexpr auto prop_doc_85 = R"DOC(Get the 2D DLR imaginary frequency grid indices.)DOC";
static constexpr auto prop_doc_86 = R"DOC(Get the 2D DLR real frequency grid indices.)DOC";
static constexpr auto prop_doc_87 = R"DOC(Get the `nda::vector` of 1D DLR frequencies :math:`\omega_l`.)DOC";
static constexpr auto prop_doc_88 = R"DOC(Get the DLR error tolerance :math:`\epsilon`.)DOC";
static constexpr auto prop_doc_89 = R"DOC(Get the 2D DLR imaginary frequency grid indices for the particle-hole channel.)DOC";
static constexpr auto prop_doc_90 = R"DOC(Get the maximum absolute Matsubara index across all mesh points.)DOC";
static constexpr auto prop_doc_91 = R"DOC(Get the hash value of the mesh.)DOC";
static constexpr auto prop_doc_92 = R"DOC(Get the 1D DLR rank (number of 1D DLR frequencies).)DOC";
static constexpr auto prop_doc_93 = R"DOC(Get the particle statistics (always fermionic for 2D DLR).)DOC";
static constexpr auto prop_doc_94 = R"DOC(Get the DLR energy cutoff :math:`\omega_{\text{max}} = \Lambda / \beta`.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_12>[] = {

   {"beta", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d_imfreq::beta)>, nullptr, prop_doc_81, nullptr},
   {"channel", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d_imfreq::channel)>, nullptr, prop_doc_82, nullptr},
   {"compressbasis", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d_imfreq::compressbasis)>, nullptr, prop_doc_83, nullptr},
   {"compressgrid", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d_imfreq::compressgrid)>, nullptr, prop_doc_84, nullptr},
   {"dlr2d_if", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d_imfreq::dlr2d_if)>, nullptr, prop_doc_85, nullptr},
   {"dlr2d_rf", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d_imfreq::dlr2d_rf)>, nullptr, prop_doc_86, nullptr},
   {"dlr_rf", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d_imfreq::dlr_rf)>, nullptr, prop_doc_87, nullptr},
   {"eps", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d_imfreq::eps)>, nullptr, prop_doc_88, nullptr},
   {"get_dlr2d_if_ph", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d_imfreq::get_dlr2d_if_ph)>, nullptr, prop_doc_89, nullptr},
   {"max_n", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d_imfreq::max_n)>, nullptr, prop_doc_90, nullptr},
   {"mesh_hash", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d_imfreq::mesh_hash)>, nullptr, prop_doc_91, nullptr},
   {"rank", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d_imfreq::rank)>, nullptr, prop_doc_92, nullptr},
   {"statistic", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d_imfreq::statistic)>, nullptr, prop_doc_93, nullptr},
   {"w_max", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::dlr2d_imfreq::w_max)>, nullptr, prop_doc_94, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_11(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_12, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_12> = {c2py::tpxx_size<_c2py_cls_12>, getitem_11, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_12> = R"DOC(Imaginary frequency 2D discrete Lehmann representation (DLR) mesh type.

A 2D imaginary frequency DLR mesh is designed for representing three-point functions
using the discrete Lehmann representation. It stores function values on a 2D grid of
Matsubara frequency pairs.

A 2D imaginary frequency DLR mesh has the following properties:

- Each mesh point is identified by a unique data index :math:`d \in \{0, 1, \ldots, N-1\}`.
- The size of the mesh :math:`N` depends on the DLR rank and the compression settings.
- A data index :math:`d` is mapped to a pair of Matsubara frequency indices :math:`(n_1, n_2)`.
- A data index :math:`d` is mapped to a pair of Matsubara frequencies :math:`(i\omega_{n_1}, i\omega_{n_2})`.

.. note::

   The index type is `std::array<long, 2>` representing the pair of Matsubara frequency indices.
   This is different from the linear data_index.

::

   #include <fmt/base.h>
   #include <triqs/mesh.hpp>
   
   int main() {
     // initialize a 2D DLR imfreq mesh with beta = 10, w_max = 1.0 and eps = 1e-10
     triqs::mesh::dlr2d_imfreq m{10, 1.0, 1e-10};
   
     // print mesh information
     fmt::println("Mesh size: {}, Rank: {}", m.size(), m.rank());
   })DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_12>;
// --------- class _c2py_cls_13 -----------
using _c2py_cls_13                                            = triqs::mesh::legendre;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_13>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_13> = "triqs.mesh.meshes.MeshLegendre";
static const auto _c2py_init_12 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_13>(),
                           c2py::c_constructor<_c2py_cls_13, double, triqs::mesh::statistic_enum, long>("beta", "statistic", "max_n")}
      .with_deprecated_params({{"S", "statistic"}, {"n_max", "max_n"}});
template <> constexpr initproc c2py::tp_init<_c2py_cls_13> = c2py::pyfkw_constructor<_c2py_init_12>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_13> =
   _c2py_init_12.doc(R"DOC(
[1] Default constructor constructs an empty mesh.

------

[2] Construct a mesh of Legendre polynomials with degrees :math:`n = 0, 1, \ldots, N - 1` on the interval :math:`[0, \beta]`
and the given particle statistics.

------

Parameters
----------
beta : {par_0}
   Inverse temperature :math:`\beta > 0`.
statistic : {par_1}
   Particle statistics.
max_n : {par_2}
   Size of the mesh, i.e. the number of Legendre polynomial used in the series expansion.
)DOC",
                     {{c2py::python_typename<double>()}, {c2py::python_typename<triqs::mesh::statistic_enum>()}, {c2py::python_typename<long>()}});
// __call__
static auto const _c2py_fun_101 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_13 const &self, long n) -> decltype(auto) { return self.operator()(n); }, "self", "n")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_13> = c2py::pyfkw<_c2py_fun_101>;

// copy
static auto const _c2py_fun_102 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy<triqs::mesh::legendre>, "m")};

// copy_from
static auto const _c2py_fun_103 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy_from<triqs::mesh::legendre>, "m1", "m2")};

// is_index_valid
static auto const _c2py_fun_104 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_13 const &self, long n) -> decltype(auto) { return self.is_index_valid(n); }, "self", "n")};

// to_data_index
static auto const _c2py_fun_105 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_13 const &self, long n) -> decltype(auto) { return self.to_data_index(n); }, "self", "n")};

// to_index
static auto const _c2py_fun_106 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_13 const &self, long d) -> decltype(auto) { return self.to_index(d); }, "self", "d")};

static const auto _c2py_doc_101 =
   _c2py_fun_101.doc(R"DOC(
Function call operator to access a mesh point by its index :math:`n \in \{0, 1, \ldots, N-1\}`.

Parameters
----------
n : {par_0}
   Index :math:`n` of the mesh point.

Returns
-------
{ret_0}
   mesh_point_t with the index :math:`n`, data index :math:`d(n) = n` and hash value of the current mesh.
)DOC",
                     {{c2py::python_typename<long>()}}, {c2py::python_typename<triqs::mesh::legendre::mesh_point_t>()});
static const auto _c2py_doc_102 =
   _c2py_fun_102.doc(R"DOC(
Get a copy of a mesh (for Python bindings).

Parameters
----------
m : {par_0}
   The mesh object to copy.

Returns
-------
{ret_0}
   Copy of the given mesh.
)DOC",
                     {{c2py::python_typename<const triqs::mesh::legendre &>()}}, {c2py::python_typename<triqs::mesh::legendre>()});
static const auto _c2py_doc_103 =
   _c2py_fun_103.doc(R"DOC(
Copy one mesh into another (for Python bindings).

Simply calls the copy assignment operator of the mesh.

Parameters
----------
m1 : {par_0}
   The mesh object to copy into.
m2 : {par_1}
   The mesh object to copy from.
)DOC",
                     {{c2py::python_typename<triqs::mesh::legendre &>()}, {c2py::python_typename<const triqs::mesh::legendre &>()}});
static const auto _c2py_doc_104 = _c2py_fun_104.doc(R"DOC(
Check if an index :math:`n` is valid.

Parameters
----------
n : {par_0}
   Index :math:`n` to check.

Returns
-------
{ret_0}
   True if :math:`0 \leq n < N`, false otherwise.
)DOC",
                                                    {{c2py::python_typename<long>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_105 = _c2py_fun_105.doc(R"DOC(
Map an index :math:`n \in \{0, 1, \ldots, N-1\}` to its corresponding data index :math:`d(n)`.

Parameters
----------
n : {par_0}
   Index :math:`n` to map.

Returns
-------
{ret_0}
   Data index :math:`d(n) = n`.
)DOC",
                                                    {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_106 = _c2py_fun_106.doc(R"DOC(
Map a data index :math:`d \in \{0, 1, \ldots, N-1\}` to the corresponding index :math:`n(d)`.

Parameters
----------
d : {par_0}
   Data index :math:`d` to map.

Returns
-------
{ret_0}
   Index :math:`n(d) = d`.
)DOC",
                                                    {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_13>[] = {
   {"copy", (PyCFunction)c2py::pyfkw<_c2py_fun_102>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_102.c_str()},
   {"copy_from", (PyCFunction)c2py::pyfkw<_c2py_fun_103>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_103.c_str()},
   {"is_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_104>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_104.c_str()},
   {"to_data_index", (PyCFunction)c2py::pyfkw<_c2py_fun_105>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_105.c_str()},
   {"to_index", (PyCFunction)c2py::pyfkw<_c2py_fun_106>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_106.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_13>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_13>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_13>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_95 = R"DOC(Get the inverse temperature :math:`\beta`.)DOC";
static constexpr auto prop_doc_96 = R"DOC(Get the hash value of the mesh.)DOC";
static constexpr auto prop_doc_97 = R"DOC(Get the particle statistics.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_13>[] = {

   {"beta", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::legendre::beta)>, nullptr, prop_doc_95, nullptr},
   {"mesh_hash", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::legendre::mesh_hash)>, nullptr, prop_doc_96, nullptr},
   {"statistic", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::legendre::statistic)>, nullptr, prop_doc_97, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_12(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_13, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_13> = {c2py::tpxx_size<_c2py_cls_13>, getitem_12, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_13> = R"DOC(Legendre mesh type.

A Legendre mesh is defined by the number of Legendre polynomials :math:`N` used in the series
expansion, an inverse temperature :math:`\beta > 0` and its particle statistics.

A Legendre mesh is an index-only mesh (no value attached to each point) and has the following properties:

- Each mesh point is identified by a unique index :math:`n \in \{0, 1, \ldots, N-1\}`.
- An index :math:`n` is mapped to the corresponding data index :math:`d` by the identity function :math:`d(n) = n`
  and vice versa.
- There is no explicit value associated with a mesh point, i.e. it carries no value at each mesh point.

Green's function containers that are based on a Legendre mesh store the coefficients :math:`f_n` of the generalized
Fourier series expansion of a function :math:`f(\tau)` in terms of Legendre polynomials, which is defined on the
imaginary time axis. To evaluate the function at an arbitrary imaginary time :math:`\tau \in [0, \beta]`, the GF
container sums up the Fourier series to the maximum degree :math:`N-1`:

.. math::

   f(\tau) \approx \sum_{n=0}^{N-1} \frac{\sqrt{2n + 1}}{\beta} f_n P_n(x(\tau)) \; ,

where :math:`P_n` denotes the Legendre polynomial of degree :math:`n`, :math:`x(\tau) = 2\tau / \beta - 1` is a
linear map from :math:`[0, \beta]` to :math:`[-1, 1]` and

.. math::

   f_n = \sqrt{2n + 1} \int_0^\beta d\tau \, f(\tau) P_n(x(\tau)) \; ,

are the expansion coefficients.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_13>;
// --------- class _c2py_cls_14 -----------
using _c2py_cls_14                                            = triqs::mesh::refreq_log;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_14>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_14> = "triqs.mesh.meshes.MeshReFreqLog";
static const auto _c2py_init_13 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_14>(), c2py::c_constructor<_c2py_cls_14, double, double, double>("eps", "w_max", "ratio")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_14> = c2py::pyfkw_constructor<_c2py_init_13>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_14> =
   _c2py_init_13.doc(R"DOC(
[1] Default constructor creates an empty mesh.

------

[2] Construct a logarithmic real frequency mesh.

------

Parameters
----------
eps : {par_0}
   Smallest positive frequency :math:`\varepsilon > 0` (cutoff near zero).
w_max : {par_1}
   Largest frequency :math:`\omega_{\mathrm{max}} \geq \varepsilon`.
ratio : {par_2}
   Common ratio :math:`r > 1` of the geometric sequence.
)DOC",
                     {{c2py::python_typename<double>()}, {c2py::python_typename<double>()}, {c2py::python_typename<double>()}});
// __call__
static auto const _c2py_fun_107 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_14 const &self, long n) -> decltype(auto) { return self.operator()(n); }, "self", "n")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_14> = c2py::pyfkw<_c2py_fun_107>;

// copy
static auto const _c2py_fun_108 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy<triqs::mesh::refreq_log>, "m")};

// copy_from
static auto const _c2py_fun_109 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy_from<triqs::mesh::refreq_log>, "m1", "m2")};

// is_index_valid
static auto const _c2py_fun_110 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_14 const &self, long n) -> decltype(auto) { return self.is_index_valid(n); }, "self", "n")};

// is_value_valid
static auto const _c2py_fun_111 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_14 const &self, double w) -> decltype(auto) { return self.is_value_valid(w); }, "self", "w")};

// to_data_index
static auto const _c2py_fun_112 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_14 const &self, long n) -> decltype(auto) { return self.to_data_index(n); }, "self", "n")};

// to_index
static auto const _c2py_fun_113 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_14 const &self, long d) -> decltype(auto) { return self.to_index(d); }, "self", "d")};

// to_value
static auto const _c2py_fun_114 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_14 const &self, long n) -> decltype(auto) { return self.to_value(n); }, "self", "n")};

// values
static auto const _c2py_fun_115 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::values<triqs::mesh::refreq_log>, "m")};

static const auto _c2py_doc_107 =
   _c2py_fun_107.doc(R"DOC(
Function call operator to access a mesh point by its index.

Parameters
----------
n : {par_0}
   Index of the mesh point.

Returns
-------
{ret_0}
   mesh_point_t at the given index.
)DOC",
                     {{c2py::python_typename<long>()}}, {c2py::python_typename<triqs::mesh::refreq_log::mesh_point_t>()});
static const auto _c2py_doc_108 =
   _c2py_fun_108.doc(R"DOC(
Get a copy of a mesh (for Python bindings).

Parameters
----------
m : {par_0}
   The mesh object to copy.

Returns
-------
{ret_0}
   Copy of the given mesh.
)DOC",
                     {{c2py::python_typename<const triqs::mesh::refreq_log &>()}}, {c2py::python_typename<triqs::mesh::refreq_log>()});
static const auto _c2py_doc_109 =
   _c2py_fun_109.doc(R"DOC(
Copy one mesh into another (for Python bindings).

Simply calls the copy assignment operator of the mesh.

Parameters
----------
m1 : {par_0}
   The mesh object to copy into.
m2 : {par_1}
   The mesh object to copy from.
)DOC",
                     {{c2py::python_typename<triqs::mesh::refreq_log &>()}, {c2py::python_typename<const triqs::mesh::refreq_log &>()}});
static const auto _c2py_doc_110 = _c2py_fun_110.doc(R"DOC(
Check if an index :math:`n` is valid.

Parameters
----------
n : {par_0}
   Index :math:`n` to check.

Returns
-------
{ret_0}
   True if :math:`0 \leq n < N`, false otherwise.
)DOC",
                                                    {{c2py::python_typename<long>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_111 = _c2py_fun_111.doc(R"DOC(
Check if a value :math:`\omega` is within the mesh range.

Parameters
----------
w : {par_0}
   Value to check.

Returns
-------
{ret_0}
   True if :math:`-\omega_{\mathrm{max}} \leq \omega \leq \omega_{\mathrm{max}}`, false otherwise.
)DOC",
                                                    {{c2py::python_typename<double>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_112 = _c2py_fun_112.doc(R"DOC(
Map an index :math:`n` to its corresponding data index :math:`d(n) = n`.

Parameters
----------
n : {par_0}
   Index :math:`n` to map.

Returns
-------
{ret_0}
   Data index :math:`d(n) = n`.
)DOC",
                                                    {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_113 = _c2py_fun_113.doc(R"DOC(
Map a data index :math:`d` to the corresponding index :math:`n(d) = d`.

Parameters
----------
d : {par_0}
   Data index :math:`d` to map.

Returns
-------
{ret_0}
   Index :math:`n(d) = d`.
)DOC",
                                                    {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_114 = _c2py_fun_114.doc(R"DOC(
Map an index :math:`n` to its corresponding value :math:`\omega_n`.

Parameters
----------
n : {par_0}
   Index to map.

Returns
-------
{ret_0}
   Value of the mesh point :math:`\omega_n`.
)DOC",
                                                    {{c2py::python_typename<long>()}}, {c2py::python_typename<double>()});
static const auto _c2py_doc_115 = _c2py_fun_115.doc(
   R"DOC(
Get the values of all mesh points in a mesh.

Parameters
----------
m : {par_0}
   A mesh object.

Returns
-------
{ret_0}
   Array containing the values of all mesh points.
)DOC",
   {{c2py::python_typename<const triqs::mesh::refreq_log &>()}},
   {c2py::python_typename<nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_14>[] = {
   {"copy", (PyCFunction)c2py::pyfkw<_c2py_fun_108>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_108.c_str()},
   {"copy_from", (PyCFunction)c2py::pyfkw<_c2py_fun_109>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_109.c_str()},
   {"is_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_110>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_110.c_str()},
   {"is_value_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_111>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_111.c_str()},
   {"to_data_index", (PyCFunction)c2py::pyfkw<_c2py_fun_112>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_112.c_str()},
   {"to_index", (PyCFunction)c2py::pyfkw<_c2py_fun_113>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_113.c_str()},
   {"to_value", (PyCFunction)c2py::pyfkw<_c2py_fun_114>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_114.c_str()},
   {"values", (PyCFunction)c2py::pyfkw<_c2py_fun_115>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_115.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_14>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_14>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_14>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_98  = R"DOC(Get the smallest positive frequency :math:`\varepsilon`.)DOC";
static constexpr auto prop_doc_99  = R"DOC(Get the hash value of the mesh.)DOC";
static constexpr auto prop_doc_100 = R"DOC(Get the vector of frequency point values.)DOC";
static constexpr auto prop_doc_101 = R"DOC(Get the common ratio :math:`r` of the geometric sequence.)DOC";
static constexpr auto prop_doc_102 = R"DOC(Get the largest frequency :math:`\omega_{\mathrm{max}}`.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_14>[] = {

   {"eps", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::refreq_log::eps)>, nullptr, prop_doc_98, nullptr},
   {"mesh_hash", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::refreq_log::mesh_hash)>, nullptr, prop_doc_99, nullptr},
   {"points", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::refreq_log::points)>, nullptr, prop_doc_100, nullptr},
   {"ratio", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::refreq_log::ratio)>, nullptr, prop_doc_101, nullptr},
   {"w_max", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::refreq_log::w_max)>, nullptr, prop_doc_102, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_13(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_14, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_14> = {c2py::tpxx_size<_c2py_cls_14>, getitem_13, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_14> = R"DOC(Logarithmic real frequency mesh type.

A logarithmic real frequency mesh is defined by a cutoff frequency :math:`\varepsilon > 0`, an upper bound
:math:`\omega_{\mathrm{max}} \geq \varepsilon` and a common ratio :math:`r > 1`. It contains a symmetric set of
:math:`N` mesh points around zero, formed by mirroring the positive geometric sequence :math:`\omega_{\mathrm{max}},
\omega_{\mathrm{max}} / r, \omega_{\mathrm{max}} / r^2, \ldots` (taken while the sequence stays above
:math:`\varepsilon`) to the negative axis. The mesh always has an even number of points and does not include zero.

A logarithmic real frequency mesh has the following properties:

- Each mesh point is identified by a unique index :math:`n \in \{0, 1, \ldots, N-1\}`.
- An index :math:`n` is mapped to the corresponding data index :math:`d` by the identity function :math:`d(n) = n`
  and vice versa.
- An index :math:`n` is mapped to the corresponding value :math:`\omega_n` by

  - :math:`\omega_n = -\omega_{\mathrm{max}} / r^n` for :math:`0 \leq n < N/2` and
  - :math:`\omega_n = \omega_{\mathrm{max}} / r^{N - 1 - n}` for :math:`N/2 \leq n \leq N - 1`,

  such that the mesh points are sorted in ascending order with :math:`\omega_0 = -\omega_{\mathrm{max}}` and
  :math:`\omega_{N-1} = \omega_{\mathrm{max}}`.
- An arbitrary value :math:`\omega \in [-\omega_{\mathrm{max}}, \omega_{\mathrm{max}}]` is mapped to the closest
  mesh point with index :math:`n` by binary search on the sorted mesh points.

Green's function containers that are based on a logarithmic real frequency mesh store the function values at the
discrete frequency points :math:`\omega_n`, i.e. :math:`f_n = f(\omega_n)`, and use linear interpolation to
evaluate the function at an arbitrary frequency :math:`\omega \in [-\omega_{\mathrm{max}}, \omega_{\mathrm{max}}]`.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_14>;
// --------- class _c2py_cls_15 -----------
using _c2py_cls_15                                            = triqs::mesh::refreq_pts;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_15>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_15> = "triqs.mesh.meshes.MeshReFreqPts";
static const auto _c2py_init_14 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_15>(), c2py::c_constructor<_c2py_cls_15, std::vector<double>>("pts")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_15> = c2py::pyfkw_constructor<_c2py_init_14>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_15> = _c2py_init_14.doc(R"DOC(
[1] Default constructor creates an empty mesh.

------

[2] Construct a real frequency mesh from a sorted vector of frequency points.

------

Parameters
----------
pts : {par_0}
   Sorted vector of frequency values.
)DOC",
                                                                      {{c2py::python_typename<std::vector<double>>()}});
// __call__
static auto const _c2py_fun_116 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_15 const &self, long n) -> decltype(auto) { return self.operator()(n); }, "self", "n")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_15> = c2py::pyfkw<_c2py_fun_116>;

// copy
static auto const _c2py_fun_117 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy<triqs::mesh::refreq_pts>, "m")};

// copy_from
static auto const _c2py_fun_118 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::copy_from<triqs::mesh::refreq_pts>, "m1", "m2")};

// is_index_valid
static auto const _c2py_fun_119 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_15 const &self, long n) -> decltype(auto) { return self.is_index_valid(n); }, "self", "n")};

// is_value_valid
static auto const _c2py_fun_120 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_15 const &self, double w) -> decltype(auto) { return self.is_value_valid(w); }, "self", "w")};

// to_data_index
static auto const _c2py_fun_121 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_15 const &self, long n) -> decltype(auto) { return self.to_data_index(n); }, "self", "n")};

// to_index
static auto const _c2py_fun_122 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_15 const &self, long d) -> decltype(auto) { return self.to_index(d); }, "self", "d")};

// to_value
static auto const _c2py_fun_123 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_15 const &self, long n) -> decltype(auto) { return self.to_value(n); }, "self", "n")};

// values
static auto const _c2py_fun_124 = c2py::dispatcher_f_kw_t{c2py::cmethod(&triqs::mesh::values<triqs::mesh::refreq_pts>, "m")};

static const auto _c2py_doc_116 =
   _c2py_fun_116.doc(R"DOC(
Function call operator to access a mesh point by its index.

Parameters
----------
n : {par_0}
   Index of the mesh point.

Returns
-------
{ret_0}
   mesh_point_t at the given index.
)DOC",
                     {{c2py::python_typename<long>()}}, {c2py::python_typename<triqs::mesh::refreq_pts::mesh_point_t>()});
static const auto _c2py_doc_117 =
   _c2py_fun_117.doc(R"DOC(
Get a copy of a mesh (for Python bindings).

Parameters
----------
m : {par_0}
   The mesh object to copy.

Returns
-------
{ret_0}
   Copy of the given mesh.
)DOC",
                     {{c2py::python_typename<const triqs::mesh::refreq_pts &>()}}, {c2py::python_typename<triqs::mesh::refreq_pts>()});
static const auto _c2py_doc_118 =
   _c2py_fun_118.doc(R"DOC(
Copy one mesh into another (for Python bindings).

Simply calls the copy assignment operator of the mesh.

Parameters
----------
m1 : {par_0}
   The mesh object to copy into.
m2 : {par_1}
   The mesh object to copy from.
)DOC",
                     {{c2py::python_typename<triqs::mesh::refreq_pts &>()}, {c2py::python_typename<const triqs::mesh::refreq_pts &>()}});
static const auto _c2py_doc_119 = _c2py_fun_119.doc(R"DOC(
Check if an index :math:`n` is valid.

Parameters
----------
n : {par_0}
   Index :math:`n` to check.

Returns
-------
{ret_0}
   True if :math:`0 \leq n < N`, false otherwise.
)DOC",
                                                    {{c2py::python_typename<long>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_120 = _c2py_fun_120.doc(R"DOC(
Check if a value :math:`\omega` is within the mesh range.

Parameters
----------
w : {par_0}
   Value to check.

Returns
-------
{ret_0}
   True if :math:`\omega_0 \leq \omega \leq \omega_{N-1}`, false otherwise.
)DOC",
                                                    {{c2py::python_typename<double>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_121 = _c2py_fun_121.doc(R"DOC(
Map an index :math:`n` to its corresponding data index :math:`d(n) = n`.

Parameters
----------
n : {par_0}
   Index :math:`n` to map.

Returns
-------
{ret_0}
   Data index :math:`d(n) = n`.
)DOC",
                                                    {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_122 = _c2py_fun_122.doc(R"DOC(
Map a data index :math:`d` to the corresponding index :math:`n(d) = d`.

Parameters
----------
d : {par_0}
   Data index :math:`d` to map.

Returns
-------
{ret_0}
   Index :math:`n(d) = d`.
)DOC",
                                                    {{c2py::python_typename<long>()}}, {c2py::python_typename<long>()});
static const auto _c2py_doc_123 = _c2py_fun_123.doc(R"DOC(
Map an index :math:`n` to its corresponding value :math:`\omega_n`.

Parameters
----------
n : {par_0}
   Index to map.

Returns
-------
{ret_0}
   Value of the mesh point :math:`\omega_n`.
)DOC",
                                                    {{c2py::python_typename<long>()}}, {c2py::python_typename<double>()});
static const auto _c2py_doc_124 = _c2py_fun_124.doc(
   R"DOC(
Get the values of all mesh points in a mesh.

Parameters
----------
m : {par_0}
   A mesh object.

Returns
-------
{ret_0}
   Array containing the values of all mesh points.
)DOC",
   {{c2py::python_typename<const triqs::mesh::refreq_pts &>()}},
   {c2py::python_typename<nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_15>[] = {
   {"copy", (PyCFunction)c2py::pyfkw<_c2py_fun_117>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_117.c_str()},
   {"copy_from", (PyCFunction)c2py::pyfkw<_c2py_fun_118>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_118.c_str()},
   {"is_index_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_119>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_119.c_str()},
   {"is_value_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_120>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_120.c_str()},
   {"to_data_index", (PyCFunction)c2py::pyfkw<_c2py_fun_121>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_121.c_str()},
   {"to_index", (PyCFunction)c2py::pyfkw<_c2py_fun_122>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_122.c_str()},
   {"to_value", (PyCFunction)c2py::pyfkw<_c2py_fun_123>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_123.c_str()},
   {"values", (PyCFunction)c2py::pyfkw<_c2py_fun_124>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_124.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_15>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_15>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_15>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_103 = R"DOC(Get the hash value of the mesh.)DOC";
static constexpr auto prop_doc_104 = R"DOC(Get the vector of frequency point values.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_15>[] = {

   {"mesh_hash", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::refreq_pts::mesh_hash)>, nullptr, prop_doc_103, nullptr},
   {"points", c2py::getter_from_method<c2py::castmc<>(&triqs::mesh::refreq_pts::points)>, nullptr, prop_doc_104, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

static PyObject *getitem_14(PyObject *self, PyObject *key) {
  static c2py::dispatcher_t<c2py::pycfun23> disp = {c2py::cfun2(c2py::getitem<_c2py_cls_15, long>)};
  return disp(self, key);
}

template <> PyMappingMethods c2py::tp_as_mapping<_c2py_cls_15> = {c2py::tpxx_size<_c2py_cls_15>, getitem_14, nullptr};

template <>
const std::string c2py::tp_doc<_c2py_cls_15> = R"DOC(Real frequency mesh type from arbitrary sorted frequency points.

A real frequency mesh is defined by its size :math:`N \geq 0` and a sorted vector of frequency values
:math:`\{\omega_0, \omega_1, \ldots, \omega_{N-1}\}`.

It has the following properties:

- Each mesh point is identified by a unique index :math:`n \in \{0, 1, \ldots, N-1\}`.
- An index :math:`n` is mapped to the corresponding data index :math:`d` by the identity function :math:`d(n) = n`
  and vice versa.
- An index :math:`n` is mapped to its corresponding value :math:`\omega_n`.
- An arbitrary value :math:`\omega \in [\omega_0, \omega_{N-1}]` is mapped to the closest mesh point using binary
  search.

Green's function containers that are based on this mesh store the function values at the discrete frequency points
:math:`\omega(n)`, i.e. :math:`f_n = f(\omega(n))`, and use linear interpolation to evaluate the function at an
arbitrary frequency :math:`\omega`.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_15>;

// ==================== module functions ====================

// evaluate
static auto const _c2py_fun_125 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::mesh::dlr2d &m,
                 nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    f_reg,
                 nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    f_sing,
                 int m_idx, int n_idx, int channel) { return triqs::mesh::evaluate(m, f_reg, f_sing, m_idx, n_idx, channel); },
              "m", "f_reg", "f_sing", "m_idx", "n_idx", "channel")};

// make_adjoint_mesh
static auto const _c2py_fun_126 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::mesh::imtime &m, long n_iw) { return triqs::mesh::make_adjoint_mesh(m, n_iw); }, "m", "n_iw"_a = -1),
   c2py::cfun([](const triqs::mesh::imfreq &m, long n_tau) { return triqs::mesh::make_adjoint_mesh(m, n_tau); }, "m", "n_tau"_a = -1),
   c2py::cfun([](const triqs::mesh::dlr_imtime &m) { return triqs::mesh::make_adjoint_mesh(m); }, "m"),
   c2py::cfun([](const triqs::mesh::dlr_imfreq &m) { return triqs::mesh::make_adjoint_mesh(m); }, "m"),
   c2py::cfun([](const triqs::mesh::retime &m, bool shift_half_bin) { return triqs::mesh::make_adjoint_mesh(m, shift_half_bin); }, "m",
              "shift_half_bin"_a = false),
   c2py::cfun([](const triqs::mesh::refreq &m, bool shift_half_bin) { return triqs::mesh::make_adjoint_mesh(m, shift_half_bin); }, "m",
              "shift_half_bin"_a = false),
   c2py::cfun([](const triqs::mesh::cyclat &m) { return triqs::mesh::make_adjoint_mesh(m); }, "m"),
   c2py::cfun([](const triqs::mesh::brzone &m) { return triqs::mesh::make_adjoint_mesh(m); }, "m")};

static const auto _c2py_doc_125 =
   _c2py_fun_125.doc(R"DOC(
Evaluate the 2D DLR approximation of a function :math:`f` at a given pair of Matsubara frequency indices.

We calculate the 2D DLR approximation using the cppdlr2d library.

Parameters
----------
m : {par_0}
   triqs::mesh::dlr2d mesh.
f_reg : {par_1}
   Regular part of the DLR coefficients (3D array of shape [3, r, r]).
f_sing : {par_2}
   Singular part of the DLR coefficients (1D array of size r).
m_idx : {par_3}
   First Matsubara frequency index.
n_idx : {par_4}
   Second Matsubara frequency index.
channel : {par_5}
   Channel index: 1 for particle-particle, 2 for particle-hole.

Returns
-------
{ret_0}
   2D DLR approximation of :math:`f(i\omega_m, i\omega_n)`.
)DOC",
                     {{c2py::python_typename<const triqs::mesh::dlr2d &>()},
                      {c2py::python_typename<nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                                   nda::borrowed<nda::mem::AddressSpace::Host>>>()},
                      {c2py::python_typename<nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                                   nda::borrowed<nda::mem::AddressSpace::Host>>>()},
                      {c2py::python_typename<int>()},
                      {c2py::python_typename<int>()},
                      {c2py::python_typename<int>()}},
                     {c2py::python_typename<std::complex<double>>()});
static const auto _c2py_doc_126 = _c2py_fun_126.doc(
   R"DOC(
[1] Create the adjoint imaginary-frequency mesh to a given imaginary-time mesh.

If :math:`N_{i\omega_n} = -1`, the number of positive Matsubara frequencies is set to :math:`N_{i\omega_n} =
N / 6`, where :math:`N` is the size of the given imaginary time mesh.

------

[2] Create the adjoint imaginary-time mesh to a given imaginary-frequency mesh.

If :math:`N = -1`, the size of the imaginary time mesh is set to :math:`N = 6 (n_{\text{max}} + 1) + 1`,
where :math:`n_{\text{max}}` is the largest positive Matsubara index in the given imaginary frequency mesh.

------

[3] Create the adjoint imaginary-frequency DLR mesh to a given imaginary-time DLR mesh.

It constructs the imaginary-frequency DLR mesh from the given imaginary-time DLR mesh.

------

[4] Create the adjoint imaginary-time DLR mesh to a given imaginary-frequency DLR mesh.

It constructs the imaginary-time DLR mesh from the given imaginary-frequency DLR mesh.

------

[5] Create the adjoint real-frequency mesh to a given real-time mesh.

The resulting frequency mesh is defined on the interval :math:`[\omega_{\text{min}}, \omega_{\text{max}}]`
with :math:`\omega_{\text{max}} = \pi (N - 1) / (N \Delta)` and :math:`\omega_{\text{min}} = -\omega_{\text{max}}`
, where :math:`N` and :math:`\Delta` are the size and step size of the given real time mesh, respectively.

If `shift_half_bin` is true, the frequency mesh is shifted by half a bin to the right, i.e. by :math:`\pi / (N \Delta)`
.

------

[6] Create the adjoint real-time mesh to a given real-frequency mesh.

The resulting time mesh is defined on the interval :math:`[t_{\text{min}}, t_{\text{max}}]` with
:math:`t_{\text{max}} = \pi (N - 1) / (N \Delta)` and :math:`t_{\text{min}} = -t_{\text{max}}`, where :math:`N` and
:math:`\Delta` are the size and step size of the given real frequency mesh, respectively.

If `shift_half_bin` is true, the time mesh is shifted by half a bin to the right, i.e. by :math:`\pi / (N \Delta)`.

------

[7] Create the adjoint Brillouin-zone mesh to a given cyclic-lattice mesh.

------

[8] Create the adjoint cyclic-lattice mesh to a given Brillouin-zone mesh.

------

Parameters
----------
m : {par_0}
   Input mesh.
n_iw : {par_1}
   Number of positive Matsubara frequencies, i.e. :math:`N_{i\omega_n}`.
n_tau : {par_2}
   Size of the imaginary time mesh.
shift_half_bin : {par_3}
   If true, shift the frequency mesh by half a bin to the right.

Returns
-------
[1] : {ret_0}
   Imaginary frequency mesh with the same :math:`\beta` and particle statistics as the given imaginary time
   mesh and :math:`N_{i\omega_n}` positive Matsubara frequencies.

[2] : {ret_1}
   Imaginary time mesh with the same :math:`\beta` and particle statistics as the given imaginary frequency
   mesh and size :math:`N`.

[3] : {ret_2}
   Imaginary frequency DLR mesh.

[4] : {ret_3}
   Imaginary time DLR mesh.

[5] : {ret_4}
   Real frequency mesh on the interval :math:`[\omega_{\text{min}}, \omega_{\text{max}}]` with :math:`N`
   equally spaced mesh points.

[6] : {ret_5}
   Real time mesh on the interval :math:`[t_{\text{min}}, t_{\text{max}}]` with :math:`N` equally spaced mesh
   points.

[7] : {ret_6}
   Brillouin zone mesh compatible with the given cyclic lattice mesh and its periodic boundary conditions.

[8] : {ret_7}
   Cyclic lattice mesh compatible with the given BZ mesh and its periodic boundary conditions.
)DOC",
   {{c2py::python_typename<const triqs::mesh::imtime &>(), c2py::python_typename<const triqs::mesh::imfreq &>(),
     c2py::python_typename<const triqs::mesh::dlr_imtime &>(), c2py::python_typename<const triqs::mesh::dlr_imfreq &>(),
     c2py::python_typename<const triqs::mesh::retime &>(), c2py::python_typename<const triqs::mesh::refreq &>(),
     c2py::python_typename<const triqs::mesh::cyclat &>(), c2py::python_typename<const triqs::mesh::brzone &>()},
    {c2py::python_typename<long>()},
    {c2py::python_typename<long>()},
    {c2py::python_typename<bool>()}},
   {c2py::python_typename<triqs::mesh::imfreq>(), c2py::python_typename<triqs::mesh::imtime>(), c2py::python_typename<triqs::mesh::dlr_imfreq>(),
    c2py::python_typename<triqs::mesh::dlr_imtime>(), c2py::python_typename<triqs::mesh::refreq>(), c2py::python_typename<triqs::mesh::retime>(),
    c2py::python_typename<triqs::mesh::brzone>(), c2py::python_typename<triqs::mesh::cyclat>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"evaluate", (PyCFunction)c2py::pyfkw<_c2py_fun_125>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_125.c_str()},
   {"make_adjoint_mesh", (PyCFunction)c2py::pyfkw<_c2py_fun_126>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_126.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "meshes", /* name of module */
                                        R"RAWDOC(Different mesh types provided by the **TRIQS** library.

Each mesh class discretises a one-dimensional (or, for :class:`MeshBrZone` and :class:`MeshCycLat`, multi-dimensional)
domain and provides the index ↔ data-index ↔ value mappings used by the TRIQS Green's function containers.

The mesh classes may be categorised as follows:

- Function space meshes: :class:`MeshDLR`, :class:`MeshLegendre`.
- Imaginary time and frequency meshes: :class:`MeshImTime`, :class:`MeshImFreq`, :class:`MeshDLRImTime`,
  :class:`MeshDLRImFreq`, :class:`MeshChebyshev`.
- Real time and frequency meshes: :class:`MeshReTime`, :class:`MeshReFreq`, :class:`MeshReFreqLog`,
  :class:`MeshReFreqPts`.
- Lattice meshes: :class:`MeshBrZone`, :class:`MeshCycLat`.

These classes are wrapped from their C++ counterparts in ``triqs::mesh`` and are re-exported at the package level under
:mod:`triqs.mesh`.
)RAWDOC",                                         /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_meshes() {

  if (not c2py::check_python_version("meshes")) return NULL;

  // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_0>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_1>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_2>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_3>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_4>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_5>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_6>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_7>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_8>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_9>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_10>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_11>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_12>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_13>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_14>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_15>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
#define _add_type(T, N) c2py::add_type_object_to_main<T>(N, m, conv_table)
  _add_type(_c2py_cls_0, "MeshImTime");
  _add_type(_c2py_cls_1, "EnergyT");
  _add_type(_c2py_cls_2, "MeshImFreq");
  _add_type(_c2py_cls_3, "MeshDLR");
  _add_type(_c2py_cls_4, "MeshDLRImTime");
  _add_type(_c2py_cls_5, "MeshDLRImFreq");
  _add_type(_c2py_cls_6, "MeshReTime");
  _add_type(_c2py_cls_7, "MeshReFreq");
  _add_type(_c2py_cls_8, "MeshCycLat");
  _add_type(_c2py_cls_9, "MeshBrZone");
  _add_type(_c2py_cls_10, "MeshChebyshev");
  _add_type(_c2py_cls_11, "MeshDLR2D");
  _add_type(_c2py_cls_12, "MeshDLR2DImFreq");
  _add_type(_c2py_cls_13, "MeshLegendre");
  _add_type(_c2py_cls_14, "MeshReFreqLog");
  _add_type(_c2py_cls_15, "MeshReFreqPts");
#undef _add_type

  c2py::pyref module = c2py::pyref::module("h5.formats");
  if (not module) return nullptr;
  c2py::pyref register_class = module.attr("register_class");

  register_h5_type<_c2py_cls_0>(register_class);
  register_h5_type<_c2py_cls_2>(register_class);
  register_h5_type<_c2py_cls_3>(register_class);
  register_h5_type<_c2py_cls_4>(register_class);
  register_h5_type<_c2py_cls_5>(register_class);
  register_h5_type<_c2py_cls_6>(register_class);
  register_h5_type<_c2py_cls_7>(register_class);
  register_h5_type<_c2py_cls_8>(register_class);
  register_h5_type<_c2py_cls_9>(register_class);
  register_h5_type<_c2py_cls_10>(register_class);
  register_h5_type<_c2py_cls_11>(register_class);
  register_h5_type<_c2py_cls_12>(register_class);
  register_h5_type<_c2py_cls_13>(register_class);
  register_h5_type<_c2py_cls_14>(register_class);
  register_h5_type<_c2py_cls_15>(register_class);

  return m;
}
#endif
// CLAIR_WRAP_GEN
