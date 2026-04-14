
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
using _c2py_cls_0                                            = triqs::lattice::bravais_lattice;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs.lattice.lattice_tools.BravaisLattice";
static auto _c2py_init_0                                     = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_0>(),
   c2py::c_constructor<_c2py_cls_0, const triqs::lattice::matrix_t &, std::vector<triqs::lattice::r_t>, std::vector<std::string>>(
      "units", "orbital_positions"_a = std::vector<triqs::lattice::r_t>{{0, 0, 0}}, "atom_orb_name"_a = std::vector<std::string>{})};
template <> constexpr initproc c2py::tp_init<_c2py_cls_0> = c2py::pyfkw_constructor<_c2py_init_0>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_0> = _c2py_init_0.doc(R"DOC(
[1] Construct a simple cubic lattice with lattice constant :math:`a = 1`.

The only atomic orbital is placed at the origin with no name.

------

[2] Construct a Bravais Lattice with given basis vectors and positions of atomic orbitals with optional names.

The matrix :math:`\mathbf{A}^T` containing the basis vectors as its rows is required to be square. The
number of dimensions of the Bravais lattices is determined by the size of the matrix.

------

Parameters
----------
units : {par_0}
   Matrix with the basis vectors :math:`\{ \mathbf{a}_1, \dots, \mathbf{a}_d \}` as its rows.
orbital_positions : {par_1}
   Atomic orbital positions :math:`\{ \mathbf{r}_1, \dots, \mathbf{r}_m \}` in the unit cell.
atom_orb_name : {par_2}
   Optional names for the atomic orbitals.
)DOC",
                                                                    {{c2py::python_typename<const triqs::lattice::matrix_t &>()},
                                                                     {c2py::python_typename<std::vector<triqs::lattice::r_t>>()},
                                                                     {c2py::python_typename<std::vector<std::string>>()}});
// contains
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_0 const &self, const triqs::lattice::r_t &_p_0) -> decltype(auto) { return self.contains(_p_0); }, "self", "_p_0")};

