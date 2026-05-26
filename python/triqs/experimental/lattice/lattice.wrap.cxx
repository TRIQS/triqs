
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
using _c2py_cls_0                                            = triqs::experimental::lattice::adaptive_options;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs.experimental.lattice.lattice.AdaptiveOptions";

static int synth_constructor_0(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(
       PyExc_RuntimeError,
       ("Error in constructing triqs::experimental::lattice::adaptive_options.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_0> *)self)->_c = new _c2py_cls_0{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::experimental::lattice::adaptive_options from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_0> *)self)->_c);
  de("tolerance", self_c.tolerance, true);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_0> = synth_constructor_0;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_0> = c2py::replace_tags(R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
tolerance : {par_0}, default=1.e-3

)DOC",
                                                                      "par", {c2py::python_typename<double>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_0 = R"DOC()DOC";
static PyObject *prop_get_dict_0(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_0> *)self)->_c);
  c2py::pydict dic;
  dic["tolerance"] = self_c.tolerance;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_0>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_0::tolerance, _c2py_cls_0>("tolerance", _c2py_doc_member_0),
   {"__dict__", (getter)prop_get_dict_0, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> const std::string c2py::tp_doc<_c2py_cls_0> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_0>;
// --------- class _c2py_cls_1 -----------
using _c2py_cls_1                                            = triqs::experimental::lattice::bz_int_options;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_1>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_1> = "triqs.experimental.lattice.lattice.BzIntOptions";

static int synth_constructor_1(PyObject *self, PyObject *args, PyObject *kwargs) {
  if (args and PyTuple_Check(args) and (PyTuple_Size(args) > 0)) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::experimental::lattice::bz_int_options.\nNo positional arguments allowed. Use keywords arguments"));
    return -1;
  }
  c2py::pydict_extractor de{kwargs};
  try {
    ((c2py::wrap<_c2py_cls_1> *)self)->_c = new _c2py_cls_1{};
  } catch (std::exception const &e) {
    PyErr_SetString(PyExc_RuntimeError,
                    ("Error in constructing triqs::experimental::lattice::bz_int_options from a Python dict.\n   "s + e.what()).c_str());
    return -1;
  }
  auto &self_c = *(((c2py::wrap<_c2py_cls_1> *)self)->_c);
  de("tolerance", self_c.tolerance, true);
  de("k_grid", self_c.k_grid, true);
  de("delta_k_grid", self_c.delta_k_grid, true);
  de("k_grid_max", self_c.k_grid_max, true);
  de("run_adaptive", self_c.run_adaptive, true);
  de("run_ptr", self_c.run_ptr, true);
  de("verbose", self_c.verbose, true);
  return de.check();
}

template <> constexpr initproc c2py::tp_init<_c2py_cls_1> = synth_constructor_1;

template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_1> = c2py::replace_tags(
   R"DOC(Synthesized constructor with the following keyword arguments:

Parameters
----------
tolerance : {par_0}, default=1.e-3

k_grid : {par_1}, default={10, 10, 10}

delta_k_grid : {par_2}, default={2, 2, 2}

k_grid_max : {par_3}, default={20, 20, 20}

run_adaptive : {par_4}, default=true

run_ptr : {par_5}, default=true

verbose : {par_6}, default=false

)DOC",
   "par",
   {c2py::python_typename<double>(), c2py::python_typename<std::array<long, 3>>(), c2py::python_typename<std::array<long, 3>>(),
    c2py::python_typename<std::array<long, 3>>(), c2py::python_typename<bool>(), c2py::python_typename<bool>(), c2py::python_typename<bool>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_1>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto _c2py_doc_member_1 = R"DOC()DOC";
constexpr auto _c2py_doc_member_2 = R"DOC(absolute tolerance of the integrated quantity)DOC";
constexpr auto _c2py_doc_member_3 = R"DOC(default PTR number of points)DOC";
constexpr auto _c2py_doc_member_4 = R"DOC(Increase step of k in the grid refinement)DOC";
constexpr auto _c2py_doc_member_5 = R"DOC(Max of kx, ky, kz)DOC";
constexpr auto _c2py_doc_member_6 = R"DOC(if false, does not run adaptive integration at the end)DOC";
constexpr auto _c2py_doc_member_7 = R"DOC(if false, does not run PTR integration, goes directly to adaptive)DOC";
static PyObject *prop_get_dict_1(PyObject *self, void *) {
  auto &self_c = *(((c2py::wrap<_c2py_cls_1> *)self)->_c);
  c2py::pydict dic;
  dic["tolerance"]    = self_c.tolerance;
  dic["k_grid"]       = self_c.k_grid;
  dic["delta_k_grid"] = self_c.delta_k_grid;
  dic["k_grid_max"]   = self_c.k_grid_max;
  dic["run_adaptive"] = self_c.run_adaptive;
  dic["run_ptr"]      = self_c.run_ptr;
  dic["verbose"]      = self_c.verbose;
  return dic.new_ref();
}

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_1>[] = {
   c2py::getsetdef_from_member<&_c2py_cls_1::tolerance, _c2py_cls_1>("tolerance", _c2py_doc_member_1),
   c2py::getsetdef_from_member<&_c2py_cls_1::k_grid, _c2py_cls_1>("k_grid", _c2py_doc_member_2),
   c2py::getsetdef_from_member<&_c2py_cls_1::delta_k_grid, _c2py_cls_1>("delta_k_grid", _c2py_doc_member_3),
   c2py::getsetdef_from_member<&_c2py_cls_1::k_grid_max, _c2py_cls_1>("k_grid_max", _c2py_doc_member_4),
   c2py::getsetdef_from_member<&_c2py_cls_1::run_adaptive, _c2py_cls_1>("run_adaptive", _c2py_doc_member_5),
   c2py::getsetdef_from_member<&_c2py_cls_1::run_ptr, _c2py_cls_1>("run_ptr", _c2py_doc_member_6),
   c2py::getsetdef_from_member<&_c2py_cls_1::verbose, _c2py_cls_1>("verbose", _c2py_doc_member_7),
   {"__dict__", (getter)prop_get_dict_1, nullptr, "", nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_1> = R"DOC(Options for the case of integrate_bz function, with both adaptive + ptr integration

The integration function we are running for Gloc currently makes an attempt to converge the integration at
each frequency using fixed k-grid integration with increasing grid density, starting from `k_grid`
and increasing in increments of `delta_k_grid` until either a given point is converged with PTR or we hit `k_grid_max`.
After that, the remaining unconverged frequency points are run with adaptive
integration until they reach a certain absolute tolerance.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_1>;
// --------- class _c2py_cls_2 -----------
using _c2py_cls_2                                            = triqs::experimental::lattice::superlattice;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_2>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_2> = "triqs.experimental.lattice.lattice.Superlattice";
static const auto _c2py_init_0                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_2, nda::basic_array<long, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>,
   nda::basic_array<long, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>("sl_units", "cluster_pts")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_2>    = c2py::pyfkw_constructor<_c2py_init_0>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_2> = _c2py_init_0.doc(
   R"DOC(
Constructor for a superlattice

Parameters
----------
sl_units : {par_0}
   Unit vectors of the superlattice of shape (2,2) or (3,3). sl_units[a, _] = unit vector a
cluster_pts : {par_1}
   Cluster defined as a set of points in original lattice coordinate.
)DOC",
   {{c2py::python_typename<nda::basic_array<long, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()},
    {c2py::python_typename<nda::basic_array<long, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()}});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_2>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0 = R"DOC(Dimension of the superlattice (2 or 3))DOC";
static constexpr auto prop_doc_1 = R"DOC(Number of cluster sites in the superlattice)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_2>[] = {

   {"dim", c2py::getter_from_method<c2py::castmc<>(&triqs::experimental::lattice::superlattice::dim)>, nullptr, prop_doc_0, nullptr},
   {"n_cluster_sites", c2py::getter_from_method<c2py::castmc<>(&triqs::experimental::lattice::superlattice::n_cluster_sites)>, nullptr, prop_doc_1,
    nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_2> = R"DOC(Representation of a superlattice for a lattice.

Defined by the unit vectors of the superlattice and a set of cluster points.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_2>;
// --------- class _c2py_cls_3 -----------
using _c2py_cls_3                                            = triqs::experimental::lattice::tb_hk;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_3>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_3> = "triqs.experimental.lattice.lattice.TbHk";
static const auto _c2py_init_1                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_3, std::vector<std::array<long, 3>>,
                       std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                                    nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>("Rs", "hoppings"),
   c2py::c_constructor<_c2py_cls_3>()};
template <> constexpr initproc c2py::tp_init<_c2py_cls_3> = c2py::pyfkw_constructor<_c2py_init_1>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_3> = _c2py_init_1.doc(R"DOC(
default constructor to zero hopping for single R vector at the origin
)DOC");
// __call__
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_3 const &self,
         nda::basic_array_view<const double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>> k_list)
         -> decltype(auto) { return self.operator()(k_list); },
      "self", "k_list"),
   c2py::cmethod([](_c2py_cls_3 const &self, std::array<double, 3> ks) -> decltype(auto) { return self.operator()(ks); }, "self", "ks")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_3> = c2py::pyfkw<_c2py_fun_0>;

// eigenvalues
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_3 const &self,
                    nda::basic_array_view<double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>> k)
                    -> decltype(auto) { return self.eigenvalues(k); },
                 "self", "k")};

