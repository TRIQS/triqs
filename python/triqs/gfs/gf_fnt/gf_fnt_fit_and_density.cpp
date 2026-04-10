// Explicit template instantiations for fit_tail, fit_hermitian_tail,
// fit_tail_on_window, fit_hermitian_tail_on_window,
// replace_by_tail, replace_by_tail_in_fit_window, and DLR density.
//
// Non-template functions (density for imfreq, refreq, legendre) are
// declared in density.hpp and discovered by clair automatically.

#include <c2py/c2py.hpp>
#include <nda/c2py/converters.hpp>
#include <triqs/c2py_converters/gf.hpp>
#include <triqs/c2py_converters/mesh.hpp>

#include <triqs/gfs.hpp>
#include <triqs/gfs/functions/functions2.hpp>
#include <triqs/gfs/functions/imfreq.hpp>
#include <triqs/gfs/functions/density.hpp>

namespace triqs::gfs {

  // ---------------------------------------------------------------------------
  // fit_tail: template <int N=0, typename G, typename A>
  //   G and A deduced from arguments, N defaults to 0.
  // ---------------------------------------------------------------------------

  // gf_view<Mesh, Target> variants
  template std::pair<array<dcomplex, 3>, double> fit_tail(gf_const_view<imfreq, matrix_valued> const &, array_const_view<dcomplex, 3> const &);
  template std::pair<array<dcomplex, 1>, double> fit_tail(gf_const_view<imfreq, scalar_valued> const &, array_const_view<dcomplex, 1> const &);
  template std::pair<array<dcomplex, 3>, double> fit_tail(gf_const_view<refreq, matrix_valued> const &, array_const_view<dcomplex, 3> const &);
  template std::pair<array<dcomplex, 1>, double> fit_tail(gf_const_view<refreq, scalar_valued> const &, array_const_view<dcomplex, 1> const &);

  // block_gf_view<Mesh, Target> variants
  template std::pair<std::vector<array<dcomplex, 3>>, double> fit_tail(block_gf_const_view<imfreq, matrix_valued> const &,
                                                                       std::vector<array_const_view<dcomplex, 3>> const &);
  template std::pair<std::vector<array<dcomplex, 1>>, double> fit_tail(block_gf_const_view<imfreq, scalar_valued> const &,
                                                                       std::vector<array_const_view<dcomplex, 1>> const &);
  template std::pair<std::vector<array<dcomplex, 3>>, double> fit_tail(block_gf_const_view<refreq, matrix_valued> const &,
                                                                       std::vector<array_const_view<dcomplex, 3>> const &);
  template std::pair<std::vector<array<dcomplex, 1>>, double> fit_tail(block_gf_const_view<refreq, scalar_valued> const &,
                                                                       std::vector<array_const_view<dcomplex, 1>> const &);

  // ---------------------------------------------------------------------------
  // fit_hermitian_tail: template <int N=0, typename G, typename A>
  //   imfreq only, gf and block_gf variants.
  // ---------------------------------------------------------------------------

  template std::pair<array<dcomplex, 3>, double> fit_hermitian_tail(gf_const_view<imfreq, matrix_valued> const &,
                                                                    array_const_view<dcomplex, 3> const &);
  template std::pair<array<dcomplex, 1>, double> fit_hermitian_tail(gf_const_view<imfreq, scalar_valued> const &,
                                                                    array_const_view<dcomplex, 1> const &);
  template std::pair<std::vector<array<dcomplex, 3>>, double> fit_hermitian_tail(block_gf_const_view<imfreq, matrix_valued> const &,
                                                                                 std::vector<array_const_view<dcomplex, 3>> const &);
  template std::pair<std::vector<array<dcomplex, 1>>, double> fit_hermitian_tail(block_gf_const_view<imfreq, scalar_valued> const &,
                                                                                 std::vector<array_const_view<dcomplex, 1>> const &);

  // ---------------------------------------------------------------------------
  // fit_tail_on_window: template <template<...> typename G, typename T>
  // fit_hermitian_tail_on_window: template <template<...> typename G, typename T>
  //   G and T deduced from gf_const_view<imfreq, matrix_valued>.
  // ---------------------------------------------------------------------------

  template auto fit_tail_on_window(gf_const_view<imfreq, matrix_valued> const &, int, int, array_const_view<dcomplex, 3>, int, int);
  template auto fit_hermitian_tail_on_window(gf_const_view<imfreq, matrix_valued> const &, int, int, array_const_view<dcomplex, 3>, int, int);

  // ---------------------------------------------------------------------------
  // replace_by_tail: template <typename T>
  // replace_by_tail_in_fit_window: template <typename T>
  //   T deduced from gf_view target (matrix_valued, T::rank=2, so 1+T::rank=3).
  // ---------------------------------------------------------------------------

  template void replace_by_tail(gf_view<imfreq, matrix_valued>, array_const_view<dcomplex, 3>, int);
  template void replace_by_tail_in_fit_window(gf_view<imfreq, matrix_valued>, array_const_view<dcomplex, 3>);

  // ---------------------------------------------------------------------------
  // density (DLR variants): abbreviated function templates.
  //   auto density(MemoryGf<mesh::dlr> auto const &g) -> matrix<dcomplex> or dcomplex
  //   auto density(MemoryGf<mesh::dlr_imfreq> auto const &g) -> delegates to dlr
  //   auto density(MemoryGf<mesh::dlr_imtime> auto const &g) -> delegates to dlr
  // ---------------------------------------------------------------------------

  template auto density(gf_const_view<dlr, matrix_valued> const &);
  template auto density(gf_const_view<dlr, scalar_valued> const &);
  template auto density(gf_const_view<dlr_imfreq, matrix_valued> const &);
  template auto density(gf_const_view<dlr_imfreq, scalar_valued> const &);
  template auto density(gf_const_view<dlr_imtime, matrix_valued> const &);
  template auto density(gf_const_view<dlr_imtime, scalar_valued> const &);

} // namespace triqs::gfs

#include "gf_fnt_fit_and_density.wrap.cxx"