// lattice_to_real_coordinates
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_0 const &self,
      const nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &v)
      -> decltype(auto) {
     return self.template lattice_to_real_coordinates<nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<>>>>(v);
   },
   "self", "v")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC(
Check if a given vector :math:`\mathbf{r}` is part of the domain.
)DOC");
static const auto _c2py_doc_1 = _c2py_fun_1.doc(
   R"DOC(
Transform a vector :math:`\mathbf{v}` from the lattice basis :math:`\{ \mathbf{a}_1, \dots, \mathbf{a}_d \}`
to the standard basis :math:`\{ \mathbf{e}_1, \dots, \mathbf{e}_d \}`.

The transformed vector is given by

.. math::

   \tilde{\mathbf{v}} = \sum_{i=1}^{d} \mathbf{a}_i v_i = A \mathbf{v} \; ,

where :math:`A` is the matrix with the lattice basis vectors as its columns.

Parameters
----------
v : {par_0}
   Vector :math:`\mathbf{v}` to be transformed.

Returns
-------
{ret_0}
   Vector :math:`\tilde{\mathbf{v}}` in the standard basis.
)DOC",
   {{c2py::python_typename<
      const nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()}},
   {c2py::python_typename<triqs::lattice::r_t>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {
   {"contains", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"lattice_to_real_coordinates", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_0>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_0>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_0>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_0 = R"DOC(Get the number of atomic orbitals in the unit cell.)DOC";
static constexpr auto prop_doc_1 = R"DOC(Get the number of dimensions of the Bravais lattice.)DOC";
static constexpr auto prop_doc_2 = R"DOC(Get a `std::vector<std::string>` containing the orbital names.)DOC";
static constexpr auto prop_doc_3 =
   R"DOC(Get a `std::vector<r_t>` containing the atomic orbital positions :math:`\{\mathbf{r}_1, \dots, \mathbf{r}_m\}`.)DOC";
static constexpr auto prop_doc_4 = R"DOC(Get the matrix :math:`\mathbf{A}^T` containing basis vectors as its rows.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_0>[] = {

   {"n_orbitals", c2py::getter_from_method<c2py::castmc<>(&triqs::lattice::bravais_lattice::n_orbitals)>, nullptr, prop_doc_0, nullptr},
   {"ndim", c2py::getter_from_method<c2py::castmc<>(&triqs::lattice::bravais_lattice::ndim)>, nullptr, prop_doc_1, nullptr},
   {"orbital_names", c2py::getter_from_method<c2py::castmc<>(&triqs::lattice::bravais_lattice::orbital_names)>, nullptr, prop_doc_2, nullptr},
   {"orbital_positions", c2py::getter_from_method<c2py::castmc<>(&triqs::lattice::bravais_lattice::orbital_positions)>, nullptr, prop_doc_3, nullptr},
   {"units", c2py::getter_from_method<c2py::castmc<>(&triqs::lattice::bravais_lattice::units)>, nullptr, prop_doc_4, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_0> = R"DOC(A Bravais lattice class.

A Bravais lattice in :math:`d` dimensions is defined by
- a set of linear independent d-dimensional basis vectors :math:`\{ \mathbf{a}_1, \dots, \mathbf{a}_d \}` and
- the positions, :math:`\{ \mathbf{r}_1, \dots, \mathbf{r}_m \}`, of atomic orbitals within each unit cell.

Optionally, the atomic orbitals can be named.

The infinite lattice points :math:`\mathbf{R}^{\mathbf{n}}` (see bravais_lattice::point_t) are generated by

.. math::

   \mathbf{R}^{\mathbf{n}} = \sum_{i=1}^{d} \mathbf{a}_i n_i = \mathbf{A} \mathbf{n} \; ,

where :math:`\mathbf{n} = (n_1, \dots, n_d) \in \mathbb{Z}^d` is an index vector (or more formally the vector :math:`\mathbf{R}^{\mathbf{n}}`
in the lattice basis) and :math:`\mathbf{A} = \big( \mathbf{a}_1 \cdots \mathbf{a}_d \big)`
is the matrix with the basis vectors as its columns.

.. note::

   Although the supported dimensions are 1, 2 and 3, the index vectors are always 3-dimensional, i.e. :math:`\mathbf{n} = (n_1, n_2, n_3)`
   . Indices :math:`n_j` with :math:`j > d` are simply ignored when computing the
   corresponding lattice vector.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_0>;
// --------- class _c2py_cls_1 -----------
using _c2py_cls_1                                            = triqs::lattice::brillouin_zone;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_1>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_1> = "triqs.lattice.lattice_tools.BrillouinZone";
static auto _c2py_init_1 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_1>(), c2py::c_constructor<_c2py_cls_1, triqs::lattice::bravais_lattice>("bl")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_1> = c2py::pyfkw_constructor<_c2py_init_1>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_1> = _c2py_init_1.doc(R"DOC(
[1] Construct a Brillouin zone for a simple cubic lattice with lattice constant :math:`a = 1`.

The reciprocal basis vectors defining the BZ are given by :math:`B = 2 \pi I`, where :math:`I` is the
:math:`3 \times 3` identity matrix.

------

[2] Construct a Brillouin zone for a given Bravais lattice.

The reciprocal basis vectors defining the BZ are given by :math:`B = 2 \pi \left( A^T \right)^{-1}`,
where :math:`A` is the matrix containing the basis vectors of the given Bravais lattice as its columns.

------

Parameters
----------
bl : {par_0}
   triqs::lattice::bravais_lattice object.
)DOC",
                                                                    {{c2py::python_typename<triqs::lattice::bravais_lattice>()}});
// lattice_to_real_coordinates
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_1 const &self,
      const nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &v)
      -> decltype(auto) {
     return self.template lattice_to_real_coordinates<nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<>>>>(v);
   },
   "self", "v")};