// eigenvectors
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_3 const &self,
                    nda::basic_array_view<double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>> k)
                    -> decltype(auto) { return self.eigenvectors(k); },
                 "self", "k")};

// get_R_idx
static auto const _c2py_fun_3 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_3 const &self, std::array<long, 3> R) -> decltype(auto) { return self.get_R_idx(R); }, "self", "R")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC(
[1] Batch BLAS evaluation at a list of k-points of shape [nk, kdim]

------

[2] Evaluation at a single k-point

------
)DOC");
static const auto _c2py_doc_1 = _c2py_fun_1.doc(
   R"DOC(
Function to take a list of kpoints and return a matrix of of energies in the diagonalized (band) space.

$$ H_{k,mn} = U_{{k},ma}^ ( t({R}_j)_{ab} * exp(2  i * {k} * {R}_j) U_{{k},nb} $$

with lattice vectors {R_j} and associated overlap (hopping) matrices {t({R}_j)_{ab}}
k needs to be represented in units of the reciprocal lattice vectors (from 0 to 1)

Parameters
----------
k : {par_0}
   k-points as an array of [nk, 3] in units of the reciprocal lattice vectors

Returns
-------
{ret_0}
   energies as a matrix of shape [nk, nbands] of band-basis energies as real doubles
)DOC",
   {{c2py::python_typename<
      nda::basic_array_view<double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>>>()}},
   {c2py::python_typename<nda::basic_array<double, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});
