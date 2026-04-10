// Explicit template instantiations and wrappers for:
//   - tau_L2_norm (template with auto return)
//   - set_from_legendre/imfreq/imtime (wrappers: C++ uses assignment via gf_keeper)
//   - set_from_pade (wrapper: calls pade())
//   - rebinning_tau, enforce_discontinuity (non-template, discovered by clair)
//   - is_gf_hermitian, is_gf_real_in_tau (template)

#include <c2py/c2py.hpp>
#include <nda/c2py/converters.hpp>
#include <triqs/c2py_converters/gf.hpp>
#include <triqs/c2py_converters/mesh.hpp>

#include <triqs/gfs.hpp>
#include <triqs/gfs/functions/dlr.hpp>
#include <triqs/gfs/functions/imfreq.hpp>
#include <triqs/gfs/functions/imtime.hpp>
#include <triqs/gfs/functions/legendre.hpp>
#include <triqs/gfs/transform/legendre_matsubara.hpp>
#include <triqs/gfs/transform/pade.hpp>

namespace triqs::gfs {

  // ---------------------------------------------------------------------------
  // tau_L2_norm: template <typename G> auto tau_L2_norm(G const &g)
  //   requires(MemoryGf<G> or is_block_gf_v<G>)
  // ---------------------------------------------------------------------------

  // gf_view variants
  template auto tau_L2_norm(gf_const_view<dlr, scalar_valued> const &);
  template auto tau_L2_norm(gf_const_view<dlr, matrix_valued> const &);
  template auto tau_L2_norm(gf_const_view<dlr_imfreq, scalar_valued> const &);
  template auto tau_L2_norm(gf_const_view<dlr_imfreq, matrix_valued> const &);
  template auto tau_L2_norm(gf_const_view<dlr_imtime, scalar_valued> const &);
  template auto tau_L2_norm(gf_const_view<dlr_imtime, matrix_valued> const &);

  // block_gf_view variants
  template auto tau_L2_norm(block_gf_const_view<dlr, scalar_valued> const &);
  template auto tau_L2_norm(block_gf_const_view<dlr, matrix_valued> const &);
  template auto tau_L2_norm(block_gf_const_view<dlr_imfreq, scalar_valued> const &);
  template auto tau_L2_norm(block_gf_const_view<dlr_imfreq, matrix_valued> const &);
  template auto tau_L2_norm(block_gf_const_view<dlr_imtime, scalar_valued> const &);
  template auto tau_L2_norm(block_gf_const_view<dlr_imtime, matrix_valued> const &);

  // ---------------------------------------------------------------------------
  // set_from_legendre / set_from_imfreq / set_from_imtime:
  //   Wrappers needed because C++ uses assignment via gf_keeper:
  //     gw = legendre_to_imfreq(gl)
  //   No C++ function called "set_from_legendre" exists.
  // ---------------------------------------------------------------------------

  // set_from_legendre -> imfreq
  void set_from_legendre(gf_view<imfreq, scalar_valued> gw, gf_const_view<legendre, scalar_valued> gl) { gw = legendre_to_imfreq(gl); }
  void set_from_legendre(gf_view<imfreq, tensor_valued<1>> gw, gf_const_view<legendre, tensor_valued<1>> gl) { gw = legendre_to_imfreq(gl); }
  void set_from_legendre(gf_view<imfreq, matrix_valued> gw, gf_const_view<legendre, matrix_valued> gl) { gw = legendre_to_imfreq(gl); }
  void set_from_legendre(gf_view<imfreq, tensor_valued<3>> gw, gf_const_view<legendre, tensor_valued<3>> gl) { gw = legendre_to_imfreq(gl); }
  void set_from_legendre(gf_view<imfreq, tensor_valued<4>> gw, gf_const_view<legendre, tensor_valued<4>> gl) { gw = legendre_to_imfreq(gl); }

  // set_from_legendre -> imtime
  void set_from_legendre(gf_view<imtime, scalar_valued> gt, gf_const_view<legendre, scalar_valued> gl) { gt = legendre_to_imtime(gl); }
  void set_from_legendre(gf_view<imtime, tensor_valued<1>> gt, gf_const_view<legendre, tensor_valued<1>> gl) { gt = legendre_to_imtime(gl); }
  void set_from_legendre(gf_view<imtime, matrix_valued> gt, gf_const_view<legendre, matrix_valued> gl) { gt = legendre_to_imtime(gl); }
  void set_from_legendre(gf_view<imtime, tensor_valued<3>> gt, gf_const_view<legendre, tensor_valued<3>> gl) { gt = legendre_to_imtime(gl); }
  void set_from_legendre(gf_view<imtime, tensor_valued<4>> gt, gf_const_view<legendre, tensor_valued<4>> gl) { gt = legendre_to_imtime(gl); }

  // set_from_imfreq (imfreq -> legendre)
  void set_from_imfreq(gf_view<legendre, scalar_valued> gl, gf_const_view<imfreq, scalar_valued> gw) { gl = imfreq_to_legendre(gw); }
  void set_from_imfreq(gf_view<legendre, tensor_valued<1>> gl, gf_const_view<imfreq, tensor_valued<1>> gw) { gl = imfreq_to_legendre(gw); }
  void set_from_imfreq(gf_view<legendre, matrix_valued> gl, gf_const_view<imfreq, matrix_valued> gw) { gl = imfreq_to_legendre(gw); }
  void set_from_imfreq(gf_view<legendre, tensor_valued<3>> gl, gf_const_view<imfreq, tensor_valued<3>> gw) { gl = imfreq_to_legendre(gw); }
  void set_from_imfreq(gf_view<legendre, tensor_valued<4>> gl, gf_const_view<imfreq, tensor_valued<4>> gw) { gl = imfreq_to_legendre(gw); }