static const auto _c2py_doc_2 = _c2py_fun_2.doc(
   R"DOC(
Transform a vector :math:`\mathbf{v}` from the reciprocal lattice basis :math:`\{ \mathbf{b}_1, \dots,
\mathbf{b}_d \}` to the standard basis :math:`\{ \mathbf{e}_1, \dots, \mathbf{e}_d \}`.

The transformed vector is given by

.. math::

   \tilde{\mathbf{v}} = \sum_{i=1}^{d} \mathbf{b}_i v_i = B \mathbf{v} \; ,

where :math:`B` is the matrix with the reciprocal lattice basis vectors as its columns.

Parameters
----------
v : {par_0}
   Vector :math:`\mathbf{v}` to be transformed.

Returns
-------
{ret_0}
   Vector :math:`\tilde{\mathbf{v}}` in the standard basis.
)DOC",
   {{c2py::python_typename<
      const nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &>()}},
   {c2py::python_typename<triqs::lattice::k_t>()});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_1>[] = {
   {"lattice_to_real_coordinates", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_1>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_tuple<_c2py_cls_1>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_tuple<_c2py_cls_1>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_5 = R"DOC(Get the underlying triqs::lattice::bravais_lattice object.)DOC";
static constexpr auto prop_doc_6 = R"DOC(Get the number of dimensions of the underlying Bravais lattice.)DOC";
static constexpr auto prop_doc_7 = R"DOC(Get the matrix :math:`B^T` containing the reciprocal basis vectors as its rows.)DOC";
static constexpr auto prop_doc_8 = R"DOC(Get the inverse matrix :math:`\left( B^T \right)^{-1}`.)DOC";
static constexpr auto prop_doc_9 = R"DOC(Get the matrix :math:`B^T` containing the reciprocal basis vectors as its rows.)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_1>[] = {

   {"lattice", c2py::getter_from_method<c2py::castmc<>(&triqs::lattice::brillouin_zone::lattice)>, nullptr, prop_doc_5, nullptr},
   {"ndim", c2py::getter_from_method<c2py::castmc<>(&triqs::lattice::brillouin_zone::ndim)>, nullptr, prop_doc_6, nullptr},
   {"reciprocal_matrix", c2py::getter_from_method<c2py::castmc<>(&triqs::lattice::brillouin_zone::reciprocal_matrix)>, nullptr, prop_doc_7, nullptr},
   {"reciprocal_matrix_inv", c2py::getter_from_method<c2py::castmc<>(&triqs::lattice::brillouin_zone::reciprocal_matrix_inv)>, nullptr, prop_doc_8,
    nullptr},
   {"units", c2py::getter_from_method<c2py::castmc<>(&triqs::lattice::brillouin_zone::units)>, nullptr, prop_doc_9, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_1> = R"DOC(A Brillouin zone class.

A Brillouin zone is the primitive unit cell in reciprocal space. It is defined by the set of reciprocal
lattice basis vectors :math:`\{ \mathbf{b}_1, \dots, \mathbf{b}_d \}`, which satisfy

.. math::

   \mathbf{b}_i \cdot \mathbf{a}_j = 2 \pi \delta_{ij} \; ,

where :math:`\{ \mathbf{a}_1, \dots, \mathbf{a}_d \}` are the basis vectors of the Bravais lattice in real space.

In matrix notation, we can write this as a system of linear equations :math:`A^T B = 2 \pi I`, where :math:`A = \big(
\mathbf{a}_1 \cdots \mathbf{a}_d \big)` and :math:`B = \big( \mathbf{b}_1 \cdots \mathbf{b}_d \big)` are the
matrices containing the basis vectors as their columns and :math:`I` is the identity matrix.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_1>;
// --------- class _c2py_cls_2 -----------
using _c2py_cls_2                                            = triqs::lattice::tight_binding;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_2>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_2> = "triqs.lattice.lattice_tools.TightBinding";
static auto _c2py_init_2                                     = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<
      _c2py_cls_2, triqs::lattice::bravais_lattice,
      std::vector<nda::basic_array<long, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>,
      std::vector<
         nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>(
      "bl", "displ_vec", "overlap_mat_vec"),
   c2py::c_constructor<_c2py_cls_2, triqs::lattice::bravais_lattice, triqs::lattice::hopping_dict>("bl", "hoppings")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_2> = c2py::pyfkw_constructor<_c2py_init_2>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_2> = _c2py_init_2.doc(
   R"DOC(
[1] Construct a tight_binding Hamiltonian on a given bravais_lattice,
given the displacements in units of the lattice basis vectors (units)
and the associated overlap (hopping) matrices.
The matrix structure is w.r.t. the atoms in the unit cell.

------

[2] Construct a tight_binding Hamiltonian on a given bravais_lattice from a hopping dictionary.

------

Parameters
----------
bl : {par_0}
   The underlying bravais lattice
displ_vec : {par_1}
   The vector of displacement vectors in units of the lattice basis vectors
overlap_mat_vec : {par_2}
   The vector of overlap (hopping) matrices
)DOC",
   {{c2py::python_typename<triqs::lattice::bravais_lattice>()},
    {c2py::python_typename<
       std::vector<nda::basic_array<long, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>()},
    {c2py::python_typename<std::vector<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>>()}});
// dispersion
static auto const _c2py_fun_3 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_2 const &self,
         const nda::basic_array_view<const double, 1, nda::C_stride_layout, 'V', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>>
            &k) -> decltype(auto) { return self.template dispersion<nda::basic_array_view<const double, 1, nda::C_stride_layout, 'V'>>(k); },
      "self", "k"),
   c2py::cmethod(
      [](_c2py_cls_2 const &self,
         const nda::basic_array_view<const double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>>
            &k) -> decltype(auto) { return self.template dispersion<nda::basic_array_view<const double, 2, nda::C_stride_layout>>(k); },
      "self", "k"),
   c2py::cmethod([](_c2py_cls_2 const &self, const triqs::mesh::brzone &k_mesh) -> decltype(auto) { return self.dispersion(k_mesh); }, "self",
                 "k_mesh"),
   c2py::cmethod([](_c2py_cls_2 const &self, int n_l) -> decltype(auto) { return self.dispersion(n_l); }, "self", "n_l")};

