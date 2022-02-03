// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2017 Igor Krivenko
// Copyright (c) 2018-2020 Simons Foundation
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

#include "../functions.hpp"
#include "../gf.hpp"
#include <cmath>
#include <algorithm>
#include <limits>
#include <utility>
#include <vector>
#include <triqs/arrays.hpp>
#include <triqs/utility/numeric_ops.hpp>
#include <triqs/utility/legendre.hpp>

namespace triqs {
  namespace atom_diag {

    using namespace triqs::arrays;

#define ATOM_DIAG atom_diag<Complex>
#define ATOM_DIAG_R atom_diag<false>
#define ATOM_DIAG_C atom_diag<true>
#define ATOM_DIAG_T typename atom_diag<Complex>

    // -----------------------------------------------------------------

    //////////////////////////////
    /// Lehmann representation ///
    //////////////////////////////

    // Generate Lehmann representation of GF defined by gf_struct
    // passing every term to proc(int bl, int n1, int n2, double pole, scalar_t residue)
    template <bool Complex, typename ProcessTerm>
    inline void atomic_g_lehmann_impl(ATOM_DIAG const &atom, double beta, gf_struct_t const &gf_struct, excluded_states_t excluded_states,
                                      ProcessTerm proc) {
      // Sort excluded states to speed up lookups
      std::sort(excluded_states.begin(), excluded_states.end());
      auto is_excluded = [&excluded_states](int A, int ia) {
        return std::binary_search(excluded_states.begin(), excluded_states.end(), std::make_pair(A, ia));
      };

      // Gibbs weights, exp(-beta E_i) / Z
      std::vector<vector<double>> weights;
      // Partition function
      double z = 0;
      int n_sp = atom.n_subspaces();
      for (int A = 0; A < n_sp; ++A) {
        int sp_dim = atom.get_subspace_dim(A);
        weights.emplace_back(nda::zeros<double>(sp_dim));
        for (int ia = 0; ia < sp_dim; ++ia) {
          double w       = std::exp(-beta * atom.get_eigenvalue(A, ia));
          weights[A](ia) = w;
          z += w;
        }
      }

      for (auto &w : weights) w /= z;

      auto const &fops = atom.get_fops();

      // Generate all terms in Lehmann representation
      int bl = 0;
      for (auto const &[block, bl_size] : gf_struct) {

        for (int inner_index1 : range(bl_size))
          for (int inner_index2 : range(bl_size)) {
            int n1 = fops[{block, inner_index1}]; // linear_index of c
            int n2 = fops[{block, inner_index2}]; // linear_index of c_dag

            for (int A = 0; A < n_sp; ++A) {                          // index of the A block. sum over all
              int B = atom.cdag_connection(n2, A);                    // index of the block connected to A by operator c_n
              if (B == -1 || atom.c_connection(n1, B) != A) continue; // no matrix element
              for (int ia = 0; ia < atom.get_subspace_dim(A); ++ia) {
                if (is_excluded(A, ia)) continue;
                for (int ib = 0; ib < atom.get_subspace_dim(B); ++ib) {
                  if (is_excluded(B, ib)) continue;
                  auto residue = (weights[A](ia) + weights[B](ib)) * atom.c_matrix(n1, B)(ia, ib) * atom.cdag_matrix(n2, A)(ib, ia);
                  auto Ea      = atom.get_eigenvalue(A, ia);
                  auto Eb      = atom.get_eigenvalue(B, ib);

                  if (std::abs(residue) < std::numeric_limits<double>::epsilon()) continue;
                  proc(bl, inner_index1, inner_index2, Eb - Ea, residue);
                }
              }
            }
          }
        ++bl;
      }
    }

    // -----------------------------------------------------------------