static const auto _c2py_doc_2 = _c2py_fun_2.doc(
   R"DOC(
Function to take a list of kpoints and return a tuple containing the band energies and the eigenvectors of H(k)

$$ H_{k,mn} = U_{{k},ma}^ ( t({R}_j)_{ab} * exp(2  i * {k} * {R}_j) U_{{k},nb} $$

with lattice displacements {R_j} and associated overlap (hopping) matrices {t({R}_j)_{ab}}
k needs to be represented in units of the reciprocal lattice vectors (from 0 to 1)

Parameters
----------
k : {par_0}
   k-points as an array of [nk, 3] in units of the reciprocal lattice vectors

Returns
-------
{ret_0}
   tuple containing band-basis energies as a matrix of real doubles with shape [nk, nbands] and complex eigenvectors as a matrix of shape
   [nk,nBand,nOrbitals]
)DOC",
   {{c2py::python_typename<
      nda::basic_array_view<double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>>>()}},
   {c2py::python_typename<std::pair<
      nda::basic_array<double, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>,
      nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>()});
static const auto _c2py_doc_3 = _c2py_fun_3.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_3>[] = {
   {"eigenvalues", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"eigenvectors", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"get_R_idx", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_3>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_h5<_c2py_cls_3>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_h5<_c2py_cls_3>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_2 = R"DOC()DOC";
static constexpr auto prop_doc_3 =
   R"DOC(Explicitly return the std::vector of Rs; this avoids a problems where this is returned as a py_range, which is inconvenient at the python level.
        Ideally can be removed/refactored later pending improvements in clair.)DOC";
static constexpr auto prop_doc_4 = R"DOC(Lazy range of 2D views into the packed coefficient array, one per R-vector.)DOC";
static constexpr auto prop_doc_5 = R"DOC(Number of R-vectors)DOC";
static constexpr auto prop_doc_6 = R"DOC(Provide number of orbitals (the dimension of the stored Hamiltonian))DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_3>[] = {

   {"elements", c2py::getter_from_method<c2py::castmc<>(&triqs::experimental::lattice::tb_hk::elements)>, nullptr, prop_doc_2, nullptr},
   {"get_R_list", c2py::getter_from_method<c2py::castmc<>(&triqs::experimental::lattice::tb_hk::get_R_list)>, nullptr, prop_doc_3, nullptr},
   {"hoppings", c2py::getter_from_method<c2py::castmc<>(&triqs::experimental::lattice::tb_hk::hoppings)>, nullptr, prop_doc_4, nullptr},
   {"n_R",
    c2py::getter_from_method_B<triqs::experimental::lattice::tb_hk, c2py::castmc<>(&triqs::experimental::lattice::fourier_polynomial<2, 3>::n_R)>,
    nullptr, prop_doc_5, nullptr},
   {"n_orbitals", c2py::getter_from_method<c2py::castmc<>(&triqs::experimental::lattice::tb_hk::n_orbitals)>, nullptr, prop_doc_6, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> const std::string c2py::tp_doc<_c2py_cls_3> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_3>;

// ==================== module functions ====================

// find_chemical_potential
static auto const _c2py_fun_4 =
   c2py::dispatcher_f_kw_t{c2py::cfun(
                              [](const double target_density, const triqs::experimental::lattice::tb_hk &H_k,
                                 const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Sigma,
                                 const triqs::experimental::lattice::bz_int_options &opt, std::string method, double precision, bool verbosity) {
                                return triqs::experimental::lattice::find_chemical_potential<triqs::mesh::imfreq>(target_density, H_k, Sigma, opt,
                                                                                                                  method, precision, verbosity);
                              },
                              "target_density", "H_k", "Sigma", "opt", "method"_a = "dichotomy", "precision"_a = 1.e-5, "verbosity"_a = false),
                           c2py::cfun(
                              [](const double target_density, const triqs::experimental::lattice::tb_hk &H_k,
                                 const triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &Sigma,
                                 const triqs::experimental::lattice::bz_int_options &opt, std::string method, double precision, bool verbosity) {
                                return triqs::experimental::lattice::find_chemical_potential<triqs::mesh::refreq>(target_density, H_k, Sigma, opt,
                                                                                                                  method, precision, verbosity);
                              },
                              "target_density", "H_k", "Sigma", "opt", "method"_a = "dichotomy", "precision"_a = 1.e-5, "verbosity"_a = false),
                           c2py::cfun(
                              [](const double target_density, const triqs::experimental::lattice::tb_hk &H_k,
                                 const triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Sigma,
                                 const triqs::experimental::lattice::bz_int_options &opt, std::string method, double precision, bool verbosity) {
                                return triqs::experimental::lattice::find_chemical_potential<triqs::mesh::imfreq>(target_density, H_k, Sigma, opt,
                                                                                                                  method, precision, verbosity);
                              },
                              "target_density", "H_k", "Sigma", "opt", "method"_a = "dichotomy", "precision"_a = 1.e-5, "verbosity"_a = false),
                           c2py::cfun(
                              [](const double target_density, const triqs::experimental::lattice::tb_hk &H_k,
                                 const triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &Sigma,
                                 const triqs::experimental::lattice::bz_int_options &opt, std::string method, double precision, bool verbosity) {
                                return triqs::experimental::lattice::find_chemical_potential<triqs::mesh::refreq>(target_density, H_k, Sigma, opt,
                                                                                                                  method, precision, verbosity);
                              },
                              "target_density", "H_k", "Sigma", "opt", "method"_a = "dichotomy", "precision"_a = 1.e-5, "verbosity"_a = false)};

// fold
static auto const _c2py_fun_5 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](const triqs::experimental::lattice::superlattice &sl,
                                         const triqs::experimental::lattice::tb_hk &tb) { return triqs::experimental::lattice::fold(sl, tb); },
                                      "sl", "tb")};