// fourier
static auto const _c2py_fun_4 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_2 const &self,
         const nda::basic_array_view<const double, 1, nda::C_stride_layout, 'V', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>>
            &k) -> decltype(auto) { return self.template fourier<nda::basic_array_view<const double, 1, nda::C_stride_layout, 'V'>>(k); },
      "self", "k"),
   c2py::cmethod(
      [](_c2py_cls_2 const &self,
         const nda::basic_array_view<const double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>>
            &k) -> decltype(auto) { return self.template fourier<nda::basic_array_view<const double, 2, nda::C_stride_layout>>(k); },
      "self", "k"),
   c2py::cmethod([](_c2py_cls_2 const &self, const triqs::mesh::brzone &k_mesh) -> decltype(auto) { return self.fourier(k_mesh); }, "self", "k_mesh"),
   c2py::cmethod([](_c2py_cls_2 const &self, int n_l) -> decltype(auto) { return self.fourier(n_l); }, "self", "n_l")};

// h5_read_construct
static auto const _c2py_fun_5 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](h5::group g, std::string subgroup_name) { return _c2py_cls_2::h5_read_construct(g, subgroup_name); }, "g", "subgroup_name")};

// lattice_to_real_coordinates
static auto const _c2py_fun_6 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_2 const &self,
      const nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &x)
      -> decltype(auto) {
     return self.template lattice_to_real_coordinates<nda::basic_array<double, 1, nda::C_layout, 'V', nda::heap_basic<nda::mem::mallocator<>>>>(x);
   },
   "self", "x")};

