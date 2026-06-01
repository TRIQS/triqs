
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

// --------- class _c2py_cls_0 -----------
using _c2py_cls_0 = triqs::gfs::gf_proxy<triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_0>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_0> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyImFreq_3";
static const auto _c2py_init_0                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_0, triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_0>    = c2py::pyfkw_constructor<_c2py_init_0>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_0> = _c2py_init_0.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_0 &self, const long &x1) -> decltype(auto) {
        return self.template
        operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>, const long &>(x1);
      },
      "self", "x1"),
   c2py::cmethod(
      [](_c2py_cls_0 &self, const triqs::mesh::matsubara_freq &x1) -> decltype(auto) {
        return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                        const triqs::mesh::matsubara_freq &>(x1);
      },
      "self", "x1")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_0> = c2py::pyfkw<_c2py_fun_0>;

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_0> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_0>;
// --------- class _c2py_cls_1 -----------
using _c2py_cls_1 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::imfreq, triqs::mesh::brzone>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_1>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_1> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyImFreq_x_BrZone_3";
static const auto _c2py_init_1                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_1, triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::imfreq, triqs::mesh::brzone>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>(
   "gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_1>    = c2py::pyfkw_constructor<_c2py_init_1>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_1> = _c2py_init_1.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_1 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_1 &self, const long &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_1 &self, const long &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_1 &self, const triqs::mesh::matsubara_freq &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::mesh::matsubara_freq &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_1 &self, const triqs::mesh::matsubara_freq &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::mesh::matsubara_freq &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_1 &self, const itertools::range::all_t &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_1 &self, const itertools::range::all_t &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_1 &self, const long &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const long &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_1 &self, const triqs::mesh::matsubara_freq &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const triqs::mesh::matsubara_freq &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_1> = c2py::pyfkw<_c2py_fun_1>;

static const auto _c2py_doc_1 = _c2py_fun_1.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_1>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_1> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_1>;
// --------- class _c2py_cls_2 -----------
using _c2py_cls_2 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::imfreq, triqs::mesh::cyclat>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_2>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_2> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyImFreq_x_CycLat_3";
static const auto _c2py_init_2                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_2, triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::imfreq, triqs::mesh::cyclat>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>(
   "gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_2>    = c2py::pyfkw_constructor<_c2py_init_2>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_2> = _c2py_init_2.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_2 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_2 &self, const long &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_2 &self, const long &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_2 &self, const triqs::mesh::matsubara_freq &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::mesh::matsubara_freq &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_2 &self, const triqs::mesh::matsubara_freq &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::mesh::matsubara_freq &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_2 &self, const itertools::range::all_t &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_2 &self, const itertools::range::all_t &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_2 &self, const long &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const long &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_2 &self, const triqs::mesh::matsubara_freq &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const triqs::mesh::matsubara_freq &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_2> = c2py::pyfkw<_c2py_fun_2>;

static const auto _c2py_doc_2 = _c2py_fun_2.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_2>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_2> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_2>;
// --------- class _c2py_cls_3 -----------
using _c2py_cls_3 = triqs::gfs::gf_proxy<triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_3>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_3> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyImTime_3";
static const auto _c2py_init_3                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_3, triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_3>    = c2py::pyfkw_constructor<_c2py_init_3>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_3> = _c2py_init_3.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_3 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_3 &self, const long &x1) -> decltype(auto) {
        return self.template
        operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>, const long &>(x1);
      },
      "self", "x1"),
   c2py::cmethod(
      [](_c2py_cls_3 &self, const double &x1) -> decltype(auto) {
        return self.template
        operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>, const double &>(x1);
      },
      "self", "x1")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_3> = c2py::pyfkw<_c2py_fun_3>;

static const auto _c2py_doc_3 = _c2py_fun_3.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_3>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_3> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_3>;
// --------- class _c2py_cls_4 -----------
using _c2py_cls_4 = triqs::gfs::gf_proxy<triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_4>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_4> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyImTime_3_R";
static const auto _c2py_init_4                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_4, triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_4>    = c2py::pyfkw_constructor<_c2py_init_4>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_4> = _c2py_init_4.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_4 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_4 &self, const long &x1) -> decltype(auto) {
        return self.template
        operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>, const long &>(x1);
      },
      "self", "x1"),
   c2py::cmethod(
      [](_c2py_cls_4 &self, const double &x1) -> decltype(auto) {
        return self.template
        operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>, const double &>(x1);
      },
      "self", "x1")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_4> = c2py::pyfkw<_c2py_fun_4>;