// gloc
static auto const _c2py_fun_6 = c2py::dispatcher_f_kw_t{
   c2py::cfun(
      [](const triqs::mesh::dlr_imfreq &w_mesh, const triqs::experimental::lattice::tb_hk &H_k, double mu,
         const triqs::experimental::lattice::bz_int_options &opt) {
        return triqs::experimental::lattice::gloc<triqs::mesh::dlr_imfreq>(w_mesh, H_k, mu, opt);
      },
      "w_mesh", "H_k", "mu", "opt"),
   c2py::cfun(
      [](const triqs::mesh::imfreq &w_mesh, const triqs::experimental::lattice::tb_hk &H_k, double mu,
         const triqs::experimental::lattice::bz_int_options &opt) {
        return triqs::experimental::lattice::gloc<triqs::mesh::imfreq>(w_mesh, H_k, mu, opt);
      },
      "w_mesh", "H_k", "mu", "opt"),
   c2py::cfun(
      [](const triqs::mesh::refreq &w_mesh, const triqs::experimental::lattice::tb_hk &H_k, double mu,
         const triqs::experimental::lattice::bz_int_options &opt) {
        return triqs::experimental::lattice::gloc<triqs::mesh::refreq>(w_mesh, H_k, mu, opt);
      },
      "w_mesh", "H_k", "mu", "opt"),
   c2py::cfun(
      [](const triqs::experimental::lattice::tb_hk &H_k, double mu, const triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Sigma,
         const triqs::experimental::lattice::bz_int_options &opt) {
        return triqs::experimental::lattice::gloc<triqs::mesh::imfreq>(H_k, mu, Sigma, opt);
      },
      "H_k", "mu", "Sigma", "opt"),
   c2py::cfun(
      [](const triqs::experimental::lattice::tb_hk &H_k, double mu, const triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &Sigma,
         const triqs::experimental::lattice::bz_int_options &opt) {
        return triqs::experimental::lattice::gloc<triqs::mesh::refreq>(H_k, mu, Sigma, opt);
      },
      "H_k", "mu", "Sigma", "opt"),
   c2py::cfun(
      [](const triqs::experimental::lattice::tb_hk &H_k, double mu, const triqs::gfs::gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &Sigma,
         const triqs::experimental::lattice::bz_int_options &opt) {
        return triqs::experimental::lattice::gloc<triqs::mesh::dlr_imfreq>(H_k, mu, Sigma, opt);
      },
      "H_k", "mu", "Sigma", "opt"),
   c2py::cfun(
      [](const triqs::experimental::lattice::tb_hk &H_k, double mu,
         const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &Sigma,
         const triqs::experimental::lattice::bz_int_options &opt) {
        return triqs::experimental::lattice::gloc<triqs::mesh::dlr_imfreq>(H_k, mu, Sigma, opt);
      },
      "H_k", "mu", "Sigma", "opt"),
   c2py::cfun(
      [](const triqs::experimental::lattice::tb_hk &H_k, double mu, const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &Sigma,
         const triqs::experimental::lattice::bz_int_options &opt) {
        return triqs::experimental::lattice::gloc<triqs::mesh::imfreq>(H_k, mu, Sigma, opt);
      },
      "H_k", "mu", "Sigma", "opt"),
   c2py::cfun(
      [](const triqs::experimental::lattice::tb_hk &H_k, double mu, const triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &Sigma,
         const triqs::experimental::lattice::bz_int_options &opt) {
        return triqs::experimental::lattice::gloc<triqs::mesh::refreq>(H_k, mu, Sigma, opt);
      },
      "H_k", "mu", "Sigma", "opt")};