static const auto _c2py_doc_3 = _c2py_fun_3.doc(
   R"DOC(
[1, 2] Calculate the dispersion, i.e. the eigenvalue-spectrum of $h_k$,
for a given momentum vector k (or array of vectors).

------

[3] Calculate the dispersion on a given k-mesh
and return the associated Green-function object

------

[4] Calculate the dispersion on a regular k-mesh
with n_l grid-points in each reciprocal direction.
Return the associated Green-function object.

------

Parameters
----------
k : {par_0}
   The momentum vector (or an array thereof) in units of the reciprocal lattice vectors
k_mesh : {par_1}
   The brillouin-zone mesh
n_l : {par_2}
   The number of grid-points for each dimension

Returns
-------
[1] : {ret_0}
   The value for $h_k$ as a complex matrix

[2] : {ret_1}
   The value for $h_k$ as a complex matrix

[3, 4] : {ret_2}
   Green function on the k_mesh initialized with the dispersion values
)DOC",
   {{c2py::python_typename<const nda::basic_array_view<const double, 1, nda::C_stride_layout, 'V', nda::default_accessor,
                                                       nda::borrowed<nda::mem::AddressSpace::Host>> &>(),
     c2py::python_typename<const nda::basic_array_view<const double, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                                       nda::borrowed<nda::mem::AddressSpace::Host>> &>()},
    {c2py::python_typename<const triqs::mesh::brzone &>()},
    {c2py::python_typename<int>()}},
   {c2py::python_typename<nda::basic_array<double, 1, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<nda::basic_array<double, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_real_valued<1>>>()});
static const auto _c2py_doc_4 = _c2py_fun_4.doc(
   R"DOC(
[1, 2] Calculate the fourier transform for a given momentum vector k (or array of vectors)

  $$ h_k =  m_j * exp(2  i * {k} * {r}_j) $$

with lattice displacements {r_j} and associated overlap (hopping) matrices {m_j}.
k needs to be represented in units of the reciprocal lattice vectors

------

[3] Calculate the fourier transform on a given k-mesh
and return the associated Green-function object

------

[4] Calculate the fourier transform on a regular k-mesh
with n_l grid-points in each reciprocal direction.
Return the associated Green-function object.

------

Parameters
----------
k : {par_0}
   The momentum vector (or an array thereof) in units of the reciprocal lattice vectors
k_mesh : {par_1}
   The brillouin-zone mesh
n_l : {par_2}
   The number of grid-points for each dimension

Returns
-------
[1] : {ret_0}
   The value for $h_k$ as a complex matrix

[2] : {ret_1}
   The value for $h_k$ as a complex matrix

[3, 4] : {ret_2}
   Green function on the k_mesh initialized with the fourier transform
)DOC",
   {{c2py::python_typename<const nda::basic_array_view<const double, 1, nda::C_stride_layout, 'V', nda::default_accessor,
                                                       nda::borrowed<nda::mem::AddressSpace::Host>> &>(),
     c2py::python_typename<const nda::basic_array_view<const double, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                                       nda::borrowed<nda::mem::AddressSpace::Host>> &>()},
    {c2py::python_typename<const triqs::mesh::brzone &>()},
    {c2py::python_typename<int>()}},
   {c2py::python_typename<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<
       nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::matrix_valued>>()});