    // Construct and return Lehmann representation
    template <bool Complex>
    gf_lehmann_t<Complex> atomic_g_lehmann(ATOM_DIAG const &atom, double beta, gf_struct_t const &gf_struct, excluded_states_t excluded_states) {
      // Prepare Lehmann GF container
      gf_lehmann_t<Complex> lehmann;
      lehmann.reserve(gf_struct.size());
      for (auto const &[block, bl_size] : gf_struct) { lehmann.emplace_back(bl_size, bl_size); }

      // Fill container
      auto fill = [&lehmann](int bl, int n1, int n2, double pole, ATOM_DIAG_T::scalar_t residue) { lehmann[bl](n1, n2).emplace_back(pole, residue); };
      atomic_g_lehmann_impl(atom, beta, gf_struct, excluded_states, fill);

      return lehmann;
    }
    template gf_lehmann_t<false> atomic_g_lehmann(ATOM_DIAG_R const &, double, gf_struct_t const &, excluded_states_t);
    template gf_lehmann_t<true> atomic_g_lehmann(ATOM_DIAG_C const &, double, gf_struct_t const &, excluded_states_t);

    // -----------------------------------------------------------------

    /// In debug mode, check that Lehmann representation object is compatible with gf_struct
    template <bool Complex, typename T> inline void check_lehmann_struct(gf_lehmann_t<Complex> const &lehmann, block_gf_view<T> g) {
#ifndef NDEBUG
      assert(lehmann.size() == g.size());
      int bl = 0;
      for (auto const &block : g) {
        //assert(block.target_shape() == lehmann[bl].shape());
        ++bl;
      }
#endif
    }

    // -----------------------------------------------------------------

    /// Fill block_gf<T> object using precomputed Lehmann representation
    template <bool Complex, typename T, typename ProcessTerm>
    inline void fill_block_gf_from_lehmann(block_gf_view<T> g, gf_lehmann_t<Complex> const &lehmann, ProcessTerm proc) {
      check_lehmann_struct<Complex>(lehmann, g);

      int bl = 0;
      for (auto &block : g) {
        auto shape = block.target_shape();
        for (int n1 : range(shape[0]))
          for (int n2 : range(shape[1])) {
            for (auto const &term : lehmann[bl](n1, n2)) proc(bl, n1, n2, term.first, term.second);
          }
        ++bl;
      }
    }

    // -----------------------------------------------------------------

    //////////////////////////
    /// GF: Imaginary time ///
    //////////////////////////

    // Returns lambda-function compatible with atomic_g_lehmann_impl
    // The lambda-function captures 'gf' and fills it when passed to atomic_g_lehmann_impl
    template <bool Complex> auto make_term_proc(double beta, block_gf_view<mesh::imtime> gf) {
      return [gf, beta](int bl, int n1, int n2, double pole, ATOM_DIAG_T::scalar_t residue) mutable {
        gf_view<imtime, scalar_valued> g = slice_target_to_scalar(gf[bl], n1, n2);

        auto w = -residue / (pole > 0 ? (1 + std::exp(-beta * pole)) : (std::exp(beta * pole) + 1));

        // Set data points
        for (auto tau : g.mesh()) g[tau] += w * (pole > 0 ? std::exp(-double(tau) * pole) : std::exp((beta - double(tau)) * pole));
      };
    }

    // -----------------------------------------------------------------

    /// G(\tau) from Lehmann representation
    template <bool Complex>
    block_gf<mesh::imtime> atomic_g_tau(gf_lehmann_t<Complex> const &lehmann, gf_struct_t const &gf_struct, mesh::imtime const &mesh) {
      double beta = mesh.domain().beta;
      auto g      = block_gf{mesh, gf_struct};
      fill_block_gf_from_lehmann<Complex>(g(), lehmann, make_term_proc<Complex>(beta, g()));
      return g;
    }
    template block_gf<mesh::imtime> atomic_g_tau<false>(gf_lehmann_t<false> const &, gf_struct_t const &, mesh::imtime const &);
    template block_gf<mesh::imtime> atomic_g_tau<true>(gf_lehmann_t<true> const &, gf_struct_t const &, mesh::imtime const &);

    // -----------------------------------------------------------------

