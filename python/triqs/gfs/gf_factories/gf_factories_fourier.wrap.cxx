
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

// make_gf_from_fourier
static auto const _c2py_fun_0 =
   c2py::dispatcher_f_kw_t{
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_valued> gin, const triqs::mesh::imfreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::imtime, triqs::mesh::imfreq, triqs::gfs::scalar_valued,
                                                   nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued> gin, const triqs::mesh::imfreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::imtime, triqs::mesh::imfreq, triqs::gfs::scalar_real_valued,
                                                   nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> gin, const triqs::mesh::imtime &mesh,
            const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::imfreq, triqs::mesh::imtime, triqs::gfs::scalar_valued,
                                                   nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::scalar_valued> gin, const triqs::mesh::refreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::retime, triqs::mesh::refreq, triqs::gfs::scalar_valued,
                                                   nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued> gin, const triqs::mesh::refreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::retime, triqs::mesh::refreq, triqs::gfs::scalar_real_valued,
                                                   nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::scalar_valued> gin, const triqs::mesh::retime &mesh,
            const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::refreq, triqs::mesh::retime, triqs::gfs::scalar_valued,
                                                   nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>> gin, const triqs::mesh::imfreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::imtime, triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>,
                                                   nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>> gin, const triqs::mesh::imfreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::imtime, triqs::mesh::imfreq, triqs::gfs::tensor_real_valued<1>,
                                                   nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>> gin, const triqs::mesh::imtime &mesh,
            const nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::imfreq, triqs::mesh::imtime, triqs::gfs::tensor_valued<1>,
                                                   nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>> gin, const triqs::mesh::refreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::retime, triqs::mesh::refreq, triqs::gfs::tensor_valued<1>,
                                                   nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>> gin, const triqs::mesh::refreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::retime, triqs::mesh::refreq, triqs::gfs::tensor_real_valued<1>,
                                                   nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>> gin, const triqs::mesh::retime &mesh,
            const nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::refreq, triqs::mesh::retime, triqs::gfs::tensor_valued<1>,
                                                   nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::matrix_valued> gin, const triqs::mesh::imfreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::imtime, triqs::mesh::imfreq, triqs::gfs::matrix_valued,
                                                   nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued> gin, const triqs::mesh::imfreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::imtime, triqs::mesh::imfreq, triqs::gfs::matrix_real_valued,
                                                   nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> gin, const triqs::mesh::imtime &mesh,
            const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::imfreq, triqs::mesh::imtime, triqs::gfs::matrix_valued,
                                                   nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::matrix_valued> gin, const triqs::mesh::refreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::retime, triqs::mesh::refreq, triqs::gfs::matrix_valued,
                                                   nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued> gin, const triqs::mesh::refreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::retime, triqs::mesh::refreq, triqs::gfs::matrix_real_valued,
                                                   nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::matrix_valued> gin, const triqs::mesh::retime &mesh,
            const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::refreq, triqs::mesh::retime, triqs::gfs::matrix_valued,
                                                   nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>> gin, const triqs::mesh::imfreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::imtime, triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>,
                                                   nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>> gin, const triqs::mesh::imfreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::imtime, triqs::mesh::imfreq, triqs::gfs::tensor_real_valued<3>,
                                                   nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>> gin, const triqs::mesh::imtime &mesh,
            const nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::imfreq, triqs::mesh::imtime, triqs::gfs::tensor_valued<3>,
                                                   nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>> gin, const triqs::mesh::refreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::retime, triqs::mesh::refreq, triqs::gfs::tensor_valued<3>,
                                                   nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>> gin, const triqs::mesh::refreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::retime, triqs::mesh::refreq, triqs::gfs::tensor_real_valued<3>,
                                                   nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>> gin, const triqs::mesh::retime &mesh,
            const nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::refreq, triqs::mesh::retime, triqs::gfs::tensor_valued<3>,
                                                   nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>> gin, const triqs::mesh::imfreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::imtime, triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>,
                                                   nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>> gin, const triqs::mesh::imfreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::imtime, triqs::mesh::imfreq, triqs::gfs::tensor_real_valued<4>,
                                                   nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>> gin, const triqs::mesh::imtime &mesh,
            const nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::imfreq, triqs::mesh::imtime, triqs::gfs::tensor_valued<4>,
                                                   nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>> gin, const triqs::mesh::refreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::retime, triqs::mesh::refreq, triqs::gfs::tensor_valued<4>,
                                                   nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>> gin, const triqs::mesh::refreq &mesh,
            const nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::retime, triqs::mesh::refreq, triqs::gfs::tensor_real_valued<4>,
                                                   nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>> gin, const triqs::mesh::retime &mesh,
            const nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &opt_args) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::mesh::refreq, triqs::mesh::retime, triqs::gfs::tensor_valued<4>,
                                                   nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout>>(gin, mesh, opt_args);
         },
         "gin", "mesh", "opt_args"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::brzone, triqs::gfs::scalar_valued> gin) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::scalar_valued>(gin);
         },
         "gin"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<1>> gin) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<1>>(gin);
         },
         "gin"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::brzone, triqs::gfs::matrix_valued> gin) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::matrix_valued>(gin);
         },
         "gin"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>> gin) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<3>>(gin);
         },
         "gin"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<4>> gin) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<4>>(gin);
         },
         "gin"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::cyclat, triqs::gfs::scalar_valued> gin) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::scalar_valued>(gin);
         },
         "gin"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<1>> gin) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<1>>(gin);
         },
         "gin"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::cyclat, triqs::gfs::matrix_valued> gin) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::matrix_valued>(gin);
         },
         "gin"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>> gin) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<3>>(gin);
         },
         "gin"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<4>> gin) {
           return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<4>>(gin);
         },
         "gin"),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> gin,
                    int n_tau) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::scalar_valued>(gin, n_tau); },
                 "gin", "n_tau"_a = -1),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>> gin,
                    int n_tau) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<1>>(gin, n_tau); },
                 "gin", "n_tau"_a = -1),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> gin,
                    int n_tau) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::matrix_valued>(gin, n_tau); },
                 "gin", "n_tau"_a = -1),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>> gin,
                    int n_tau) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<3>>(gin, n_tau); },
                 "gin", "n_tau"_a = -1),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>> gin,
                    int n_tau) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<4>>(gin, n_tau); },
                 "gin", "n_tau"_a = -1),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_valued> gin,
                    int n_iw) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::scalar_valued>(gin, n_iw); },
                 "gin", "n_iw"_a = -1),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued> gin,
                    int n_iw) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::scalar_real_valued>(gin, n_iw); },
                 "gin", "n_iw"_a = -1),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>> gin,
                    int n_iw) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<1>>(gin, n_iw); },
                 "gin", "n_iw"_a = -1),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>> gin,
                    int n_iw) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_real_valued<1>>(gin, n_iw); },
                 "gin", "n_iw"_a = -1),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::matrix_valued> gin,
                    int n_iw) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::matrix_valued>(gin, n_iw); },
                 "gin", "n_iw"_a = -1),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued> gin,
                    int n_iw) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::matrix_real_valued>(gin, n_iw); },
                 "gin", "n_iw"_a = -1),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>> gin,
                    int n_iw) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<3>>(gin, n_iw); },
                 "gin", "n_iw"_a = -1),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>> gin,
                    int n_iw) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_real_valued<3>>(gin, n_iw); },
                 "gin", "n_iw"_a = -1),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>> gin,
                    int n_iw) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<4>>(gin, n_iw); },
                 "gin", "n_iw"_a = -1),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>> gin,
                    int n_iw) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_real_valued<4>>(gin, n_iw); },
                 "gin", "n_iw"_a = -1),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::scalar_valued> gin,
                    bool shift_half_bin) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::scalar_valued>(gin, shift_half_bin); },
                 "gin", "shift_half_bin"_a = false),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>> gin,
                    bool shift_half_bin) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<1>>(gin, shift_half_bin); },
                 "gin", "shift_half_bin"_a = false),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::matrix_valued> gin,
                    bool shift_half_bin) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::matrix_valued>(gin, shift_half_bin); },
                 "gin", "shift_half_bin"_a = false),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>> gin,
                    bool shift_half_bin) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<3>>(gin, shift_half_bin); },
                 "gin", "shift_half_bin"_a = false),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>> gin,
                    bool shift_half_bin) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<4>>(gin, shift_half_bin); },
                 "gin", "shift_half_bin"_a = false),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::scalar_valued> gin,
                    bool shift_half_bin) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::scalar_valued>(gin, shift_half_bin); },
                 "gin", "shift_half_bin"_a = false),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued> gin,
                    bool shift_half_bin) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::scalar_real_valued>(gin, shift_half_bin); },
                 "gin", "shift_half_bin"_a = false),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>> gin,
                    bool shift_half_bin) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<1>>(gin, shift_half_bin); },
                 "gin", "shift_half_bin"_a = false),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>> gin,
                    bool shift_half_bin) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_real_valued<1>>(gin, shift_half_bin); },
                 "gin", "shift_half_bin"_a = false),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::matrix_valued> gin,
                    bool shift_half_bin) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::matrix_valued>(gin, shift_half_bin); },
                 "gin", "shift_half_bin"_a = false),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued> gin,
                    bool shift_half_bin) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::matrix_real_valued>(gin, shift_half_bin); },
                 "gin", "shift_half_bin"_a = false),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>> gin,
                    bool shift_half_bin) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<3>>(gin, shift_half_bin); },
                 "gin", "shift_half_bin"_a = false),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>> gin,
                    bool shift_half_bin) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_real_valued<3>>(gin, shift_half_bin); },
                 "gin", "shift_half_bin"_a = false),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>> gin,
                    bool shift_half_bin) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_valued<4>>(gin, shift_half_bin); },
                 "gin", "shift_half_bin"_a = false),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>> gin,
                    bool shift_half_bin) { return triqs::gfs::make_gf_from_fourier<0, triqs::gfs::tensor_real_valued<4>>(gin, shift_half_bin); },
                 "gin", "shift_half_bin"_a = false),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>, triqs::mesh::imfreq, 1>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 1, true>, triqs::mesh::imfreq,
              1>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::imtime &m,
            const std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>, triqs::mesh::imtime, 1>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>, triqs::mesh::refreq, 1>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 1, true>, triqs::mesh::refreq,
              1>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::retime &m,
            const std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>, triqs::mesh::retime, 1>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true>, triqs::mesh::imfreq, 2>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 1, true>,
              triqs::mesh::imfreq, 2>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::imtime &m,
            const std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true>, triqs::mesh::imtime, 2>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true>, triqs::mesh::refreq, 2>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 1, true>,
              triqs::mesh::refreq, 2>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::retime &m,
            const std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true>, triqs::mesh::retime, 2>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>, triqs::mesh::imfreq, 3>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 1, true>, triqs::mesh::imfreq,
              3>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::imtime &m,
            const std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>, triqs::mesh::imtime, 3>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>, triqs::mesh::refreq, 3>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 1, true>, triqs::mesh::refreq,
              3>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::retime &m,
            const std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>, triqs::mesh::retime, 3>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true>, triqs::mesh::imfreq, 4>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 1, true>,
              triqs::mesh::imfreq, 4>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::imtime &m,
            const std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true>, triqs::mesh::imtime, 4>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true>, triqs::mesh::refreq, 4>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 1, true>,
              triqs::mesh::refreq, 4>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::retime &m,
            const std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true>, triqs::mesh::retime, 4>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true>, triqs::mesh::imfreq, 5>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 1, true>,
              triqs::mesh::imfreq, 5>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::imtime &m,
            const std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true>, triqs::mesh::imtime, 5>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true>, triqs::mesh::refreq, 5>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 1, true>,
              triqs::mesh::refreq, 5>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &gin,
            const triqs::mesh::retime &m,
            const std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true>, triqs::mesh::retime, 5>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true>, triqs::mesh::imfreq, 1>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 2, true>, triqs::mesh::imfreq,
              1>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::imtime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true>, triqs::mesh::imtime, 1>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true>, triqs::mesh::refreq, 1>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 2, true>, triqs::mesh::refreq,
              1>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::retime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true>, triqs::mesh::retime, 1>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true>, triqs::mesh::imfreq, 2>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 2, true>,
              triqs::mesh::imfreq, 2>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::imtime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true>, triqs::mesh::imtime, 2>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true>, triqs::mesh::refreq, 2>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 2, true>,
              triqs::mesh::refreq, 2>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::retime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true>, triqs::mesh::retime, 2>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true>, triqs::mesh::imfreq, 3>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 2, true>, triqs::mesh::imfreq,
              3>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::imtime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true>, triqs::mesh::imtime, 3>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true>, triqs::mesh::refreq, 3>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 2, true>, triqs::mesh::refreq,
              3>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::retime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true>, triqs::mesh::retime, 3>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true>, triqs::mesh::imfreq, 4>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 2, true>,
              triqs::mesh::imfreq, 4>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::imtime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true>, triqs::mesh::imtime, 4>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true>, triqs::mesh::refreq, 4>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 2, true>,
              triqs::mesh::refreq, 4>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::retime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true>, triqs::mesh::retime, 4>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true>, triqs::mesh::imfreq, 5>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 2, true>,
              triqs::mesh::imfreq, 5>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::imtime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true>, triqs::mesh::imtime, 5>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true>, triqs::mesh::refreq, 5>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 2, true>,
              triqs::mesh::refreq, 5>(gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &gin,
            const triqs::mesh::retime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &known_moments) {
           return triqs::gfs::make_gf_from_fourier<
              0, triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true>, triqs::mesh::retime, 5>(
              gin, m, known_moments);
         },
         "gin", "m", "known_moments"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 1, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 2, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 1, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 2, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 1, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 2, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 1, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 2, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 1, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 2, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 1, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 2, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 1, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 2, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 1, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 2, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 1, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 2, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &gin,
                    const int &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 1, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 2, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &gin,
                    const bool &args) { return triqs::gfs::make_gf_from_fourier<0>(gin, args); },
                 "gin", "args"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &gin) {
           return triqs::gfs::make_gf_from_fourier<0>(gin);
         },
         "gin"),
      c2py::cfun(
         [](triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr_imfreq>, triqs::gfs::scalar_valued> g_in) {
           return triqs::gfs::make_gf_from_fourier(g_in);
         },
         "g_in"),
      c2py::cfun(
         [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr_imfreq>, triqs::gfs::scalar_valued> g_in) {
           return triqs::gfs::make_gf_from_fourier(g_in);
         },
         "g_in"),
      c2py::cfun(
         [](triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr_imtime>, triqs::gfs::scalar_valued> g_in) {
           return triqs::gfs::make_gf_from_fourier(g_in);
         },
         "g_in"),
      c2py::cfun(
         [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr_imtime>, triqs::gfs::scalar_valued> g_in) {
           return triqs::gfs::make_gf_from_fourier(g_in);
         },
         "g_in"),
      c2py::cfun(
         [](triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr_imfreq>, triqs::gfs::matrix_valued> g_in) {
           return triqs::gfs::make_gf_from_fourier(g_in);
         },
         "g_in"),
      c2py::cfun(
         [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr_imfreq>, triqs::gfs::matrix_valued> g_in) {
           return triqs::gfs::make_gf_from_fourier(g_in);
         },
         "g_in"),
      c2py::cfun(
         [](triqs::gfs::gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr_imtime>, triqs::gfs::matrix_valued> g_in) {
           return triqs::gfs::make_gf_from_fourier(g_in);
         },
         "g_in"),
      c2py::cfun(
         [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr_imtime>, triqs::gfs::matrix_valued> g_in) {
           return triqs::gfs::make_gf_from_fourier(g_in);
         },
         "g_in")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC()DOC");
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"make_gf_from_fourier", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "gf_factories_fourier", /* name of module */
                                        R"RAWDOC()RAWDOC",      /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_gf_factories_fourier() {

  if (not c2py::check_python_version("gf_factories_fourier")) return NULL;

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