// make_tb_hk_from_w90_hr_file
static auto const _c2py_fun_7 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const std::string &w90_path_and_seedname) { return triqs::experimental::lattice::make_tb_hk_from_w90_hr_file(w90_path_and_seedname); },
   "w90_path_and_seedname")};

// make_tb_hk_from_w90_tb_file
static auto const _c2py_fun_8 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const std::string &w90_path_and_seedname) { return triqs::experimental::lattice::make_tb_hk_from_w90_tb_file(w90_path_and_seedname); },
   "w90_path_and_seedname")};

// read_wannier90_hr_data
static auto const _c2py_fun_9 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const std::string &w90_path_and_seedname) { return triqs::experimental::lattice::read_wannier90_hr_data(w90_path_and_seedname); },
              "w90_path_and_seedname")};

// read_wannier90_tb_data
static auto const _c2py_fun_10 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const std::string &w90_path_and_seedname) { return triqs::experimental::lattice::read_wannier90_tb_data(w90_path_and_seedname); },
              "w90_path_and_seedname")};

static const auto _c2py_doc_4 =
   _c2py_fun_4.doc(R"DOC(
Compute the density of the lattice Green's function with a self-energy for one spin channel

Parameters
----------
target_density : {par_0}
   total on-site density to determine mu
H_k : {par_1}
   A tight binding Hamiltonian
Sigma : {par_2}
   The self-energy
opt : {par_3}
   Container for options related integration of the BZ
method : {par_4}
   Root finding method used to calculate mu
precision : {par_5}
   Precision of mu value calculated
verbosity : {par_6}
   Printing verbosity level for mu calculation

Returns
-------
{ret_0}
   Electron density of the lattice Green's function for a single spin channel
)DOC",
                   {{c2py::python_typename<const double>()},
                    {c2py::python_typename<const triqs::experimental::lattice::tb_hk &>()},
                    {c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &>(),
                     c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &>(),
                     c2py::python_typename<const triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &>(),
                     c2py::python_typename<const triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &>()},
                    {c2py::python_typename<const triqs::experimental::lattice::bz_int_options &>()},
                    {c2py::python_typename<std::string>()},
                    {c2py::python_typename<double>()},
                    {c2py::python_typename<bool>()}},
                   {c2py::python_typename<double>()});