    /// G(\tau) from atom_diag
    template <bool Complex>
    block_gf<mesh::imtime> atomic_g_tau(ATOM_DIAG const &atom, double beta, gf_struct_t const &gf_struct, size_t n_tau,
                                        excluded_states_t const &excluded_states) {
      auto g = block_gf<imtime>{{beta, Fermion, static_cast<long>(n_tau)}, gf_struct};
      atomic_g_lehmann_impl(atom, beta, gf_struct, excluded_states, make_term_proc<Complex>(beta, g()));
      return g;
    }
    template block_gf<mesh::imtime> atomic_g_tau(ATOM_DIAG_R const &, double, gf_struct_t const &, size_t, excluded_states_t const &);
    template block_gf<mesh::imtime> atomic_g_tau(ATOM_DIAG_C const &, double, gf_struct_t const &, size_t, excluded_states_t const &);

    // -----------------------------------------------------------------

    /////////////////////////////////
    /// GF: Matsubara frequencies ///
    /////////////////////////////////

    // Returns lambda-function compatible with atomic_g_lehmann_impl
    // The lambda-function captures 'gf' and fills it when passed to atomic_g_lehmann_impl
    template <bool Complex> auto make_term_proc(block_gf_view<imfreq> gf) {
      return [gf](int bl, int n1, int n2, double pole, ATOM_DIAG_T::scalar_t residue) mutable {
        gf_view<imfreq, scalar_valued> g = slice_target_to_scalar(gf[bl], n1, n2);

        // Set data points
        for (auto iw : g.mesh()) g[iw] += residue / (iw - pole);
      };
    }

    // -----------------------------------------------------------------

    /// G(i\omega) from Lehmann representation
    template <bool Complex>
    block_gf<mesh::imfreq> atomic_g_iw(gf_lehmann_t<Complex> const &lehmann, gf_struct_t const &gf_struct, mesh::imfreq const &mesh) {
      auto g = block_gf{mesh, gf_struct};
      fill_block_gf_from_lehmann<Complex>(g(), lehmann, make_term_proc<Complex>(g()));
      return g;
    }
    template block_gf<mesh::imfreq> atomic_g_iw<false>(gf_lehmann_t<false> const &, gf_struct_t const &, mesh::imfreq const &);
    template block_gf<mesh::imfreq> atomic_g_iw<true>(gf_lehmann_t<true> const &, gf_struct_t const &, mesh::imfreq const &);

    // -----------------------------------------------------------------

    /// G(i\omega) from atom_diag
    template <bool Complex>
    block_gf<mesh::imfreq> atomic_g_iw(ATOM_DIAG const &atom, double beta, gf_struct_t const &gf_struct, size_t n_iw,
                                       excluded_states_t const &excluded_states) {
      auto g = block_gf<mesh::imfreq>{{beta, Fermion, n_iw}, gf_struct};
      atomic_g_lehmann_impl(atom, beta, gf_struct, excluded_states, make_term_proc<Complex>(g()));
      return g;
    }
    template block_gf<mesh::imfreq> atomic_g_iw(ATOM_DIAG_R const &, double, gf_struct_t const &, size_t, excluded_states_t const &);
    template block_gf<mesh::imfreq> atomic_g_iw(ATOM_DIAG_C const &, double, gf_struct_t const &, size_t, excluded_states_t const &);

    // -----------------------------------------------------------------

    /////////////////////////////////
    /// GF: Legendre coefficients ///
    /////////////////////////////////

    // Returns lambda-function compatible with atomic_g_lehmann_impl
    // The lambda-function captures 'gf' and fills it when passed to atomic_g_lehmann_impl
    template <bool Complex> auto make_term_proc(double beta, block_gf_view<mesh::legendre> gf) {
      return [gf, beta](int bl, int n1, int n2, double pole, ATOM_DIAG_T::scalar_t residue) mutable {
        gf_view<legendre, scalar_valued> g = slice_target_to_scalar(gf[bl], n1, n2);

        // Set data points
        double x = beta * pole / 2;
        double w = -beta / (2 * std::cosh(x));
        for (auto l : g.mesh()) {
          auto ll = int(l);
          g[l] += residue * w * std::sqrt(2 * ll + 1) * (ll % 2 == 0 ? 1 : std::copysign(1, -x)) * triqs::utility::mod_cyl_bessel_i(ll, std::abs(x));
        }
      };
    }

    // -----------------------------------------------------------------

