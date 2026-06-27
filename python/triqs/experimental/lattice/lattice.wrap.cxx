
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

constexpr auto _c2py_doc_member_0 = R"DOC(Target absolute error of the adaptive integration.)DOC";
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

template <>
const std::string c2py::tp_doc<_c2py_cls_0> = R"DOC(Options controlling the adaptive Brillouin-zone integration.

This struct is intended to be extended as improved adaptive integrators are added.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_0>;
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

constexpr auto _c2py_doc_member_1 = R"DOC(Absolute tolerance of the integrated quantity.)DOC";
constexpr auto _c2py_doc_member_2 = R"DOC(Initial number of PTR k-points along each direction.)DOC";
constexpr auto _c2py_doc_member_3 = R"DOC(Increment of the k-grid size at each refinement step.)DOC";
constexpr auto _c2py_doc_member_4 = R"DOC(Maximum number of k-points along each direction.)DOC";
constexpr auto _c2py_doc_member_5 = R"DOC(Whether to run adaptive integration on the remaining points.)DOC";
constexpr auto _c2py_doc_member_6 = R"DOC(Whether to run PTR integration before the adaptive step.)DOC";
constexpr auto _c2py_doc_member_7 = R"DOC(Whether to print the convergence progress.)DOC";
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
const std::string c2py::tp_doc<_c2py_cls_1> = R"DOC(Options controlling the combined PTR and adaptive Brillouin-zone integration.

The integration first attempts to converge each frequency with fixed k-grid (PTR) integration of
increasing grid density, starting from `k_grid` and increasing in steps of `delta_k_grid` until a point converges
or the grid reaches `k_grid_max`. The remaining unconverged frequency points are then integrated adaptively until
they reach the requested absolute tolerance.)DOC"
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
Construct a superlattice from its unit vectors and cluster points.

Parameters
----------
sl_units : {par_0}
   Unit vectors of the superlattice of shape `(2, 2)` or `(3, 3)`, where `sl_units[a, :]` is the
   :math:`a`-th unit vector in the coordinates of the underlying lattice.
cluster_pts : {par_1}
   Cluster points, i.e. the underlying-lattice sites of one superlattice unit cell, given in
   lattice coordinates.
)DOC",
   {{c2py::python_typename<nda::basic_array<long, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()},
    {c2py::python_typename<nda::basic_array<long, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()}});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_2>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0 = R"DOC(Get the dimension of the superlattice.)DOC";
static constexpr auto prop_doc_1 = R"DOC(Get the number of cluster sites in one superlattice unit cell.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_2>[] = {

   {"dim", c2py::getter_from_method<c2py::castmc<>(&triqs::experimental::lattice::superlattice::dim)>, nullptr, prop_doc_0, nullptr},
   {"n_cluster_sites", c2py::getter_from_method<c2py::castmc<>(&triqs::experimental::lattice::superlattice::n_cluster_sites)>, nullptr, prop_doc_1,
    nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_2> = R"DOC(Representation of a superlattice built on top of an underlying Bravais lattice.

A superlattice is defined by its unit vectors, expressed in the coordinates of the underlying lattice, and
a set of cluster points that enumerate the underlying-lattice sites contained in one superlattice unit cell. It
stores the transformation matrix from lattice to superlattice coordinates and is used to fold lattice functions
onto the superlattice.)DOC"
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
const std::string c2py::tp_ctor_doc<_c2py_cls_3> = _c2py_init_1.doc(
   R"DOC(
[1] Construct a tight-binding Hamiltonian from lattice vectors and their hopping matrices.

------

[2] Default constructor: a single zero hopping matrix for the lattice vector at the origin.

------

Parameters
----------
Rs : {par_0}
   List of lattice vectors :math:`\mathbf{R}`.
hoppings : {par_1}
   List of hopping (overlap) matrices :math:`t(\mathbf{R})`, one per lattice vector.
)DOC",
   {{c2py::python_typename<std::vector<std::array<long, 3>>>()},
    {c2py::python_typename<std::vector<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>()}});
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

static const auto _c2py_doc_0 = _c2py_fun_0.doc(
   R"DOC(
[1] Evaluate the Fourier polynomial at a batch of k-points given as a matrix.

------

[2] Evaluate the Fourier polynomial at a single k-point.

------

Parameters
----------
k_list : {par_0}
   Matrix of k-points of shape `[nk, kdim]`.
ks : {par_1}
   The k-point at which to evaluate.

Returns
-------
[1] : {ret_0}
   Array of shape `[nk, coeff_shape...]` holding the value at each k-point.

[2] : {ret_1}
   Value of the Fourier polynomial at `ks`.
)DOC",
   {{c2py::python_typename<
       nda::basic_array_view<const double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>>>()},
    {c2py::python_typename<std::array<double, 3>>()}},
   {c2py::python_typename<
       nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});
