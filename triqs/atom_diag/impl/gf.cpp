/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2016, P. Seth, I. Krivenko, M. Ferrero and O. Parcollet
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

template<bool Complex>
gf_lehmann_t<Complex> atomic_g_lehmann(ATOM_DIAG const& atom, double beta, gf_struct_t const& gf_struct,
                                       excluded_states_t excluded_states) {
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
  weights.emplace_back(sp_dim, 0);
  for (int ia = 0; ia < sp_dim; ++ia) {
   double w = std::exp(-beta * atom.get_eigenvalue(A, ia));
   weights[A](ia) = w;
   z += w;
  }
 }

 for (auto & w : weights) w /= z;

 // Prepare Lehmann GF container
 gf_lehmann_t<Complex> g;
 g.reserve(gf_struct.size());
 for(auto const& block : gf_struct)
  g.emplace_back(block.second.size(), block.second.size());

 auto const & fops = atom.get_fops();

 // Fill gf_lehmann_t with terms
 int bl = 0;
 for (auto const& block : gf_struct) {
  int bl_size = block.second.size();

  for (int inner_index1 : range(bl_size))
  for (int inner_index2 : range(bl_size)) {
    int n1 = fops[{block.first, block.second[inner_index1]}]; // linear_index of c
    int n2 = fops[{block.first, block.second[inner_index2]}]; // linear_index of c_dag

    auto & terms = g[bl](inner_index1, inner_index2);

    for (int A = 0; A < n_sp; ++A) {                         // index of the A block. sum over all
     int B = atom.cdag_connection(n2, A);                    // index of the block connected to A by operator c_n
     if (B == -1 || atom.c_connection(n1, B) != A) continue; // no matrix element
     for (int ia = 0; ia < atom.get_subspace_dim(A); ++ia) {
      if (is_excluded(A, ia)) continue;
      for (int ib = 0; ib < atom.get_subspace_dim(B); ++ib) {
       if (is_excluded(B, ib)) continue;
       auto residue = (weights[A](ia) + weights[B](ib)) *
                      atom.c_matrix(n1, B)(ia, ib) * atom.cdag_matrix(n2, A)(ib, ia);
       auto Ea = atom.get_eigenvalue(A, ia);
       auto Eb = atom.get_eigenvalue(B, ib);

       if(std::abs(residue) < std::numeric_limits<double>::epsilon()) continue;
       terms.emplace_back(Eb - Ea, residue);
      }
     }
    }
   }
  ++bl;
 }

 return g;
}
template gf_lehmann_t<false> atomic_g_lehmann(ATOM_DIAG_R const&, double, gf_struct_t const&,
                                              excluded_states_t);
template gf_lehmann_t<true> atomic_g_lehmann(ATOM_DIAG_C const&, double, gf_struct_t const&,
                                             excluded_states_t);

// -----------------------------------------------------------------
template<typename M, typename Fill>
inline block_gf<M> make_and_fill_block_gf(gf_struct_t const& gf_struct, gf_mesh<M> const& mesh, Fill fill) {
 std::vector<std::string> block_names;
 std::vector<gf<M>> g_blocks;

 int bl = 0;
 for (auto const& block : gf_struct) {
  block_names.push_back(block.first);
  int bl_size = block.second.size();
  g_blocks.push_back(gf<M>{mesh, {bl_size, bl_size}});
  for(int n1 : range(bl_size))
  for(int n2 : range(bl_size)) {
   fill(bl, n1, n2, slice_target_to_scalar(g_blocks.back(), n1, n2));
  }
  ++bl;
 }

 return make_block_gf(block_names, g_blocks);
}

// -----------------------------------------------------------------
template<bool Complex>
inline void check_lehmann_struct(gf_lehmann_t<Complex> const& lehmann, gf_struct_t const& gf_struct) {
#ifndef NDEBUG
 assert(lehmann.size() == gf_struct.size());
 int bl = 0;
 for(auto const& b : gf_struct) {
  auto s = lehmann[bl].shape();
  assert(s[0] == b.second.size() && s[1] == b.second.size());
  ++bl;
 }
#endif
}

