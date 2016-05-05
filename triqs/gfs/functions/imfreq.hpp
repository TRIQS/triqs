/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2013 by O. Parcollet
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
 using triqs::arrays::conj; // not found on gcc 5

 /// ---------------------------  A few specific functions ---------------------------------

 /// This function takes a g(i omega_n) on half mesh (positive omega_n) and returns a gf on the whole mesh
 /**
   using G(-i omega_n) = G(i omega_n)^* for real G(tau) functions.
 */
 template <typename T> gf<imfreq, T> make_gf_from_real_gf(gf_const_view<imfreq, T> g) {
  if (!g.mesh().positive_only()) TRIQS_RUNTIME_ERROR << "gf imfreq is not for omega_n >0, real_to_complex does not apply";
  auto const &dat = g.data();
  auto sh = dat.shape();
  int is_boson = (g.mesh().domain().statistic == Boson);
  long L = sh[0];
  sh[0] = 2 * sh[0] - is_boson;
  array<dcomplex, std14::decay_t<decltype(dat)>::rank> new_data(sh);
  auto _ = arrays::ellipsis{};
  if (is_boson) new_data(L - 1, _) = dat(0, _);
  int L1 = (is_boson ? L - 1 : L);
  for (int u = is_boson; u < L; ++u) {
   new_data(L1 + u, _) = dat(u, _);
   new_data(L - 1 - u, _) = conj(dat(u, _));
  }
  return {gf_mesh<imfreq>{g.mesh().domain(), L}, std::move(new_data), g.singularity(), g.indices()};
 }

 /// Test if gf is real in tau
 /**
   @param g the gf
   @param tolerance tolerance threshold $\epsilon$
   @return true iif $$\forall n,\; \max_{ab}|g^*_{ab}(-i\omega_n)-g_{ab}(i\omega_n)|<\epsilon$$
  */
 template <typename T> bool is_gf_real_in_tau(gf_const_view<imfreq, T> g, double tolerance = 1.e-13) {
  if (g.mesh().positive_only()) return true;
  using triqs::arrays::max_element; // the case real, complex is not found by ADL
  for (auto const &w : g.mesh().get_positive_freq())
   if (max_element(abs(conj(g(-w)) - g(w))) > tolerance) return false;
  return true;
 }
 template <typename T> bool is_gf_real_in_tau(gf_view<imfreq, T> g, double tolerance = 1.e-13) {
  return is_gf_real_in_tau(make_const_view(g), tolerance);
 }
 template <typename T> bool is_gf_real_in_tau(gf<imfreq, T> const &g, double tolerance = 1.e-13) {
  return is_gf_real_in_tau(make_const_view(g), tolerance);
 }

 /// Make a const view of the positive frequency part of the function
 template <typename G> view_type_t<G> positive_freq_view(G &&g) {
  static_assert(std::is_same<typename std14::decay_t<G>::variable_t, imfreq>::value,
                "positive_freq_view only makes senses for imfreq gf");
  if (g.mesh().positive_only()) return g;
  long L = g.mesh().size();
  long L1 = (L + 1) / 2; // fermion : L is even. boson, L = 2p+1 --> p+1
  int is_boson = (g.mesh().domain().statistic == Boson);
  return {g.mesh().get_positive_freq(), g.data()(range(L1 - is_boson, L), arrays::ellipsis()), g.singularity(), g.indices()};
 }

 /// Make_real_in_tau symmetrize the function in freq, to ensure its FT is real.
 template <typename T> gf<imfreq, T> make_real_in_tau(gf_const_view<imfreq, T> g) {
  return make_gf_from_real_gf(positive_freq_view(g));
 }
 template <typename T> gf<imfreq, T> make_real_in_tau(gf_view<imfreq, T> g) { return make_real_in_tau(make_const_view(g)); }
 template <typename T> gf<imfreq, T> make_real_in_tau(gf<imfreq, T> const &g) { return make_real_in_tau(make_const_view(g)); }
}
}
