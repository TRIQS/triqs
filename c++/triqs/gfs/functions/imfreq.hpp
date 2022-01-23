// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once
namespace triqs::gfs {

  using nda::conj; // not found on gcc 5

  // ---------------------------  A few specific functions ---------------------------------

  /**
   * This function takes a g(i omega_n) on half mesh (positive omega_n) and returns a gf on the whole mesh
   * using G(-i omega_n) = G(i omega_n)^* for real G(tau) functions.
   */
  template <typename T, typename Layout> gf<mesh::imfreq, T> make_gf_from_real_gf(gf_const_view<mesh::imfreq, T, Layout> g) {
    if (!g.mesh().positive_only()) TRIQS_RUNTIME_ERROR << "gf imfreq is not for omega_n >0, real_to_complex does not apply";
    auto const &dat = g.data();
    auto sh         = dat.shape();
    int is_boson    = (g.mesh().domain().statistic == Boson);
    long L          = sh[0];
    sh[0]           = 2 * sh[0] - is_boson;
    array<dcomplex, std::decay_t<decltype(dat)>::rank> new_data(sh);
    auto _ = nda::ellipsis{};
    if (is_boson) new_data(L - 1, _) = dat(0, _);
    int L1 = (is_boson ? L - 1 : L);
    for (int u = is_boson; u < L; ++u) {
      new_data(L1 + u, _)    = dat(u, _);
      new_data(L - 1 - u, _) = conj(dat(u, _));
    }
    return {mesh::imfreq{g.mesh().domain(), static_cast<size_t>(L)}, std::move(new_data), g.indices()};
  }

  /// Make a const view of the positive frequency part of the function
  template <typename G> view_or_type_t<std::decay_t<G>> positive_freq_view(G &&g) requires(is_gf_v<G>) {
    static_assert(std::is_same<typename std::decay_t<G>::mesh_t, mesh::imfreq>::value, "positive_freq_view only makes senses for imfreq gf");
    static_assert(std::decay_t<G>::is_view or std::is_lvalue_reference_v<G>, "Cannot construct a positive_freq_view from a temporary gf");
    if (g.mesh().positive_only()) return g;
    long L       = g.mesh().size();
    long L1      = (L + 1) / 2; // fermion : L is even. boson, L = 2p+1 --> p+1
    int is_boson = (g.mesh().domain().statistic == Boson);
    return {g.mesh().get_positive_freq(), g.data()(range(L1 - is_boson, L), nda::ellipsis()), g.indices()};
  }

