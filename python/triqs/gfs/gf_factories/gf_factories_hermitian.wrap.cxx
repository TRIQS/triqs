
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

// fit_gf_dlr
static auto const _c2py_fun_0 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_valued> &g, double w_max, double eps,
                 bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>> &g, double w_max, double eps,
                 bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::matrix_valued> &g, double w_max, double eps,
                 bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>> &g, double w_max, double eps,
                 bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>> &g, double w_max, double eps,
                 bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::imtime, triqs::mesh::brzone>, triqs::gfs::scalar_valued> g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::imtime>, triqs::gfs::scalar_valued> g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::imtime, triqs::mesh::cyclat>, triqs::gfs::scalar_valued> g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::imtime>, triqs::gfs::scalar_valued> g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::imtime, triqs::mesh::brzone>, triqs::gfs::scalar_valued,
                                           nda::C_stride_layout, 1, true>
                    g,
                 double w_max, double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::imtime>, triqs::gfs::scalar_valued,
                                           nda::C_stride_layout, 1, true>
                    g,
                 double w_max, double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::imtime, triqs::mesh::cyclat>, triqs::gfs::scalar_valued,
                                           nda::C_stride_layout, 1, true>
                    g,
                 double w_max, double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::imtime>, triqs::gfs::scalar_valued,
                                           nda::C_stride_layout, 1, true>
                    g,
                 double w_max, double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::imtime, triqs::mesh::brzone>, triqs::gfs::matrix_valued> g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::imtime>, triqs::gfs::matrix_valued> g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::imtime, triqs::mesh::cyclat>, triqs::gfs::matrix_valued> g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::imtime>, triqs::gfs::matrix_valued> g, double w_max,
                 double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::imtime, triqs::mesh::brzone>, triqs::gfs::matrix_valued,
                                           nda::C_stride_layout, 1, true>
                    g,
                 double w_max, double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::imtime>, triqs::gfs::matrix_valued,
                                           nda::C_stride_layout, 1, true>
                    g,
                 double w_max, double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::imtime, triqs::mesh::cyclat>, triqs::gfs::matrix_valued,
                                           nda::C_stride_layout, 1, true>
                    g,
                 double w_max, double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::imtime>, triqs::gfs::matrix_valued,
                                           nda::C_stride_layout, 1, true>
                    g,
                 double w_max, double eps, bool symmetrize) { return triqs::gfs::fit_gf_dlr(g, w_max, eps, symmetrize); },
              "g", "w_max", "eps", "symmetrize"_a = true)};

