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
namespace triqs::gfs {

  using triqs::arrays::conj; // not found on gcc 5

  /// ---------------------------  A few specific functions ---------------------------------

  /// This function takes a g(i omega_n) on half mesh (positive omega_n) and returns a gf on the whole mesh
  /**
   using G(-i omega_n) = G(i omega_n)^* for real G(tau) functions.
 */
  template <typename T> gf<imfreq, T> make_gf_from_real_gf(gf_const_view<imfreq, T> g) {
    if (!g.mesh().positive_only()) TRIQS_RUNTIME_ERROR << "gf imfreq is not for omega_n >0, real_to_complex does not apply";
    auto const &dat = g.data();
    auto sh         = dat.shape();
    int is_boson    = (g.mesh().domain().statistic == Boson);
    long L          = sh[0];
    sh[0]           = 2 * sh[0] - is_boson;
    array<dcomplex, std::decay_t<decltype(dat)>::rank> new_data(sh);
    auto _ = arrays::ellipsis{};
    if (is_boson) new_data(L - 1, _) = dat(0, _);
    int L1 = (is_boson ? L - 1 : L);
    for (int u = is_boson; u < L; ++u) {
      new_data(L1 + u, _)    = dat(u, _);
      new_data(L - 1 - u, _) = conj(dat(u, _));
    }
    return {gf_mesh<imfreq>{g.mesh().domain(), L}, std::move(new_data), g.indices()};
  }

  /// Test if gf is real in tau
  /**
   @param g the gf
   @param tolerance tolerance threshold $\epsilon$
   @return true iif $$\forall n,\; \max_{ab}|g^*_{ab}(-i\omega_n)-g_{ab}(i\omega_n)|<\epsilon$$
  */
  template <typename G> std::enable_if_t<is_gf<G>::value, bool> is_gf_real_in_tau(G const &g, double tolerance = 1.e-13) {
    static_assert(std::is_same<typename std::decay_t<G>::variable_t, imfreq>::value, "is_gf_real_in_tau only makes senses for imfreq gf");
    if (g.mesh().positive_only()) return true;
    using triqs::arrays::max_element; // the case real, complex is not found by ADL
    for (auto const &w : g.mesh().get_positive_freq())
      if (max_element(abs(conj(g(-w)) - g(w))) > tolerance) return false;
    return true;
  }
  template <typename G> std::enable_if_t<is_block_gf_or_view<G>::value, bool> is_gf_real_in_tau(G const &g, double tolerance = 1.e-13) {
    return std::all_of(g.begin(), g.end(), [&](auto &g_bl) { return is_gf_real_in_tau(g_bl, tolerance); });
  }

  /// Make a const view of the positive frequency part of the function
  template <typename G> view_type_t<G> positive_freq_view(G &&g) {
    static_assert(std::is_same<typename std::decay_t<G>::variable_t, imfreq>::value, "positive_freq_view only makes senses for imfreq gf");
    if (g.mesh().positive_only()) return g;
    long L       = g.mesh().size();
    long L1      = (L + 1) / 2; // fermion : L is even. boson, L = 2p+1 --> p+1
    int is_boson = (g.mesh().domain().statistic == Boson);
    return {g.mesh().get_positive_freq(), g.data()(range(L1 - is_boson, L), triqs::arrays::ellipsis()), g.indices()};
  }

  /// Make_real_in_tau symmetrize the function in freq, to ensure its FT is real.
  template <typename G> std::enable_if_t<is_gf<G>::value, typename G::regular_type> make_real_in_tau(G const &g) {
    static_assert(std::is_same<typename std::decay_t<G>::variable_t, imfreq>::value, "make_real_in_tau only makes senses for imfreq gf");
    return make_gf_from_real_gf(positive_freq_view(g));
  }
  template <typename G> std::enable_if_t<is_block_gf_or_view<G>::value, typename G::regular_type> make_real_in_tau(G const &g) {
    return map_block_gf(make_real_in_tau<typename G::g_t>, g);
  }

  // ------------------------------------------------------------------------------------------------------

  template <template <typename, typename> typename G, typename T> auto restricted_view(G<imfreq, T> const &g, int n_max) {
    auto iw_mesh = gf_mesh<imfreq>{g.mesh().domain().beta, Fermion, n_max};

    auto const &old_mesh = g.mesh();
    int idx_min          = old_mesh.index_to_linear(iw_mesh.first_index());
    int idx_max          = old_mesh.index_to_linear(iw_mesh.last_index());
    auto data_view       = g.data()(range(idx_min, idx_max + 1), ellipsis());

    return typename G<imfreq, T>::const_view_type{iw_mesh, data_view};
  }

  template <typename T> void replace_by_tail(gf_view<imfreq, T> g, array_const_view<dcomplex, 1 + T::rank> tail, int n_min) {
    for (auto const &iw : g.mesh())
      if (iw.index() >= n_min or iw.index() < -n_min) g[iw] = tail_eval(tail, iw);
  }

  template <typename T> void replace_by_tail_in_fit_window(gf_view<imfreq, T> g, array_const_view<dcomplex, 1 + T::rank> tail) {
    int n_pts_in_fit_range = int(std::round(tail_fitter::default_tail_fraction() * g.mesh().size() / 2));
    int n_min              = g.mesh().last_index() - n_pts_in_fit_range;
    replace_by_tail(g, tail, n_min);
  }

  // FIXME For backward compatibility only
  // Fit_tail on a window
  template <template <typename, typename> typename G, typename T>
  auto fit_tail_on_window(G<imfreq, T> const &g, int n_min, int n_max, array_const_view<dcomplex, 3> known_moments, int n_tail_max,
                          int expansion_order) {
    if (n_max == -1) n_max = g.mesh().last_index();
    auto g_rview         = restricted_view(g, n_max);
    double tail_fraction = double(n_max - n_min) / n_max;
    g_rview.mesh().set_tail_fit_parameters(tail_fraction, n_tail_max, expansion_order);
    return fit_tail(g_rview, known_moments);
  }
} // namespace triqs::gfs