static const auto _c2py_doc_1 = _c2py_fun_1.doc(
   R"DOC(
Compute the band-basis energies of :math:`H(\mathbf{k})` for a list of k-points.

For each k-point the Hamiltonian is diagonalized and the resulting energies are returned in the band
basis:

.. math::

   
   H_{k,mn} = U_{\mathbf{k},ma}^\dagger \left( \sum_j t(\mathbf{R}_j)_{ab} e^{2 \pi i \mathbf{k} \cdot
   \mathbf{R}_j} \right) U_{\mathbf{k},nb} \; ,

with lattice vectors :math:`\{\mathbf{R}_j\}` and associated overlap (hopping) matrices 
:math:`\{t(\mathbf{R}_j)_{ab}\}`.

The k-points must be given in units of the reciprocal lattice vectors (from 0 to 1).

Parameters
----------
k : {par_0}
   k-points as an array of shape [nk, 3] in units of the reciprocal lattice vectors.

Returns
-------
{ret_0}
   Band-basis energies as a real matrix of shape [nk, nbands].
)DOC",
   {{c2py::python_typename<
      nda::basic_array_view<double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>>>()}},
   {c2py::python_typename<nda::basic_array<double, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>()});
static const auto _c2py_doc_2 = _c2py_fun_2.doc(
   R"DOC(
Compute the band-basis energies and eigenvectors of :math:`H(\mathbf{k})` for a list of k-points.

For each k-point the Hamiltonian is diagonalized and both the band energies and the eigenvectors are
returned:

.. math::

   
   H_{k,mn} = U_{\mathbf{k},ma}^\dagger \left( \sum_j t(\mathbf{R}_j)_{ab} e^{2 \pi i \mathbf{k} \cdot
   \mathbf{R}_j} \right) U_{\mathbf{k},nb} \; ,

with lattice displacements :math:`\{\mathbf{R}_j\}` and associated overlap (hopping) matrices 
:math:`\{t(\mathbf{R}_j)_{ab}\}`.

The k-points must be given in units of the reciprocal lattice vectors (from 0 to 1).

Parameters
----------
k : {par_0}
   k-points as an array of shape [nk, 3] in units of the reciprocal lattice vectors.

Returns
-------
{ret_0}
   Tuple containing the band-basis energies as a real matrix of shape [nk, nbands] and the complex
   eigenvectors as an array of shape [nk, nbands, nOrbitals].
)DOC",
   {{c2py::python_typename<
      nda::basic_array_view<double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>>>()}},
   {c2py::python_typename<triqs::experimental::lattice::tb_hk::eigenvectors_t>()});
static const auto _c2py_doc_3 = _c2py_fun_3.doc(R"DOC(
Get the storage index of a given R-vector.

Throws a `TRIQS_RUNTIME_ERROR` if the R-vector is not present.

Parameters
----------
R : {par_0}
   Lattice vector :math:`\mathbf{R}`.

Returns
-------
{ret_0}
   Storage index of `R` in the list of R-vectors.
)DOC",
                                                {{c2py::python_typename<std::array<long, 3>>()}}, {c2py::python_typename<long>()});

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

static constexpr auto prop_doc_2 = R"DOC(Get a lazy range over the (R-vector, hopping-matrix) pairs (const overload).)DOC";
static constexpr auto prop_doc_3 = R"DOC(Get the list of real-space lattice vectors.)DOC";
static constexpr auto prop_doc_4 = R"DOC(Get a lazy range over the hopping matrices, one per R-vector (const overload).)DOC";
static constexpr auto prop_doc_5 = R"DOC(Get the number of R-vectors.)DOC";
static constexpr auto prop_doc_6 = R"DOC(Get the number of orbitals, i.e. the dimension of the Hamiltonian matrices.)DOC";

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

template <>
const std::string c2py::tp_doc<_c2py_cls_3> = R"DOC(Tight-binding Hamiltonian :math:`H(\mathbf{k})` on a 3D lattice.

A tight-binding Hamiltonian is defined by a set of lattice vectors :math:`\mathbf{R}` and the associated
hopping (overlap) matrices :math:`t(\mathbf{R})` between orbitals. It represents

.. math::

   H(\mathbf{k}) = \sum_\mathbf{R} t(\mathbf{R}) \, e^{2 \pi i \, \mathbf{k} \cdot \mathbf{R}} \; ,

and provides access to the hopping matrices, band energies and eigenvectors at a list of k-points, comparison,
stream output and HDF5 serialization.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_3>;

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
[1, 2] Find the chemical potential that yields a target density for a block self-energy.

This function adjusts the chemical potential :math:`\mu` with a root finder until the total density of the
local block Green's function (summed over all blocks) matches the target density.