static const auto _c2py_doc_4 = _c2py_fun_4.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_4>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_4> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_4>;
// --------- class _c2py_cls_5 -----------
using _c2py_cls_5 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::imtime, triqs::mesh::brzone>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_5>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_5> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyImTime_x_BrZone_3";
static const auto _c2py_init_5                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_5, triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::imtime, triqs::mesh::brzone>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>(
   "gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_5>    = c2py::pyfkw_constructor<_c2py_init_5>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_5> = _c2py_init_5.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_5 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_5 &self, const long &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_5 &self, const long &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_5 &self, const double &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_5 &self, const double &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_5 &self, const itertools::range::all_t &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_5 &self, const itertools::range::all_t &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_5 &self, const long &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const long &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_5 &self, const double &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const double &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_5> = c2py::pyfkw<_c2py_fun_5>;

static const auto _c2py_doc_5 = _c2py_fun_5.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_5>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_5> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_5>;
// --------- class _c2py_cls_6 -----------
using _c2py_cls_6 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::imtime, triqs::mesh::brzone>, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_6>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_6> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyImTime_x_BrZone_3_R";
static const auto _c2py_init_6                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_6,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::imtime, triqs::mesh::brzone>, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_6>    = c2py::pyfkw_constructor<_c2py_init_6>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_6> = _c2py_init_6.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_6 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_6 &self, const long &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_6 &self, const long &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_6 &self, const double &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_6 &self, const double &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_6 &self, const itertools::range::all_t &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>>, const itertools::range::all_t &,
                                           const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_6 &self, const itertools::range::all_t &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>>, const itertools::range::all_t &,
                                           const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_6 &self, const long &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_real_valued<3>>, const long &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_6 &self, const double &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_real_valued<3>>, const double &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_6> = c2py::pyfkw<_c2py_fun_6>;

static const auto _c2py_doc_6 = _c2py_fun_6.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_6>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_6> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_6>;
// --------- class _c2py_cls_7 -----------
using _c2py_cls_7 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::imtime, triqs::mesh::cyclat>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_7>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_7> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyImTime_x_CycLat_3";
static const auto _c2py_init_7                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_7, triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::imtime, triqs::mesh::cyclat>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>(
   "gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_7>    = c2py::pyfkw_constructor<_c2py_init_7>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_7> = _c2py_init_7.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_7 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_7 &self, const long &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_7 &self, const long &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_7 &self, const double &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_7 &self, const double &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_7 &self, const itertools::range::all_t &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_7 &self, const itertools::range::all_t &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_7 &self, const long &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const long &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_7 &self, const double &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const double &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_7> = c2py::pyfkw<_c2py_fun_7>;

static const auto _c2py_doc_7 = _c2py_fun_7.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_7>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_7> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_7>;
// --------- class _c2py_cls_8 -----------
using _c2py_cls_8 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::imtime, triqs::mesh::cyclat>, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_8>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_8> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyImTime_x_CycLat_3_R";
static const auto _c2py_init_8                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_8,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::imtime, triqs::mesh::cyclat>, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_8>    = c2py::pyfkw_constructor<_c2py_init_8>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_8> = _c2py_init_8.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_8 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_8 &self, const long &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_8 &self, const long &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_8 &self, const double &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_8 &self, const double &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_8 &self, const itertools::range::all_t &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>>, const itertools::range::all_t &,
                                           const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_8 &self, const itertools::range::all_t &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>>, const itertools::range::all_t &,
                                           const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_8 &self, const long &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_real_valued<3>>, const long &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_8 &self, const double &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_real_valued<3>>, const double &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_8> = c2py::pyfkw<_c2py_fun_8>;

static const auto _c2py_doc_8 = _c2py_fun_8.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_8>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_8> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_8>;
// --------- class _c2py_cls_9 -----------
using _c2py_cls_9 = triqs::gfs::gf_proxy<triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_9>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_9> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyReFreq_3";
static const auto _c2py_init_9                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_9, triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_9>    = c2py::pyfkw_constructor<_c2py_init_9>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_9> = _c2py_init_9.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_9 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_9 &self, const long &x1) -> decltype(auto) {
        return self.template
        operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>, const long &>(x1);
      },
      "self", "x1"),
   c2py::cmethod(
      [](_c2py_cls_9 &self, const double &x1) -> decltype(auto) {
        return self.template
        operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>, const double &>(x1);
      },
      "self", "x1")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_9> = c2py::pyfkw<_c2py_fun_9>;

static const auto _c2py_doc_9 = _c2py_fun_9.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_9>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_9> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_9>;
// --------- class _c2py_cls_10 -----------
using _c2py_cls_10 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::refreq, triqs::mesh::brzone>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_10>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_10> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyReFreq_x_BrZone_3";
static const auto _c2py_init_10                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_10,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::refreq, triqs::mesh::brzone>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_10>    = c2py::pyfkw_constructor<_c2py_init_10>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_10> = _c2py_init_10.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_10 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_10 &self, const long &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_10 &self, const long &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_10 &self, const double &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_10 &self, const double &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_10 &self, const itertools::range::all_t &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_10 &self, const itertools::range::all_t &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_10 &self, const long &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const long &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_10 &self, const double &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const double &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_10> = c2py::pyfkw<_c2py_fun_10>;

static const auto _c2py_doc_10 = _c2py_fun_10.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_10>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_10> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_10>;
// --------- class _c2py_cls_11 -----------
using _c2py_cls_11 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::refreq, triqs::mesh::cyclat>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_11>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_11> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyReFreq_x_CycLat_3";
static const auto _c2py_init_11                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_11,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::refreq, triqs::mesh::cyclat>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_11>    = c2py::pyfkw_constructor<_c2py_init_11>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_11> = _c2py_init_11.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_11 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_11 &self, const long &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_11 &self, const long &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_11 &self, const double &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_11 &self, const double &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_11 &self, const itertools::range::all_t &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_11 &self, const itertools::range::all_t &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_11 &self, const long &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const long &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_11 &self, const double &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const double &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_11> = c2py::pyfkw<_c2py_fun_11>;

static const auto _c2py_doc_11 = _c2py_fun_11.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_11>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_11> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_11>;
// --------- class _c2py_cls_12 -----------
using _c2py_cls_12 = triqs::gfs::gf_proxy<triqs::gfs::gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_12>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_12> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyReTime_3";
static const auto _c2py_init_12                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_12, triqs::gfs::gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_12>    = c2py::pyfkw_constructor<_c2py_init_12>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_12> = _c2py_init_12.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_12 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_12 &self, const long &x1) -> decltype(auto) {
        return self.template
        operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>, const long &>(x1);
      },
      "self", "x1"),
   c2py::cmethod(
      [](_c2py_cls_12 &self, const double &x1) -> decltype(auto) {
        return self.template
        operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>, const double &>(x1);
      },
      "self", "x1")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_12> = c2py::pyfkw<_c2py_fun_12>;

static const auto _c2py_doc_12 = _c2py_fun_12.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_12>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_12> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_12>;
// --------- class _c2py_cls_13 -----------
using _c2py_cls_13 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::retime, triqs::mesh::brzone>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_13>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_13> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyReTime_x_BrZone_3";
static const auto _c2py_init_13                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_13,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::retime, triqs::mesh::brzone>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_13>    = c2py::pyfkw_constructor<_c2py_init_13>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_13> = _c2py_init_13.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_13 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_13 &self, const long &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_13 &self, const long &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_13 &self, const double &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_13 &self, const double &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_13 &self, const itertools::range::all_t &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::retime, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_13 &self, const itertools::range::all_t &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::retime, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_13 &self, const long &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const long &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_13 &self, const double &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const double &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_13> = c2py::pyfkw<_c2py_fun_13>;