  /**
   * Test if a Green function object fullfills the fundamental property mentioned below up to a fixed tolerance $\epsilon$
   * Depending on the mesh and target rank one of the following properties is checked
   * $G[i\omega] == \frac{1}{2} ( G[i\omega] + conj(G[-i\omega]) )$
   * $G[\tau] == \frac{1}{2} ( G[\tau] + conj(G[\tau]) )$
   * $G[i\omega](i,j) == \frac{1}{2} ( G[i\omega](i,j) + conj(G[-i\omega](j,i)) )$
   * $G[\tau](i,j) == \frac{1}{2} ( G[\tau](i,j) + conj(G[\tau](j,i)) )$
   * $G[i\omega](i,j,k,l) == \frac{1}{2} ( G[i\omega](i,j,k,l) + conj(G[-i\omega](k,l,i,j)) )$
   * $G[\tau](i,j,k,l) == \frac{1}{2} ( G[\tau](i,j,k,l) + conj(G[\tau](k,l,i,j)) )$
   *
   * @param g The Green function object to check the symmetry for
   * @param tolerance The tolerance $\epsilon$ for the check [default=1e-12]
   *
   * @tparam The Green function type
   *
   * @return true iif the fundamental property holds for all points of the mesh
   */
  template <typename G> bool is_gf_hermitian(G const &g, double tolerance = 1.e-12) requires(is_gf_v<G> or is_block_gf_v<G>) {
    if constexpr (is_gf_v<G>) {
      using target_t = typename G::target_t;
      using mesh_t   = typename std::decay_t<G>::mesh_t;
      static_assert(std::is_same_v<mesh_t, mesh::imfreq> or std::is_same_v<mesh_t, mesh::imtime>,
                    "is_gf_hermitian requires an imfreq or imtime Green function");
      static_assert(target_t::rank == 0 or target_t::rank == 2 or target_t::rank == 4,
                    "is_gf_hermitian requires a Green function with a target rank of 0, 2 or 4.");

      if constexpr (std::is_same_v<mesh_t, mesh::imfreq>) { // === gf<imfreq>
        if (g.mesh().positive_only()) return true;
        //for (auto const &w : g.mesh().get_positive_freq()) {
        for (auto const &w : g.mesh()) {
          if constexpr (target_t::rank == 0) { // ------ scalar_valued
            if (abs(conj(g[-w]) - g[w]) > tolerance) return false;
          } else if constexpr (target_t::rank == 2) { // matrix_valued FIXME transpose(g[-w])
            for (auto [i, j] : g.target_indices())
              if (abs(conj(g[-w](j, i)) - g[w](i, j)) > tolerance) return false;
          } else { // ---------------------------------- tensor_valued<4>
            for (auto [i, j, k, l] : g.target_indices())
              if (abs(conj(g[-w](k, l, i, j)) - g[w](i, j, k, l)) > tolerance) return false;
          }
        }
        return true;
      } else { // === gf<imtime>
        for (auto const &t : g.mesh()) {
          if constexpr (target_t::rank == 0) { // ------ scalar_valued
            if (abs(conj(g[t]) - g[t]) > tolerance) return false;
          } else if constexpr (target_t::rank == 2) { // matrix_valued
            for (auto [i, j] : g.target_indices())
              if (abs(conj(g[t](j, i)) - g[t](i, j)) > tolerance) return false;
          } else { // ---------------------------------- tensor_valued<4>
            for (auto [i, j, k, l] : g.target_indices())
              if (abs(conj(g[t](k, l, i, j)) - g[t](i, j, k, l)) > tolerance) return false;
          }
        }
        return true;
      }
    } else { // Block Green function
      return std::all_of(g.begin(), g.end(), [&](auto &g_bl) { return is_gf_hermitian<typename G::g_t>(g_bl, tolerance); });
    }
  }

  /**
   * Test if a Matsubara Green function object has an associated imaginary-time Green function
   * with an imaginary part below a fixed tolerance $\epsilon$
   * The following property is checked
   * $G[i\omega](...) == conj(G[-i\omega](...))$
   *
   * @param g The Green function object to check the property for
   * @param tolerance The tolerance $\epsilon$ for the check [default=1e-12]
   *
   * @tparam The Green function type
   *
   * @return true iif the property holds for all points of the mesh
   */
  template <typename G> bool is_gf_real_in_tau(G const &g, double tolerance = 1.e-12) {
    if constexpr (is_gf_v<G>) {
      using target_t = typename G::target_t;
      using mesh_t   = typename std::decay_t<G>::mesh_t;
      static_assert(std::is_same_v<mesh_t, mesh::imfreq>,
                    "is_gf_hermitian requires an imfreq Green function");

      if (g.mesh().positive_only()) return true;
      for (auto const &w : g.mesh()) {
        if constexpr (target_t::rank == 0) { // ---- scalar_valued
          if (abs(conj(g[-w]) - g[w]) > tolerance) return false;
	} else {
          if (max_element(abs(conj(g[-w]) - g[w])) > tolerance) return false;
	}
      }
      return true;
    } else { // Block Green function
      return std::all_of(g.begin(), g.end(), [&](auto &g_bl) { return is_gf_real_in_tau<typename G::g_t>(g_bl, tolerance); });
    }
  }