// -----------------------------------------------------------------
template<bool Complex>
block_gf<imtime> atomic_g_tau(gf_lehmann_t<Complex> const& lehmann,
                              gf_struct_t const& gf_struct,
                              gf_mesh<imtime> const& mesh) {
 check_lehmann_struct<Complex>(lehmann, gf_struct);

 double beta = mesh.domain().beta;
 auto fill = [&lehmann, beta](int bl, int n1, int n2, gf_view<imtime, scalar_valued> g) {
  // Iterate over all terms in Lehmann representation
  for(auto term : lehmann[bl](n1, n2)) {
   auto pole = term.first;
   auto residue = term.second;
   auto w = -residue / (pole > 0 ? (1 + std::exp(-beta*pole)) : (std::exp(beta*pole) + 1));

   // Set data points
   for(auto tau : g.mesh())
    g[tau] += w * (pole > 0 ? std::exp(-double(tau)*pole) : std::exp((beta-double(tau))*pole));

   // Set tail
   auto & tail = g.singularity();
   auto tail_coeff = residue;
   int omax = tail.order_max();
   for(int o = 1; o <= omax; ++o) {
    tail(o) += tail_coeff;
    tail_coeff *= pole;
   }
  }
 };
 return make_and_fill_block_gf<imtime>(gf_struct, mesh, fill);
}

template<bool Complex>
block_gf<imtime> atomic_g_tau(ATOM_DIAG const& atom,
                              double beta, gf_struct_t const& gf_struct, int n_tau,
                              excluded_states_t const& excluded_states) {
 return atomic_g_tau<Complex>(atomic_g_lehmann(atom, beta, gf_struct, excluded_states),
                              gf_struct, {beta, Fermion, n_tau});
}
template block_gf<imtime> atomic_g_tau(ATOM_DIAG_R const&, double, gf_struct_t const&, int,
                                       excluded_states_t const&);
template block_gf<imtime> atomic_g_tau(ATOM_DIAG_C const&, double, gf_struct_t const&, int,
                                       excluded_states_t const&);

// -----------------------------------------------------------------
template<bool Complex>
block_gf<imfreq> atomic_g_iw(gf_lehmann_t<Complex> const& lehmann,
                             gf_struct_t const& gf_struct,
                             gf_mesh<imfreq> const& mesh) {
 auto fill = [&lehmann](int bl, int n1, int n2, gf_view<imfreq, scalar_valued> g) {
  // Iterate over all terms in Lehmann representation
  for(auto term : lehmann[bl](n1, n2)) {
   auto pole = term.first;
   auto residue = term.second;

   // Set data points
   for(auto iw : g.mesh()) g[iw] += residue / (iw - pole);

   // Set tail
   auto & tail = g.singularity();
   auto tail_coeff = residue;
   int omax = tail.order_max();
   for(int o = 1; o <= omax; ++o) {
    tail(o) += tail_coeff;
    tail_coeff *= pole;
   }
  }
 };
 return make_and_fill_block_gf<imfreq>(gf_struct, mesh, fill);
}

template<bool Complex>
block_gf<imfreq> atomic_g_iw(ATOM_DIAG const& atom,
                             double beta, gf_struct_t const& gf_struct, int n_iw,
                             excluded_states_t const& excluded_states) {
 return atomic_g_iw<Complex>(atomic_g_lehmann(atom, beta, gf_struct, excluded_states),
                             gf_struct, {beta, Fermion, n_iw});
}
template block_gf<imfreq> atomic_g_iw(ATOM_DIAG_R const&, double, gf_struct_t const&, int,
                                      excluded_states_t const&);
