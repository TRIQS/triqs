#include <c2py/c2py.hpp>
#include <nda/c2py/converters.hpp>
#include <triqs/c2py_converters/gf.hpp>
#include <triqs/c2py_converters/mesh.hpp>

#include <triqs/gfs.hpp>
#include <triqs/gfs/functions/functions2.hpp>
#include <triqs/gfs/functions/imfreq.hpp>

namespace triqs::gfs {

  // Wrappers for set_from_gf_data_mul_LR, rank 3
  void _set_from_gf_data_mul_LR(array_view<double, 3> a, matrix<double> const &l, array<double, 3> const &b, matrix<double> const &r) {
    set_from_gf_data_mul_LR(a, l, b, r);
  }

  void _set_from_gf_data_mul_LR(array_view<dcomplex, 3> a, matrix<dcomplex> const &l, array<dcomplex, 3> const &b, matrix<dcomplex> const &r) {
    set_from_gf_data_mul_LR(a, l, b, r);
  }

  // Backward compat internal function
  inline gf<mesh::imfreq, matrix_valued> _make_gf_from_real_gf(gf_view<mesh::imfreq, matrix_valued> g) {
    return make_gf_from_real_gf(make_const_view(g));
  }

} // namespace triqs::gfs

#include "wrapped_aux_other.wrap.cxx"
