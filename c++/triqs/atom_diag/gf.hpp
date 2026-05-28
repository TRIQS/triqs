// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
// Copyright (c) 2017 Igor Krivenko
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Build the atomic Green's function from a solved diagonalization problem on different meshes.
 */

#pragma once

#include <vector>
#include <triqs/gfs.hpp>
#include "./atom_diag.hpp"

namespace triqs {
  namespace atom_diag {

    using namespace triqs::gfs;

    /**
     * @addtogroup triqs-atom-diag
     * @{
     */

    /**
     * @brief Lehmann representation of a single matrix element of the Green's function.
     *
     * @details A list of pole and residue pairs with pole 
     * \f[ 
     *   p = E_{B',j} - E_{B,i} 
     * \f] 
     * and residue
     * \f[
     *   r_ p = \bigl(e^{-\beta E_{B,i}} + \eta\, e^{-\beta E_{B',j}}\bigr)
     *   \langle B, i\,|\, \hat c_a \,|\, B', j\rangle \langle B', j\,|\, \hat c_b^\dagger \,|\, B, i\rangle / Z 
     * \f].
     */
    template <bool Complex> using gf_scalar_lehmann_t = std::vector<std::pair<double, typename atom_diag<Complex>::scalar_t>>;

    /**
     * @brief Lehmann representation of a block matrix-valued Green's function.
     *
     * @details Outer list indexed by Green's-function block (per `gf_struct`), inner matrix indexed by orbital pair
     * `(a, b)` inside the block; each matrix entry is the Lehmann representation of \f$ G_{ab}(z) \f$ for the
     * corresponding orbital pair.
     */
    template <bool Complex> using gf_lehmann_t = std::vector<matrix<gf_scalar_lehmann_t<Complex>>>;

    /**
     * @brief List of excluded eigenstates.
     * 
     * @details Given as \f$ (B, i) \f$ pairs (subspace index \f$ B \f$ and eigenstate index \f$ i \f$ inside the 
     * subspace, with \f$ 0 \le i < \dim(B) \f$). Excluded states do not contribute to the Lehmann sum.
     */
    using excluded_states_t = std::vector<std::pair<int, int>>;

    /**
     * @brief Build the Lehmann representation of the atomic Green's function.
     *
     * @details Iterates over every pair of eigenstates \f$ (B, i) \f$, \f$ (B', j) \f$ of the Hamiltonian (subject to
     * the non-zero connection constraint) and collects all non-vanishing pole - residue pairs \f$ (p, r_p) \f$. The 
     * resulting structure can then be evaluated on any of the supported target meshes via the corresponding builders.
     *
     * @tparam Complex Specialization of the diagonalization problem (real or complex Hamiltonian).
     * @param atom Solved diagonalization problem.
     * @param beta Inverse temperature \f$ \beta > 0 \f$.
     * @param gf_struct Block structure of the Green's function: block name -> list of inner indices.
     * @param excluded_states Eigenstates to exclude from the Lehmann sum, as \f$ (B, i) \f$ pairs.
     * @return Atomic Green's function in the Lehmann representation.
     */
    template <bool Complex>
    gf_lehmann_t<Complex> atomic_g_lehmann(atom_diag<Complex> const &atom, double beta, gf_struct_t const &gf_struct,
                                           excluded_states_t excluded_states = {});

    /**
     * @brief Build the atomic imaginary-time Green's function from a precomputed Lehmann representation.
     *
     * @details For each \f$ (p, r_p) \f$ in the Lehmann representation, the fermionic imaginary-time evaluator is
     * \f[
     *   G(\tau) = \sum_p r_p \, \frac{-e^{-\tau p}}{1 + e^{-\beta p}}, \quad \tau \in [0, \beta].
     * \f]
     *
     * @tparam Complex Type of the Lehmann residues (real or complex).
     * @param lehmann Lehmann representation of the Green's function.
     * @param gf_struct Block structure of the Green's function: block name -> list of inner indices.
     * @param mesh Imaginary-time mesh used to evaluate the Green's function.
     * @return Atomic Green's function \f$ G_{ab}(\tau) \f$.
     */
    template <bool Complex>
    block_gf<imtime> atomic_g_tau(gf_lehmann_t<Complex> const &lehmann, gf_struct_t const &gf_struct, mesh::imtime const &mesh);

