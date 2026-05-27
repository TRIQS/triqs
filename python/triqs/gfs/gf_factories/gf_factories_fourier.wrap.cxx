
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
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 1, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 2, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 1, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 2, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_valued> g, const triqs::mesh::imfreq &m,
            const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued> g, const triqs::mesh::imfreq &m,
            const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> g, const triqs::mesh::imtime &m,
            const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::imtime &m,
            const std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::imtime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::scalar_valued> g, const triqs::mesh::refreq &m,
            const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued> g, const triqs::mesh::refreq &m,
            const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::scalar_valued> g, const triqs::mesh::retime &m,
            const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::retime &m,
            const std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::retime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 1, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::brzone, triqs::gfs::scalar_valued> g) { return triqs::gfs::make_gf_from_fourier(g); },
                 "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::cyclat, triqs::gfs::scalar_valued> g) { return triqs::gfs::make_gf_from_fourier(g); },
                 "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 1, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 2, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 1, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 2, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>> g, const triqs::mesh::imfreq &m,
            const nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>> g, const triqs::mesh::imfreq &m,
            const nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>> g, const triqs::mesh::imtime &m,
            const nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::imtime &m,
            const std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::imtime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>> g, const triqs::mesh::refreq &m,
            const nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>> g, const triqs::mesh::refreq &m,
            const nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>> g, const triqs::mesh::retime &m,
            const nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::retime &m,
            const std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::retime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 2, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<1>> g) { return triqs::gfs::make_gf_from_fourier(g); },
                 "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<1>> g) { return triqs::gfs::make_gf_from_fourier(g); },
                 "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 1, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 2, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 1, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 2, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::matrix_valued> g, const triqs::mesh::imfreq &m,
            const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued> g, const triqs::mesh::imfreq &m,
            const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> g, const triqs::mesh::imtime &m,
            const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::imtime &m,
            const std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::imtime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::matrix_valued> g, const triqs::mesh::refreq &m,
            const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued> g, const triqs::mesh::refreq &m,
            const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::matrix_valued> g, const triqs::mesh::retime &m,
            const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::retime &m,
            const std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::retime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 3, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::brzone, triqs::gfs::matrix_valued> g) { return triqs::gfs::make_gf_from_fourier(g); },
                 "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::cyclat, triqs::gfs::matrix_valued> g) { return triqs::gfs::make_gf_from_fourier(g); },
                 "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 1, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 2, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 1, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 2, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>> g, const triqs::mesh::imfreq &m,
            const nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>> g, const triqs::mesh::imfreq &m,
            const nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>> g, const triqs::mesh::imtime &m,
            const nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::imtime &m,
            const std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::imtime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>> g, const triqs::mesh::refreq &m,
            const nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>> g, const triqs::mesh::refreq &m,
            const nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>> g, const triqs::mesh::retime &m,
            const nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::retime &m,
            const std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::retime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 4, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>> g) { return triqs::gfs::make_gf_from_fourier(g); },
                 "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>> g) { return triqs::gfs::make_gf_from_fourier(g); },
                 "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 1, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 2, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g,
                    const int &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 1, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 2, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g,
                    const bool &x) { return triqs::gfs::make_gf_from_fourier(g, x); },
                 "g", "x"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>> g, const triqs::mesh::imfreq &m,
            const nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>> g, const triqs::mesh::imfreq &m,
            const nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::imfreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>> g, const triqs::mesh::imtime &m,
            const nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::imtime &m,
            const std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::imtime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>> g, const triqs::mesh::refreq &m,
            const nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>> g, const triqs::mesh::refreq &m,
            const nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::refreq &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>> g, const triqs::mesh::retime &m,
            const nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &km) { return triqs::gfs::make_gf_from_fourier(g, m, km); },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g,
            const triqs::mesh::retime &m,
            const std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                               nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g,
            const triqs::mesh::retime &m,
            const std::vector<std::vector<nda::basic_array<std::complex<double>, 5, nda::C_layout, 'A',
                                                           nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>> &km) {
           return triqs::gfs::make_gf_from_fourier(g, m, km);
         },
         "g", "m", "km"),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<4>> g) { return triqs::gfs::make_gf_from_fourier(g); },
                 "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<4>> g) { return triqs::gfs::make_gf_from_fourier(g); },
                 "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g) {
           return triqs::gfs::make_gf_from_fourier(g);
         },
         "g"),
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

static const auto _c2py_doc_0 = _c2py_fun_0.doc(
   R"DOC(
Build a Green's function on the conjugate mesh by Fourier transform.

Applies to every overload. The supported conjugate mesh
pairs are imaginary time and Matsubara frequencies, real time and
real frequency, cyclic lattice and Brillouin zone, and DLR
imaginary time and DLR Matsubara frequencies. Product meshes
pairing one of these Fourier axes with a lattice axis, as well as
block and block-of-block Green's function containers, are
transformed block- and component-wise. All target ranks (scalar,
vector, matrix, rank-3, rank-4) are supported.

Beyond the input Green's function, the following optional
positional arguments may be passed; which one applies depends on
the input mesh.

  - ``n_iw``: the number of positive Matsubara frequencies in
    the output mesh, when the input lives on imaginary time
    (default is -1, which selects the default size from the input).
  - ``n_tau``: the number of imaginary-time points in the output
    mesh, when the input lives on Matsubara frequencies
    (default is -1, which selects the default size from the input).
  - ``shift_half_bin``: a half-bin shift flag for the output
    mesh, for the real-time and real-frequency pair (default is false).
  - ``mesh`` and ``known_moments``: an explicit output mesh
    together with an array of high-frequency moments, for the
    imaginary-time and Matsubara and the real-time and
    real-frequency pairs. The known-moment tail correction
    improves accuracy at high frequency.

Parameters
----------
g : {par_0}
   The input Green's function.

Returns
-------
{ret_0}
   A new Green's function on the conjugate mesh.
)DOC",
   {{c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &>()}},
   {c2py::python_typename<triqs::gfs::block_gf<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_layout>>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"make_gf_from_fourier", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {
   PyModuleDef_HEAD_INIT,
   "gf_factories_fourier",                                                                                /* name of module */
   R"RAWDOC(Factory that builds a Green's function on the conjugate mesh by a Fourier transform.)RAWDOC", /* module documentation, may be NULL */
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
