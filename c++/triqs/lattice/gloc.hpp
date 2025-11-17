#pragma once
#include <nda/nda.hpp>
#include <stdexcept>
#include <triqs/gfs.hpp>
#include <itertools/omp_chunk.hpp>
#include <triqs/gfs/gf/targets.hpp>
#include <triqs/lattice/brillouin_zone.hpp>
#include <triqs/tb/tb_hamiltonian.hpp>
#include <triqs/lattice/bz_integrators.hpp>
#include <triqs/utility/root_finder.hpp>

namespace triqs {
  namespace lattice {

    /**
   * @brief Compute the non-interacting local Green's function on a given mesh from a tight binding Hamiltonian
   *
   * @tparam Mesh The mesh type.
   * @param mesh
   * @param tb_hamiltonian A tight binding Hamiltonian
   * @param mu Chemical potential
   * @param opt Container for options related integration of the BZ
   * @return gloc, the local Green's function on frequency mesh of the provided self-energy
   */
    template <typename Mesh>
    gfs::gf<Mesh, gfs::matrix_valued> gloc(Mesh const &w_mesh, tb::tb_hamiltonian const &H_k, double mu, bz_int_options const &opt) {

      auto I       = nda::eye<dcomplex>(H_k.n_orbitals());
      namespace ph = placeholders;
      auto expr_kw = inv((ph::w + mu) * I - nda::clef::make_expr(H_k)(ph::kx, ph::ky, ph::kz));
      // call the integration for this block
      return integrate_bz(expr_kw, w_mesh, opt);
    }

    /**
   * @brief Compute local Green's function on a given mesh from a tight binding Hamiltonian
   * @tparam Mesh The mesh type.
   *
   * @param tb_hamiltonian A tight binding Hamiltonian
   * @param mu Chemical potential
   * @param Sigma The self-energy
   * @param opt Container for options related integration of the BZ
   * @return gloc, the local Green's function on frequency mesh of the provided self-energy
   */
    template <typename Mesh>
    gfs::gf<Mesh, gfs::matrix_valued> gloc(tb::tb_hamiltonian const &H_k, double mu, gfs::gf<Mesh, gfs::matrix_valued> const &Sigma,
                                           bz_int_options const &opt) {

      int n_orbitals = Sigma.target_shape()[0];
      auto I         = nda::eye<dcomplex>(n_orbitals);
      if (n_orbitals != H_k.n_orbitals()) {
        throw std::runtime_error("Number of orbitals in Hk " + std::to_string(H_k.n_orbitals()) + " not matched to shape of self energy "
                                 + std::to_string(n_orbitals));
      }
      namespace ph = placeholders;
      auto expr_kw = inv((ph::w + mu) * I - nda::clef::make_expr(H_k)(ph::kx, ph::ky, ph::kz) - nda::clef::make_expr(std::move(Sigma))[ph::w]);
      // call the integration for this block
      return integrate_bz(expr_kw, Sigma.mesh(), opt);
    }

    /**
   * @brief Compute local Green's function on a given mesh as a block GF, from a tight binding Hamiltonian
   * @tparam Mesh The mesh type.
   *
   * @param tb_hamiltonian A tight binding Hamiltonian
   * @param mu Chemical potential
   * @param Sigma The self-energy
   * @param opt Container for options related integration of the BZ
   * @return gloc, the local Green's function on frequency mesh of the provided self-energy
   */
    template <typename Mesh>
    gfs::block_gf<Mesh, gfs::matrix_valued> gloc(tb::tb_hamiltonian const &H_k, double mu, gfs::block_gf<Mesh, gfs::matrix_valued> const &Sigma,
                                                 bz_int_options const &opt) {

      auto Gloc_result = gfs::block_gf<Mesh>(Sigma[0].mesh(), Sigma.gf_struct());
      for (auto block : range(Sigma.size())) { Gloc_result[block] = gloc(H_k, mu, Sigma[block], opt); }
      return Gloc_result;
    }

