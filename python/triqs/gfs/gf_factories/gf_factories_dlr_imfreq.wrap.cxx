
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

// find_w_max
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> &g, double eps, bool symmetrize, double w_max_init,
                 double w_max_max) { return triqs::gfs::find_w_max(g, eps, symmetrize, w_max_init, w_max_max); },
              "g", "eps"_a = 1e-10, "symmetrize"_a = true, "w_max_init"_a = 1.0, "w_max_max"_a = 200.0),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g, double eps,
                 bool symmetrize, double w_max_init, double w_max_max) { return triqs::gfs::find_w_max(g, eps, symmetrize, w_max_init, w_max_max); },
              "g", "eps"_a = 1e-10, "symmetrize"_a = true, "w_max_init"_a = 1.0, "w_max_max"_a = 200.0),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>> &g, double eps, bool symmetrize,
                 double w_max_init, double w_max_max) { return triqs::gfs::find_w_max(g, eps, symmetrize, w_max_init, w_max_max); },
              "g", "eps"_a = 1e-10, "symmetrize"_a = true, "w_max_init"_a = 1.0, "w_max_max"_a = 200.0),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g, double eps,
                 bool symmetrize, double w_max_init, double w_max_max) { return triqs::gfs::find_w_max(g, eps, symmetrize, w_max_init, w_max_max); },
              "g", "eps"_a = 1e-10, "symmetrize"_a = true, "w_max_init"_a = 1.0, "w_max_max"_a = 200.0),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &g, double eps, bool symmetrize, double w_max_init,
                 double w_max_max) { return triqs::gfs::find_w_max(g, eps, symmetrize, w_max_init, w_max_max); },
              "g", "eps"_a = 1e-10, "symmetrize"_a = true, "w_max_init"_a = 1.0, "w_max_max"_a = 200.0),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g, double eps,
                 bool symmetrize, double w_max_init, double w_max_max) { return triqs::gfs::find_w_max(g, eps, symmetrize, w_max_init, w_max_max); },
              "g", "eps"_a = 1e-10, "symmetrize"_a = true, "w_max_init"_a = 1.0, "w_max_max"_a = 200.0),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>> &g, double eps, bool symmetrize,
                 double w_max_init, double w_max_max) { return triqs::gfs::find_w_max(g, eps, symmetrize, w_max_init, w_max_max); },
              "g", "eps"_a = 1e-10, "symmetrize"_a = true, "w_max_init"_a = 1.0, "w_max_max"_a = 200.0),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g, double eps,
                 bool symmetrize, double w_max_init, double w_max_max) { return triqs::gfs::find_w_max(g, eps, symmetrize, w_max_init, w_max_max); },
              "g", "eps"_a = 1e-10, "symmetrize"_a = true, "w_max_init"_a = 1.0, "w_max_max"_a = 200.0),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>> &g, double eps, bool symmetrize,
                 double w_max_init, double w_max_max) { return triqs::gfs::find_w_max(g, eps, symmetrize, w_max_init, w_max_max); },
              "g", "eps"_a = 1e-10, "symmetrize"_a = true, "w_max_init"_a = 1.0, "w_max_max"_a = 200.0),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g, double eps,
                 bool symmetrize, double w_max_init, double w_max_max) { return triqs::gfs::find_w_max(g, eps, symmetrize, w_max_init, w_max_max); },
              "g", "eps"_a = 1e-10, "symmetrize"_a = true, "w_max_init"_a = 1.0, "w_max_max"_a = 200.0)};