------

[3, 4] Find the chemical potential that yields a target density for a single-block self-energy.

This function adjusts the chemical potential :math:`\mu` with a root finder until the density of the local
Green's function matches the target density.

------

Parameters
----------
target_density : {par_0}
   Target total on-site density used to determine :math:`\mu`.
H_k : {par_1}
   Tight-binding Hamiltonian :math:`H(\mathbf{k})`.
Sigma : {par_2}
   Block self-energy :math:`\Sigma(\omega)`.
opt : {par_3}
   Options controlling the Brillouin-zone integration.
method : {par_4}
   Root-finding method used to determine :math:`\mu`, either `"dichotomy"` or `"bisection"`.
precision : {par_5}
   Absolute precision at which :math:`\mu` is determined.
verbosity : {par_6}
   Whether to print the convergence progress.

Returns
-------
{ret_0}
   Chemical potential :math:`\mu` reproducing the target density.
)DOC",
                   {{c2py::python_typename<const double>()},
                    {c2py::python_typename<const triqs::experimental::lattice::tb_hk &>()},
                    {c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &>(),
                     c2py::python_typename<const triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &>()},
                    {c2py::python_typename<const triqs::experimental::lattice::bz_int_options &>()},
                    {c2py::python_typename<std::string>()},
                    {c2py::python_typename<double>()},
                    {c2py::python_typename<bool>()}},
                   {c2py::python_typename<double>()});
static const auto _c2py_doc_5 = _c2py_fun_5.doc(R"DOC(
Fold a tight-binding Hamiltonian onto a superlattice.

Parameters
----------
sl : {par_0}
   Superlattice onto which the Hamiltonian is folded.
tb : {par_1}
   Tight-binding Hamiltonian defined on the underlying lattice.

Returns
-------
{ret_0}
   Tight-binding Hamiltonian folded onto the superlattice.
)DOC",
                                                {{c2py::python_typename<const triqs::experimental::lattice::superlattice &>()},
                                                 {c2py::python_typename<const triqs::experimental::lattice::tb_hk &>()}},
                                                {c2py::python_typename<triqs::experimental::lattice::tb_hk>()});
static const auto _c2py_doc_6 =
   _c2py_fun_6.doc(R"DOC(
[1, 2, 3] Compute the non-interacting local Green's function from a tight-binding Hamiltonian on a given mesh.

The local Green's function is obtained by integrating :math:`[(\omega + \mu) I - H(\mathbf{k})]^{-1}` over
the Brillouin zone for each frequency of the mesh.

------

[4, 5, 6] Compute the interacting local Green's function from a tight-binding Hamiltonian and a self-energy.

The local Green's function is obtained by integrating
:math:`[(\omega + \mu) I - H(\mathbf{k}) - \Sigma(\omega)]^{-1}` over the Brillouin zone for each frequency of the
self-energy mesh. The number of orbitals of the self-energy must match that of the Hamiltonian.

------

[7, 8, 9] Compute the interacting local Green's function as a block Green's function.

This overload applies the single-block calculation to each block of the given block self-energy and
collects the results into a block Green's function with the same block structure.

------

Parameters
----------
w_mesh : {par_0}
   Frequency mesh on which the Brillouin-zone integration is performed for each frequency.
H_k : {par_1}
   Tight-binding Hamiltonian :math:`H(\mathbf{k})`.
mu : {par_2}
   Chemical potential :math:`\mu`.
opt : {par_3}
   Options controlling the Brillouin-zone integration.
Sigma : {par_4}
   Self-energy :math:`\Sigma(\omega)` defining the frequency mesh.

Returns
-------
[1] : {ret_0}
   Local Green's function on the given frequency mesh.

[2] : {ret_1}
   Local Green's function on the given frequency mesh.

[3] : {ret_2}
   Local Green's function on the given frequency mesh.

[4] : {ret_3}
   Local Green's function on the frequency mesh of the given self-energy.

[5] : {ret_4}
   Local Green's function on the frequency mesh of the given self-energy.

[6] : {ret_5}
   Local Green's function on the frequency mesh of the given self-energy.

[7] : {ret_6}
   Local block Green's function on the frequency mesh of the given self-energy.

[8] : {ret_7}
   Local block Green's function on the frequency mesh of the given self-energy.

[9] : {ret_8}
   Local block Green's function on the frequency mesh of the given self-energy.
)DOC",
                   {{c2py::python_typename<const triqs::mesh::dlr_imfreq &>(), c2py::python_typename<const triqs::mesh::imfreq &>(),
                     c2py::python_typename<const triqs::mesh::refreq &>()},
                    {c2py::python_typename<const triqs::experimental::lattice::tb_hk &>()},
                    {c2py::python_typename<double>()},
                    {c2py::python_typename<const triqs::experimental::lattice::bz_int_options &>()},
                    {c2py::python_typename<const triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &>(),
                     c2py::python_typename<const triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::matrix_valued> &>(),
                     c2py::python_typename<const triqs::gfs::gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &>()}},
                   {c2py::python_typename<triqs::gfs::gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>>(),
                    c2py::python_typename<triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>>(),
                    c2py::python_typename<triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>>(),
                    c2py::python_typename<triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>>(),
                    c2py::python_typename<triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>>(),
                    c2py::python_typename<triqs::gfs::gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>>(),
                    c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued>>(),
                    c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::matrix_valued>>(),
                    c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::refreq, triqs::gfs::matrix_valued>>()});
