// Copyright (c) 2023 Simons Foundation
// Copyright (c) 2023 Hugo U.R. Strand
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
#include "nda/declarations.hpp"
#include "triqs/gfs/gf/gf.hpp"
#include "triqs/gfs/block/block_gf.hpp"
#include "../../mesh/imtime.hpp"
#include "../../mesh/imfreq.hpp"
#include "../../mesh/dlr_imtime.hpp"
#include "../../mesh/dlr_imfreq.hpp"
#include "../..//mesh/dlr.hpp"
namespace triqs::gfs {

  using mesh::dlr;
  using mesh::dlr_imfreq;
  using mesh::dlr_imtime;

  //-------------------------------------------------------
  // Transformation of DLR Green's functions
  // ------------------------------------------------------

  template <int N, int... Ns, typename F, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto apply_to_mesh(F const &f, G const &g) {
    using M = typename G::mesh_t;
    if constexpr (sizeof...(Ns) > 0) {
      return apply_to_mesh<Ns...>(f, apply_to_mesh<N>(f, g));
    } else if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return apply_to_mesh<N>(f, gbl); }, g);
    } else {
      static_assert(mesh::is_product<M>, "requires product mesh");
      auto gfl      = f(flatten_gf_2d<N>(g));
      auto out_mesh = mesh::prod{triqs::tuple::replace<N>(g.mesh(), gfl.mesh())};
      auto g_out    = gf{out_mesh, g.target_shape()};
      unflatten_gf_2d<N>(g_out, gfl);
      return g_out;
    }
  }

  /**
   * @brief Transform a DLR imaginary-time or DLR Matsubara Green's function to its DLR-coefficient representation.
   *
   * @details The input must live on an imaginary-time or frequency DLR mesh. The output lives on the corresponding 
   * DLR (coefficient) mesh. 
   * 
   * For block Green's functions the transform is applied block-wise. 
   *
   * @tparam N Index of the mesh component to transform (default \f$ 0 \f$).
   * @tparam Ns Additional mesh component indices for product meshes.
   * @tparam G The type of the input Green's function.
   * @param g The DLR imaginary-time or DLR Matsubara Green's function.
   * @return The same Green's function expressed in DLR coefficients.
   */
  template <int N = 0, int... Ns, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto make_gf_dlr(G const &g) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return make_gf_dlr<N, Ns...>(gbl); }, g);
    } else if constexpr (mesh::is_product<M>) {
      return apply_to_mesh<N, Ns...>([&](auto const &gfl) { return make_gf_dlr(gfl); }, g);
    } else {
      static_assert(N == 0, "N must be 0 for non-product meshes");
      static_assert(nda::AnyOf<M, dlr_imtime, dlr_imfreq>, "Mesh must be dlr_imtime or dlr_imfreq");
      auto result = gf{dlr{g.mesh()}, g.target_shape()};
      if constexpr (std::is_same_v<M, dlr_imtime>)
        result.data() = result.mesh().dlr_it().vals2coefs(g.data());
      else { // dlr_imfreq
        auto beta_inv = 1. / result.mesh().beta();
        result.data() = beta_inv * result.mesh().dlr_if().vals2coefs(g.data());
      }
      return result;
    }
  }

  /**
   * @brief Fit an imaginary-time Green's function with a Discrete Lehmann Representation.
   *
   * @details Builds a DLR coefficient Green's function by least-squares fitting the input Green's function (on a 
   * uniform imaginary-time mesh) on a DLR grid specified by the spectral cutoff \f$ \omega_{\max} \f$ and tolerance
   * \f$ \epsilon \f$.
   *
   * @tparam N Index of the mesh component to fit (default \f$ 0 \f$).
   * @tparam Ns Additional mesh component indices for product meshes.
   * @tparam G The type of the input Green's function.
   * @param g The imaginary-time Green's function to fit.
   * @param w_max Maximum real frequency captured by the DLR basis.
   * @param eps Target accuracy of the DLR representation.
   * @param symmetrize If true, the DLR grid is symmetric about \f$ \omega = 0 \f$.
   * @return A Green's function on the DLR coefficient mesh.
   */
  template <int N = 0, int... Ns, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto fit_gf_dlr(G const &g, double w_max, double eps, bool symmetrize = true) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return fit_gf_dlr<N, Ns...>(gbl, w_max, eps, symmetrize); }, g);
    } else if constexpr (mesh::is_product<M>) {
      return apply_to_mesh<N, Ns...>([&](auto const &gfl) { return fit_gf_dlr(gfl, w_max, eps, symmetrize); }, g);
    } else {
      static_assert(N == 0, "N must be 0 for non-product meshes");
      static_assert(std::is_same_v<M, mesh::imtime>, "Input mesh must be imtime");
      auto tvals    = nda::array_adapter(std::array{g.mesh().size()}, [&](auto i) { return g.mesh()[i].value() / g.mesh().beta(); });
      auto mesh     = dlr{g.mesh().beta(), g.mesh().statistic(), w_max, eps, symmetrize};
      auto result   = gf{mesh, g.target_shape()};
      result.data() = result.mesh().dlr_it().fitvals2coefs(make_regular(tvals), g.data());
      return result;
    }
  }

  /**
   * @brief Build a DLR imaginary-time Green's function from a DLR-coefficient or DLR-Matsubara input.
   *
   * @details The output lives on the imaginary-time DLR mesh associated with the input's DLR grid. Block and product 
   * meshes are handled recursively.
   *
   * @tparam N Index of the mesh component to transform (default \f$ 0 \f$).
   * @tparam Ns Additional mesh component indices for product meshes.
   * @tparam G The type of the input Green's function.
   * @param g A Green's function on a Matsubara or coefficient DLR mesh. 
   * @return The Green's function evaluated on the imaginary-time DLR mesh.
   */
  template <int N = 0, int... Ns, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto make_gf_dlr_imtime(G const &g) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return make_gf_dlr_imtime<N, Ns...>(gbl); }, g);
    } else if constexpr (mesh::is_product<M>) {
      return apply_to_mesh<N, Ns...>([&](auto const &gfl) { return make_gf_dlr_imtime(gfl); }, g);
    } else if constexpr (std::is_same_v<M, dlr_imfreq>) {
      return make_gf_dlr_imtime(make_gf_dlr(g));
    } else {
      static_assert(N == 0, "N must be 0 for non-product meshes");
      static_assert(std::is_same_v<M, mesh::dlr>, "Input mesh must be dlr");
      auto result   = gf{dlr_imtime{g.mesh()}, g.target_shape()};
      result.data() = g.mesh().dlr_it().coefs2vals(g.data());
      return result;
    }
  }

  /**
   * @brief Build a DLR Matsubara Green's function from a DLR-coefficient or DLR-imaginary-time input.
   *
   * @details The output lives on the Matsubara DLR mesh associated with the input's DLR grid. Block and product meshes 
   * are handled recursively.
   *
   * @tparam N Index of the mesh component to transform (default \f$ 0 \f$).
   * @tparam Ns Additional mesh component indices for product meshes.
   * @tparam G The type of the input Green's function.
   * @param g A Green's function on an imaginary-time or coefficient DLR mesh.
   * @return The Green's function evaluated on the Matsubara DLR mesh.
   */
  template <int N = 0, int... Ns, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto make_gf_dlr_imfreq(G const &g) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return make_gf_dlr_imfreq<N, Ns...>(gbl); }, g);
    } else if constexpr (mesh::is_product<M>) {
      return apply_to_mesh<N, Ns...>([&](auto const &gfl) { return make_gf_dlr_imfreq(gfl); }, g);
    } else if constexpr (std::is_same_v<M, dlr_imtime>) {
      return make_gf_dlr_imfreq(make_gf_dlr(g));
    } else {
      static_assert(N == 0, "N must be 0 for non-product meshes");
      static_assert(std::is_same_v<M, mesh::dlr>, "Input mesh must be dlr");
      auto result   = gf{dlr_imfreq{g.mesh()}, g.target_shape()};
      auto beta     = result.mesh().beta();
      result.data() = beta * g.mesh().dlr_if().coefs2vals(g.data());
      return result;
    }
  }

  namespace detail {
    template <typename G>
      requires(MemoryGf<G> or is_block_gf_v<G>)
    mesh::imfreq const &imfreq_mesh_of(G const &g) {
      if constexpr (is_block_gf_v<G>)
        return g[0].mesh();
      else
        return g.mesh();
    }

    // Factored out so make_gf_dlr_imfreq and find_w_max build the dlr_imfreq mesh
    // once (the cppdlr SVD is not cheap) and reuse it across blocks / search iterations.
    template <typename G>
      requires(MemoryGf<G> or is_block_gf_v<G>)
    auto sample_on_dlr_imfreq(G const &g, dlr_imfreq const &dlr_mesh) {
      if constexpr (is_block_gf_v<G>) {
        return map_block_gf([&](auto const &gbl) { return sample_on_dlr_imfreq(gbl, dlr_mesh); }, g);
      } else {
        static_assert(std::is_same_v<typename G::mesh_t, mesh::imfreq>, "Input mesh must be imfreq");
        auto const &iw_mesh = g.mesh();
        auto [iw_lo, iw_hi] = dlr_mesh.min_max_frequencies();
        if (not iw_mesh.is_index_valid(iw_lo.n) or not iw_mesh.is_index_valid(iw_hi.n))
          TRIQS_RUNTIME_ERROR << "make_gf_dlr_imfreq: input imfreq mesh does not cover the requested DLR frequency range";
        auto result = gf{dlr_mesh, g.target_shape()};
        for (auto const &mp : dlr_mesh) result[mp] = g[iw_mesh(mp.index())];
        return result;
      }
    }
  } // namespace detail

  /**
   * @brief Sample an imaginary frequency Green's function on the DLR Matsubara frequency nodes
   * defined by (w_max, eps, symmetrize).
   *
   * @details Constructs a fresh triqs::mesh::dlr_imfreq mesh from \p w_max, \p eps and \p symmetrize
   * (using the same beta and statistic as \p g) and returns a Gf on that mesh whose values are read
   * off \p g at the corresponding Matsubara indices.
   *
   * For a block_gf, the DLR mesh is built once and reused across blocks (the underlying cppdlr SVD
   * is not cheap). For a product mesh, the operation is applied to the imfreq axis selected by the
   * template parameter pack <N, Ns...>.
   *
   * It throws an exception if the input imfreq mesh of \p g does not cover the Matsubara frequency
   * range of the constructed DLR mesh.
   *
   * @tparam N     Index of the imfreq axis to transform when \p g has a product mesh. Must be 0 for
   *               non-product meshes.
   * @tparam Ns    Additional axis indices for nested product-mesh transformations.
   * @tparam G     A MemoryGf or block_gf type with an imfreq mesh on the selected axis.
   * @param  g          Input Green's function on an imfreq mesh (or product mesh containing one).
   * @param  w_max      DLR energy cutoff used to build the dlr_imfreq mesh.
   * @param  eps        DLR accuracy used to build the dlr_imfreq mesh.
   * @param  symmetrize If true, build a particle-hole symmetric DLR mesh.
   * @return A Gf on a freshly built dlr_imfreq mesh, sampled from \p g.
   */
  template <int N = 0, int... Ns, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto make_gf_dlr_imfreq(G const &g, double w_max, double eps, bool symmetrize = true) {
    using M = typename G::mesh_t;
    if constexpr (mesh::is_product<M>) {
      return apply_to_mesh<N, Ns...>([&](auto const &gfl) { return make_gf_dlr_imfreq(gfl, w_max, eps, symmetrize); }, g);
    } else {
      static_assert(N == 0, "N must be 0 for non-product meshes");
      auto const &m0 = detail::imfreq_mesh_of(g);
      auto dlr_mesh  = dlr_imfreq{m0.beta(), m0.statistic(), w_max, eps, symmetrize};
      return detail::sample_on_dlr_imfreq(g, dlr_mesh);
    }
  }

  /**
   * @brief Find a DLR energy cutoff \f$ \omega_{\mathrm{max}} \f$ that reproduces an imfreq Green's
   * function within tolerance \p eps after a DLR round-trip.
   *
   * @details Starting from \p w_max_init, the cutoff is grown by a factor of 1.5 per iteration. For
   * each candidate \f$ \omega_{\mathrm{max}} \f$ the function
   * - builds a triqs::mesh::dlr_imfreq mesh from (\f$ \omega_{\mathrm{max}} \f$, \p eps, \p symmetrize),
   * - samples \p g on that DLR mesh,
   * - reconstructs the Gf on the original imfreq mesh of \p g via the DLR coefficients, and
   * - measures the element-wise round-trip error
   *   \f[ e = \max_{i\omega_n, \mathrm{indices}} \left| g(i\omega_n) - g_{\mathrm{rec}}(i\omega_n) \right| \;. \f]
   * The smallest \f$ \omega_{\mathrm{max}} \le \f$ \p w_max_max with \f$ e < \f$ \p eps is returned.
   *
   * Candidate cutoffs whose DLR frequency range exceeds the range of the input imfreq mesh of \p g
   * are skipped (they would produce an invalid DLR mesh).
   *
   * For a block_gf, the error reported per iteration is the worst-case error across all blocks.
   *
   * It throws an exception if \p w_max_init exceeds \p w_max_max, or if no candidate cutoff
   * \f$ \le \f$ \p w_max_max achieves \f$ e < \f$ \p eps.
   *
   * @tparam G An imfreq MemoryGf or block_gf type.
   * @param  g           Input Green's function on an imfreq mesh.
   * @param  eps         Target DLR accuracy and round-trip error tolerance.
   * @param  symmetrize  If true, use particle-hole symmetric DLR meshes.
   * @param  w_max_init  Initial value of the DLR cutoff to try (must be \f$ \le \f$ \p w_max_max).
   * @param  w_max_max   Maximum DLR cutoff to try before giving up.
   * @return The smallest \f$ \omega_{\mathrm{max}} \le \f$ \p w_max_max for which the round-trip
   *         error is below \p eps.
   */
  template <int = 0, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  double find_w_max(G const &g, double eps = 1e-10, bool symmetrize = true, double w_max_init = 1.0, double w_max_max = 200.0) {
    if (w_max_init > w_max_max)
      TRIQS_RUNTIME_ERROR << "find_w_max: w_max_init (" << w_max_init << ") > w_max_max (" << w_max_max << ")";

    auto const &m0 = detail::imfreq_mesh_of(g);

    for (double w_max = w_max_init; w_max <= w_max_max; w_max *= 1.5) {
      auto dlr_mesh       = dlr_imfreq{m0.beta(), m0.statistic(), w_max, eps, symmetrize};
      auto [iw_lo, iw_hi] = dlr_mesh.min_max_frequencies();
      if (not m0.is_index_valid(iw_lo.n) or not m0.is_index_valid(iw_hi.n)) continue;

      auto g_dlr_iw  = detail::sample_on_dlr_imfreq(g, dlr_mesh);
      auto g_rec     = make_gf_imfreq(make_gf_dlr(g_dlr_iw), m0.n_iw());
      auto data_err  = [](auto const &x, auto const &y) { return double(max_element(abs(x.data() - y.data()))); };
      double max_err = 0.0;
      if constexpr (is_block_gf_v<G>) {
        for (long b = 0; b < long(g.size()); ++b) max_err = std::max(max_err, data_err(g[b], g_rec[b]));
      } else {
        max_err = data_err(g, g_rec);
      }
      if (max_err < eps) return w_max;
    }
    TRIQS_RUNTIME_ERROR << "find_w_max: no w_max <= " << w_max_max << " yields round-trip error < eps = " << eps;
  }

  /**
   * @brief Build a uniform imaginary-time Green's function from any DLR representation.
   *
   * @details Evaluates the DLR Green's function on a regular imaginary-time mesh of size \f$ n_{\tau} \f$. Inputs on 
   * imaginary-time or Matsubara DLR meshes are first converted to DLR coefficients internally.
   * 
   * @tparam N Index of the mesh component to transform (default \f$ 0 \f$).
   * @tparam Ns Additional mesh component indices for product meshes.
   * @tparam G The type of the input Green's function.
   * @param g A Green's function on any DLR mesh.
   * @param n_tau Number of points of the output imaginary-time mesh.
   * @return The Green's function evaluated on a uniform imaginary-time mesh.
   */
  template <int N = 0, int... Ns, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto make_gf_imtime(G const &g, long n_tau) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return make_gf_imtime<N, Ns...>(gbl, n_tau); }, g);
    } else if constexpr (mesh::is_product<M>) {
      return apply_to_mesh<N, Ns...>([&](auto const &gfl) { return make_gf_imtime(gfl, n_tau); }, g);
    } else if constexpr (nda::AnyOf<M, dlr_imtime, dlr_imfreq>) {
      return make_gf_imtime(make_gf_dlr(g), n_tau);
    } else { // M == dlr
      static_assert(N == 0, "N must be 0 for non-product meshes");
      static_assert(std::is_same_v<M, dlr>, "Input mesh must be dlr");
      auto result = gf{mesh::imtime{g.mesh().beta(), g.mesh().statistic(), n_tau}, g.target_shape()};
      for (auto tau : result.mesh()) result[tau] = g(tau.value());
      return result;
    }
  }

  /**
   * @brief Build a uniform Matsubara Green's function from any DLR representation.
   *
   * @details Evaluates the DLR Green's function on a regular Matsubara mesh with \f$ n_{i\omega} \f$ positive Matsubara 
   * frequencies. Input on imaginary-time or Matsubara DLR meshes are first converted to DLR coefficients internally.
   *
   * @tparam N Index of the mesh component to transform (default \f$ 0 \f$).
   * @tparam Ns Additional mesh component indices for product meshes.
   * @tparam G The type of the input Green's function.
   * @param g A Green's function on any DLR mesh.
   * @param n_iw Number of positive Matsubara frequencies in the output mesh.
   * @return The Green's function evaluated on a uniform Matsubara mesh.
   */
  template <int N = 0, int... Ns, typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto make_gf_imfreq(G const &g, long n_iw) {
    using M = typename G::mesh_t;
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return make_gf_imfreq<N, Ns...>(gbl, n_iw); }, g);
    } else if constexpr (mesh::is_product<M>) {
      return apply_to_mesh<N, Ns...>([&](auto const &gfl) { return make_gf_imfreq(gfl, n_iw); }, g);
    } else if constexpr (nda::AnyOf<M, dlr_imtime, dlr_imfreq>) {
      return make_gf_imfreq(make_gf_dlr(g), n_iw);
    } else { // M == dlr
      static_assert(N == 0, "N must be 0 for non-product meshes");
      static_assert(std::is_same_v<M, dlr>, "Input mesh must be dlr");
      auto result = gf{mesh::imfreq{g.mesh().beta(), g.mesh().statistic(), n_iw}, g.target_shape()};
      for (auto w : result.mesh()) result[w] = g(w.value());
      return result;
    }
  }

  /**
   * @brief Calculate the \f$ L^2 \f$ norm of a DLR Green's function.
   *
   * @tparam G The type of the input Green's function.
   * @param g A Green's function on any DLR mesh.
   * @return The \f$ L^2 \f$ norm of the input Green's function, either as a scalar (if target rank is 0) or as an 
   * array of norms for each element in the target domain (if target rank is greater than 0). 
   */
  template <typename G>
    requires(MemoryGf<G> or is_block_gf_v<G>)
  auto tau_L2_norm(G const &g) {
    using M = typename G::mesh_t;
    static_assert(nda::AnyOf<M, dlr, dlr_imfreq, dlr_imtime>, "Input mesh must be one of dlr, dlr_imfreq, dlr_imtime");
    if constexpr (is_block_gf_v<G>) {
      return map_block_gf([&](auto const &gbl) { return tau_L2_norm(gbl); }, g);
    } else if constexpr (nda::AnyOf<M, dlr_imtime, dlr_imfreq>) {
      return tau_L2_norm(make_gf_dlr(g));
    } else { // M == dlr
      if constexpr (G::target_rank == 0) {
        // use abs here to avoid sqrt of negative numbers, which can happen due to numerical errors
        return std::sqrt(std::abs(g.mesh().dlr_it().innerprod(g.data(), g.data())));
      } else {
        auto result = nda::array<double, G::target_rank>(g.target_shape());
        nda::for_each(g.target_shape(), [&](auto... is) {
          auto dat      = g.data()(nda::range::all, is...);
          result(is...) = std::sqrt(std::abs(g.mesh().dlr_it().innerprod(dat, dat)));
        });

        return result;
      }
    }
  }

} // namespace triqs::gfs
