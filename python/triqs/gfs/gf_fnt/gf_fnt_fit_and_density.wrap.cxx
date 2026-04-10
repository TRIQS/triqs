
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

// density
static auto const _c2py_fun_0 =
   c2py::dispatcher_f_kw_t{
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imfreq> g,
                    nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                          nda::borrowed<nda::mem::AddressSpace::Host>>
                       _p_1) { return triqs::gfs::density(g, _p_1); },
                 "g",
                 "_p_1"_a = nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                  nda::borrowed<nda::mem::AddressSpace::Host>>{}),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued> g,
                    nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                          nda::borrowed<nda::mem::AddressSpace::Host>>
                       _p_1) { return triqs::gfs::density(g, _p_1); },
                 "g",
                 "_p_1"_a = nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                  nda::borrowed<nda::mem::AddressSpace::Host>>{}),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::legendre> g) { return triqs::gfs::density(g); }, "g"),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::legendre, triqs::gfs::scalar_valued> g) { return triqs::gfs::density(g); }, "g"),
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::matrix_valued, nda::C_stride_layout> &g) {
           return triqs::gfs::density<triqs::gfs::gf_const_view<triqs::mesh::dlr>>(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::scalar_valued, nda::C_stride_layout> &g) {
           return triqs::gfs::density<triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::scalar_valued>>(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::matrix_valued, nda::C_stride_layout> &g) {
           return triqs::gfs::density<triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime>>(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued, nda::C_stride_layout> &g) {
           return triqs::gfs::density<triqs::gfs::gf_const_view<triqs::mesh::dlr_imtime, triqs::gfs::scalar_valued>>(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout> &g) {
           return triqs::gfs::density<triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq>>(g);
         },
         "g"),
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout> &g) {
           return triqs::gfs::density<triqs::gfs::gf_const_view<triqs::mesh::dlr_imfreq, triqs::gfs::scalar_valued>>(g);
         },
         "g"),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::refreq> g, double beta) { return triqs::gfs::density(g, beta); }, "g", "beta"),
      c2py::cfun(
         [](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::scalar_valued> g, double beta) { return triqs::gfs::density(g, beta); }, "g",
         "beta"),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::refreq> g) { return triqs::gfs::density(g); }, "g"),
      c2py::cfun([](triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::scalar_valued> g) { return triqs::gfs::density(g); }, "g")};

// fit_hermitian_tail
static auto const _c2py_fun_1 =
   c2py::dispatcher_f_kw_t{
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout> &g,
            const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &known_moments) {
           return triqs::gfs::fit_hermitian_tail<0, triqs::gfs::gf_const_view<triqs::mesh::imfreq>,
                                                 nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout>>(g, known_moments);
         },
         "g",
         "known_moments"_a = nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                   nda::borrowed<nda::mem::AddressSpace::Host>>{}),
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout> &g,
            const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &known_moments) {
           return triqs::gfs::fit_hermitian_tail<0, triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued>,
                                                 nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout>>(g, known_moments);
         },
         "g",
         "known_moments"_a = nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                   nda::borrowed<nda::mem::AddressSpace::Host>>{}),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &bg,
            const std::vector<nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                    nda::borrowed<nda::mem::AddressSpace::Host>>,
                              std::allocator<nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                                   nda::borrowed<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::fit_hermitian_tail<
              0, triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>,
              std::vector<nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout>>>(bg, known_moments);
         },
         "bg",
         "known_moments"_a =
            std::vector<nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                              nda::borrowed<nda::mem::AddressSpace::Host>>,
                        std::allocator<nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                             nda::borrowed<nda::mem::AddressSpace::Host>>>>{}),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &bg,
            const std::vector<nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                    nda::borrowed<nda::mem::AddressSpace::Host>>,
                              std::allocator<nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                                   nda::borrowed<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::fit_hermitian_tail<
              0, triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>,
              std::vector<nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout>>>(bg, known_moments);
         },
         "bg",
         "known_moments"_a =
            std::vector<nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                              nda::borrowed<nda::mem::AddressSpace::Host>>,
                        std::allocator<nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                             nda::borrowed<nda::mem::AddressSpace::Host>>>>{})};

// fit_hermitian_tail_on_window
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &g, int n_min, int n_max,
      nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                            nda::borrowed<nda::mem::AddressSpace::Host>>
         known_moments,
      int n_tail_max, int expansion_order) {
     return triqs::gfs::fit_hermitian_tail_on_window<triqs::gfs::gf_const_view, triqs::gfs::matrix_valued>(g, n_min, n_max, known_moments, n_tail_max,
                                                                                                           expansion_order);
   },
   "g", "n_min", "n_max", "known_moments", "n_tail_max", "expansion_order")};

