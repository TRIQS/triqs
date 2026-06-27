#pragma once

#include "./bz_integrators.hpp"
#include "./tb_hk.hpp"
#include "../utility/root_finder.hpp"
#include "../../gfs.hpp"
#include "../../gfs/gf/targets.hpp"

#include <itertools/omp_chunk.hpp>
#include <nda/nda.hpp>

#include <functional>
#include <stdexcept>
#include <string>
#include <utility>

namespace triqs::experimental::lattice {

  /**
   * @addtogroup triqs-experimental-lattice
   * @{
   */

  /**
   * @brief Compute the non-interacting local Green's function from a tight-binding Hamiltonian on a given mesh.
   *
   * @details The local Green's function is obtained by integrating \f$ [(\omega + \mu) I - H(\mathbf{k})]^{-1} \f$ over
   * the Brillouin zone for each frequency of the mesh.
   *
   * @tparam Mesh Frequency mesh type.
   * @param w_mesh Frequency mesh on which the Brillouin-zone integration is performed for each frequency.
   * @param H_k Tight-binding Hamiltonian \f$ H(\mathbf{k}) \f$.
   * @param mu Chemical potential \f$ \mu \f$.
   * @param opt Options controlling the Brillouin-zone integration.
   * @return Local Green's function on the given frequency mesh.
   */
  template <typename Mesh> gfs::gf<Mesh, gfs::matrix_valued> gloc(Mesh const &w_mesh, tb_hk const &H_k, double mu, bz_int_options const &opt) {

    auto I       = nda::eye<dcomplex>(H_k.n_orbitals());
    namespace ph = placeholders;
    auto expr_kw = nda::linalg::inv((ph::w + mu) * I - nda::clef::make_expr(H_k)(ph::kx, ph::ky, ph::kz));
    // call the integration for this block
    return integrate_bz(expr_kw, w_mesh, opt);
  }

  /**
   * @brief Compute the interacting local Green's function from a tight-binding Hamiltonian and a self-energy.
   *
   * @details The local Green's function is obtained by integrating
   * \f$ [(\omega + \mu) I - H(\mathbf{k}) - \Sigma(\omega)]^{-1} \f$ over the Brillouin zone for each frequency of the
   * self-energy mesh. The number of orbitals of the self-energy must match that of the Hamiltonian.
   *
   * @tparam Mesh Frequency mesh type.
   * @param H_k Tight-binding Hamiltonian \f$ H(\mathbf{k}) \f$.
   * @param mu Chemical potential \f$ \mu \f$.
   * @param Sigma Self-energy \f$ \Sigma(\omega) \f$ defining the frequency mesh.
   * @param opt Options controlling the Brillouin-zone integration.
   * @return Local Green's function on the frequency mesh of the given self-energy.
   */
  template <typename Mesh>
  gfs::gf<Mesh, gfs::matrix_valued> gloc(tb_hk const &H_k, double mu, gfs::gf<Mesh, gfs::matrix_valued> const &Sigma, bz_int_options const &opt) {

    int n_orbitals = Sigma.target_shape()[0];
    auto I         = nda::eye<dcomplex>(n_orbitals);
    if (n_orbitals != H_k.n_orbitals()) {
      throw std::runtime_error("Number of orbitals in Hk " + std::to_string(H_k.n_orbitals()) + " not matched to shape of self energy "
                               + std::to_string(n_orbitals));
    }
    namespace ph = placeholders;
    auto expr_kw =
       nda::linalg::inv((ph::w + mu) * I - nda::clef::make_expr(H_k)(ph::kx, ph::ky, ph::kz) - nda::clef::make_expr(std::move(Sigma))[ph::w]);
    // call the integration for this block
    return integrate_bz(expr_kw, Sigma.mesh(), opt);
  }

