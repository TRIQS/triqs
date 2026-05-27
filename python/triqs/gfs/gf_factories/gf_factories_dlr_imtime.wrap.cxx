
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

// make_gf_dlr_imtime
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::scalar_valued> &g) { return triqs::gfs::make_gf_dlr_imtime(g); }, "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued> &g) { return triqs::gfs::make_gf_dlr_imtime(g); }, "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<1>> &g) { return triqs::gfs::make_gf_dlr_imtime(g); },
              "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<1>> &g) { return triqs::gfs::make_gf_dlr_imtime(g); },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::matrix_valued> &g) { return triqs::gfs::make_gf_dlr_imtime(g); }, "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &g) { return triqs::gfs::make_gf_dlr_imtime(g); }, "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>> &g) { return triqs::gfs::make_gf_dlr_imtime(g); },
              "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<3>> &g) { return triqs::gfs::make_gf_dlr_imtime(g); },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<4>> &g) { return triqs::gfs::make_gf_dlr_imtime(g); },
              "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<4>> &g) { return triqs::gfs::make_gf_dlr_imtime(g); },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::scalar_valued> g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::scalar_valued> g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::scalar_valued> g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::scalar_valued> g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>
            g) { return triqs::gfs::make_gf_dlr_imtime(g); },
      "g"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>
            g) { return triqs::gfs::make_gf_dlr_imtime(g); },
      "g"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>
            g) { return triqs::gfs::make_gf_dlr_imtime(g); },
      "g"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>
            g) { return triqs::gfs::make_gf_dlr_imtime(g); },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::matrix_valued> g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::matrix_valued> g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::matrix_valued> g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::matrix_valued> g) {
        return triqs::gfs::make_gf_dlr_imtime(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>
            g) { return triqs::gfs::make_gf_dlr_imtime(g); },
      "g"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>
            g) { return triqs::gfs::make_gf_dlr_imtime(g); },
      "g"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>
            g) { return triqs::gfs::make_gf_dlr_imtime(g); },
      "g"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>
            g) { return triqs::gfs::make_gf_dlr_imtime(g); },
      "g")};

// make_gf_imtime
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::scalar_valued> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<1>> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<1>> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<1>> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::matrix_valued> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<3>> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<3>> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<4>> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<4>> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<4>> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::scalar_valued> g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::scalar_valued> g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::scalar_valued> g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::scalar_valued> g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>
            g,
         long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
      "g", "n_tau"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>
            g,
         long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
      "g", "n_tau"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>
            g,
         long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
      "g", "n_tau"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>
            g,
         long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
      "g", "n_tau"),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::matrix_valued> g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::matrix_valued> g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::matrix_valued> g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::matrix_valued> g,
                 long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
              "g", "n_tau"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>
            g,
         long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
      "g", "n_tau"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>
            g,
         long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
      "g", "n_tau"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>
            g,
         long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
      "g", "n_tau"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>
            g,
         long n_tau) { return triqs::gfs::make_gf_imtime(g, n_tau); },
      "g", "n_tau")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(
   R"DOC(
Build a DLR imaginary-time Green's function from a DLR-coefficient or DLR-Matsubara input.

Applies to every overload. The input lives on a single
DLR coefficient mesh or DLR Matsubara frequency mesh, or on a
product mesh pairing a DLR axis with a lattice axis. Block and
block-of-block Green's function containers are handled
component-wise, and product meshes are handled axis-wise. All
target ranks (scalar, vector, matrix, rank-3, rank-4) are
supported. The output lives on the DLR imaginary-time mesh
associated with the input's DLR grid.

Parameters
----------
g : {par_0}
   A Green's function on a DLR coefficient or DLR Matsubara frequency mesh.

Returns
-------
{ret_0}
   The Green's function evaluated on the DLR imaginary-time mesh.
)DOC",
   {{c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::scalar_valued> &>()}},
   {c2py::python_typename<triqs::gfs::gf<triqs::mesh::dlr_imtime, typename triqs::gfs::_target_from_type_rank<std::complex<double>, 0>::type>>()});
static const auto _c2py_doc_1 = _c2py_fun_1.doc(
   R"DOC(
Build a uniform imaginary-time Green's function from any DLR representation.

Applies to every overload. The input lives on a single
DLR coefficient, DLR imaginary-time, or DLR Matsubara frequency
mesh, or on a product mesh pairing a DLR axis with a lattice
axis. Block and block-of-block Green's function containers are
handled component-wise, and product meshes are handled axis-wise.
All target ranks (scalar, vector, matrix, rank-3, rank-4) are
supported. Inputs on a DLR imaginary-time or DLR Matsubara
frequency mesh are first converted to DLR coefficients
internally.

Parameters
----------
g : {par_0}
   A Green's function on a DLR coefficient, DLR imaginary-time, or DLR Matsubara frequency mesh.
n_tau : {par_1}
   Number of points of the output imaginary-time mesh.

Returns
-------
{ret_0}
   The Green's function on a uniform imaginary-time mesh.
)DOC",
   {{c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::scalar_valued> &>()}, {c2py::python_typename<long>()}},
   {c2py::python_typename<triqs::gfs::gf<triqs::mesh::imtime, typename triqs::gfs::_target_from_type_rank<std::complex<double>, 0>::type>>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"make_gf_dlr_imtime", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"make_gf_imtime", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {
   PyModuleDef_HEAD_INIT,
   "gf_factories_dlr_imtime", /* name of module */
   R"RAWDOC(Factories that build a Green's function on the DLR imaginary-time mesh or on a standard imtime mesh from a DLR representation.)RAWDOC", /* module documentation, may be NULL */
   -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
   module_methods,
   NULL,
   NULL,
   NULL,
   NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_gf_factories_dlr_imtime() {

  if (not c2py::check_python_version("gf_factories_dlr_imtime")) return NULL;

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