// make_gf_dlr
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued> &g) { return triqs::gfs::make_gf_dlr(g); }, "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued> &g) { return triqs::gfs::make_gf_dlr(g); }, "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<1>> &g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<1>> &g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<1>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued> &g) { return triqs::gfs::make_gf_dlr(g); }, "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued> &g) { return triqs::gfs::make_gf_dlr(g); }, "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<3>> &g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<3>> &g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<3>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<4>> &g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<4>> &g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::dlr_imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr_imtime, triqs::mesh::brzone>, triqs::gfs::scalar_valued> g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr_imtime>, triqs::gfs::scalar_valued> g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr_imfreq, triqs::mesh::brzone>, triqs::gfs::scalar_valued> g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr_imfreq>, triqs::gfs::scalar_valued> g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr_imtime, triqs::mesh::cyclat>, triqs::gfs::scalar_valued> g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr_imtime>, triqs::gfs::scalar_valued> g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr_imfreq, triqs::mesh::cyclat>, triqs::gfs::scalar_valued> g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr_imfreq>, triqs::gfs::scalar_valued> g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr_imtime, triqs::mesh::brzone>, triqs::gfs::scalar_valued,
                                           nda::C_stride_layout, 1, true>
                    g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr_imtime>, triqs::gfs::scalar_valued,
                                           nda::C_stride_layout, 1, true>
                    g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr_imfreq, triqs::mesh::brzone>, triqs::gfs::scalar_valued,
                                           nda::C_stride_layout, 1, true>
                    g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr_imfreq>, triqs::gfs::scalar_valued,
                                           nda::C_stride_layout, 1, true>
                    g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr_imtime, triqs::mesh::cyclat>, triqs::gfs::scalar_valued,
                                           nda::C_stride_layout, 1, true>
                    g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr_imtime>, triqs::gfs::scalar_valued,
                                           nda::C_stride_layout, 1, true>
                    g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr_imfreq, triqs::mesh::cyclat>, triqs::gfs::scalar_valued,
                                           nda::C_stride_layout, 1, true>
                    g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr_imfreq>, triqs::gfs::scalar_valued,
                                           nda::C_stride_layout, 1, true>
                    g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr_imtime, triqs::mesh::brzone>, triqs::gfs::matrix_valued> g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr_imtime>, triqs::gfs::matrix_valued> g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr_imfreq, triqs::mesh::brzone>, triqs::gfs::matrix_valued> g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr_imfreq>, triqs::gfs::matrix_valued> g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr_imtime, triqs::mesh::cyclat>, triqs::gfs::matrix_valued> g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr_imtime>, triqs::gfs::matrix_valued> g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::dlr_imfreq, triqs::mesh::cyclat>, triqs::gfs::matrix_valued> g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun(
      [](triqs::gfs::gf_const_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr_imfreq>, triqs::gfs::matrix_valued> g) {
        return triqs::gfs::make_gf_dlr(g);
      },
      "g"),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr_imtime, triqs::mesh::brzone>, triqs::gfs::matrix_valued,
                                           nda::C_stride_layout, 1, true>
                    g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr_imtime>, triqs::gfs::matrix_valued,
                                           nda::C_stride_layout, 1, true>
                    g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr_imfreq, triqs::mesh::brzone>, triqs::gfs::matrix_valued,
                                           nda::C_stride_layout, 1, true>
                    g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::brzone, triqs::mesh::dlr_imfreq>, triqs::gfs::matrix_valued,
                                           nda::C_stride_layout, 1, true>
                    g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr_imtime, triqs::mesh::cyclat>, triqs::gfs::matrix_valued,
                                           nda::C_stride_layout, 1, true>
                    g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr_imtime>, triqs::gfs::matrix_valued,
                                           nda::C_stride_layout, 1, true>
                    g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::dlr_imfreq, triqs::mesh::cyclat>, triqs::gfs::matrix_valued,
                                           nda::C_stride_layout, 1, true>
                    g) { return triqs::gfs::make_gf_dlr(g); },
              "g"),
   c2py::cfun([](triqs::gfs::block_gf_view<triqs::mesh::prod<triqs::mesh::cyclat, triqs::mesh::dlr_imfreq>, triqs::gfs::matrix_valued,
                                           nda::C_stride_layout, 1, true>
                    g) { return triqs::gfs::make_gf_dlr(g); },
              "g")};

