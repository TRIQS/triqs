
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

// set_from_fourier
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_valued> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_valued> g_in,
                 nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> g_in,
                 nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::scalar_valued> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_real_valued, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::scalar_valued> g_in,
                 nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::scalar_valued> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::retime, triqs::gfs::scalar_valued> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::scalar_valued> g_in,
                 nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::cyclat, triqs::gfs::scalar_valued> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::brzone, triqs::gfs::scalar_valued> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::scalar_valued> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::brzone, triqs::gfs::scalar_valued> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::cyclat, triqs::gfs::scalar_valued> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::scalar_valued> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>> g_in,
                 nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>> g_in,
                 nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<1>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>> g_in,
                 nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<1>> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>> g_in,
                 nda::basic_array_view<const std::complex<double>, 2, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<1>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<1>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<1>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<1>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<1>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<1>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::matrix_valued> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::matrix_valued> g_in,
                 nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> g_in,
                 nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::matrix_valued> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_real_valued, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::matrix_valued> g_in,
                 nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::matrix_valued> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::retime, triqs::gfs::matrix_valued> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::matrix_valued> g_in,
                 nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::cyclat, triqs::gfs::matrix_valued> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::brzone, triqs::gfs::matrix_valued> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::matrix_valued> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::brzone, triqs::gfs::matrix_valued> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::cyclat, triqs::gfs::matrix_valued> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::matrix_valued> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>> g_in,
                 nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>> g_in,
                 nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<3>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>> g_in,
                 nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<3>> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>> g_in,
                 nda::basic_array_view<const std::complex<double>, 4, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>> g_in,
                 nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>> g_in,
                 nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_real_valued<4>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>> g_in,
                 nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::retime, triqs::gfs::tensor_valued<4>> g_out,
                 triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>> g_in,
                 nda::basic_array_view<const std::complex<double>, 5, nda::C_stride_layout, 'A', nda::default_accessor,
                                       nda::borrowed<nda::mem::AddressSpace::Host>>
                    known_moments) { return triqs::gfs::set_from_fourier(g_out, g_in, known_moments); },
              "g_out", "g_in", "known_moments"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<4>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<4>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<4>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<4>> g_out,
         triqs::gfs::gf_const_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<4>> g_in) { return triqs::gfs::set_from_fourier(g_out, g_in); },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<4>> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::brzone, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, false> g_out,
         triqs::gfs::block_gf_view<triqs::mesh::cyclat, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> g_in) {
        return triqs::gfs::set_from_fourier(g_out, g_in);
      },
      "g_out", "g_in")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC()DOC");
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"set_from_fourier", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "gf_fnt_fourier",  /* name of module */
                                        R"RAWDOC()RAWDOC", /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_gf_fnt_fourier() {

  if (not c2py::check_python_version("gf_fnt_fourier")) return NULL;

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
