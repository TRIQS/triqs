/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2016 by M. Ferrero, O. Parcollet
 * Copyright (C) 2018 The Simons Foundation, Authors: H. UR Strand, M. Zingl
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once

namespace triqs {
  namespace gfs {

    //-------------------------------------------------------
    // For Imaginary Matsubara Frequency functions
    // ------------------------------------------------------

    /// Density
    /**
     * Computes the density of the Gf g, i.e $g(\tau=0^-)$
     * Uses tail moments n=1, 2, and 3
     */
    arrays::matrix<dcomplex> density(gf_const_view<mesh::imfreq> g, array_const_view<dcomplex, 3> = {});
    dcomplex density(gf_const_view<mesh::imfreq, scalar_valued> g, array_const_view<dcomplex, 1> = {});

    arrays::matrix<dcomplex> density(gf_const_view<mesh::legendre> g);
    dcomplex density(gf_const_view<mesh::legendre, scalar_valued> g);

    //-------------------------------------------------------
    // For Real Frequency functions
    // ------------------------------------------------------

    arrays::matrix<dcomplex> density(gf_const_view<mesh::refreq> g, double beta);
    dcomplex density(gf_const_view<mesh::refreq, scalar_valued> g, double beta);

    arrays::matrix<dcomplex> density(gf_const_view<mesh::refreq> g);
    dcomplex density(gf_const_view<mesh::refreq, scalar_valued> g);

    //-------------------------------------------------------
    // General Version for Block Gf
    // ------------------------------------------------------

    template <typename BGf, int R>
    auto density(BGf const &gin, std::vector<array<dcomplex, R>> const &known_moments) REQUIRES(is_block_gf_v<BGf>) {

      using mesh_t = typename BGf::mesh_t;
      static_assert(std::is_same_v<mesh_t, mesh::imfreq> or std::is_same_v<mesh_t, mesh::refreq>, "Density Function must be called with either an imfreq or a refreq Green Function");

      using r_t = decltype(density(gin[0], known_moments[0]));
      std::vector<r_t> dens_vec;

      TRIQS_ASSERT2(gin.size() == known_moments.size(), "Density: Require equal number of blocks in block_gf and known_moments vector");

      for (auto [gin_bl, km_bl] : itertools::zip(gin, known_moments)) dens_vec.push_back(density(gin_bl, km_bl));
      return dens_vec;
    }

  } // namespace gfs

  namespace clef {
    TRIQS_CLEF_MAKE_FNT_LAZY(density);
  }
} // namespace triqs