  // set_from_imtime (imtime -> legendre)
  void set_from_imtime(gf_view<legendre, scalar_valued> gl, gf_const_view<imtime, scalar_valued> gt) { gl = imtime_to_legendre(gt); }
  void set_from_imtime(gf_view<legendre, tensor_valued<1>> gl, gf_const_view<imtime, tensor_valued<1>> gt) { gl = imtime_to_legendre(gt); }
  void set_from_imtime(gf_view<legendre, matrix_valued> gl, gf_const_view<imtime, matrix_valued> gt) { gl = imtime_to_legendre(gt); }
  void set_from_imtime(gf_view<legendre, tensor_valued<3>> gl, gf_const_view<imtime, tensor_valued<3>> gt) { gl = imtime_to_legendre(gt); }
  void set_from_imtime(gf_view<legendre, tensor_valued<4>> gl, gf_const_view<imtime, tensor_valued<4>> gt) { gl = imtime_to_legendre(gt); }

  // ---------------------------------------------------------------------------
  // set_from_pade: wrapper calling pade(gw, giw, n_points, freq_offset)
  //   pade scalar_valued is non-template; multi-target is an abbreviated template.
  // ---------------------------------------------------------------------------

  void set_from_pade(gf_view<refreq, scalar_valued> gw, gf_const_view<imfreq, scalar_valued> giw, int n_points, double freq_offset) {
    pade(gw, giw, n_points, freq_offset);
  }
  void set_from_pade(gf_view<refreq, tensor_valued<1>> gw, gf_const_view<imfreq, tensor_valued<1>> giw, int n_points, double freq_offset) {
    pade(gw, giw, n_points, freq_offset);
  }
  void set_from_pade(gf_view<refreq, matrix_valued> gw, gf_const_view<imfreq, matrix_valued> giw, int n_points, double freq_offset) {
    pade(gw, giw, n_points, freq_offset);
  }
  void set_from_pade(gf_view<refreq, tensor_valued<3>> gw, gf_const_view<imfreq, tensor_valued<3>> giw, int n_points, double freq_offset) {
    pade(gw, giw, n_points, freq_offset);
  }
  void set_from_pade(gf_view<refreq, tensor_valued<4>> gw, gf_const_view<imfreq, tensor_valued<4>> giw, int n_points, double freq_offset) {
    pade(gw, giw, n_points, freq_offset);
  }

  // ---------------------------------------------------------------------------
  // rebinning_tau: non-template, discovered by clair.
  // enforce_discontinuity: non-template, discovered by clair.
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // is_gf_hermitian: template <typename G> bool is_gf_hermitian(G const &, double)
  //   requires(is_gf_v<G> or is_block_gf_v<G>)
  // ---------------------------------------------------------------------------

  // imfreq
  template bool is_gf_hermitian(gf_const_view<imfreq, scalar_valued> const &, double);
  template bool is_gf_hermitian(gf_const_view<imfreq, matrix_valued> const &, double);
  template bool is_gf_hermitian(gf_const_view<imfreq, tensor_valued<4>> const &, double);

  template bool is_gf_hermitian(block_gf_const_view<imfreq, scalar_valued> const &, double);
  template bool is_gf_hermitian(block_gf_const_view<imfreq, matrix_valued> const &, double);
  template bool is_gf_hermitian(block_gf_const_view<imfreq, tensor_valued<4>> const &, double);

  template bool is_gf_hermitian(block2_gf_const_view<imfreq, scalar_valued> const &, double);
  template bool is_gf_hermitian(block2_gf_const_view<imfreq, matrix_valued> const &, double);
  template bool is_gf_hermitian(block2_gf_const_view<imfreq, tensor_valued<4>> const &, double);

  // imtime
  template bool is_gf_hermitian(gf_const_view<imtime, scalar_valued> const &, double);
  template bool is_gf_hermitian(gf_const_view<imtime, matrix_valued> const &, double);
  template bool is_gf_hermitian(gf_const_view<imtime, tensor_valued<4>> const &, double);

  template bool is_gf_hermitian(block_gf_const_view<imtime, scalar_valued> const &, double);
  template bool is_gf_hermitian(block_gf_const_view<imtime, matrix_valued> const &, double);
  template bool is_gf_hermitian(block_gf_const_view<imtime, tensor_valued<4>> const &, double);

  template bool is_gf_hermitian(block2_gf_const_view<imtime, scalar_valued> const &, double);
  template bool is_gf_hermitian(block2_gf_const_view<imtime, matrix_valued> const &, double);
  template bool is_gf_hermitian(block2_gf_const_view<imtime, tensor_valued<4>> const &, double);

  // ---------------------------------------------------------------------------
  // is_gf_real_in_tau: template <typename G> bool is_gf_real_in_tau(G const &, double)
  //   imfreq only.
  // ---------------------------------------------------------------------------

  template bool is_gf_real_in_tau(gf_const_view<imfreq, scalar_valued> const &, double);
  template bool is_gf_real_in_tau(gf_const_view<imfreq, matrix_valued> const &, double);

  template bool is_gf_real_in_tau(block_gf_const_view<imfreq, scalar_valued> const &, double);
  template bool is_gf_real_in_tau(block_gf_const_view<imfreq, matrix_valued> const &, double);

} // namespace triqs::gfs

#include "gf_fnt_misc.wrap.cxx"
