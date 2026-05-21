
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

// enforce_discontinuity
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](triqs::gfs::gf_view<triqs::mesh::legendre> gl,
      nda::basic_array_view<const double, 2, nda::C_stride_layout, 'A', nda::default_accessor, nda::borrowed<nda::mem::AddressSpace::Host>> disc) {
     return triqs::gfs::enforce_discontinuity(gl, disc);
   },
   "gl", "disc")};

// is_gf_hermitian
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout> &g, double tolerance) {
        return triqs::gfs::is_gf_hermitian<triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued>>(g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout> &g,
                 double tolerance) { return triqs::gfs::is_gf_hermitian<triqs::gfs::gf_const_view<triqs::mesh::imfreq>>(g, tolerance); },
              "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout> &g, double tolerance) {
        return triqs::gfs::is_gf_hermitian<triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>>>(g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g, double tolerance) {
        return triqs::gfs::is_gf_hermitian<triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>>(
           g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g, double tolerance) {
        return triqs::gfs::is_gf_hermitian<triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>>(
           g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g, double tolerance) {
        return triqs::gfs::is_gf_hermitian<
           triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true>>(g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g, double tolerance) {
        return triqs::gfs::is_gf_hermitian<triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true>>(
           g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g, double tolerance) {
        return triqs::gfs::is_gf_hermitian<triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true>>(
           g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g, double tolerance) {
        return triqs::gfs::is_gf_hermitian<
           triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true>>(g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout> &g, double tolerance) {
        return triqs::gfs::is_gf_hermitian<triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_valued>>(g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout> &g,
                 double tolerance) { return triqs::gfs::is_gf_hermitian<triqs::gfs::gf_const_view<triqs::mesh::imtime>>(g, tolerance); },
              "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout> &g, double tolerance) {
        return triqs::gfs::is_gf_hermitian<triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>>>(g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g, double tolerance) {
        return triqs::gfs::is_gf_hermitian<triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>>(
           g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g, double tolerance) {
        return triqs::gfs::is_gf_hermitian<triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>>(
           g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g, double tolerance) {
        return triqs::gfs::is_gf_hermitian<
           triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true>>(g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g, double tolerance) {
        return triqs::gfs::is_gf_hermitian<triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true>>(
           g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g, double tolerance) {
        return triqs::gfs::is_gf_hermitian<triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true>>(
           g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g, double tolerance) {
        return triqs::gfs::is_gf_hermitian<
           triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true>>(g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12)};

// is_gf_real_in_tau
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout> &g, double tolerance) {
        return triqs::gfs::is_gf_real_in_tau<triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued>>(g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout> &g,
                 double tolerance) { return triqs::gfs::is_gf_real_in_tau<triqs::gfs::gf_const_view<triqs::mesh::imfreq>>(g, tolerance); },
              "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g, double tolerance) {
        return triqs::gfs::is_gf_real_in_tau<
           triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>>(g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g, double tolerance) {
        return triqs::gfs::is_gf_real_in_tau<
           triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>>(g, tolerance);
      },
      "g", "tolerance"_a = 1.e-12)};

// rebinning_tau
static auto const _c2py_fun_3 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::gfs::gf_const_view<triqs::mesh::imtime> &g, unsigned long new_n_tau) { return triqs::gfs::rebinning_tau(g, new_n_tau); }, "g",
   "new_n_tau")};

// set_from_imfreq
static auto const _c2py_fun_4 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::legendre, triqs::gfs::scalar_valued> gl,
                 triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> gw) { return triqs::gfs::set_from_imfreq(gl, gw); },
              "gl", "gw"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::legendre, triqs::gfs::tensor_valued<1>> gl,
                 triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>> gw) { return triqs::gfs::set_from_imfreq(gl, gw); },
              "gl", "gw"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::legendre, triqs::gfs::matrix_valued> gl,
                 triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> gw) { return triqs::gfs::set_from_imfreq(gl, gw); },
              "gl", "gw"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::legendre, triqs::gfs::tensor_valued<3>> gl,
                 triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>> gw) { return triqs::gfs::set_from_imfreq(gl, gw); },
              "gl", "gw"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::legendre, triqs::gfs::tensor_valued<4>> gl,
                 triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>> gw) { return triqs::gfs::set_from_imfreq(gl, gw); },
              "gl", "gw")};

// set_from_imtime
static auto const _c2py_fun_5 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::legendre, triqs::gfs::scalar_valued> gl,
                 triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_valued> gt) { return triqs::gfs::set_from_imtime(gl, gt); },
              "gl", "gt"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::legendre, triqs::gfs::tensor_valued<1>> gl,
                 triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>> gt) { return triqs::gfs::set_from_imtime(gl, gt); },
              "gl", "gt"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::legendre, triqs::gfs::matrix_valued> gl,
                 triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::matrix_valued> gt) { return triqs::gfs::set_from_imtime(gl, gt); },
              "gl", "gt"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::legendre, triqs::gfs::tensor_valued<3>> gl,
                 triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>> gt) { return triqs::gfs::set_from_imtime(gl, gt); },
              "gl", "gt"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::legendre, triqs::gfs::tensor_valued<4>> gl,
                 triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>> gt) { return triqs::gfs::set_from_imtime(gl, gt); },
              "gl", "gt")};

