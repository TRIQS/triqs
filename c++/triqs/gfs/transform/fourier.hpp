// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Philipp Dumitrescu, Michel Ferrero, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides the Fourier transform factories and the lazy `fourier(...)` assignment for Green's functions.
 */

#pragma once

#include "../gf/flatten.hpp"
#include "../gf/gf.hpp"
#include "../gf/gf_const_view.hpp"
#include "../gf/gf_view.hpp"
#include "../gf/targets.hpp"
#include "../block/block_gf.hpp"
#include "../block/map.hpp"
#include "../functions/dlr.hpp"

#include "../../mesh/adjoint.hpp"
#include "../../mesh/brzone.hpp"
#include "../../mesh/cyclat.hpp"
#include "../../mesh/dlr_imfreq.hpp"
#include "../../mesh/dlr_imtime.hpp"
#include "../../mesh/imfreq.hpp"
#include "../../mesh/imtime.hpp"
#include "../../mesh/prod.hpp"
#include "../../mesh/refreq.hpp"
#include "../../mesh/retime.hpp"
#include "../../utility/exceptions.hpp"
#include "../../utility/tuple_tools.hpp"

#include <itertools/itertools.hpp>

#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace triqs::gfs {

  // Names elevated from nda for the declarations below.
  using nda::array;
  using nda::array_const_view;

  // Trait yielding the conjugate (Fourier-adjoint) mesh of V, used for static checks below.
  template <typename V> using _mesh_fourier_image = decltype(make_adjoint_mesh(V()));

  // Vector-valued (tensor_valued<1>) Green's function aliases used by the core FFTW implementation.
  template <typename V> using gf_vec_t   = gf<V, tensor_valued<1>>;
  template <typename V> using gf_vec_vt  = gf_view<V, tensor_valued<1>>;
  template <typename V> using gf_vec_cvt = gf_const_view<V, tensor_valued<1>>;

  // matsubara
  gf_vec_t<mesh::imfreq> _fourier_impl(mesh::imfreq const &iw_mesh, gf_vec_cvt<mesh::imtime> gt, array_const_view<dcomplex, 2> known_moments = {});
  gf_vec_t<mesh::imtime> _fourier_impl(mesh::imtime const &tau_mesh, gf_vec_cvt<mesh::imfreq> gw, array_const_view<dcomplex, 2> known_moments = {});

  // dlr
  inline gf_vec_t<mesh::dlr_imfreq> _fourier_impl(mesh::dlr_imfreq const &, gf_vec_cvt<mesh::dlr_imtime> gt) { return make_gf_dlr_imfreq(gt); }
  inline gf_vec_t<mesh::dlr_imtime> _fourier_impl(mesh::dlr_imtime const &, gf_vec_cvt<mesh::dlr_imfreq> gw) { return make_gf_dlr_imtime(gw); }

  // real
  gf_vec_t<mesh::refreq> _fourier_impl(mesh::refreq const &w_mesh, gf_vec_cvt<mesh::retime> gt, array_const_view<dcomplex, 2> known_moments = {});
  gf_vec_t<mesh::retime> _fourier_impl(mesh::retime const &t_mesh, gf_vec_cvt<mesh::refreq> gw, array_const_view<dcomplex, 2> known_moments = {});

  // lattice
  gf_vec_t<mesh::cyclat> _fourier_impl(mesh::cyclat const &r_mesh, gf_vec_cvt<mesh::brzone> gk);
  gf_vec_t<mesh::brzone> _fourier_impl(mesh::brzone const &k_mesh, gf_vec_cvt<mesh::cyclat> gr);

  // Regroup the gf data, call the vector-valued core implementation, then unflatten the result (gin: input, gout: output, opt_args: e.g. moments).
  template <int N, typename M1, typename M2, typename T1, typename T2, typename... OptArgs>
  void _fourier(gf_const_view<M1, T1> gin, gf_view<M2, T2> gout, OptArgs const &...opt_args) {

    static_assert(std::is_same_v<typename T1::complex_t, T2>, "Incompatible target types for fourier transform");

    // pb std::get<0> would not work on a non composite mesh. We use a little lambda to deduce ref and type
    auto const &out_mesh = [&gout]() -> auto const & { // NB must return a reference
      using m_t = std::decay_t<decltype(gout.mesh())>;
      if constexpr (mesh::is_product<m_t>)
        return std::get<N>(gout.mesh());
      else
        return gout.mesh();
    }();

    // FIXME : Code failed with nda optimisation relaxing assumption on iterator order.
    // between nda::for_each and the flatten which were not inverse of each other any more
    // TODO : put back the optimisation in nda (MACRO ??)
    // some tests will fail and check in a test that flatten_2d and the inverse op are indeed inverse...
    auto gin_fl = [&gin]() {
      using gfl_t = typename decltype(flatten_gf_2d<N>(gin))::complex_t;
      return gfl_t{flatten_gf_2d<N>(gin)};
    }();

    auto gout_fl = _fourier_impl(out_mesh, gin_fl, flatten_2d(opt_args)...);
    unflatten_gf_2d<N>(gout, gout_fl);
  }

  /**
   * @ingroup triqs-gfs-fourier
   * @brief Build a new Green's function on the conjugate mesh by Fourier transforming an input Green's function.
   *
   * @details Supports the standard pairs of conjugate meshes:
   *
   * - imaginary time \f$ \tau \in [0, \beta] \f$ \f$ \leftrightarrow \f$ Matsubara frequencies \f$ i\omega_n \f$,
   * - real time \f$ t \f$ \f$ \leftrightarrow \f$ real frequencies \f$ \omega \f$,
   * - imaginary-time DLR \f$ \leftrightarrow \f$ Matsubara-frequency DLR,
   * - cyclic lattice \f$ \leftrightarrow \f$ Brillouin zone.
   *
   * The output mesh may be supplied explicitly; if omitted, the conjugate mesh of the input is used. For
   * Matsubara/real-frequency pairs an optional integer (\f$ n_{\tau} \f$ or \f$ n_{i\omega} \f$) controls the size of
   * the generated mesh. Known high-frequency moments may be passed to improve accuracy near the tail.
   *
   * For block Green's functions, the transform is applied block by block.
   *
   * @tparam N Index of the mesh component to transform (default \f$ 0 \f$).
   * @tparam M1 Mesh type of the input Green's function.
   * @tparam M2 Mesh type of the output Green's function.
   * @tparam T Target type of the input Green's function.
   * @param gin The input Green's function.
   * @param mesh The output mesh on the conjugate variable.
   * @param opt_args Optional arguments (e.g. known high-frequency moments).
   * @return A new Green's function on the conjugate mesh.
   */
  template <int N = 0, typename M1, typename M2, typename T, typename... OptArgs>
  auto make_gf_from_fourier(gf_const_view<M1, T> gin, M2 const &mesh, OptArgs const &...opt_args) {
    static_assert(N >= 0 && N < n_variables<M1>, "Mesh index exceeds Gf Mesh Rank");
    static_assert(n_variables<M2> == 1, "Cannot fourier transform on cartesian product mesh");

    if constexpr (n_variables<M1> == 1) { // === single mesh
      static_assert(n_variables<M2> == 1, "Incompatible mesh ranks");
      static_assert(N == 0, "Fourier transforming gf with mesh of rank 1 but fourier index N > 1");
      static_assert(std::is_same_v<M2, _mesh_fourier_image<M1>>, "There is no Fourier transform between these two meshes");
      auto gout = gf<M2, typename T::complex_t>{mesh, gin.target_shape()};
      _fourier<N>(gin, gout(), opt_args...);
      return gout;
    } else { // === prod mesh
      static_assert(std::is_same_v<M2, _mesh_fourier_image<std::tuple_element_t<N, M1>>>, "There is no Fourier transform between these two meshes");
      auto mesh_tpl = triqs::tuple::replace<N>(gin.mesh().components(), mesh);
      auto out_mesh = mesh::prod{mesh_tpl};
      using mesh_t  = typename std::decay_t<decltype(out_mesh)>;
      auto gout     = gf<mesh_t, typename T::complex_t>{out_mesh, gin.target_shape()};
      _fourier<N>(gin, gout(), opt_args...);
      return gout;
    }
  }

  /// Fourier transform a Brillouin-zone Green's function to the conjugate cyclic-lattice mesh.
  template <int N = 0, typename T> auto make_gf_from_fourier(gf_const_view<mesh::brzone, T> gin) {
    return make_gf_from_fourier(gin, make_adjoint_mesh(gin.mesh()));
  }

  /// Fourier transform a cyclic-lattice Green's function to the conjugate Brillouin-zone mesh.
  template <int N = 0, typename T> auto make_gf_from_fourier(gf_const_view<mesh::cyclat, T> gin) {
    return make_gf_from_fourier(gin, make_adjoint_mesh(gin.mesh()));
  }

  /// Fourier transform a Matsubara-frequency Green's function to imaginary time (`n_tau` time points, -1 for the default).
  template <int N = 0, typename T> gf<mesh::imtime, T> make_gf_from_fourier(gf_const_view<mesh::imfreq, T> gin, int n_tau = -1) {
    return make_gf_from_fourier(gin, make_adjoint_mesh(gin.mesh(), n_tau));
  }

  /// Fourier transform an imaginary-time Green's function to Matsubara frequencies (`n_iw` positive frequencies, -1 for the default).
  template <int N = 0, typename T> gf<mesh::imfreq, typename T::complex_t> make_gf_from_fourier(gf_const_view<mesh::imtime, T> gin, int n_iw = -1) {
    return make_gf_from_fourier(gin, make_adjoint_mesh(gin.mesh(), n_iw));
  }

  /// Fourier transform a Matsubara-frequency DLR Green's function to the imaginary-time DLR mesh.
  template <int N = 0, typename T> gf<mesh::dlr_imtime, T> make_gf_from_fourier(gf_const_view<mesh::dlr_imfreq, T> gin) {
    return make_gf_from_fourier(gin, make_adjoint_mesh(gin.mesh()));
  }

  /// Fourier transform an imaginary-time DLR Green's function to the Matsubara-frequency DLR mesh.
  template <int N = 0, typename T> gf<mesh::dlr_imfreq, typename T::complex_t> make_gf_from_fourier(gf_const_view<mesh::dlr_imtime, T> gin) {
    return make_gf_from_fourier(gin, make_adjoint_mesh(gin.mesh()));
  }

  /// Fourier transform a real-frequency Green's function to real time (set `shift_half_bin` to shift the time mesh by half a bin).
  template <int N = 0, typename T> gf<mesh::retime, T> make_gf_from_fourier(gf_const_view<mesh::refreq, T> gin, bool shift_half_bin = false) {
    return make_gf_from_fourier(gin, make_adjoint_mesh(gin.mesh(), shift_half_bin));
  }

  /// Fourier transform a real-time Green's function to real frequencies (set `shift_half_bin` to shift the frequency mesh by half a bin).
  template <int N = 0, typename T>
  gf<mesh::refreq, typename T::complex_t> make_gf_from_fourier(gf_const_view<mesh::retime, T> gin, bool shift_half_bin = false) {
    return make_gf_from_fourier(gin, make_adjoint_mesh(gin.mesh(), shift_half_bin));
  }

  // Worker tag used by make_gf_from_multi_fourier to fold one per-axis transform via operator&.
  template <int N> struct _fou_wk {};

  // Apply the N-th axis Fourier transform of gin (the operator& building block of make_gf_from_multi_fourier).
  template <int N, typename G>
  auto operator&(G &&gin, _fou_wk<N> const &) { // NOLINT(cppcoreguidelines-missing-std-forward): gin is only read, not forwarded into a sink
    return make_gf_from_fourier<N>(gin, make_adjoint_mesh(std::get<N>(gin.mesh())));
  }

  /**
   * @ingroup triqs-gfs-fourier
   * @brief Fourier transform several components of a product-mesh Green's function at once.
   *
   * @details Each axis listed in the template pack `Ns` is transformed to its conjugate mesh, in turn.
   *
   * @tparam Ns Indices of the mesh components to transform.
   * @tparam V Product mesh type of the input Green's function.
   * @tparam T Target type of the input Green's function.
   * @param gin The input Green's function on a product mesh.
   * @return A new Green's function with the selected components Fourier transformed.
   */
  template <int... Ns, typename V, typename T> auto make_gf_from_multi_fourier(gf_const_view<V, T> gin) { return (gin() & ... & _fou_wk<Ns>{}); }

  /// Fourier transform two components of a product-mesh Green's function onto the explicit meshes `m1` and `m2`.
  template <int N1, int N2, typename M1, typename M2, typename... Vs, typename T>
  auto make_gf_from_fourier(gf_const_view<mesh::prod<Vs...>, T> gin, M1 &&m1, M2 &&m2) {
    static_assert(sizeof...(Vs) >= 2, "Green function mesh rank incompatible with mesh indices");

    auto g1 = make_gf_from_fourier<N1>(gin(), std::forward<M1>(m1));
    auto g2 = make_gf_from_fourier<N2>(g1(), std::forward<M2>(m2));

    return g2;
  }

  /// Fourier transform three components of a product-mesh Green's function onto the explicit meshes `m1`, `m2` and `m3`.
  template <int N1, int N2, int N3, typename M1, typename M2, typename M3, typename... Vs, typename T>
  auto make_gf_from_fourier(gf_const_view<mesh::prod<Vs...>, T> gin, M1 &&m1, M2 &&m2, M3 &&m3) {
    static_assert(sizeof...(Vs) >= 3, "Green function mesh rank incompatible with mesh indices");

    auto g1 = make_gf_from_fourier<N1, N2>(gin(), std::forward<M1>(m1), std::forward<M2>(m2));
    auto g2 = make_gf_from_fourier<N3>(g1(), std::forward<M3>(m3));

    return g2;
  }

  /// Fourier transform the components `Ns` of a product-mesh Green's function onto their default conjugate meshes.
  template <int... Ns, typename... Vs, typename T> auto make_gf_from_fourier(gf_const_view<mesh::prod<Vs...>, T> gin) {
    return make_gf_from_fourier<Ns...>(gin(), make_adjoint_mesh(std::get<Ns>(gin.mesh()))...);
  }

  /// Fourier transform a block Green's function, with per-block known high-frequency moments.
  template <int N = 0, typename G, typename M, int R>
  auto make_gf_from_fourier(G const &gin, M const &m, std::vector<array<dcomplex, R>> const &known_moments)
    requires(is_block_gf_v<G>)
  {

    using r_t = decltype(make_gf_from_fourier<N>(gin[0], m, known_moments[0]));
    std::vector<r_t> g_vec;

    TRIQS_ASSERT2(gin.size() == known_moments.size(), "Fourier: Require equal number of blocks in block_gf and known_moments vector");

    for (auto [gin_bl, km_bl] : itertools::zip(gin, known_moments)) g_vec.push_back(make_gf_from_fourier<N>(gin_bl, m, km_bl));
    return make_block_gf(gin.block_names(), std::move(g_vec));
  }

  /// Fourier transform a Block2 Green's function, with per-block known high-frequency moments.
  template <int N = 0, typename G, typename M, int R>
  auto make_gf_from_fourier(G const &gin, M const &m, std::vector<std::vector<array<dcomplex, R>>> const &known_moments)
    requires(is_block_gf_v<G>)
  {

    using r_t = decltype(make_gf_from_fourier<N>(gin(0, 0), m, known_moments[0][0]));
    std::vector<std::vector<r_t>> g_vecvec;

    TRIQS_ASSERT2(gin.size1() == known_moments.size(), "Fourier: Require matching block structure between gin and known_moments");

    for (int i : range(gin.size1())) {
      TRIQS_ASSERT2(gin.size2() == known_moments[i].size(), "Fourier: Require matching block structure between gin and known_moments");

      std::vector<r_t> g_vec;
      for (int j : range(gin.size2())) g_vec.push_back(make_gf_from_fourier<N>(gin(i, j), m, known_moments[i][j]));

      g_vecvec.push_back(std::move(g_vec));
    }
    return block2_gf_of<r_t>{gin.block_names(), std::move(g_vecvec)};
  }

  /// Fourier transform a block Green's function block-wise, forwarding any extra arguments to each block.
  template <int N = 0, int... Ns, typename G, typename... Args>
  auto make_gf_from_fourier(G const &gin, Args const &...args)
    requires(is_block_gf_v<G>)
  {
    auto l = [&](auto &&g_bl) { return make_gf_from_fourier<N, Ns...>(make_const_view(g_bl), args...); };
    return map_block_gf(l, gin);
  }

  /// Overload accepting a triqs::gfs::gf_view input; delegates to the const-view implementation.
  template <int N = 0, int... Ns, typename V, typename T, typename... Args> auto make_gf_from_fourier(gf_view<V, T> gin, Args &&...args) {
    return make_gf_from_fourier<N, Ns...>(make_const_view(gin), std::forward<Args>(args)...);
  }

  /// Overload accepting a triqs::gfs::gf input; delegates to the const-view implementation.
  template <int N = 0, int... Ns, typename V, typename T, typename... Args> auto make_gf_from_fourier(gf<V, T> const &gin, Args &&...args) {
    return make_gf_from_fourier<N, Ns...>(gf_const_view{gin}, std::forward<Args>(args)...);
  }

  // Lazy Fourier expression: keeps a const view on the source gf and the (possibly reference) optional arguments of the call.
  template <int N, typename GCV, typename... Args> struct _fourier_lazy {
    GCV g;
    std::tuple<Args...> args; // Args can be a ref.
  };

  /**
   * @ingroup triqs-gfs-fourier
   * @fn auto fourier(G const &g, Args &&...args)
   * @brief Build a lazy Fourier transform expression for use in `g_out = fourier(g_in, ...)` assignments.
   *
   * @details The returned object holds a const view on `g` and the optional arguments; the actual transform is
   * performed when it is assigned into a target Green's function, whose mesh selects the conjugate variable.
   *
   * @tparam N Index of the mesh component to transform (default \f$ 0 \f$).
   * @tparam G The type of the input Green's function.
   * @tparam Args Types of the optional arguments (e.g. known high-frequency moments).
   * @param g The input Green's function.
   * @param args Optional arguments forwarded to the transform.
   * @return A lazy expression assignable into a Green's function on the conjugate mesh.
   */
  template <int N = 0, typename G, typename... Args> _fourier_lazy<N, typename G::const_view_type, Args...> fourier(G const &g, Args &&...args) {
    return {g(), {std::forward<Args>(args)...}};
  }

  // Realize the lazy Fourier expression for gx = fourier(gy): static-checks the mesh/target compatibility and calls _fourier.
  template <int N, typename M1, typename T1, typename M2, typename T2, typename... Args>
  void triqs_gf_view_assign_delegation(gf_view<M1, T1> lhs_g, _fourier_lazy<N, gf_const_view<M2, T2>, Args...> const &rhs) {
    static_assert(std::is_same_v<typename T1::real_t, typename T2::real_t>, "Error : in gx = fourier(gy), gx and gy must have the same target");

    if constexpr (n_variables<M1> == 1) // === single mesh
      static_assert(std::is_same_v<M2, _mesh_fourier_image<M1>>, "There is no Fourier transform between these two meshes");
    else { // === prod mesh
      using mesh_res_t = decltype(triqs::tuple::replace<N>(rhs.g.mesh().components(), make_adjoint_mesh(std::get<N>(rhs.g.mesh()))));
      static_assert(std::is_same_v<typename M1::m_tuple_t, mesh_res_t>, "Meshes in assignment don't match");
    }

    // check the size of the "inactive" dimensions

    std::apply([&](auto &&...u) { _fourier<N>(rhs.g, lhs_g, u...); }, rhs.args); // calls _fourier( rhs.g, lhs_g, rhs.args...)
  }

} // namespace triqs::gfs

namespace nda::clef {
  // Make fourier usable inside lazy CLEF expressions.
  TRIQS_CLEF_MAKE_FNT_LAZY(fourier);
} // namespace nda::clef
