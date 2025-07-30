#pragma once
#include <nda/nda.hpp>
#include <stdexcept>
#include <triqs/gfs.hpp>
#include <itertools/omp_chunk.hpp>
#include <triqs/gfs/gf/targets.hpp>
#include <triqs/lattice/brillouin_zone.hpp>
#include <triqs/lattice/tb_hamiltonian.hpp>
#include <triqs/lattice/bz_integrators.hpp>
#include <triqs/utility/gf_utils.hpp>

// TODO perhaps move this somewhere else?
namespace nda::temp {
  CLEF_MAKE_FNT_LAZY(inverse);
}

namespace triqs {

  /**
   * @brief Compute local Green's function on a given mesh using tight binding Hamiltonian
   *        In particular, this function works on a single gf object, for a single spin channel,
   *        by the function which works on block2_gf above.
   *
   * @param tb_hopping A tight binding hopping t(k) (matrix valued)
   * @param mu Chemical potential
   * @param Sigma_embed Self energy in the embedded basis? TODO I don't think this was relevant
   * @param Sigma_DC ? TODO JC would like this to be optional
   * @param bz_int_options Option of the BZ integration
   * @param d_H ? for magnetic fields etc?
   * @return gloc = gf on the mesh
   */
  template <typename Mesh>
  gfs::gf<Mesh, gfs::matrix_valued> gloc(tb_hamiltonian const &H_k, double mu, gfs::gf<Mesh, gfs::matrix_valued> const &Sigma,
                                         lattice::bz_int_options const &opt) {

    int n_orbitals = Sigma.target_shape()[0];
    auto I         = nda::eye<dcomplex>(n_orbitals);
    if (n_orbitals != H_k.n_orbitals()) {
      throw std::runtime_error("Number of orbitals in Hk " + std::to_string(H_k.n_orbitals()) + " not matched to shape of self energy "
                               + std::to_string(n_orbitals));
    }
    namespace ph = lattice::placeholders;
    auto expr_kw =
       nda::temp::inverse((ph::w + mu) * I - nda::clef::make_expr(H_k)(ph::kx, ph::ky, ph::kz) - nda::clef::make_expr(std::move(Sigma))[ph::w]);
    // call the integration for this block
    return lattice::integrate_bz(expr_kw, Sigma.mesh(), opt);
  }

  // TODO should I have a blockgf one for spin up/spin down ?
  /** TODO this is just a way to use of a more general GF object.
  * Can I merge this with the above function to simplify?
  */
  /*   template <typename G>
    requires(gfs::is_gf_v<G> or gfs::is_block_gf_v<G>)
  G gloc(tb_hamiltonian const &H_k, double mu, G const &Sigma, lattice::bz_int_options const &opt) {
    return gloc(H_k, mu, Sigma, opt);
  } */

  /** @cond DOXYGEN_SKIP_THIS */

  template gfs::gf<mesh::imfreq, gfs::matrix_valued> gloc(tb_hamiltonian const &H_k, double mu,
                                                          gfs::gf<mesh::imfreq, gfs::matrix_valued> const &Sigma, lattice::bz_int_options const &opt);

  template gfs::gf<mesh::refreq, gfs::matrix_valued> gloc(tb_hamiltonian const &H_k, double mu,
                                                          gfs::gf<mesh::refreq, gfs::matrix_valued> const &Sigma, lattice::bz_int_options const &opt);

  /** @endcond */

} // namespace triqs