// set_from_legendre
static auto const _c2py_fun_6 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> gw,
                 triqs::gfs::gf_const_view<triqs::mesh::legendre, triqs::gfs::scalar_valued> gl) { return triqs::gfs::set_from_legendre(gw, gl); },
              "gw", "gl"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>> gw,
                 triqs::gfs::gf_const_view<triqs::mesh::legendre, triqs::gfs::tensor_valued<1>> gl) { return triqs::gfs::set_from_legendre(gw, gl); },
              "gw", "gl"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> gw,
                 triqs::gfs::gf_const_view<triqs::mesh::legendre, triqs::gfs::matrix_valued> gl) { return triqs::gfs::set_from_legendre(gw, gl); },
              "gw", "gl"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>> gw,
                 triqs::gfs::gf_const_view<triqs::mesh::legendre, triqs::gfs::tensor_valued<3>> gl) { return triqs::gfs::set_from_legendre(gw, gl); },
              "gw", "gl"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>> gw,
                 triqs::gfs::gf_const_view<triqs::mesh::legendre, triqs::gfs::tensor_valued<4>> gl) { return triqs::gfs::set_from_legendre(gw, gl); },
              "gw", "gl"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued> gt,
                 triqs::gfs::gf_const_view<triqs::mesh::legendre, triqs::gfs::scalar_valued> gl) { return triqs::gfs::set_from_legendre(gt, gl); },
              "gt", "gl"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>> gt,
                 triqs::gfs::gf_const_view<triqs::mesh::legendre, triqs::gfs::tensor_valued<1>> gl) { return triqs::gfs::set_from_legendre(gt, gl); },
              "gt", "gl"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued> gt,
                 triqs::gfs::gf_const_view<triqs::mesh::legendre, triqs::gfs::matrix_valued> gl) { return triqs::gfs::set_from_legendre(gt, gl); },
              "gt", "gl"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>> gt,
                 triqs::gfs::gf_const_view<triqs::mesh::legendre, triqs::gfs::tensor_valued<3>> gl) { return triqs::gfs::set_from_legendre(gt, gl); },
              "gt", "gl"),
   c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>> gt,
                 triqs::gfs::gf_const_view<triqs::mesh::legendre, triqs::gfs::tensor_valued<4>> gl) { return triqs::gfs::set_from_legendre(gt, gl); },
              "gt", "gl")};

// set_from_pade
static auto const _c2py_fun_7 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued> gw,
                                         triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> giw, int n_points,
                                         double freq_offset) { return triqs::gfs::set_from_pade(gw, giw, n_points, freq_offset); },
                                      "gw", "giw", "n_points"_a = 100, "freq_offset"_a = 0.0),
                           c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<1>> gw,
                                         triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>> giw, int n_points,
                                         double freq_offset) { return triqs::gfs::set_from_pade(gw, giw, n_points, freq_offset); },
                                      "gw", "giw", "n_points"_a = 100, "freq_offset"_a = 0.0),
                           c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued> gw,
                                         triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> giw, int n_points,
                                         double freq_offset) { return triqs::gfs::set_from_pade(gw, giw, n_points, freq_offset); },
                                      "gw", "giw", "n_points"_a = 100, "freq_offset"_a = 0.0),
                           c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<3>> gw,
                                         triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>> giw, int n_points,
                                         double freq_offset) { return triqs::gfs::set_from_pade(gw, giw, n_points, freq_offset); },
                                      "gw", "giw", "n_points"_a = 100, "freq_offset"_a = 0.0),
                           c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::refreq, triqs::gfs::tensor_valued<4>> gw,
                                         triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>> giw, int n_points,
                                         double freq_offset) { return triqs::gfs::set_from_pade(gw, giw, n_points, freq_offset); },
                                      "gw", "giw", "n_points"_a = 100, "freq_offset"_a = 0.0)};