    /**
   * @brief Compute the density of the lattice Green's function with a self-energy for one spin channel
   *
   * @tparam Mesh The mesh type.
   *
   * @param target_density total on-site density to determine mu
   * @param tb_hamiltonian A tight binding Hamiltonian
   * @param mu Chemical potential
   * @param Sigma The self-energy
   * @param opt Container for options related integration of the BZ
   * @return Electron density of the lattice Green's function for a single spin channel
   */
    template <typename Mesh>
    double find_chemical_potential(double const target_density, tb::tb_hamiltonian const &H_k, gfs::block_gf<Mesh, gfs::matrix_valued> const &Sigma,
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
      return std::get<0>(
         utility::root_finder(method, f, 0.0, target_density, precision, 0.5, 1000, "Chemical Potential", "Total Density", verbosity));
    }

    /**
   * @brief Compute the density of the lattice Green's function with a self-energy for one spin channel
   *
   * @tparam Mesh The mesh type.
   *
   * @param target_density total on-site density to determine mu
   * @param tb_hamiltonian A tight binding Hamiltonian
   * @param mu Chemical potential
   * @param Sigma The self-energy
   * @param opt Container for options related integration of the BZ
   * @return Electron density of the lattice Green's function for a single spin channel
   */
    template <typename Mesh>
    double find_chemical_potential(double const target_density, tb::tb_hamiltonian const &H_k, gfs::gf<Mesh, gfs::matrix_valued> const &Sigma,
                                   bz_int_options const &opt, std::string method = "dichotomy", double precision = 1.e-5, bool verbosity = false) {

      std::function<double(double)> f = [&H_k, &Sigma, &opt](double mu) { // density function
        auto Gloc = gloc(H_k, mu, Sigma, opt);
        return real(nda::trace(density(Gloc)));
      };
      return std::get<0>(
         utility::root_finder(method, f, 0.0, target_density, precision, 0.5, 1000, "Chemical Potential", "Total Density", verbosity));
    }

    /** @cond DOXYGEN_SKIP_THIS */

    template double find_chemical_potential(double const target_density, tb::tb_hamiltonian const &H_k,
                                            gfs::gf<mesh::imfreq, gfs::matrix_valued> const &Sigma, bz_int_options const &opt, std::string method,
                                            double precision, bool verbosity);
    template double find_chemical_potential(double const target_density, tb::tb_hamiltonian const &H_k,
                                            gfs::gf<mesh::refreq, gfs::matrix_valued> const &Sigma, bz_int_options const &opt, std::string method,
                                            double precision, bool verbosity);
    template double find_chemical_potential(double const target_density, tb::tb_hamiltonian const &H_k,
                                            gfs::block_gf<mesh::imfreq, gfs::matrix_valued> const &Sigma, bz_int_options const &opt,
                                            std::string method, double precision, bool verbosity);
    template double find_chemical_potential(double const target_density, tb::tb_hamiltonian const &H_k,
                                            gfs::block_gf<mesh::refreq, gfs::matrix_valued> const &Sigma, bz_int_options const &opt,
                                            std::string method, double precision, bool verbosity);

    template gfs::gf<mesh::imfreq, gfs::matrix_valued> gloc(tb::tb_hamiltonian const &H_k, double mu,
                                                            gfs::gf<mesh::imfreq, gfs::matrix_valued> const &Sigma, bz_int_options const &opt);

    template gfs::block_gf<mesh::imfreq, gfs::matrix_valued>
    gloc(tb::tb_hamiltonian const &H_k, double mu, gfs::block_gf<mesh::imfreq, gfs::matrix_valued> const &Sigma, bz_int_options const &opt);

    template gfs::gf<mesh::refreq, gfs::matrix_valued> gloc(tb::tb_hamiltonian const &H_k, double mu,
                                                            gfs::gf<mesh::refreq, gfs::matrix_valued> const &Sigma, bz_int_options const &opt);

    template gfs::block_gf<mesh::refreq, gfs::matrix_valued>
    gloc(tb::tb_hamiltonian const &H_k, double mu, gfs::block_gf<mesh::refreq, gfs::matrix_valued> const &Sigma, bz_int_options const &opt);

    template gfs::gf<mesh::imfreq, gfs::matrix_valued> gloc(mesh::imfreq const &w_mesh, tb::tb_hamiltonian const &H_k, double mu,
                                                            bz_int_options const &opt);

    template gfs::gf<mesh::refreq, gfs::matrix_valued> gloc(mesh::refreq const &w_mesh, tb::tb_hamiltonian const &H_k, double mu,
                                                            bz_int_options const &opt);

    /** @endcond */

  } // namespace lattice
} // namespace triqs