// fit_tail
static auto const _c2py_fun_3 =
   c2py::dispatcher_f_kw_t{
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout> &g,
            const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &known_moments) {
           return triqs::gfs::fit_tail<0, triqs::gfs::gf_const_view<triqs::mesh::imfreq>,
                                       nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout>>(g, known_moments);
         },
         "g",
         "known_moments"_a = nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                   nda::borrowed<nda::mem::AddressSpace::Host>>{}),
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout> &g,
            const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &known_moments) {
           return triqs::gfs::fit_tail<0, triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued>,
                                       nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout>>(g, known_moments);
         },
         "g",
         "known_moments"_a = nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                   nda::borrowed<nda::mem::AddressSpace::Host>>{}),
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout> &g,
            const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &known_moments) {
           return triqs::gfs::fit_tail<0, triqs::gfs::gf_const_view<triqs::mesh::refreq>,
                                       nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout>>(g, known_moments);
         },
         "g",
         "known_moments"_a = nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                   nda::borrowed<nda::mem::AddressSpace::Host>>{}),
      c2py::cfun(
         [](const triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout> &g,
            const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                        nda::borrowed<nda::mem::AddressSpace::Host>> &known_moments) {
           return triqs::gfs::fit_tail<0, triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::scalar_valued>,
                                       nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout>>(g, known_moments);
         },
         "g",
         "known_moments"_a = nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                   nda::borrowed<nda::mem::AddressSpace::Host>>{}),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &bg,
            const std::vector<nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                    nda::borrowed<nda::mem::AddressSpace::Host>>,
                              std::allocator<nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                                   nda::borrowed<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::fit_tail<0, triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>,
                                       std::vector<nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout>>>(bg, known_moments);
         },
         "bg",
         "known_moments"_a =
            std::vector<nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                              nda::borrowed<nda::mem::AddressSpace::Host>>,
                        std::allocator<nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                             nda::borrowed<nda::mem::AddressSpace::Host>>>>{}),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &bg,
            const std::vector<nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                    nda::borrowed<nda::mem::AddressSpace::Host>>,
                              std::allocator<nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                                   nda::borrowed<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::fit_tail<0, triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>,
                                       std::vector<nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout>>>(bg, known_moments);
         },
         "bg",
         "known_moments"_a =
            std::vector<nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                              nda::borrowed<nda::mem::AddressSpace::Host>>,
                        std::allocator<nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                             nda::borrowed<nda::mem::AddressSpace::Host>>>>{}),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &bg,
            const std::vector<nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                    nda::borrowed<nda::mem::AddressSpace::Host>>,
                              std::allocator<nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                                   nda::borrowed<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::fit_tail<0, triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true>,
                                       std::vector<nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout>>>(bg, known_moments);
         },
         "bg",
         "known_moments"_a =
            std::vector<nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                              nda::borrowed<nda::mem::AddressSpace::Host>>,
                        std::allocator<nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                             nda::borrowed<nda::mem::AddressSpace::Host>>>>{}),
      c2py::cfun(
         [](const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &bg,
            const std::vector<nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                    nda::borrowed<nda::mem::AddressSpace::Host>>,
                              std::allocator<nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                                   nda::borrowed<nda::mem::AddressSpace::Host>>>> &known_moments) {
           return triqs::gfs::fit_tail<0, triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true>,
                                       std::vector<nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout>>>(bg, known_moments);
         },
         "bg",
         "known_moments"_a =
            std::vector<nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                              nda::borrowed<nda::mem::AddressSpace::Host>>,
                        std::allocator<nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                             nda::borrowed<nda::mem::AddressSpace::Host>>>>{})};

// fit_tail_on_window
static auto const _c2py_fun_4 = c2py::dispatcher_f_kw_t{c2py::cfun(
   [](const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> &g, int n_min, int n_max,
      nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                            nda::borrowed<nda::mem::AddressSpace::Host>>
         known_moments,
      int n_tail_max, int expansion_order) {
     return triqs::gfs::fit_tail_on_window<triqs::gfs::gf_const_view, triqs::gfs::matrix_valued>(g, n_min, n_max, known_moments, n_tail_max,
                                                                                                 expansion_order);
   },
   "g", "n_min", "n_max", "known_moments", "n_tail_max", "expansion_order")};

// replace_by_tail
static auto const _c2py_fun_5 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> g,
                                         nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                               nda::borrowed<nda::mem::AddressSpace::Host>>
                                            tail,
                                         int n_min) { return triqs::gfs::replace_by_tail<triqs::gfs::matrix_valued>(g, tail, n_min); },
                                      "g", "tail", "n_min")};

// replace_by_tail_in_fit_window
static auto const _c2py_fun_6 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](triqs::gfs::gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued> g,
                                         nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                               nda::borrowed<nda::mem::AddressSpace::Host>>
                                            tail) { return triqs::gfs::replace_by_tail_in_fit_window<triqs::gfs::matrix_valued>(g, tail); },
                                      "g", "tail")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(
   R"DOC(
[1] Density

Computes the density of the Gf g, i.e $g(=0^-)$
Uses tail moments n=1, 2, and 3

------

[5, 6] Computes the density $$- G( = ) $$

------

Parameters
----------
g : {par_0}
   The Green function

Returns
-------
[5] : {ret_0}
   auto A tensor/matrix or a scalar, depending on the target

[6] : {ret_1}
   auto A tensor/matrix or a scalar, depending on the target
)DOC",
   {{c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::matrix_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::dlr, triqs::gfs::scalar_valued, nda::C_stride_layout> &>()}},
   {c2py::python_typename<
       nda::basic_array<std::complex<double>, 2, nda::C_layout, 'M', nda::heap_basic<nda::mem::mallocator<nda::mem::AddressSpace::Host>>>>(),
    c2py::python_typename<std::complex<double>>()});