// make_gf_dlr_imfreq
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::scalar_valued> &g) { return triqs::gfs::make_gf_dlr_imfreq(g); }, "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued> &g) { return triqs::gfs::make_gf_dlr_imfreq(g); }, "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<1>> &g) { return triqs::gfs::make_gf_dlr_imfreq(g); },
              "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<1>> &g) { return triqs::gfs::make_gf_dlr_imfreq(g); },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::matrix_valued> &g) { return triqs::gfs::make_gf_dlr_imfreq(g); }, "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued> &g) { return triqs::gfs::make_gf_dlr_imfreq(g); }, "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>> &g) { return triqs::gfs::make_gf_dlr_imfreq(g); },
              "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<3>> &g) { return triqs::gfs::make_gf_dlr_imfreq(g); },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<4>> &g) { return triqs::gfs::make_gf_dlr_imfreq(g); },
              "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<4>> &g) { return triqs::gfs::make_gf_dlr_imfreq(g); },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> &g, double w_max, double eps,
                 bool symmetrize) { return triqs::gfs::make_gf_dlr_imfreq(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::make_gf_dlr_imfreq(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>> &g, double w_max, double eps,
                 bool symmetrize) { return triqs::gfs::make_gf_dlr_imfreq(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::make_gf_dlr_imfreq(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &g, double w_max, double eps,
                 bool symmetrize) { return triqs::gfs::make_gf_dlr_imfreq(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::make_gf_dlr_imfreq(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>> &g, double w_max, double eps,
                 bool symmetrize) { return triqs::gfs::make_gf_dlr_imfreq(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::make_gf_dlr_imfreq(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>> &g, double w_max, double eps,
                 bool symmetrize) { return triqs::gfs::make_gf_dlr_imfreq(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::make_gf_dlr_imfreq(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::scalar_valued> g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::scalar_valued> g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::scalar_valued> g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::scalar_valued> g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>
            g) { return triqs::gfs::make_gf_dlr_imfreq(g); },
      "g"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>
            g) { return triqs::gfs::make_gf_dlr_imfreq(g); },
      "g"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>
            g) { return triqs::gfs::make_gf_dlr_imfreq(g); },
      "g"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>
            g) { return triqs::gfs::make_gf_dlr_imfreq(g); },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::matrix_valued> g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::matrix_valued> g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::matrix_valued> g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::matrix_valued> g) {
        return triqs::gfs::make_gf_dlr_imfreq(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>
            g) { return triqs::gfs::make_gf_dlr_imfreq(g); },
      "g"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>
            g) { return triqs::gfs::make_gf_dlr_imfreq(g); },
      "g"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>
            g) { return triqs::gfs::make_gf_dlr_imfreq(g); },
      "g"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>
            g) { return triqs::gfs::make_gf_dlr_imfreq(g); },
      "g")};

// make_gf_imfreq
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::scalar_valued> &g, long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
      "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<1>> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<1>> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<1>> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::matrix_valued> &g, long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
      "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<3>> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<3>> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<4>> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<4>> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<4>> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::scalar_valued> g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::scalar_valued> g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::scalar_valued> g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::scalar_valued> g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>
            g,
         long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
      "g", "n_iw"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>
            g,
         long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
      "g", "n_iw"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>
            g,
         long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
      "g", "n_iw"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>
            g,
         long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
      "g", "n_iw"),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::matrix_valued> g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::matrix_valued> g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::matrix_valued> g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::matrix_valued> g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::brzone>, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>
            g,
         long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
      "g", "n_iw"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr>, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>
            g,
         long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
      "g", "n_iw"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr, triqs::mesh::cyclat>, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>
            g,
         long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
      "g", "n_iw"),
   c2py::cfun(
      [](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr>, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>
            g,
         long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
      "g", "n_iw"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr2d, triqs::gfs::scalar_valued> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"_a = 0),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr2d, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"_a = 0),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr2d, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"_a = 0),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr2d, triqs::gfs::tensor_valued<1>> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"_a = 0),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr2d, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"_a = 0),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr2d, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"_a = 0),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr2d, triqs::gfs::matrix_valued> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"_a = 0),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr2d, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"_a = 0),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr2d, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"_a = 0),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr2d, triqs::gfs::tensor_valued<3>> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"_a = 0),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr2d, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"_a = 0),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr2d, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"_a = 0),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr2d, triqs::gfs::tensor_valued<4>> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"_a = 0),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr2d, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"_a = 0),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::dlr2d, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g,
                 long n_iw) { return triqs::gfs::make_gf_imfreq(g, n_iw); },
              "g", "n_iw"_a = 0)};