// tau_L2_norm
static auto const _c2py_fun_8 =
   c2py::dispatcher_f_kw_t{
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::scalar_valued, nda::C_stride_layout> &g) {
           return triqs::gfs::tau_L2_norm<triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::scalar_valued>>(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::matrix_valued, nda::C_stride_layout> &g) {
           return triqs::gfs::tau_L2_norm<triqs::gfs::gf_const_view<triqs::mesh::dlr>>(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout> &g) {
           return triqs::gfs::tau_L2_norm<triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued>>(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout> &g) {
           return triqs::gfs::tau_L2_norm<triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq>>(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued, nda::C_stride_layout> &g) {
           return triqs::gfs::tau_L2_norm<triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued>>(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued, nda::C_stride_layout> &g) {
           return triqs::gfs::tau_L2_norm<triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime>>(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::tau_L2_norm<triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>>(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::tau_L2_norm<triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>>(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::tau_L2_norm<
              triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>>(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::tau_L2_norm<
              triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>>(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::tau_L2_norm<
              triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>>(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
           return triqs::gfs::tau_L2_norm<
              triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>>(g);
         },
         "g")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC()DOC");
static const auto _c2py_doc_1 = _c2py_fun_1.doc(
   R"DOC(
Test if a Green function object fullfills the fundamental property mentioned below up to a fixed tolerance $$
Depending on the mesh and target rank one of the following properties is checked
$G[i] == {1}{2} ( G[i] + conj(G[-i]) )$
$G[] == {1}{2} ( G[] + conj(G[]) )$
$G[i](i,j) == {1}{2} ( G[i](i,j) + conj(G[-i](j,i)) )$
$G[](i,j) == {1}{2} ( G[](i,j) + conj(G[](j,i)) )$
$G[i](i,j,k,l) == {1}{2} ( G[i](i,j,k,l) + conj(G[-i](k,l,i,j)) )$
$G[](i,j,k,l) == {1}{2} ( G[](i,j,k,l) + conj(G[](k,l,i,j)) )$

Parameters
----------
g : {par_0}
   The Green function object to check the symmetry for
tolerance : {par_1}
   The tolerance $$ for the check [default=1e-12]

Returns
-------
{ret_0}
   true iif the fundamental property holds for all points of the mesh
)DOC",
   {{c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &>()},
    {c2py::python_typename<double>()}},
   {c2py::python_typename<bool>()});
static const auto _c2py_doc_2 = _c2py_fun_2.doc(
   R"DOC(
Test if a Matsubara Green function object has an associated imaginary-time Green function
with an imaginary part below a fixed tolerance $$
The following property is checked
$G[i](...) == conj(G[-i](...))$

Parameters
----------
g : {par_0}
   The Green function object to check the property for
tolerance : {par_1}
   The tolerance $$ for the check [default=1e-12]

Returns
-------
{ret_0}
   true iif the property holds for all points of the mesh
)DOC",
   {{c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &>()},
    {c2py::python_typename<double>()}},
   {c2py::python_typename<bool>()});
static const auto _c2py_doc_3 = _c2py_fun_3.doc(R"DOC()DOC");
static const auto _c2py_doc_4 = _c2py_fun_4.doc(R"DOC()DOC");
static const auto _c2py_doc_5 = _c2py_fun_5.doc(R"DOC()DOC");
static const auto _c2py_doc_6 = _c2py_fun_6.doc(R"DOC()DOC");
static const auto _c2py_doc_7 = _c2py_fun_7.doc(R"DOC()DOC");
static const auto _c2py_doc_8 = _c2py_fun_8.doc(R"DOC(
L2 tau norm of DLR Green's function
)DOC");
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"enforce_discontinuity", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"is_gf_hermitian", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"is_gf_real_in_tau", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"rebinning_tau", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {"set_from_imfreq", (PyCFunction)c2py::pyfkw<_c2py_fun_4>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_4.c_str()},
   {"set_from_imtime", (PyCFunction)c2py::pyfkw<_c2py_fun_5>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_5.c_str()},
   {"set_from_legendre", (PyCFunction)c2py::pyfkw<_c2py_fun_6>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_6.c_str()},
   {"set_from_pade", (PyCFunction)c2py::pyfkw<_c2py_fun_7>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_7.c_str()},
   {"tau_L2_norm", (PyCFunction)c2py::pyfkw<_c2py_fun_8>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_8.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "gf_fnt_misc",     /* name of module */
                                        R"RAWDOC()RAWDOC", /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_gf_fnt_misc() {

  if (not c2py::check_python_version("gf_fnt_misc")) return NULL;

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