    /**
     * @brief Build the atomic imaginary-time Green's function directly from a solved diagonalization problem.
     *
     * @details Internally builds the Lehmann representation and evaluates it on the requested mesh,
     * \f[
     *   G(\tau) = \sum_p r_p \, \frac{-e^{-\tau p}}{1 + e^{-\beta p}}, \quad \tau \in [0, \beta].
     * \f]
     *
     * @tparam Complex Specialization of the diagonalization problem (real or complex Hamiltonian).
     * @param atom Solved diagonalization problem.
     * @param beta Inverse temperature \f$ \beta > 0 \f$.
     * @param gf_struct Block structure of the Green's function: block name -> list of inner indices.
     * @param n_tau Number of imaginary-time points.
     * @param excluded_states Eigenstates to exclude from the Lehmann sum, as \f$ (B, i) \f$ pairs.
     * @return Atomic Green's function \f$ G_{ab}(\tau) \f$.
     */
    template <bool Complex>
    block_gf<imtime> atomic_g_tau(atom_diag<Complex> const &atom, double beta, gf_struct_t const &gf_struct, int n_tau,
                                  excluded_states_t const &excluded_states = {});

    /**
     * @brief Build the atomic Matsubara Green's function from a precomputed Lehmann representation.
     *
     * @details For each \f$ (p, r_p) \f$ in the Lehmann representation, the evaluator is
     * \f[
     *   G(i\omega) = \sum_p \frac{r_p}{i\omega - p}.
     * \f]
     *
     * @tparam Complex Type of the Lehmann residues (real or complex).
     * @param lehmann Lehmann representation of the Green's function.
     * @param gf_struct Block structure of the Green's function: block name -> list of inner indices.
     * @param mesh Matsubara mesh used to evaluate the Green's function.
     * @return Atomic Green's function \f$ G_{ab}(i\omega) \f$.
     */
    template <bool Complex>
    block_gf<imfreq> atomic_g_iw(gf_lehmann_t<Complex> const &lehmann, gf_struct_t const &gf_struct, mesh::imfreq const &mesh);

    /**
     * @brief Build the atomic Matsubara Green's function directly from a solved diagonalization problem.
     *
     * @details Internally builds the Lehmann representation and evaluates
     * \f[
     *   G(i\omega) = \sum_p \frac{r_p}{i\omega - p}
     * \f]
     * on the requested mesh.
     *
     * @tparam Complex Specialization of the diagonalization problem (real or complex Hamiltonian).
     * @param atom Solved diagonalization problem.
     * @param beta Inverse temperature \f$ \beta > 0 \f$.
     * @param gf_struct Block structure of the Green's function: block name -> list of inner indices.
     * @param n_iw Number of positive Matsubara frequencies.
     * @param excluded_states Eigenstates to exclude from the Lehmann sum, as \f$ (B, i) \f$ pairs.
     * @return Atomic Green's function \f$ G_{ab}(i\omega) \f$.
     */
    template <bool Complex>
    block_gf<imfreq> atomic_g_iw(atom_diag<Complex> const &atom, double beta, gf_struct_t const &gf_struct, int n_iw,
                                 excluded_states_t const &excluded_states = {});

    /**
     * @brief Build the atomic Green's function in the Legendre basis from a precomputed Lehmann representation.
     *
     * @details The Legendre coefficients are obtained from the imaginary-time form by the standard projection
     * \f[
     *   G_\ell = \sqrt{2\ell + 1}\, \int_0^\beta d\tau\, P_\ell(2\tau/\beta - 1)\, G(\tau) \;.
     * \f]
     *
     * @tparam Complex Type of the Lehmann residues (real or complex).
     * @param lehmann Lehmann representation of the Green's function.
     * @param gf_struct Block structure of the Green's function: block name -> list of inner indices.
     * @param mesh Legendre mesh used to evaluate the Green's function.
     * @return Atomic Green's function \f$ G_{ab}(\ell) \f$.
     */
    template <bool Complex>
    block_gf<legendre> atomic_g_l(gf_lehmann_t<Complex> const &lehmann, gf_struct_t const &gf_struct, mesh::legendre const &mesh);