static const auto _c2py_doc_13 = _c2py_fun_13.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_13>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_13> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_13>;
// --------- class _c2py_cls_14 -----------
using _c2py_cls_14 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::retime, triqs::mesh::cyclat>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_14>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_14> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyReTime_x_CycLat_3";
static const auto _c2py_init_14                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_14,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::retime, triqs::mesh::cyclat>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_14>    = c2py::pyfkw_constructor<_c2py_init_14>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_14> = _c2py_init_14.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_14 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_14 &self, const long &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_14 &self, const long &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_14 &self, const double &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_14 &self, const double &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_14 &self, const itertools::range::all_t &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::retime, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_14 &self, const itertools::range::all_t &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::retime, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_14 &self, const long &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const long &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_14 &self, const double &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const double &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_14> = c2py::pyfkw<_c2py_fun_14>;

static const auto _c2py_doc_14 = _c2py_fun_14.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_14>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_14> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_14>;
// --------- class _c2py_cls_15 -----------
using _c2py_cls_15 = triqs::gfs::gf_proxy<triqs::gfs::gf_view<triqs::mesh::legendre, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_15>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_15> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyLegendre_3";
static const auto _c2py_init_15                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_15, triqs::gfs::gf_view<triqs::mesh::legendre, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_15>    = c2py::pyfkw_constructor<_c2py_init_15>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_15> = _c2py_init_15.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_15 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_15 &self, const long &x1) -> decltype(auto) {
        return self.template
        operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>, const long &>(x1);
      },
      "self", "x1"),
   c2py::cmethod(
      [](_c2py_cls_15 &self, const double &x1) -> decltype(auto) {
        return self.template
        operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>, const double &>(x1);
      },
      "self", "x1")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_15> = c2py::pyfkw<_c2py_fun_15>;

static const auto _c2py_doc_15 = _c2py_fun_15.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_15>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_15> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_15>;
// --------- class _c2py_cls_16 -----------
using _c2py_cls_16 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::legendre, triqs::mesh::brzone>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_16>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_16> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyLegendre_x_BrZone_3";
static const auto _c2py_init_16                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_16,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::legendre, triqs::mesh::brzone>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_16>    = c2py::pyfkw_constructor<_c2py_init_16>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_16> = _c2py_init_16.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_16 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_16 &self, const long &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_16 &self, const long &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_16 &self, const double &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_16 &self, const double &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_16 &self, const itertools::range::all_t &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::legendre, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_16 &self, const itertools::range::all_t &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::legendre, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_16 &self, const long &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const long &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_16 &self, const double &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const double &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_16> = c2py::pyfkw<_c2py_fun_16>;

static const auto _c2py_doc_16 = _c2py_fun_16.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_16>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_16> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_16>;
// --------- class _c2py_cls_17 -----------
using _c2py_cls_17 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::legendre, triqs::mesh::cyclat>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_17>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_17> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyLegendre_x_CycLat_3";
static const auto _c2py_init_17                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_17,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::legendre, triqs::mesh::cyclat>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_17>    = c2py::pyfkw_constructor<_c2py_init_17>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_17> = _c2py_init_17.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_17 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_17 &self, const long &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_17 &self, const long &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_17 &self, const double &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_17 &self, const double &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_17 &self, const itertools::range::all_t &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::legendre, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_17 &self, const itertools::range::all_t &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::legendre, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_17 &self, const long &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const long &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_17 &self, const double &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const double &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_17> = c2py::pyfkw<_c2py_fun_17>;

static const auto _c2py_doc_17 = _c2py_fun_17.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_17>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_17> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_17>;
// --------- class _c2py_cls_18 -----------
using _c2py_cls_18 = triqs::gfs::gf_proxy<triqs::gfs::gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_18>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_18> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyDLR_3";
static const auto _c2py_init_18                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_18, triqs::gfs::gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_18>    = c2py::pyfkw_constructor<_c2py_init_18>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_18> = _c2py_init_18.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_18 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_18 &self, const long &x1) -> decltype(auto) {
        return self.template
        operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>, const long &>(x1);
      },
      "self", "x1"),
   c2py::cmethod(
      [](_c2py_cls_18 &self, const double &x1) -> decltype(auto) {
        return self.template
        operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>, const double &>(x1);
      },
      "self", "x1"),
   c2py::cmethod(
      [](_c2py_cls_18 &self, const triqs::mesh::matsubara_freq &x1) -> decltype(auto) {
        return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                        const triqs::mesh::matsubara_freq &>(x1);
      },
      "self", "x1")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_18> = c2py::pyfkw<_c2py_fun_18>;