template block_gf<imfreq> atomic_g_iw(ATOM_DIAG_C const&, double, gf_struct_t const&, int,
                                      excluded_states_t const&);

// -----------------------------------------------------------------
template<bool Complex>
block_gf<legendre> atomic_g_l(gf_lehmann_t<Complex> const& lehmann,
                              gf_struct_t const& gf_struct,
                              gf_mesh<legendre> const& mesh) {
 double beta = mesh.domain().beta;
 auto fill = [&lehmann, beta](int bl, int n1, int n2, gf_view<legendre, scalar_valued> g) {
  // Iterate over all terms in Lehmann representation
  for(auto term : lehmann[bl](n1, n2)) {
   auto pole = term.first;
   auto residue = term.second;

   // Set data points
   double x = beta * pole / 2;
   double w = -beta / (2 * std::cosh(x));
   for(auto l : g.mesh()) {
    auto ll = int(l);
    g[l] += residue * w * std::sqrt(2*ll+1) * (ll%2==0 ? 1 : std::copysign(1,-x))
                        * triqs::utility::mod_cyl_bessel_i(ll,std::abs(x));
   }
  }
 };
 return make_and_fill_block_gf<legendre>(gf_struct, mesh, fill);
}

template<bool Complex>
block_gf<legendre> atomic_g_l(ATOM_DIAG const& atom,
                              double beta, gf_struct_t const& gf_struct, int n_l,
                              excluded_states_t const& excluded_states) {
 return atomic_g_l<Complex>(atomic_g_lehmann(atom, beta, gf_struct, excluded_states),
                            gf_struct, {beta, Fermion, static_cast<size_t>(n_l)});
}
template block_gf<legendre> atomic_g_l(ATOM_DIAG_R const&, double, gf_struct_t const&, int,
                                       excluded_states_t const&);
template block_gf<legendre> atomic_g_l(ATOM_DIAG_C const&, double, gf_struct_t const&, int,
                                       excluded_states_t const&);

// -----------------------------------------------------------------
template<bool Complex>
block_gf<refreq> atomic_g_w(gf_lehmann_t<Complex> const& lehmann,
                              gf_struct_t const& gf_struct,
                              gf_mesh<refreq> const& mesh,
                              double broadening) {
 auto fill = [&lehmann, broadening](int bl, int n1, int n2, gf_view<refreq, scalar_valued> g) {
  // Iterate over all terms in Lehmann representation
  for(auto term : lehmann[bl](n1, n2)) {
   auto pole = term.first;
   auto residue = term.second;

   // Set data points
   for(auto w : g.mesh()) g[w] += residue / (w + 1_j * broadening - pole);

   // Set tail
   auto & tail = g.singularity();
   std::complex<double> tail_coeff = residue;
   int omax = tail.order_max();
   for(int o = 1; o <= omax; ++o) {
    tail(o) += tail_coeff;
    tail_coeff *= (pole - 1_j * broadening);
   }
  }
 };
 return make_and_fill_block_gf<refreq>(gf_struct, mesh, fill);
}

template<bool Complex>
block_gf<refreq> atomic_g_w(ATOM_DIAG const& atom,
                            double beta, gf_struct_t const& gf_struct,
                            std::pair<double, double> const& energy_window, int n_w,
                            double broadening,
                            excluded_states_t const& excluded_states) {
 return atomic_g_w<Complex>(atomic_g_lehmann(atom, beta, gf_struct, excluded_states),
                            gf_struct, {energy_window.first, energy_window.second, n_w}, broadening);
}
template block_gf<refreq> atomic_g_w(ATOM_DIAG_R const&, double, gf_struct_t const&,
                                     std::pair<double, double> const&, int, double,
                                     excluded_states_t const&);
template block_gf<refreq> atomic_g_w(ATOM_DIAG_C const&, double, gf_struct_t const&,
                                     std::pair<double, double> const&, int, double,
                                     excluded_states_t const&);
}}