    /// G_\ell from Lehmann representation
    template <bool Complex>
    block_gf<mesh::legendre> atomic_g_l(gf_lehmann_t<Complex> const &lehmann, gf_struct_t const &gf_struct, mesh::legendre const &mesh) {
      double beta = mesh.domain().beta;
      auto g      = block_gf{mesh, gf_struct};
      fill_block_gf_from_lehmann<Complex>(g(), lehmann, make_term_proc<Complex>(beta, g()));
      return g;
    }
    template block_gf<legendre> atomic_g_l<false>(gf_lehmann_t<false> const &, gf_struct_t const &, mesh::legendre const &);
    template block_gf<legendre> atomic_g_l<true>(gf_lehmann_t<true> const &, gf_struct_t const &, mesh::legendre const &);

    // -----------------------------------------------------------------

    /// G_\ell from atom_diag
    template <bool Complex>
    block_gf<mesh::legendre> atomic_g_l(ATOM_DIAG const &atom, double beta, gf_struct_t const &gf_struct, int n_l,
                                        excluded_states_t const &excluded_states) {
      auto g = block_gf<legendre>{{beta, Fermion, n_l}, gf_struct};
      atomic_g_lehmann_impl(atom, beta, gf_struct, excluded_states, make_term_proc<Complex>(beta, g()));
      return g;
    }
    template block_gf<legendre> atomic_g_l(ATOM_DIAG_R const &, double, gf_struct_t const &, int, excluded_states_t const &);
    template block_gf<legendre> atomic_g_l(ATOM_DIAG_C const &, double, gf_struct_t const &, int, excluded_states_t const &);

    // -----------------------------------------------------------------

    ////////////////////////////
    /// GF: Real frequencies ///
    ////////////////////////////

    // Returns lambda-function compatible with atomic_g_lehmann_impl
    // The lambda-function captures 'gf' and fills it when passed to atomic_g_lehmann_impl
    template <bool Complex> auto make_term_proc(block_gf_view<refreq> gf, double broadening) {
      return [gf, broadening](int bl, int n1, int n2, double pole, ATOM_DIAG_T::scalar_t residue) mutable {
        gf_view<refreq, scalar_valued> g = slice_target_to_scalar(gf[bl], n1, n2);

        // Set data points
        for (auto w : g.mesh()) g[w] += residue / (w + 1i * broadening - pole);
      };
    }

    // -----------------------------------------------------------------

    /// G(\omega) from Lehmann representation
    template <bool Complex>
    block_gf<mesh::refreq> atomic_g_w(gf_lehmann_t<Complex> const &lehmann, gf_struct_t const &gf_struct, mesh::refreq const &mesh,
                                      double broadening) {
      auto g = block_gf{mesh, gf_struct};
      fill_block_gf_from_lehmann<Complex>(g(), lehmann, make_term_proc<Complex>(g(), broadening));
      return g;
    }
    template block_gf<mesh::refreq> atomic_g_w<false>(gf_lehmann_t<false> const &, gf_struct_t const &, mesh::refreq const &, double);
    template block_gf<mesh::refreq> atomic_g_w<true>(gf_lehmann_t<true> const &, gf_struct_t const &, mesh::refreq const &, double);

    // -----------------------------------------------------------------

    /// G(\omega) from atom_diag
    template <bool Complex>
    block_gf<mesh::refreq> atomic_g_w(ATOM_DIAG const &atom, double beta, gf_struct_t const &gf_struct,
                                      std::pair<double, double> const &energy_window, int n_w, double broadening,
                                      excluded_states_t const &excluded_states) {
      auto g = block_gf<mesh::refreq>{{energy_window.first, energy_window.second, n_w}, gf_struct};
      atomic_g_lehmann_impl(atom, beta, gf_struct, excluded_states, make_term_proc<Complex>(g(), broadening));
      return g;
    }
    template block_gf<mesh::refreq> atomic_g_w(ATOM_DIAG_R const &, double, gf_struct_t const &, std::pair<double, double> const &, int, double,
                                               excluded_states_t const &);
    template block_gf<mesh::refreq> atomic_g_w(ATOM_DIAG_C const &, double, gf_struct_t const &, std::pair<double, double> const &, int, double,
                                               excluded_states_t const &);
  } // namespace atom_diag
} // namespace triqs