static const auto _c2py_doc_18 = _c2py_fun_18.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_18>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_18> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_18>;
// --------- class _c2py_cls_19 -----------
using _c2py_cls_19 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_19>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_19> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyDLR_x_BrZone_3";
static const auto _c2py_init_19                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_19, triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>(
   "gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_19>    = c2py::pyfkw_constructor<_c2py_init_19>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_19> = _c2py_init_19.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_19 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_19 &self, const long &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_19 &self, const long &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_19 &self, const double &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_19 &self, const double &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_19 &self, const triqs::mesh::matsubara_freq &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::mesh::matsubara_freq &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_19 &self, const triqs::mesh::matsubara_freq &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::mesh::matsubara_freq &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_19 &self, const itertools::range::all_t &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_19 &self, const itertools::range::all_t &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_19 &self, const long &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const long &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_19 &self, const double &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const double &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_19 &self, const triqs::mesh::matsubara_freq &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const triqs::mesh::matsubara_freq &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_19> = c2py::pyfkw<_c2py_fun_19>;

static const auto _c2py_doc_19 = _c2py_fun_19.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_19>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_19> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_19>;
// --------- class _c2py_cls_20 -----------
using _c2py_cls_20 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_20>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_20> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyDLR_x_CycLat_3";
static const auto _c2py_init_20                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_20, triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>(
   "gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_20>    = c2py::pyfkw_constructor<_c2py_init_20>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_20> = _c2py_init_20.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_20 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_20 &self, const long &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_20 &self, const long &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_20 &self, const double &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_20 &self, const double &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_20 &self, const triqs::mesh::matsubara_freq &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::mesh::matsubara_freq &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_20 &self, const triqs::mesh::matsubara_freq &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::mesh::matsubara_freq &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_20 &self, const itertools::range::all_t &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_20 &self, const itertools::range::all_t &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_20 &self, const long &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const long &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_20 &self, const double &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const double &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_20 &self, const triqs::mesh::matsubara_freq &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const triqs::mesh::matsubara_freq &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_20> = c2py::pyfkw<_c2py_fun_20>;

static const auto _c2py_doc_20 = _c2py_fun_20.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_20>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_20> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_20>;
// --------- class _c2py_cls_21 -----------
using _c2py_cls_21 = triqs::gfs::gf_proxy<triqs::gfs::gf_view<triqs::mesh::dlr2d, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_21>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_21> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyDLR2D_3";
static const auto _c2py_init_21                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_21, triqs::gfs::gf_view<triqs::mesh::dlr2d, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_21>    = c2py::pyfkw_constructor<_c2py_init_21>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_21> = _c2py_init_21.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_21 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_21 &self, const std::pair<triqs::mesh::matsubara_freq, triqs::mesh::matsubara_freq> &x1) -> decltype(auto) {
     return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                     const std::pair<triqs::mesh::matsubara_freq, triqs::mesh::matsubara_freq> &>(x1);
   },
   "self", "x1")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_21> = c2py::pyfkw<_c2py_fun_21>;

static const auto _c2py_doc_21 = _c2py_fun_21.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_21>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_21> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_21>;
// --------- class _c2py_cls_22 -----------
using _c2py_cls_22 = triqs::gfs::gf_proxy<triqs::gfs::gf_view<triqs::mesh::chebyshev, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_22>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_22> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyChebyshev_3";
static const auto _c2py_init_22                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_22, triqs::gfs::gf_view<triqs::mesh::chebyshev, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_22>    = c2py::pyfkw_constructor<_c2py_init_22>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_22> = _c2py_init_22.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_22 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_22 &self, const long &x1) -> decltype(auto) {
        return self.template
        operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>, const long &>(x1);
      },
      "self", "x1"),
   c2py::cmethod(
      [](_c2py_cls_22 &self, const double &x1) -> decltype(auto) {
        return self.template
        operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>, const double &>(x1);
      },
      "self", "x1")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_22> = c2py::pyfkw<_c2py_fun_22>;

static const auto _c2py_doc_22 = _c2py_fun_22.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_22>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_22> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_22>;
// --------- class _c2py_cls_23 -----------
using _c2py_cls_23 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::chebyshev, triqs::mesh::brzone>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_23>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_23> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyChebyshev_x_BrZone_3";
static const auto _c2py_init_23                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_23,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::chebyshev, triqs::mesh::brzone>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_23>    = c2py::pyfkw_constructor<_c2py_init_23>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_23> = _c2py_init_23.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_23 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_23 &self, const long &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_23 &self, const long &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_23 &self, const double &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_23 &self, const double &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_23 &self, const itertools::range::all_t &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::chebyshev, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_23 &self, const itertools::range::all_t &x1, const std::array<double, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::chebyshev, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<double, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_23 &self, const long &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const long &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_23 &self, const double &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const double &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_23> = c2py::pyfkw<_c2py_fun_23>;

static const auto _c2py_doc_23 = _c2py_fun_23.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_23>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_23> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_23>;
// --------- class _c2py_cls_24 -----------
using _c2py_cls_24 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::chebyshev, triqs::mesh::cyclat>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_24>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_24> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyChebyshev_x_CycLat_3";
static const auto _c2py_init_24                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_24,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::chebyshev, triqs::mesh::cyclat>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_24>    = c2py::pyfkw_constructor<_c2py_init_24>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_24> = _c2py_init_24.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_24 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_24 &self, const long &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_24 &self, const long &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const long &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_24 &self, const double &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_24 &self, const double &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const double &, const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_24 &self, const itertools::range::all_t &x1, const std::array<long, 3> &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::chebyshev, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const std::array<long, 3> &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_24 &self, const itertools::range::all_t &x1, const triqs::lattice::bravais_lattice::point_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::chebyshev, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const triqs::lattice::bravais_lattice::point_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_24 &self, const long &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const long &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_24 &self, const double &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const double &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_24> = c2py::pyfkw<_c2py_fun_24>;

static const auto _c2py_doc_24 = _c2py_fun_24.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_24>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_24> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_24>;
// --------- class _c2py_cls_25 -----------
using _c2py_cls_25 = triqs::gfs::gf_proxy<triqs::gfs::gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_25>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_25> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyBrZone_3";
static const auto _c2py_init_25                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_25, triqs::gfs::gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_25>    = c2py::pyfkw_constructor<_c2py_init_25>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_25> = _c2py_init_25.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_25 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_25 &self, const std::array<long, 3> &x1) -> decltype(auto) {
        return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                        const std::array<long, 3> &>(x1);
      },
      "self", "x1"),
   c2py::cmethod(
      [](_c2py_cls_25 &self, const std::array<double, 3> &x1) -> decltype(auto) {
        return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                        const std::array<double, 3> &>(x1);
      },
      "self", "x1")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_25> = c2py::pyfkw<_c2py_fun_25>;

