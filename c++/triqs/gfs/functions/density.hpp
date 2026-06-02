// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Hugo U. R. Strand, Nils Wentzell

#pragma once
#include "../gf/gf_view.hpp"
namespace triqs {
  namespace gfs {

    //-------------------------------------------------------
    // For Imaginary Matsubara Frequency functions
    // ------------------------------------------------------

    /**
     * @brief Compute the density from a Green's function.
     *
     * @details The density is reconstructed from the imaginary-frequency data using the high-frequency tail moments 
     * \f$ n = 1, 2, 3 \f$. Known moments may be passed explicitly; if omitted, they are obtained by tail fitting.
     *
     * @param g Input Green's function.
     * @param known_moments Array of known high-frequency moments.
     * @return The density matrix.
     */
    nda::matrix<dcomplex> density(gf_const_view<mesh::imfreq> g, array_const_view<dcomplex, 3> known_moments = {});

    /**
     * @brief Compute the density from a Green's function.
     *
     * @details The density is reconstructed from the imaginary-frequency data using the high-frequency tail moments 
     * \f$ n = 1, 2, 3 \f$. Known moments may be passed explicitly; if omitted, they are obtained by tail fitting.
     *
     * @param g Input Green's function.
     * @param known_moments Array of known high-frequency moments.
     * @return The scalar density.
     */
    dcomplex density(gf_const_view<mesh::imfreq, scalar_valued> g, array_const_view<dcomplex, 1> known_moments = {});

    /**
     * @brief Compute the density from a Green's function.
     *
     * @details The density is obtained directly from the Legendre coefficients.
     *
     * @param g Input Green's function.
     * @return The density matrix.
     */
    nda::matrix<dcomplex> density(gf_const_view<mesh::legendre> g);

    /**
     * @brief Compute the density from a Green's function.
     *
     * @details The density is obtained directly from the Legendre coefficients.
     *
     * @param g Input Green's function.
     * @return The scalar density.
     */
    dcomplex density(gf_const_view<mesh::legendre, scalar_valued> g);

    //-------------------------------------------------------
    // DLR
    // ------------------------------------------------------

    /**
     * @brief Compute the density from a Green's function.
     *
     * @param g Input Green's function.
     * @return The density (matrix).
     */
    auto density(MemoryGf<mesh::dlr> auto const &g) {
      auto res = make_regular(-g(g.mesh().beta()));
      // Transpose to get <cdag_i c_j> instead of <cdag_j c_i>
      if constexpr (requires { transpose(res); }) {
        res = transpose(res);
        return res;
      } else
        return res;
    }

    /**
     * @brief Compute the density from a Green's function.
     *
     * @param g Input Green's function.
     * @return The density (matrix).
     */
    auto density(MemoryGf<mesh::dlr_imtime> auto const &g) { return density(make_gf_dlr(g)); }

    /**
     * @brief Compute the density from a Green's function.
     *
     * @param g Input Green's function.
     * @return The density (matrix).
     */
    auto density(MemoryGf<mesh::dlr_imfreq> auto const &g) { return density(make_gf_dlr(g)); }

    //-------------------------------------------------------
    // For Real Frequency functions
    // ------------------------------------------------------

    /**
     * @brief Compute the density from a Green's function.
     *
     * @details Obtained by integrating the spectral function weighted by the Fermi function at temperature \f$ T = 
     * 1/\beta \f$.
     *
     * @param g Input Green's function.
     * @param beta Inverse temperature.
     * @return The density matrix.
     */
    nda::matrix<dcomplex> density(gf_const_view<mesh::refreq> g, double beta);

    /**
     * @brief Compute the density from a Green's function.
     *
     * @details Obtained by integrating the spectral function weighted by the Fermi function at temperature \f$ T = 
     * 1/\beta \f$.
     *
     * @param g Input Green's function.
     * @param beta Inverse temperature.
     * @return The scalar density.
     */
    dcomplex density(gf_const_view<mesh::refreq, scalar_valued> g, double beta);

    /**
     * @brief Compute the zero-temperature density from a real-frequency Green's function.
     *
     * @details Obtained by integrating the spectral function over the negative real axis (\f$ T = 0 \f$ Fermi 
     * function).
     *
     * @param g Input Green's function.
     * @return The density matrix.
     */
    nda::matrix<dcomplex> density(gf_const_view<mesh::refreq> g);

    /**
     * @brief Compute the zero-temperature density from a real-frequency Green's function.
     *
     * @details Obtained by integrating the spectral function over the negative real axis (\f$ T = 0 \f$ Fermi 
     * function).
     *
     * @param g Input Green's function.
     * @return The scalar density.
     */
    dcomplex density(gf_const_view<mesh::refreq, scalar_valued> g);

    //-------------------------------------------------------
    // General Version for Block Gf
    // ------------------------------------------------------

    /**
     * @brief Compute the density block-wise from a block Green's function on a Matsubara or real-frequency mesh.
     *
     * @details The density of each block is evaluated with its own known high-frequency moments.
     *
     * @tparam BGf The type of the block Green's function.
     * @tparam R The rank of the per-block known-moment arrays.
     * @param gin The block Green's function.
     * @param known_moments Array of known high-frequency moments.
     * @return A vector of per-block densities (matrix or scalar depending on the target shape of each block).
     */
    template <typename BGf, int R>
    auto density(BGf const &gin, std::vector<array<dcomplex, R>> const &known_moments)
      requires(is_block_gf_v<BGf>)
    {

      using mesh_t = typename BGf::mesh_t;
      static_assert(std::is_same_v<mesh_t, mesh::imfreq> or std::is_same_v<mesh_t, mesh::refreq>,
                    "Density Function must be called with either an imfreq or a refreq Green Function");

      using r_t = decltype(density(gin[0], known_moments[0]));
      std::vector<r_t> dens_vec;

      TRIQS_ASSERT2(gin.size() == known_moments.size(), "Density: Require equal number of blocks in block_gf and known_moments vector");

      for (auto [gin_bl, km_bl] : itertools::zip(gin, known_moments)) dens_vec.push_back(density(gin_bl, km_bl));
      return dens_vec;
    }

  } // namespace gfs
} // namespace triqs

namespace nda::clef {
  TRIQS_CLEF_MAKE_FNT_LAZY(density);
}