static const auto _c2py_doc_5 = _c2py_fun_5.doc(R"DOC(
Fold the tight-binding Hamiltonian into a superlattice
)DOC");
static const auto _c2py_doc_6 =
   _c2py_fun_6.doc(R"DOC(
[1, 2, 3] Compute the non-interacting local Green's function on a given mesh from a tight binding Hamiltonian

------

[4, 5, 6] Compute local Green's function on a given mesh from a tight binding Hamiltonian

------

[7, 8, 9] Compute local Green's function on a given mesh as a block GF, from a tight binding Hamiltonian

------

Parameters
----------
w_mesh : {par_0}
   A frequency mesh, where integration will be performed for each frequency.
H_k : {par_1}
   A tight binding Hamiltonian
mu : {par_2}
   Chemical potential
opt : {par_3}
   Container for options related integration of the BZ
Sigma : {par_4}
   The self-energy

Returns
-------
[1, 6] : {ret_0}
   gloc, the local Green's function on frequency mesh of the provided self-energy

[2, 4] : {ret_1}
   gloc, the local Green's function on frequency mesh of the provided self-energy

[3, 5] : {ret_2}
   gloc, the local Green's function on frequency mesh of the provided self-energy

[7] : {ret_3}
   gloc, the local Green's function on frequency mesh of the provided self-energy

[8] : {ret_4}
   gloc, the local Green's function on frequency mesh of the provided self-energy

[9] : {ret_5}
   gloc, the local Green's function on frequency mesh of the provided self-energy
)DOC",
                   {{c2py::python_typename<const triqs::mesh::dlr_imfreq &>(), c2py::python_typename<const triqs::mesh::imfreq &>(),
                     c2py::python_typename<const triqs::mesh::refreq &>()},
                    {c2py::python_typename<const triqs::experimental::lattice::tb_hk &>()},
                    {c2py::python_typename<double>()},
                    {c2py::python_typename<const triqs::experimental::lattice::bz_int_options &>()},
                    {c2py::python_typename<const triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &>(),
                     c2py::python_typename<const triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &>(),
                     c2py::python_typename<const triqs::gfs::gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &>(),
                     c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &>(),
                     c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &>(),
                     c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &>()}},
                   {c2py::python_typename<triqs::gfs::gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>>(),
                    c2py::python_typename<triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>>(),
                    c2py::python_typename<triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>>(),
                    c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>>(),
                    c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>>(),
                    c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>>()});
static const auto _c2py_doc_7 =
   _c2py_fun_7.doc(R"DOC(
Reads in Wannier90 *_hr.dat file and uses the contents to create a tb_hk object.

Parameters
----------
w90_path_and_seedname : {par_0}
   String containing the path to the Wannier90 files, appended with the "seedname" as it is called in Wannier90;
       directory should include path/seedname_tb.dat, where "path/seedname" is the provided string.

Returns
-------
{ret_0}
   tb_hk object, where units of energy are in eV.
)DOC",
                   {{c2py::python_typename<const std::string &>()}}, {c2py::python_typename<triqs::experimental::lattice::tb_hk>()});