// make_hermitian
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout> &g) {
        return triqs::gfs::make_hermitian<triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued>>(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_hermitian<triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>>(
           g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_hermitian<triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true>>(
           g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout> &g) {
        return triqs::gfs::make_hermitian<triqs::gfs::gf_const_view<triqs::mesh::imfreq>>(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_hermitian<triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>>(
           g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_hermitian<triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true>>(
           g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout> &g) {
        return triqs::gfs::make_hermitian<triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>>>(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_hermitian<
           triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true>>(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_hermitian<
           triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true>>(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout> &g) {
        return triqs::gfs::make_hermitian<triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_valued>>(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_hermitian<triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>>(
           g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_hermitian<triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true>>(
           g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout> &g) {
        return triqs::gfs::make_hermitian<triqs::gfs::gf_const_view<triqs::mesh::imtime>>(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_hermitian<triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>>(
           g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_hermitian<triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true>>(
           g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout> &g) {
        return triqs::gfs::make_hermitian<triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>>>(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_hermitian<
           triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true>>(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_hermitian<
           triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true>>(g);
      },
      "g")};

// make_real_in_tau
static auto const _c2py_fun_3 = c2py::dispatcher_f_kw_t{
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout> &g) {
        return triqs::gfs::make_real_in_tau<triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued>>(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_real_in_tau<triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>>(
           g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_real_in_tau<triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true>>(
           g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout> &g) {
        return triqs::gfs::make_real_in_tau<triqs::gfs::gf_const_view<triqs::mesh::imfreq>>(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_real_in_tau<triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>>(
           g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_real_in_tau<triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true>>(
           g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout> &g) {
        return triqs::gfs::make_real_in_tau<triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>>>(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &g) {
        return triqs::gfs::make_real_in_tau<
           triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true>>(g);
      },
      "g"),
   c2py::cfun(
      [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &g) {
        return triqs::gfs::make_real_in_tau<
           triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true>>(g);
      },
      "g")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(
   R"DOC(
Fit an imaginary-time Green's function with a Discrete Lehmann Representation.

Applies to every overload. The input lives on a single
imaginary-time mesh or on a product mesh pairing an
imaginary-time axis with a lattice axis. Block and block-of-block
Green's function containers are handled component-wise, and
product meshes are handled axis-wise. All target ranks (scalar,
vector, matrix, rank-3, rank-4) are supported. The DLR grid is
specified by the spectral cutoff ``w_max`` and the tolerance
``eps``.

Parameters
----------
g : {par_0}
   The imaginary-time Green's function to fit.
w_max : {par_1}
   Maximum real frequency captured by the DLR basis.
eps : {par_2}
   Target accuracy of the DLR representation.
symmetrize : {par_3}
   If true, the DLR grid is symmetric about zero frequency.

Returns
-------
{ret_0}
   A Green's function on the DLR coefficient mesh.
)DOC",
   {{c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_valued> &>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<double>()},
    {c2py::python_typename<bool>()}},
   {c2py::python_typename<triqs::gfs::gf<triqs::mesh::dlr, typename triqs::gfs::_target_from_type_rank<std::complex<double>, 0>::type>>()});
static const auto _c2py_doc_1 = _c2py_fun_1.doc(
   R"DOC(
Transform a DLR imaginary-time or DLR Matsubara Green's function to its DLR-coefficient representation.

Applies to every overload. The input lives on a single
DLR imaginary-time or DLR Matsubara frequency mesh, or on a
product mesh pairing a DLR axis with a lattice axis. Block and
block-of-block Green's function containers are handled
component-wise, and product meshes are handled axis-wise. All
target ranks (scalar, vector, matrix, rank-3, rank-4) are
supported.

Parameters
----------
g : {par_0}
   The input Green's function.

Returns
-------
{ret_0}
   The Green's function expressed in DLR coefficients.
)DOC",
   {{c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued> &>()}},
   {c2py::python_typename<triqs::gfs::gf<triqs::mesh::dlr, typename triqs::gfs::_target_from_type_rank<std::complex<double>, 0>::type>>()});
static const auto _c2py_doc_2 = _c2py_fun_2.doc(
   R"DOC(
Symmetrize a Green's function so that it satisfies the hermitian symmetry.

Depending on the mesh and target rank, one of the following transformations is applied:

- :math:`G(i\omega) \rightarrow \frac{1}{2} [ G(i\omega) + G^*(-i\omega) ]`
- :math:`G(\tau) \rightarrow \frac{1}{2} [ G(\tau) + G^*(\tau) ]`
- :math:`G_{i,j}(i\omega) \rightarrow \frac{1}{2} [ G_{i,j}(i\omega) + G_{j,i}^*(i\omega) ]`
- :math:`G_{i,j}(\tau) \rightarrow \frac{1}{2} [ G_{i,j}(\tau) + G_{j,i}^*(\tau) ]`
- :math:`G_{i,j,k,l}(i\omega) \rightarrow \frac{1}{2} [ G_{i,j,k,l}(i\omega)] + G_{k,l,i,j}^*(i\omega) ]`
- :math:`G_{i,j,k,l}(\tau) \rightarrow \frac{1}{2} [ G_{i,j,k,l}(\tau) + G_{k,l,i,j}(\tau) ]`

For block Green's functions, the symmetrization is applied block-wise.

Parameters
----------
g : {par_0}
   The Green's function to symmetrize.

Returns
-------
[1] : {ret_0}
   The symmetrized Green's function.

[2] : {ret_1}
   The symmetrized Green's function.

[3] : {ret_2}
   The symmetrized Green's function.

[4] : {ret_3}
   The symmetrized Green's function.

[5] : {ret_4}
   The symmetrized Green's function.

[6] : {ret_5}
   The symmetrized Green's function.

[7] : {ret_6}
   The symmetrized Green's function.

[8] : {ret_7}
   The symmetrized Green's function.

[9] : {ret_8}
   The symmetrized Green's function.

[10] : {ret_9}
   The symmetrized Green's function.

[11] : {ret_10}
   The symmetrized Green's function.

[12] : {ret_11}
   The symmetrized Green's function.

[13] : {ret_12}
   The symmetrized Green's function.

[14] : {ret_13}
   The symmetrized Green's function.

[15] : {ret_14}
   The symmetrized Green's function.

[16] : {ret_15}
   The symmetrized Green's function.

[17] : {ret_16}
   The symmetrized Green's function.

[18] : {ret_17}
   The symmetrized Green's function.
)DOC",
   {{c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &>()}},
   {c2py::python_typename<typename triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true>::regular_type>(),
    c2py::python_typename<typename triqs::gfs::gf_const_view<triqs::mesh::imfreq>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true>::regular_type>(),
    c2py::python_typename<typename triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true>::regular_type>(),
    c2py::python_typename<typename triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::scalar_valued>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true>::regular_type>(),
    c2py::python_typename<typename triqs::gfs::gf_const_view<triqs::mesh::imtime>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true>::regular_type>(),
    c2py::python_typename<typename triqs::gfs::gf_const_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imtime, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true>::regular_type>()});
static const auto _c2py_doc_3 = _c2py_fun_3.doc(
   R"DOC(
Symmetrize a Matsubara Green's function so that its imaginary-time partner is real-valued.

The transformation applied is :math:`G_{i,j,\dots}(i\omega) \rightarrow \frac{1}{2} [ G_{i,j,\dots}(i\omega)
+ G_{i,j,\dots}^*(-i\omega) ]`.

For block Green's functions, the symmetrization is applied block-wise.

Parameters
----------
g : {par_0}
   The Matsubara Green's function to symmetrize.

Returns
-------
[1] : {ret_0}
   The symmetrized Green's function.

[2] : {ret_1}
   The symmetrized Green's function.

[3] : {ret_2}
   The symmetrized Green's function.

[4] : {ret_3}
   The symmetrized Green's function.

[5] : {ret_4}
   The symmetrized Green's function.

[6] : {ret_5}
   The symmetrized Green's function.

[7] : {ret_6}
   The symmetrized Green's function.

[8] : {ret_7}
   The symmetrized Green's function.

[9] : {ret_8}
   The symmetrized Green's function.
)DOC",
   {{c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true> &>()}},
   {c2py::python_typename<typename triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 2, true>::regular_type>(),
    c2py::python_typename<typename triqs::gfs::gf_const_view<triqs::mesh::imfreq>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 2, true>::regular_type>(),
    c2py::python_typename<typename triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 1, true>::regular_type>(),
    c2py::python_typename<
       typename triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::tensor_valued<4>, nda::C_stride_layout, 2, true>::regular_type>()});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"fit_gf_dlr", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"make_gf_dlr", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"make_hermitian", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"make_real_in_tau", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {
   PyModuleDef_HEAD_INIT,
   "gf_factories_hermitian",                                                                                         /* name of module */
   R"RAWDOC(Factories that symmetrize a Green's function and that build a Green's function on the DLR mesh.)RAWDOC", /* module documentation, may be NULL */
   -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
   module_methods,
   NULL,
   NULL,
   NULL,
   NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_gf_factories_hermitian() {

  if (not c2py::check_python_version("gf_factories_hermitian")) return NULL;

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