static const auto _c2py_doc_0 =
   _c2py_fun_0.doc(R"DOC(
Find the smallest DLR energy cutoff that reproduces a Matsubara Green's function within a target accuracy.

Applies to every overload. Starting from ``w_max_init``, the
cutoff is grown by a factor of 1.5 per iteration. For each
candidate cutoff, the function builds a DLR Matsubara mesh from
(``w_max``, ``eps``, ``symmetrize``), samples ``g`` on that DLR mesh, reconstructs
the Green's function on the original Matsubara mesh via the DLR
coefficients, and measures the worst-case round-trip error across
Matsubara indices and target indices. The smallest ``w_max`` not
exceeding ``w_max_max`` whose round-trip error is below ``eps`` is
returned.

Candidate cutoffs whose DLR frequency range exceeds the Matsubara
range of the input are skipped. For a block Green's function, the
reported per-iteration error is the worst case across all blocks.
An exception is raised if ``w_max_init`` exceeds ``w_max_max``, or if no
candidate cutoff up to ``w_max_max`` achieves an error below ``eps``.

Parameters
----------
g : {par_0}
   Input Green's function on a Matsubara frequency mesh.
eps : {par_1}
   Target DLR accuracy and round-trip error tolerance.
symmetrize : {par_2}
   If `true`, use particle-hole symmetric DLR meshes.
w_max_init : {par_3}
   Initial value of the DLR cutoff to try (must not exceed ``w_max_max``).
w_max_max : {par_4}
   Maximum DLR cutoff to try before giving up.

Returns
-------
{ret_0}
   The smallest ``w_max`` not exceeding ``w_max_max`` for which the round-trip error is below ``eps``.
)DOC",
                   {{c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> &>()},
                    {c2py::python_typename<double>()},
                    {c2py::python_typename<bool>()},
                    {c2py::python_typename<double>()},
                    {c2py::python_typename<double>()}},
                   {c2py::python_typename<double>()});
static const auto _c2py_doc_1 = _c2py_fun_1.doc(
   R"DOC(
Build a DLR Matsubara Green's function from a DLR-coefficient or DLR-imaginary-time input.

Applies to every overload. The input lives on a single
DLR coefficient mesh or DLR imaginary-time mesh, or on a product
mesh pairing a DLR axis with a lattice axis. Block and
block-of-block Green's function containers are handled
component-wise, and product meshes are handled axis-wise. All
target ranks (scalar, vector, matrix, rank-3, rank-4) are
supported. The output lives on the DLR Matsubara frequency mesh
associated with the input's DLR grid.

Parameters
----------
g : {par_0}
   A Green's function on a DLR coefficient or DLR imaginary-time mesh.

Returns
-------
{ret_0}
   The Green's function evaluated on the DLR Matsubara frequency mesh.
)DOC",
   {{c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::scalar_valued> &>()}},
   {c2py::python_typename<triqs::gfs::gf<triqs::mesh::dlr_imfreq, typename triqs::gfs::_target_from_type_rank<std::complex<double>, 0>::type>>()});
static const auto _c2py_doc_2 = _c2py_fun_2.doc(
   R"DOC(
Build a uniform Matsubara Green's function from any DLR representation.

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
n_iw : {par_1}
   Number of positive Matsubara frequencies in the output mesh.

Returns
-------
{ret_0}
   The Green's function on a uniform Matsubara frequency mesh.
)DOC",
   {{c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::scalar_valued> &>()}, {c2py::python_typename<long>()}},
   {c2py::python_typename<triqs::gfs::gf<triqs::mesh::imfreq, typename triqs::gfs::_target_from_type_rank<std::complex<double>, 0>::type>>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"find_w_max", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"make_gf_dlr_imfreq", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"make_gf_imfreq", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {
   PyModuleDef_HEAD_INIT,
   "gf_factories_dlr_imfreq", /* name of module */
   R"RAWDOC(Factories that build a Green's function on the DLR Matsubara mesh or on a standard imfreq mesh from a DLR representation.)RAWDOC", /* module documentation, may be NULL */
   -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
   module_methods,
   NULL,
   NULL,
   NULL,
   NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_gf_factories_dlr_imfreq() {

  if (not c2py::check_python_version("gf_factories_dlr_imfreq")) return NULL;

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