  /**
   * @brief Compute the interacting local Green's function as a block Green's function.
   *
   * @details This overload applies the single-block calculation to each block of the given block self-energy and
   * collects the results into a block Green's function with the same block structure.
   *
   * @tparam Mesh Frequency mesh type.
   * @param H_k Tight-binding Hamiltonian \f$ H(\mathbf{k}) \f$.
   * @param mu Chemical potential \f$ \mu \f$.
   * @param Sigma Block self-energy \f$ \Sigma(\omega) \f$ defining the block structure and frequency mesh.
   * @param opt Options controlling the Brillouin-zone integration.
   * @return Local block Green's function on the frequency mesh of the given self-energy.
   */
  template <typename Mesh>
  gfs::block_gf<Mesh, gfs::matrix_valued> gloc(tb_hk const &H_k, double mu, gfs::block_gf<Mesh, gfs::matrix_valued> const &Sigma,
                                               bz_int_options const &opt) {

    auto Gloc_result = gfs::block_gf<Mesh>(Sigma[0].mesh(), Sigma.gf_struct());
    for (auto block : range(Sigma.size())) { Gloc_result[block] = gloc(H_k, mu, Sigma[block], opt); }
    return Gloc_result;
  }

  /**
   * @brief Find the chemical potential that yields a target density for a block self-energy.
   *
   * @details This function adjusts the chemical potential \f$ \mu \f$ with a root finder until the total density of the
   * local block Green's function (summed over all blocks) matches the target density.
   *
   * @tparam Mesh Frequency mesh type.
   * @param target_density Target total on-site density used to determine \f$ \mu \f$.
   * @param H_k Tight-binding Hamiltonian \f$ H(\mathbf{k}) \f$.
   * @param Sigma Block self-energy \f$ \Sigma(\omega) \f$.
   * @param opt Options controlling the Brillouin-zone integration.
   * @param method Root-finding method used to determine \f$ \mu \f$, either `"dichotomy"` or `"bisection"`.
   * @param precision Absolute precision at which \f$ \mu \f$ is determined.
   * @param verbosity Whether to print the convergence progress.
   * @return Chemical potential \f$ \mu \f$ reproducing the target density.
   */
  template <typename Mesh>
  double find_chemical_potential(double const target_density, tb_hk const &H_k, gfs::block_gf<Mesh, gfs::matrix_valued> const &Sigma,
                                 bz_int_options const &opt, std::string method = "dichotomy", double precision = 1.e-5, bool verbosity = false) {

    // density for a block GF
    std::function<double(double)> f = [&H_k, &Sigma, &opt](double mu) {
      double n = 0;
      for (auto block : range(Sigma.size())) {
        auto Gloc = gloc(H_k, mu, Sigma[block], opt);
        n += real(nda::trace(density(Gloc)));
      }
      return n;
    };
    return std::get<0>(utility::root_finder(method, f, 0.0, target_density, precision, 0.5, 1000, "Chemical Potential", "Total Density", verbosity));
  }

  /**
   * @brief Find the chemical potential that yields a target density for a single-block self-energy.
   *
   * @details This function adjusts the chemical potential \f$ \mu \f$ with a root finder until the density of the local
   * Green's function matches the target density.
   *
   * @tparam Mesh Frequency mesh type.
   * @param target_density Target on-site density used to determine \f$ \mu \f$.
   * @param H_k Tight-binding Hamiltonian \f$ H(\mathbf{k}) \f$.
   * @param Sigma Self-energy \f$ \Sigma(\omega) \f$.
   * @param opt Options controlling the Brillouin-zone integration.
   * @param method Root-finding method used to determine \f$ \mu \f$, either `"dichotomy"` or `"bisection"`.
   * @param precision Absolute precision at which \f$ \mu \f$ is determined.
   * @param verbosity Whether to print the convergence progress.
   * @return Chemical potential \f$ \mu \f$ reproducing the target density.
   */
  template <typename Mesh>
  double find_chemical_potential(double const target_density, tb_hk const &H_k, gfs::gf<Mesh, gfs::matrix_valued> const &Sigma,
                                 bz_int_options const &opt, std::string method = "dichotomy", double precision = 1.e-5, bool verbosity = false) {

    std::function<double(double)> f = [&H_k, &Sigma, &opt](double mu) { // density function
      auto Gloc = gloc(H_k, mu, Sigma, opt);
      return real(nda::trace(density(Gloc)));
    };
    return std::get<0>(utility::root_finder(method, f, 0.0, target_density, precision, 0.5, 1000, "Chemical Potential", "Total Density", verbosity));
  }

  /** @} */

} // namespace triqs::experimental::lattice