static const auto _c2py_doc_1 = _c2py_fun_1.doc(
   R"DOC(
[1, 2] Fit the tail of a Green function using a least-squares fitting procedure
imposing the symmetry :math:`G[i](i,j) = G[-i](j,i)^*`

------

[3, 4] Fit the tail of a Block Green function using a least-squares fitting procedure
imposing the symmetry :math:`G[i](i,j) = G[-i](j,i)^*` for each block

------

Parameters
----------
g : {par_0}
   The Green function object to fit the tail for
known_moments : {par_1}
   The object containing the known high-frequency moments
bg : {par_2}
   The Block Green function object to fit the tail for

Returns
-------
[1] : {ret_0}
   A pair of the tail object and the fitting error

[2] : {ret_1}
   A pair of the tail object and the fitting error
)DOC",
   {{c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout> &>()},
    {c2py::python_typename<const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                       nda::borrowed<nda::mem::AddressSpace::Host>> &>(),
     c2py::python_typename<const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                       nda::borrowed<nda::mem::AddressSpace::Host>> &>()},
    {c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &>()}},
   {c2py::python_typename<std::pair<typename nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                                   nda::borrowed<nda::mem::AddressSpace::Host>>::regular_type,
                                    double>>(),
    c2py::python_typename<std::pair<typename nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                                   nda::borrowed<nda::mem::AddressSpace::Host>>::regular_type,
                                    double>>()});
static const auto _c2py_doc_2 = _c2py_fun_2.doc(R"DOC()DOC");
static const auto _c2py_doc_3 = _c2py_fun_3.doc(
   R"DOC(
[1, 2, 3, 4] Fit the tail of a Green function using a least-squares fitting procedure

------

[5, 6, 7, 8] Fit the tail of a Block Green function using a least-squares fitting procedure

------

Parameters
----------
g : {par_0}
   The Green function object to fit the tail for
known_moments : {par_1}
   The object containing the known high-frequency moments
bg : {par_2}
   The Block Green function object to fit the tail for

Returns
-------
[1, 3] : {ret_0}
   A pair of the tail object and the fitting error

[2, 4] : {ret_1}
   A pair of the tail object and the fitting error
)DOC",
   {{c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout> &>(),
     c2py::python_typename<const triqs::gfs::gf_const_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout> &>()},
    {c2py::python_typename<const nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                       nda::borrowed<nda::mem::AddressSpace::Host>> &>(),
     c2py::python_typename<const nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                       nda::borrowed<nda::mem::AddressSpace::Host>> &>()},
    {c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::imfreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::matrix_valued, nda::C_stride_layout, 1, true> &>(),
     c2py::python_typename<const triqs::gfs::block_gf_view<triqs::mesh::refreq, triqs::gfs::scalar_valued, nda::C_stride_layout, 1, true> &>()}},
   {c2py::python_typename<std::pair<typename nda::basic_array_view<const std::complex<double>, 3, nda::C_stride_layout, 'A', nda::default_accessor,
                                                                   nda::borrowed<nda::mem::AddressSpace::Host>>::regular_type,
                                    double>>(),
    c2py::python_typename<std::pair<typename nda::basic_array_view<const std::complex<double>, 1, nda::C_stride_layout, 'A', nda::default_accessor,
                                                                   nda::borrowed<nda::mem::AddressSpace::Host>>::regular_type,
                                    double>>()});
static const auto _c2py_doc_4 = _c2py_fun_4.doc(R"DOC()DOC");
static const auto _c2py_doc_5 = _c2py_fun_5.doc(R"DOC()DOC");
static const auto _c2py_doc_6 = _c2py_fun_6.doc(R"DOC()DOC");
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"density", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"fit_hermitian_tail", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"fit_hermitian_tail_on_window", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"fit_tail", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {"fit_tail_on_window", (PyCFunction)c2py::pyfkw<_c2py_fun_4>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_4.c_str()},
   {"replace_by_tail", (PyCFunction)c2py::pyfkw<_c2py_fun_5>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_5.c_str()},
   {"replace_by_tail_in_fit_window", (PyCFunction)c2py::pyfkw<_c2py_fun_6>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_6.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "gf_fnt_fit_and_density", /* name of module */
                                        R"RAWDOC()RAWDOC",        /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_gf_fnt_fit_and_density() {

  if (not c2py::check_python_version("gf_fnt_fit_and_density")) return NULL;

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