  /**
   * Symmetrize a Green function object to fullfill fundamental Green function properties.
   * Depending on the mesh and target rank one of the following transformations is performed
   * $G[i\omega] \rightarrow \frac{1}{2} ( G[i\omega] + conj(G[-i\omega]) )$
   * $G[\tau] \rightarrow \frac{1}{2} ( G[\tau] + conj(G[\tau]) )$
   * $G[i\omega](i,j) \rightarrow \frac{1}{2} ( G[i\omega](i,j) + conj(G[-i\omega](j,i)) )$
   * $G[\tau](i,j) \rightarrow \frac{1}{2} ( G[\tau](i,j) + conj(G[\tau](j,i)) )$
   * $G[i\omega](i,j,k,l) \rightarrow \frac{1}{2} ( G[i\omega](i,j,k,l) + conj(G[-i\omega](k,l,i,j)) )$
   * $G[\tau](i,j,k,l) \rightarrow \frac{1}{2} ( G[\tau](i,j,k,l) + conj(G[\tau](k,l,i,j)) )$
   *
   * @param g The Green function object to symmetrize
   *
   * @tparam The Green function type
   *
   * @return The symmetrized Green function object
   */
  template <typename G> typename G::regular_type make_hermitian(G const &g) requires(is_gf_v<G> or is_block_gf_v<G>) {
    if constexpr (is_gf_v<G>) {
      using target_t = typename G::target_t;
      using mesh_t   = typename std::decay_t<G>::mesh_t;
      static_assert(std::is_same_v<mesh_t, mesh::imfreq> or std::is_same_v<mesh_t, mesh::imtime>,
                    "make_hermitian requires an imfreq or imtime Green function");
      static_assert(target_t::rank == 0 or target_t::rank == 2 or target_t::rank == 4,
                    "make_hermitian requires a Green function with a target rank of 0, 2 or 4.");

      if constexpr (std::is_same_v<mesh_t, mesh::imfreq>) { // === gf<imfreq>
        if (g.mesh().positive_only()) return typename G::regular_type{g};
        auto g_sym = typename G::regular_type{g};
        for (auto const &w : g.mesh()) {
          if constexpr (target_t::rank == 0) // ---- scalar_valued
            g_sym[w] = 0.5 * (g[w] + conj(g[-w]));
          else if constexpr (target_t::rank == 2) // matrix_valued FIXME transpose(g[-w])
            for (auto [i, j] : g.target_indices()) g_sym[w](i, j) = 0.5 * (g[w](i, j) + conj(g[-w](j, i)));
          else // ---------------------------------- tensor_valued<4>
            for (auto [i, j, k, l] : g.target_indices()) g_sym[w](i, j, k, l) = 0.5 * (g[w](i, j, k, l) + conj(g[-w](k, l, i, j)));
        }
        return g_sym;

      } else { // === gf<imtime>
        auto g_sym = typename G::regular_type{g};
        for (auto const &t : g.mesh()) {
          if constexpr (target_t::rank == 0) // ---- scalar_valued
            g_sym[t] = 0.5 * (g[t] + conj(g[t]));
          else if constexpr (target_t::rank == 2) // matrix_valued
            for (auto [i, j] : g.target_indices()) g_sym[t](i, j) = 0.5 * (g[t](i, j) + conj(g[t](j, i)));
          else // ---------------------------------- tensor_valued<4>
            for (auto [i, j, k, l] : g.target_indices()) g_sym[t](i, j, k, l) = 0.5 * (g[t](i, j, k, l) + conj(g[t](k, l, i, j)));
        }
        return g_sym;
      }
    } else if (is_block_gf_v<G>) { // Block Green function
      return map_block_gf(make_hermitian<typename G::g_t>, g);
    }
  }