static const auto _c2py_doc_8 =
   _c2py_fun_8.doc(R"DOC(
Reads in Wannier90 *_tb.dat file and uses the contents to create a tb_hk object.

Parameters
----------
w90_path_and_seedname : {par_0}
   String containing the path to the Wannier90 files, appended with the "seedname" as it is called in Wannier90;
       directory should include path/seedname_tb.dat, where "path/seedname" is the provided string.

Returns
-------
{ret_0}
   tb_hk object, where units of energy are in eV.
)DOC",
                   {{c2py::python_typename<const std::string &>()}}, {c2py::python_typename<triqs::experimental::lattice::tb_hk>()});
static const auto _c2py_doc_9 = _c2py_fun_9.doc(
   R"DOC(
Reads in Wannier90 *_hr.dat file and produces the R vectors in crystal coordinates and tight binding parameters

Parameters
----------
w90_path_and_seedname : {par_0}
   String containing the path to the Wannier90 files, appended with the "seedname" as it is called in Wannier90;
       directory should include path/seedname_hr.dat, where "path/seedname" is the provided string.

Returns
-------
{ret_0}
   tuple containing R[nR, 3], HR[nR, nOrb, nOrb]
      R is in crystal/lattice coordinates, HR is in units of eV
)DOC",
   {{c2py::python_typename<const std::string &>()}},
   {c2py::python_typename<std::tuple<std::vector<std::array<long, 3>>,
                                     std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                                                  nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>>()});
static const auto _c2py_doc_10 = _c2py_fun_10.doc(
   R"DOC(
Reads in Wannier90 *_tb.dat file and produces the R vectors in crystal coordinates,
tight binding parameters, position matrix elements in the real-space Wannier basis, and lattice vectors

Parameters
----------
w90_path_and_seedname : {par_0}
   String containing the path to the Wannier90 files, appended with the "seedname" as it is called in Wannier90;
       directory should include path/seedname_tb.dat, where "path/seedname" is the provided string.

Returns
-------
{ret_0}
   tuple containing HR[nR, nOrb, nOrb], R[nR, 3], r_position_op[nR, nOrb, nOrb, 3], lattice_vectors[3,3]
      HR is in units of eV, R is in crystal/lattice coordinates, position_op is in Angstrom, lattice_vectors in angstrom
)DOC",
   {{c2py::python_typename<const std::string &>()}},
   {c2py::python_typename<std::tuple<
      std::vector<std::array<long, 3>>,
      std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>,
      std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>,
      nda::basic_array<double, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"find_chemical_potential", (PyCFunction)c2py::pyfkw<_c2py_fun_4>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_4.c_str()},
   {"fold", (PyCFunction)c2py::pyfkw<_c2py_fun_5>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_5.c_str()},
   {"gloc", (PyCFunction)c2py::pyfkw<_c2py_fun_6>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_6.c_str()},
   {"make_tb_hk_from_w90_hr_file", (PyCFunction)c2py::pyfkw<_c2py_fun_7>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_7.c_str()},
   {"make_tb_hk_from_w90_tb_file", (PyCFunction)c2py::pyfkw<_c2py_fun_8>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_8.c_str()},
   {"read_wannier90_hr_data", (PyCFunction)c2py::pyfkw<_c2py_fun_9>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_9.c_str()},
   {"read_wannier90_tb_data", (PyCFunction)c2py::pyfkw<_c2py_fun_10>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_10.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {
   PyModuleDef_HEAD_INIT,
   "lattice",                                                                                          /* name of module */
   R"RAWDOC(Experimental lattice tools (tight binding, BZ integration, gloc, Wannier loaders))RAWDOC", /* module documentation, may be NULL */
   -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
   module_methods,
   NULL,
   NULL,
   NULL,
   NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_lattice() {

  if (not c2py::check_python_version("lattice")) return NULL;

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

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
#define _add_type(T, N) c2py::add_type_object_to_main<T>(N, m, conv_table)
  _add_type(_c2py_cls_0, "AdaptiveOptions");
  _add_type(_c2py_cls_1, "BzIntOptions");
  _add_type(_c2py_cls_2, "Superlattice");
  _add_type(_c2py_cls_3, "TbHk");
#undef _add_type

  c2py::pyref module = c2py::pyref::module("h5.formats");
  if (not module) return nullptr;
  c2py::pyref register_class = module.attr("register_class");

  register_h5_type<_c2py_cls_3>(register_class);

  return m;
}
#endif
// CLAIR_WRAP_GEN