static const auto _c2py_doc_7 =
   _c2py_fun_7.doc(R"DOC(
Build a tight-binding Hamiltonian from a Wannier90 `*_hr.dat` file.

This is a convenience wrapper that reads the file and keeps the lattice vectors and hopping matrices.

Parameters
----------
w90_path_and_seedname : {par_0}
   Path to the Wannier90 files including the seedname, such that the file to read is
   `<w90_path_and_seedname>_hr.dat`.

Returns
-------
{ret_0}
   Tight-binding Hamiltonian with hoppings in units of eV.
)DOC",
                   {{c2py::python_typename<const std::string &>()}}, {c2py::python_typename<triqs::experimental::lattice::tb_hk>()});
static const auto _c2py_doc_8 =
   _c2py_fun_8.doc(R"DOC(
Build a tight-binding Hamiltonian from a Wannier90 `*_tb.dat` file.

This is a convenience wrapper that reads the file and keeps only the lattice vectors and hopping matrices,
discarding the position operators and unit-cell vectors.

Parameters
----------
w90_path_and_seedname : {par_0}
   Path to the Wannier90 files including the seedname, such that the file to read is
   `<w90_path_and_seedname>_tb.dat`.

Returns
-------
{ret_0}
   Tight-binding Hamiltonian with hoppings in units of eV.
)DOC",
                   {{c2py::python_typename<const std::string &>()}}, {c2py::python_typename<triqs::experimental::lattice::tb_hk>()});
static const auto _c2py_doc_9 =
   _c2py_fun_9.doc(R"DOC(
Read a Wannier90 `*_hr.dat` file and return the lattice vectors and hopping matrices.

The file is located by appending `_hr.dat` to the given path and seedname. The lattice vectors
:math:`\mathbf{R}` are returned in crystal (lattice) coordinates and the hoppings in eV.

Parameters
----------
w90_path_and_seedname : {par_0}
   Path to the Wannier90 files including the seedname, such that the file to read is
   `<w90_path_and_seedname>_hr.dat`.

Returns
-------
{ret_0}
   Tuple containing the lattice vectors :math:`\mathbf{R}` (shape `[nR, 3]`) and the hopping matrices (shape
   `[nR, nOrb, nOrb]`).
)DOC",
                   {{c2py::python_typename<const std::string &>()}}, {c2py::python_typename<triqs::experimental::lattice::w90_hr_data_t>()});
static const auto _c2py_doc_10 =
   _c2py_fun_10.doc(R"DOC(
Read a Wannier90 `*_tb.dat` file and return the lattice vectors, hopping matrices, position operators and
unit-cell vectors.

The file is located by appending `_tb.dat` to the given path and seedname. All quantities are returned in
the units used by Wannier90: hoppings in eV, lattice vectors :math:`\mathbf{R}` in crystal (lattice) coordinates,
position operators in Angstrom and unit-cell vectors in Angstrom.

Parameters
----------
w90_path_and_seedname : {par_0}
   Path to the Wannier90 files including the seedname, such that the file to read is
   `<w90_path_and_seedname>_tb.dat`.

Returns
-------
{ret_0}
   Tuple containing the lattice vectors :math:`\mathbf{R}` (shape `[nR, 3]`), the hopping matrices (shape
   `[nR, nOrb, nOrb]`), the position operators in the real-space Wannier basis (shape `[nR, nOrb, nOrb, 3]`) and the
   unit-cell vectors (shape `[3, 3]`).
)DOC",
                    {{c2py::python_typename<const std::string &>()}}, {c2py::python_typename<triqs::experimental::lattice::w90_tb_data_t>()});
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
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "lattice", /* name of module */
                                        R"RAWDOC(Experimental lattice tools.

.. warning::

   Experimental and unstable -- the API may change without notice.

Tools for tight-binding calculations on a lattice: tight-binding Hamiltonians H(k),
Brillouin-zone integration, local (k-integrated) Green's functions and the associated
chemical-potential search, folding onto a superlattice, and loaders for Wannier90 output files.
)RAWDOC",                                          /* module documentation, may be NULL */
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
