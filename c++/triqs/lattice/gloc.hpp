#pragma once
#include <nda/nda.hpp>
#include <stdexcept>
#include <triqs/gfs.hpp>
#include <itertools/omp_chunk.hpp>
#include <triqs/gfs/gf/targets.hpp>
#include <triqs/lattice/brillouin_zone.hpp>
#include <triqs/lattice/tb_hamiltonian.hpp>
#include <triqs/lattice/bz_integrators.hpp>

namespace triqs {

  /**
   * @brief Compute the non-interacting local Green's function on a given mesh from a tight binding Hamiltonian
   *
   * @tparam Mesh 
   * @param mesh 
   * @param tb_hopping A tight binding Hamiltonian
   * @param mu Chemical potential
   * @param Sigma The self-energy
   * @param opt Container for options related integration of the BZ   
   * @return gloc, the local Green's function on frequency mesh of the provided self-energy
   */
  template <typename Mesh>
  gfs::gf<Mesh, gfs::matrix_valued> gloc(Mesh const &w_mesh, tb_hamiltonian const &H_k, double mu, triqs::lattice::bz_int_options const &opt) {

    auto I       = nda::eye<dcomplex>(H_k.n_orbitals());
    namespace ph = triqs::lattice::placeholders;
    auto expr_kw = inverse((ph::w + mu) * I - nda::clef::make_expr(H_k)(ph::kx, ph::ky, ph::kz));
    // call the integration for this block
    return triqs::lattice::integrate_bz(expr_kw, w_mesh, opt);
  }

  /**
   * @brief Compute local Green's function on a given mesh from a tight binding Hamiltonian
   *
   * @param tb_hopping A tight binding Hamiltonian
   * @param mu Chemical potential
   * @param Sigma The self-energy
   * @param opt Container for options related integration of the BZ   
   * @return gloc, the local Green's function on frequency mesh of the provided self-energy
   */
  template <typename Mesh>
  gfs::gf<Mesh, gfs::matrix_valued> gloc(tb_hamiltonian const &H_k, double mu, gfs::gf<Mesh, gfs::matrix_valued> const &Sigma,
                                         triqs::lattice::bz_int_options const &opt) {

    int n_orbitals = Sigma.target_shape()[0];
    auto I         = nda::eye<dcomplex>(n_orbitals);
    if (n_orbitals != H_k.n_orbitals()) {
      throw std::runtime_error("Number of orbitals in Hk " + std::to_string(H_k.n_orbitals()) + " not matched to shape of self energy "
                               + std::to_string(n_orbitals));
    }
    namespace ph = triqs::lattice::placeholders;
    auto expr_kw = inverse((ph::w + mu) * I - nda::clef::make_expr(H_k)(ph::kx, ph::ky, ph::kz) - nda::clef::make_expr(std::move(Sigma))[ph::w]);
    // call the integration for this block
    return triqs::lattice::integrate_bz(expr_kw, Sigma.mesh(), opt);
  }

  // FIXME : I could put this function to match the interface of modest, but it seems contrived when done in the full orbital space.
  /**
   * @brief Compute local Green's function on a given mesh using tight binding Hamiltonian
   *
   * @param tb_hopping A tight binding Hamiltonian
   * @param mu Chemical potential
   * @param Sigma_dynamic The dynamic part of the embedded self-energy.
   * @param Sigma_static The static part of the embedded self-energy.
   * @param opt Container for options related integration of the BZ   
   * @return gloc, the local Green's function on frequency mesh of the provided self-energy
   */
  /*   template <typename Mesh>
  gfs::gf<Mesh, gfs::matrix_valued> gloc(tb_hamiltonian const &H_k, double mu, gfs::gf<Mesh, gfs::matrix_valued> const &Sigma_dynamic,
                                         gfs::gf<Mesh, gfs::matrix_valued> const &Sigma_static, lattice::bz_int_options const &opt) {

    // call the integration for this block
    return gloc(H_k, mu, Sigma_dynamic + Sigma_static, opt);
  } */

  // TODO should I have a blockgf one for spin up/spin down ?
  /** TODO this is just a way to use of a more general GF object.
  * Can I merge this with the above function to simplify?
  */
  /*   template <typename G>
    requires(gfs::is_gf_v<G> or gfs::is_block_gf_v<G>)
  G gloc(tb_hamiltonian const &H_k, double mu, G const &Sigma, lattice::bz_int_options const &opt) {
    return gloc(H_k, mu, Sigma, opt);
  } */

  // Add DLR functions?

  /** @cond DOXYGEN_SKIP_THIS */

  template gfs::gf<mesh::imfreq, gfs::matrix_valued> gloc(tb_hamiltonian const &H_k, double mu,
                                                          gfs::gf<mesh::imfreq, gfs::matrix_valued> const &Sigma, lattice::bz_int_options const &opt);

  template gfs::gf<mesh::refreq, gfs::matrix_valued> gloc(tb_hamiltonian const &H_k, double mu,
                                                          gfs::gf<mesh::refreq, gfs::matrix_valued> const &Sigma, lattice::bz_int_options const &opt);

  template gfs::gf<mesh::imfreq, gfs::matrix_valued> gloc(mesh::imfreq const &w_mesh, tb_hamiltonian const &H_k, double mu,
                                                          triqs::lattice::bz_int_options const &opt);

  template gfs::gf<mesh::refreq, gfs::matrix_valued> gloc(mesh::refreq const &w_mesh, tb_hamiltonian const &H_k, double mu,
                                                          triqs::lattice::bz_int_options const &opt);

  /** @endcond */

} // namespace triqs