static const auto _c2py_doc_5 = _c2py_fun_5.doc(R"DOC()DOC");
static const auto _c2py_doc_6 = _c2py_fun_6.doc(R"DOC(
Transform into real coordinates.
)DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_2>[] = {
   {"dispersion", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {"fourier", (PyCFunction)c2py::pyfkw<_c2py_fun_4>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_4.c_str()},
   {"h5_read_construct", (PyCFunction)c2py::pyfkw<_c2py_fun_5>, METH_VARARGS | METH_KEYWORDS | METH_STATIC, _c2py_doc_5.c_str()},
   {"lattice_to_real_coordinates", (PyCFunction)c2py::pyfkw<_c2py_fun_6>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_6.c_str()},
   {"__write_hdf5__", c2py::tpxx_write_h5<_c2py_cls_2>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_h5<_c2py_cls_2>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_h5<_c2py_cls_2>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

static constexpr auto prop_doc_10 = R"DOC(Return a vector containing all displacement vectors)DOC";
static constexpr auto prop_doc_11 = R"DOC(Underlying lattice)DOC";
static constexpr auto prop_doc_12 = R"DOC(Number of orbitals / bands, i.e. size of the matrix t(k))DOC";
static constexpr auto prop_doc_13 = R"DOC(Return a vector containing all overlap matrices)DOC";

// ----- Member and property table ----

template <>
constinit PyGetSetDef c2py::tp_getset<_c2py_cls_2>[] = {

   {"displ_vec", c2py::getter_from_method<c2py::castmc<>(&triqs::lattice::tight_binding::displ_vec)>, nullptr, prop_doc_10, nullptr},
   {"lattice", c2py::getter_from_method<c2py::castmc<>(&triqs::lattice::tight_binding::lattice)>, nullptr, prop_doc_11, nullptr},
   {"n_orbitals", c2py::getter_from_method<c2py::castmc<>(&triqs::lattice::tight_binding::n_orbitals)>, nullptr, prop_doc_12, nullptr},
   {"overlap_mat_vec", c2py::getter_from_method<c2py::castmc<>(&triqs::lattice::tight_binding::overlap_mat_vec)>, nullptr, prop_doc_13, nullptr},
   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <>
const std::string c2py::tp_doc<_c2py_cls_2> = R"DOC(For tightbinding Hamiltonian with fully localised orbitals
Overlap between orbital is taken as unit matrix.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_2>;

// ==================== module functions ====================

// dos
static auto const _c2py_fun_7 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::lattice::tight_binding &TB, int nkpts, int neps) { return triqs::lattice::dos(TB, nkpts, neps); }, "TB", "nkpts", "neps")};

// dos_patch
static auto const _c2py_fun_8 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::lattice::tight_binding &TB,
      const nda::basic_array<double, 2, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>> &triangles, int neps,
      int ndiv) { return triqs::lattice::dos_patch(TB, triangles, neps, ndiv); },
   "TB", "triangles", "neps", "ndiv")};

static const auto _c2py_doc_7 = _c2py_fun_7.doc(R"DOC()DOC");
static const auto _c2py_doc_8 = _c2py_fun_8.doc(R"DOC()DOC");
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"dos", (PyCFunction)c2py::pyfkw<_c2py_fun_7>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_7.c_str()},
   {"dos_patch", (PyCFunction)c2py::pyfkw<_c2py_fun_8>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_8.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "lattice_tools",   /* name of module */
                                        R"RAWDOC()RAWDOC", /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_lattice_tools() {

  if (not c2py::check_python_version("lattice_tools")) return NULL;

  // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_0>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_1>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_2>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
#define _add_type(T, N) c2py::add_type_object_to_main<T>(N, m, conv_table)
  _add_type(_c2py_cls_0, "BravaisLattice");
  _add_type(_c2py_cls_1, "BrillouinZone");
  _add_type(_c2py_cls_2, "TightBinding");
#undef _add_type

  c2py::pyref module = c2py::pyref::module("h5.formats");
  if (not module) return nullptr;
  c2py::pyref register_class = module.attr("register_class");

  register_h5_type<_c2py_cls_0>(register_class);
  register_h5_type<_c2py_cls_1>(register_class);
  register_h5_type<_c2py_cls_2>(register_class);

  return m;
}
#endif
// CLAIR_WRAP_GEN