static const auto _c2py_doc_25 = _c2py_fun_25.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_25>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_25> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_25>;
// --------- class _c2py_cls_26 -----------
using _c2py_cls_26 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::imfreq>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_26>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_26> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyBrZone_x_ImFreq_3";
static const auto _c2py_init_26                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_26,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::imfreq>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_26>    = c2py::pyfkw_constructor<_c2py_init_26>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_26> = _c2py_init_26.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_26 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_26 &self, const std::array<long, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_26 &self, const std::array<long, 3> &x1, const triqs::mesh::matsubara_freq &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const triqs::mesh::matsubara_freq &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_26 &self, const std::array<double, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_26 &self, const std::array<double, 3> &x1, const triqs::mesh::matsubara_freq &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const triqs::mesh::matsubara_freq &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_26 &self, const itertools::range::all_t &x1, const long &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_26 &self, const itertools::range::all_t &x1, const triqs::mesh::matsubara_freq &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const triqs::mesh::matsubara_freq &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_26 &self, const std::array<long, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>>, const std::array<long, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_26 &self, const std::array<double, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>>, const std::array<double, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_26> = c2py::pyfkw<_c2py_fun_26>;

static const auto _c2py_doc_26 = _c2py_fun_26.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_26>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_26> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_26>;
// --------- class _c2py_cls_27 -----------
using _c2py_cls_27 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::imtime>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_27>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_27> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyBrZone_x_ImTime_3";
static const auto _c2py_init_27                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_27,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::imtime>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_27>    = c2py::pyfkw_constructor<_c2py_init_27>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_27> = _c2py_init_27.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_27 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_27 &self, const std::array<long, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_27 &self, const std::array<long, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_27 &self, const std::array<double, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_27 &self, const std::array<double, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_27 &self, const itertools::range::all_t &x1, const long &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_27 &self, const itertools::range::all_t &x1, const double &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_27 &self, const std::array<long, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>>, const std::array<long, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_27 &self, const std::array<double, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>>, const std::array<double, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_27> = c2py::pyfkw<_c2py_fun_27>;

static const auto _c2py_doc_27 = _c2py_fun_27.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_27>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_27> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_27>;
// --------- class _c2py_cls_28 -----------
using _c2py_cls_28 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::imtime>, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_28>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_28> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyBrZone_x_ImTime_3_R";
static const auto _c2py_init_28                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_28,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::imtime>, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_28>    = c2py::pyfkw_constructor<_c2py_init_28>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_28> = _c2py_init_28.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_28 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_28 &self, const std::array<long, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_28 &self, const std::array<long, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_28 &self, const std::array<double, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_28 &self, const std::array<double, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_28 &self, const itertools::range::all_t &x1, const long &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_real_valued<3>>, const itertools::range::all_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_28 &self, const itertools::range::all_t &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_real_valued<3>>, const itertools::range::all_t &,
                                           const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_28 &self, const std::array<long, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>>, const std::array<long, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_28 &self, const std::array<double, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>>, const std::array<double, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_28> = c2py::pyfkw<_c2py_fun_28>;

static const auto _c2py_doc_28 = _c2py_fun_28.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_28>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_28> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_28>;
// --------- class _c2py_cls_29 -----------
using _c2py_cls_29 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::refreq>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_29>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_29> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyBrZone_x_ReFreq_3";
static const auto _c2py_init_29                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_29,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::refreq>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_29>    = c2py::pyfkw_constructor<_c2py_init_29>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_29> = _c2py_init_29.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_29 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_29 &self, const std::array<long, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_29 &self, const std::array<long, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_29 &self, const std::array<double, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_29 &self, const std::array<double, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_29 &self, const itertools::range::all_t &x1, const long &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_29 &self, const itertools::range::all_t &x1, const double &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_29 &self, const std::array<long, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>>, const std::array<long, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_29 &self, const std::array<double, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>>, const std::array<double, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_29> = c2py::pyfkw<_c2py_fun_29>;

static const auto _c2py_doc_29 = _c2py_fun_29.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_29>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_29> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_29>;
// --------- class _c2py_cls_30 -----------
using _c2py_cls_30 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::retime>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_30>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_30> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyBrZone_x_ReTime_3";
static const auto _c2py_init_30                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_30,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::retime>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_30>    = c2py::pyfkw_constructor<_c2py_init_30>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_30> = _c2py_init_30.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_30 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_30 &self, const std::array<long, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_30 &self, const std::array<long, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_30 &self, const std::array<double, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_30 &self, const std::array<double, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_30 &self, const itertools::range::all_t &x1, const long &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_30 &self, const itertools::range::all_t &x1, const double &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_30 &self, const std::array<long, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::retime, triqs::gfs::tensor_valued<3>>, const std::array<long, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_30 &self, const std::array<double, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::retime, triqs::gfs::tensor_valued<3>>, const std::array<double, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_30> = c2py::pyfkw<_c2py_fun_30>;

static const auto _c2py_doc_30 = _c2py_fun_30.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_30>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_30> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_30>;
// --------- class _c2py_cls_31 -----------
using _c2py_cls_31 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::legendre>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_31>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_31> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyBrZone_x_Legendre_3";
static const auto _c2py_init_31                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_31,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::legendre>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_31>    = c2py::pyfkw_constructor<_c2py_init_31>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_31> = _c2py_init_31.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_31 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_31 &self, const std::array<long, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_31 &self, const std::array<long, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_31 &self, const std::array<double, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_31 &self, const std::array<double, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_31 &self, const itertools::range::all_t &x1, const long &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_31 &self, const itertools::range::all_t &x1, const double &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_31 &self, const std::array<long, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::legendre, triqs::gfs::tensor_valued<3>>, const std::array<long, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_31 &self, const std::array<double, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::legendre, triqs::gfs::tensor_valued<3>>, const std::array<double, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_31> = c2py::pyfkw<_c2py_fun_31>;