  /**
   * Symmetrize a Matsubara Green function object such that the associated imaginary-time
   * propagator is fully real-valued. The following transformation is performed
   * $G[i\omega](...) \rightarrow \frac{1}{2} ( G[i\omega](...) + conj(G[-i\omega](...)) )$
   *
   * @param g The Green function object to symmetrize
   *
   * @tparam The Green function type
   *
   * @return The symmetrized Green function object
   */
  template <typename G> typename G::regular_type make_real_in_tau(G const &g) requires(is_gf_v<G> or is_block_gf_v<G>) {
    if constexpr (is_gf_v<G>) {
      using target_t = typename G::target_t;
      using mesh_t   = typename std::decay_t<G>::mesh_t;
      static_assert(std::is_same_v<mesh_t, mesh::imfreq>,
                    "make_real_in_tau requires an imfreq Green function");

      if (g.mesh().positive_only()) return typename G::regular_type{g};
      auto g_sym = typename G::regular_type{g};
      for (auto const &w : g.mesh()) {
        g_sym[w] = 0.5 * (g[w] + conj(g[-w]));
      }
      return g_sym;
    } else if (is_block_gf_v<G>) { // Block Green function
      return map_block_gf(make_real_in_tau<typename G::g_t>, g);
    }
  }

  // ------------------------------------------------------------------------------------------------------

  template <template <typename, typename, typename ...> typename G, typename T> auto restricted_view(G<mesh::imfreq, T> const &g, int n_max) {
    auto iw_mesh = mesh::imfreq{g.mesh().domain().beta, Fermion, n_max};

    auto const &old_mesh = g.mesh();
    int idx_min          = old_mesh.index_to_linear(iw_mesh.first_index());
    int idx_max          = old_mesh.index_to_linear(iw_mesh.last_index());
    auto data_view       = g.data()(range(idx_min, idx_max + 1), ellipsis());

    return typename G<mesh::imfreq, T>::const_view_type{iw_mesh, data_view, g.indices()};
  }

  template <typename T> void replace_by_tail(gf_view<mesh::imfreq, T> g, array_const_view<dcomplex, 1 + T::rank> tail, int n_min) {
    for (auto const &iw : g.mesh())
      if (iw.index() >= n_min or iw.index() < -n_min) g[iw] = tail_eval(tail, iw);
  }

  template <typename T> void replace_by_tail_in_fit_window(gf_view<mesh::imfreq, T> g, array_const_view<dcomplex, 1 + T::rank> tail) {
    int n_pts_in_fit_range = int(std::round(g.mesh().get_tail_fitter().get_tail_fraction() * g.mesh().size() / 2));
    int n_min              = g.mesh().last_index() - n_pts_in_fit_range;
    replace_by_tail(g, tail, n_min);
  }

  // FIXME For backward compatibility only
  // Fit_tail on a window
  template <template <typename, typename, typename ...> typename G, typename T>
  auto fit_tail_on_window(G<mesh::imfreq, T> const &g, int n_min, int n_max, array_const_view<dcomplex, 3> known_moments, int n_tail_max,
                          int expansion_order) {
    if (n_max == -1) n_max = g.mesh().last_index();
    auto g_rview         = restricted_view(g, n_max);
    double tail_fraction = double(n_max - n_min) / n_max;
    g_rview.mesh().set_tail_fit_parameters(tail_fraction, n_tail_max, expansion_order);
    return fit_tail(g_rview, known_moments);
  }

  // Fit_tail on a window with the constraint of hermitian moment matrices
  template <template <typename, typename ...> typename G, typename T>
  auto fit_hermitian_tail_on_window(G<mesh::imfreq, T> const &g, int n_min, int n_max, array_const_view<dcomplex, 3> known_moments, int n_tail_max,
                                    int expansion_order) {
    if (n_max == -1) n_max = g.mesh().last_index();
    auto g_rview         = restricted_view(g, n_max);
    double tail_fraction = double(n_max - n_min) / n_max;
    g_rview.mesh().set_tail_fit_parameters(tail_fraction, n_tail_max, expansion_order);
    return fit_hermitian_tail(g_rview, known_moments);
  }
} // namespace triqs::gfs