    /**
     * @brief Build the atomic Green's function in the Legendre basis directly from a solved diagonalization problem.
     *
     * @details Internally builds the Lehmann representation and evaluates the corresponding Legendre coefficients
     * \f[
     *   G_\ell = \sqrt{2\ell + 1}\, \int_0^\beta d\tau\, P_\ell(2\tau/\beta - 1)\, G(\tau) \;.
     * \f]
     *
     * @tparam Complex Specialization of the diagonalization problem (real or complex Hamiltonian).
     * @param atom Solved diagonalization problem.
     * @param beta Inverse temperature \f$ \beta > 0 \f$.
     * @param gf_struct Block structure of the Green's function: block name -> list of inner indices.
     * @param n_l Number of Legendre coefficients to compute.
     * @param excluded_states Eigenstates to exclude from the Lehmann sum, as \f$ (B, i) \f$ pairs.
     * @return Atomic Green's function \f$ G_{ab}(\ell) \f$.
     */
    template <bool Complex>
    block_gf<legendre> atomic_g_l(atom_diag<Complex> const &atom, double beta, gf_struct_t const &gf_struct, int n_l,
                                  excluded_states_t const &excluded_states = {});

    /**
     * @brief Build the atomic retarded Green's function on a real-frequency mesh from a precomputed Lehmann
     * representation.
     *
     * @details For each \f$ (p, r_p) \f$ in the Lehmann representation, the retarded evaluator is
     * \f[
     *   G(\omega) = \sum_p \frac{r_p}{\omega + i\eta - p} \; ,
     * \f]
     * with the broadening \f$ \eta \f$.
     * 
     * @tparam Complex Type of the Lehmann residues (real or complex).
     * @param lehmann Lehmann representation of the Green's function.
     * @param gf_struct Block structure of the Green's function: block name -> list of inner indices.
     * @param mesh Real-frequency mesh used to evaluate the Green's function.
     * @param broadening Lorentzian broadening \f$ \eta \f$ of the spectrum (small positive imaginary-frequency shift).
     * @return Atomic Green's function \f$ G_{ab}(\omega) \f$.
     */
    template <bool Complex>
    block_gf<refreq> atomic_g_w(gf_lehmann_t<Complex> const &lehmann, gf_struct_t const &gf_struct, mesh::refreq const &mesh, double broadening = 0);

    /**
     * @brief Build the atomic retarded Green's function on a real-frequency mesh directly from a solved
     * diagonalization problem.
     *
     * @details Internally builds the Lehmann representation, constructs a real-frequency mesh from the requested energy 
     * window and number of frequency points, and evaluates 
     * \f[
     *   G(\omega) = \sum_p \frac{r_p}{\omega + i\eta - p} \; ,
     * \f]
     * with the broadening \f$ \eta \f$.
     *
     * @tparam Complex Specialization of the diagonalization problem (real or complex Hamiltonian).
     * @param atom Solved diagonalization problem.
     * @param beta Inverse temperature \f$ \beta > 0 \f$.
     * @param gf_struct Block structure of the Green's function: block name -> list of inner indices.
     * @param energy_window Energy window \f$ (\omega_{\text{min}}, \omega_{\text{max}}) \f$ of the real-frequency
     * mesh.
     * @param n_w Number of frequency points.
     * @param broadening Lorentzian broadening \f$ \eta \f$ of the spectrum (small positive imaginary-frequency shift).
     * @param excluded_states Eigenstates to exclude from the Lehmann sum, as \f$ (B, i) \f$ pairs.
     * @return Atomic Green's function \f$ G_{ab}(\omega) \f$.
     */
    template <bool Complex>
    block_gf<refreq> atomic_g_w(atom_diag<Complex> const &atom, double beta, gf_struct_t const &gf_struct,
                                std::pair<double, double> const &energy_window, int n_w, double broadening = 0,
                                excluded_states_t const &excluded_states = {});

    /** @} */

  } // namespace atom_diag
} // namespace triqs