static const auto _c2py_doc_31 = _c2py_fun_31.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_31>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_31> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_31>;
// --------- class _c2py_cls_32 -----------
using _c2py_cls_32 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_32>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_32> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyBrZone_x_DLR_3";
static const auto _c2py_init_32                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_32, triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>(
   "gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_32>    = c2py::pyfkw_constructor<_c2py_init_32>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_32> = _c2py_init_32.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_32 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_32 &self, const std::array<long, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_32 &self, const std::array<long, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_32 &self, const std::array<long, 3> &x1, const triqs::mesh::matsubara_freq &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const triqs::mesh::matsubara_freq &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_32 &self, const std::array<double, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_32 &self, const std::array<double, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_32 &self, const std::array<double, 3> &x1, const triqs::mesh::matsubara_freq &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const triqs::mesh::matsubara_freq &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_32 &self, const itertools::range::all_t &x1, const long &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_32 &self, const itertools::range::all_t &x1, const double &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_32 &self, const itertools::range::all_t &x1, const triqs::mesh::matsubara_freq &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const triqs::mesh::matsubara_freq &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_32 &self, const std::array<long, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>>, const std::array<long, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_32 &self, const std::array<double, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>>, const std::array<double, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_32> = c2py::pyfkw<_c2py_fun_32>;

static const auto _c2py_doc_32 = _c2py_fun_32.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_32>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_32> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_32>;
// --------- class _c2py_cls_33 -----------
using _c2py_cls_33 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::chebyshev>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_33>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_33> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyBrZone_x_Chebyshev_3";
static const auto _c2py_init_33                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_33,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::chebyshev>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_33>    = c2py::pyfkw_constructor<_c2py_init_33>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_33> = _c2py_init_33.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_33 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_33 &self, const std::array<long, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_33 &self, const std::array<long, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_33 &self, const std::array<double, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_33 &self, const std::array<double, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<double, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_33 &self, const itertools::range::all_t &x1, const long &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_33 &self, const itertools::range::all_t &x1, const double &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_33 &self, const std::array<long, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::chebyshev, triqs::gfs::tensor_valued<3>>, const std::array<long, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_33 &self, const std::array<double, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::chebyshev, triqs::gfs::tensor_valued<3>>, const std::array<double, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_33> = c2py::pyfkw<_c2py_fun_33>;

static const auto _c2py_doc_33 = _c2py_fun_33.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_33>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_33> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_33>;
// --------- class _c2py_cls_34 -----------
using _c2py_cls_34 = triqs::gfs::gf_proxy<triqs::gfs::gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_34>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_34> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyCycLat_3";
static const auto _c2py_init_34                               = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_34, triqs::gfs::gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_34>    = c2py::pyfkw_constructor<_c2py_init_34>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_34> = _c2py_init_34.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_34 = c2py::dispatcher_f_kw_t{
   c2py::cmethod(
      [](_c2py_cls_34 &self, const std::array<long, 3> &x1) -> decltype(auto) {
        return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                        const std::array<long, 3> &>(x1);
      },
      "self", "x1"),
   c2py::cmethod(
      [](_c2py_cls_34 &self, const triqs::lattice::bravais_lattice::point_t &x1) -> decltype(auto) {
        return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                        const triqs::lattice::bravais_lattice::point_t &>(x1);
      },
      "self", "x1")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_34> = c2py::pyfkw<_c2py_fun_34>;

static const auto _c2py_doc_34 = _c2py_fun_34.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_34>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_34> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_34>;
// --------- class _c2py_cls_35 -----------
using _c2py_cls_35 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::imfreq>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_35>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_35> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyCycLat_x_ImFreq_3";
static const auto _c2py_init_35                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_35,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::imfreq>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_35>    = c2py::pyfkw_constructor<_c2py_init_35>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_35> = _c2py_init_35.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_35 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_35 &self, const std::array<long, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_35 &self, const std::array<long, 3> &x1, const triqs::mesh::matsubara_freq &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const triqs::mesh::matsubara_freq &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_35 &self, const triqs::lattice::bravais_lattice::point_t &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_35 &self, const triqs::lattice::bravais_lattice::point_t &x1, const triqs::mesh::matsubara_freq &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const triqs::mesh::matsubara_freq &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_35 &self, const itertools::range::all_t &x1, const long &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_35 &self, const itertools::range::all_t &x1, const triqs::mesh::matsubara_freq &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const triqs::mesh::matsubara_freq &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_35 &self, const std::array<long, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>>, const std::array<long, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_35 &self, const triqs::lattice::bravais_lattice::point_t &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_35> = c2py::pyfkw<_c2py_fun_35>;

static const auto _c2py_doc_35 = _c2py_fun_35.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_35>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_35> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_35>;
// --------- class _c2py_cls_36 -----------
using _c2py_cls_36 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::imtime>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_36>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_36> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyCycLat_x_ImTime_3";
static const auto _c2py_init_36                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_36,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::imtime>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_36>    = c2py::pyfkw_constructor<_c2py_init_36>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_36> = _c2py_init_36.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_36 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_36 &self, const std::array<long, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_36 &self, const std::array<long, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_36 &self, const triqs::lattice::bravais_lattice::point_t &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_36 &self, const triqs::lattice::bravais_lattice::point_t &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_36 &self, const itertools::range::all_t &x1, const long &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_36 &self, const itertools::range::all_t &x1, const double &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_36 &self, const std::array<long, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>>, const std::array<long, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_36 &self, const triqs::lattice::bravais_lattice::point_t &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_36> = c2py::pyfkw<_c2py_fun_36>;

static const auto _c2py_doc_36 = _c2py_fun_36.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_36>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_36> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_36>;
// --------- class _c2py_cls_37 -----------
using _c2py_cls_37 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::imtime>, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_37>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_37> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyCycLat_x_ImTime_3_R";
static const auto _c2py_init_37                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_37,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::imtime>, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_37>    = c2py::pyfkw_constructor<_c2py_init_37>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_37> = _c2py_init_37.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_37 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_37 &self, const std::array<long, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_37 &self, const std::array<long, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_37 &self, const triqs::lattice::bravais_lattice::point_t &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_37 &self, const triqs::lattice::bravais_lattice::point_t &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_37 &self, const itertools::range::all_t &x1, const long &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_real_valued<3>>, const itertools::range::all_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_37 &self, const itertools::range::all_t &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_real_valued<3>>, const itertools::range::all_t &,
                                           const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_37 &self, const std::array<long, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>>, const std::array<long, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_37 &self, const triqs::lattice::bravais_lattice::point_t &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_37> = c2py::pyfkw<_c2py_fun_37>;

static const auto _c2py_doc_37 = _c2py_fun_37.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_37>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_37> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_37>;
// --------- class _c2py_cls_38 -----------
using _c2py_cls_38 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::refreq>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_38>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_38> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyCycLat_x_ReFreq_3";
static const auto _c2py_init_38                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_38,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::refreq>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_38>    = c2py::pyfkw_constructor<_c2py_init_38>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_38> = _c2py_init_38.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_38 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_38 &self, const std::array<long, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_38 &self, const std::array<long, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_38 &self, const triqs::lattice::bravais_lattice::point_t &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_38 &self, const triqs::lattice::bravais_lattice::point_t &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_38 &self, const itertools::range::all_t &x1, const long &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_38 &self, const itertools::range::all_t &x1, const double &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_38 &self, const std::array<long, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>>, const std::array<long, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_38 &self, const triqs::lattice::bravais_lattice::point_t &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_38> = c2py::pyfkw<_c2py_fun_38>;

static const auto _c2py_doc_38 = _c2py_fun_38.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_38>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_38> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_38>;
// --------- class _c2py_cls_39 -----------
using _c2py_cls_39 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::retime>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_39>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_39> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyCycLat_x_ReTime_3";
static const auto _c2py_init_39                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_39,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::retime>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_39>    = c2py::pyfkw_constructor<_c2py_init_39>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_39> = _c2py_init_39.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_39 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_39 &self, const std::array<long, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_39 &self, const std::array<long, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_39 &self, const triqs::lattice::bravais_lattice::point_t &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_39 &self, const triqs::lattice::bravais_lattice::point_t &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_39 &self, const itertools::range::all_t &x1, const long &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_39 &self, const itertools::range::all_t &x1, const double &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_39 &self, const std::array<long, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::retime, triqs::gfs::tensor_valued<3>>, const std::array<long, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_39 &self, const triqs::lattice::bravais_lattice::point_t &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::retime, triqs::gfs::tensor_valued<3>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_39> = c2py::pyfkw<_c2py_fun_39>;

static const auto _c2py_doc_39 = _c2py_fun_39.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_39>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_39> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_39>;
// --------- class _c2py_cls_40 -----------
using _c2py_cls_40 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::legendre>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_40>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_40> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyCycLat_x_Legendre_3";
static const auto _c2py_init_40                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_40,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::legendre>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_40>    = c2py::pyfkw_constructor<_c2py_init_40>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_40> = _c2py_init_40.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_40 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_40 &self, const std::array<long, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_40 &self, const std::array<long, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_40 &self, const triqs::lattice::bravais_lattice::point_t &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_40 &self, const triqs::lattice::bravais_lattice::point_t &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_40 &self, const itertools::range::all_t &x1, const long &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_40 &self, const itertools::range::all_t &x1, const double &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_40 &self, const std::array<long, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::legendre, triqs::gfs::tensor_valued<3>>, const std::array<long, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_40 &self, const triqs::lattice::bravais_lattice::point_t &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::legendre, triqs::gfs::tensor_valued<3>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_40> = c2py::pyfkw<_c2py_fun_40>;

static const auto _c2py_doc_40 = _c2py_fun_40.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_40>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_40> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_40>;
// --------- class _c2py_cls_41 -----------
using _c2py_cls_41 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_41>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_41> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyCycLat_x_DLR_3";
static const auto _c2py_init_41                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_41, triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>(
   "gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_41>    = c2py::pyfkw_constructor<_c2py_init_41>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_41> = _c2py_init_41.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_41 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_41 &self, const std::array<long, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_41 &self, const std::array<long, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_41 &self, const std::array<long, 3> &x1, const triqs::mesh::matsubara_freq &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const triqs::mesh::matsubara_freq &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_41 &self, const triqs::lattice::bravais_lattice::point_t &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_41 &self, const triqs::lattice::bravais_lattice::point_t &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_41 &self, const triqs::lattice::bravais_lattice::point_t &x1, const triqs::mesh::matsubara_freq &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const triqs::mesh::matsubara_freq &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_41 &self, const itertools::range::all_t &x1, const long &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_41 &self, const itertools::range::all_t &x1, const double &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_41 &self, const itertools::range::all_t &x1, const triqs::mesh::matsubara_freq &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &,
                                           const triqs::mesh::matsubara_freq &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_41 &self, const std::array<long, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>>, const std::array<long, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_41 &self, const triqs::lattice::bravais_lattice::point_t &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_41> = c2py::pyfkw<_c2py_fun_41>;

static const auto _c2py_doc_41 = _c2py_fun_41.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_41>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_41> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_41>;
// --------- class _c2py_cls_42 -----------
using _c2py_cls_42 = triqs::gfs::gf_proxy<
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::chebyshev>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>;
template <> constexpr bool c2py::is_wrapped<_c2py_cls_42>     = true;
template <> inline constexpr auto c2py::tp_name<_c2py_cls_42> = "triqs.gfs.wrapped_aux_target_rank_3.CallProxyCycLat_x_Chebyshev_3";
static const auto _c2py_init_42                               = c2py::dispatcher_c_kw_t{c2py::c_constructor<
   _c2py_cls_42,
   triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::chebyshev>, triqs::gfs::tensor_valued<3>, nda::C_stride_layout>>("gv")};
template <> constexpr initproc c2py::tp_init<_c2py_cls_42>    = c2py::pyfkw_constructor<_c2py_init_42>;
template <> const std::string c2py::tp_ctor_doc<_c2py_cls_42> = _c2py_init_42.doc(R"DOC()DOC");
// __call__
static auto const _c2py_fun_42 =
   c2py::dispatcher_f_kw_t{
      c2py::cmethod(
         [](_c2py_cls_42 &self, const std::array<long, 3> &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_42 &self, const std::array<long, 3> &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const std::array<long, 3> &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_42 &self, const triqs::lattice::bravais_lattice::point_t &x1, const long &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_42 &self, const triqs::lattice::bravais_lattice::point_t &x1, const double &x2) -> decltype(auto) {
           return self.template operator()<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A', nda::heap_basic<nda::mem::mallocator<>>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_42 &self, const itertools::range::all_t &x1, const long &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const long &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_42 &self, const itertools::range::all_t &x1, const double &x2) -> decltype(auto) {
           return self.template
           operator()<triqs::gfs::gf<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>>, const itertools::range::all_t &, const double &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_42 &self, const std::array<long, 3> &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::chebyshev, triqs::gfs::tensor_valued<3>>, const std::array<long, 3> &,
                                           const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2"),
      c2py::cmethod(
         [](_c2py_cls_42 &self, const triqs::lattice::bravais_lattice::point_t &x1, const itertools::range::all_t &x2) -> decltype(auto) {
           return self.template operator()<triqs::gfs::gf<triqs::mesh::chebyshev, triqs::gfs::tensor_valued<3>>,
                                           const triqs::lattice::bravais_lattice::point_t &, const itertools::range::all_t &>(x1, x2);
         },
         "self", "x1", "x2")};

template <> inline constexpr ternaryfunc c2py::tp_call<_c2py_cls_42> = c2py::pyfkw<_c2py_fun_42>;

static const auto _c2py_doc_42 = _c2py_fun_42.doc(R"DOC()DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_42>[] = {

   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <> const std::string c2py::tp_doc<_c2py_cls_42> = R"DOC()DOC" + c2py::tp_ctor_doc<_c2py_cls_42>;

// ==================== module functions ====================

//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {
   PyModuleDef_HEAD_INIT,
   "wrapped_aux_target_rank_3", /* name of module */
   R"RAWDOC(Internal ``CallProxy`` dispatchers that implement ``Gf.__call__`` for target rank 3. Not intended for direct use.)RAWDOC", /* module documentation, may be NULL */
   -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
   module_methods,
   NULL,
   NULL,
   NULL,
   NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_wrapped_aux_target_rank_3() {

  if (not c2py::check_python_version("wrapped_aux_target_rank_3")) return NULL;

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
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_16>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_17>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_18>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_19>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_20>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_21>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_22>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_23>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_24>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_25>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_26>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_27>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_28>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_29>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_30>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_31>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_32>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_33>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_34>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_35>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_36>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_37>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_38>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_39>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_40>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_41>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_42>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
#define _add_type(T, N) c2py::add_type_object_to_main<T>(N, m, conv_table)
  _add_type(_c2py_cls_0, "CallProxyImFreq_3");
  _add_type(_c2py_cls_1, "CallProxyImFreq_x_BrZone_3");
  _add_type(_c2py_cls_2, "CallProxyImFreq_x_CycLat_3");
  _add_type(_c2py_cls_3, "CallProxyImTime_3");
  _add_type(_c2py_cls_4, "CallProxyImTime_3_R");
  _add_type(_c2py_cls_5, "CallProxyImTime_x_BrZone_3");
  _add_type(_c2py_cls_6, "CallProxyImTime_x_BrZone_3_R");
  _add_type(_c2py_cls_7, "CallProxyImTime_x_CycLat_3");
  _add_type(_c2py_cls_8, "CallProxyImTime_x_CycLat_3_R");
  _add_type(_c2py_cls_9, "CallProxyReFreq_3");
  _add_type(_c2py_cls_10, "CallProxyReFreq_x_BrZone_3");
  _add_type(_c2py_cls_11, "CallProxyReFreq_x_CycLat_3");
  _add_type(_c2py_cls_12, "CallProxyReTime_3");
  _add_type(_c2py_cls_13, "CallProxyReTime_x_BrZone_3");
  _add_type(_c2py_cls_14, "CallProxyReTime_x_CycLat_3");
  _add_type(_c2py_cls_15, "CallProxyLegendre_3");
  _add_type(_c2py_cls_16, "CallProxyLegendre_x_BrZone_3");
  _add_type(_c2py_cls_17, "CallProxyLegendre_x_CycLat_3");
  _add_type(_c2py_cls_18, "CallProxyDLR_3");
  _add_type(_c2py_cls_19, "CallProxyDLR_x_BrZone_3");
  _add_type(_c2py_cls_20, "CallProxyDLR_x_CycLat_3");
  _add_type(_c2py_cls_21, "CallProxyDLR2D_3");
  _add_type(_c2py_cls_22, "CallProxyChebyshev_3");
  _add_type(_c2py_cls_23, "CallProxyChebyshev_x_BrZone_3");
  _add_type(_c2py_cls_24, "CallProxyChebyshev_x_CycLat_3");
  _add_type(_c2py_cls_25, "CallProxyBrZone_3");
  _add_type(_c2py_cls_26, "CallProxyBrZone_x_ImFreq_3");
  _add_type(_c2py_cls_27, "CallProxyBrZone_x_ImTime_3");
  _add_type(_c2py_cls_28, "CallProxyBrZone_x_ImTime_3_R");
  _add_type(_c2py_cls_29, "CallProxyBrZone_x_ReFreq_3");
  _add_type(_c2py_cls_30, "CallProxyBrZone_x_ReTime_3");
  _add_type(_c2py_cls_31, "CallProxyBrZone_x_Legendre_3");
  _add_type(_c2py_cls_32, "CallProxyBrZone_x_DLR_3");
  _add_type(_c2py_cls_33, "CallProxyBrZone_x_Chebyshev_3");
  _add_type(_c2py_cls_34, "CallProxyCycLat_3");
  _add_type(_c2py_cls_35, "CallProxyCycLat_x_ImFreq_3");
  _add_type(_c2py_cls_36, "CallProxyCycLat_x_ImTime_3");
  _add_type(_c2py_cls_37, "CallProxyCycLat_x_ImTime_3_R");
  _add_type(_c2py_cls_38, "CallProxyCycLat_x_ReFreq_3");
  _add_type(_c2py_cls_39, "CallProxyCycLat_x_ReTime_3");
  _add_type(_c2py_cls_40, "CallProxyCycLat_x_Legendre_3");
  _add_type(_c2py_cls_41, "CallProxyCycLat_x_DLR_3");
  _add_type(_c2py_cls_42, "CallProxyCycLat_x_Chebyshev_3");
#undef _add_type

  return m;
}
#endif
// CLAIR_WRAP_GEN
