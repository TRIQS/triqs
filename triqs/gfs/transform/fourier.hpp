/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by M. Ferrero, O. Parcollet
 * Copyright (C) 2018- by Simons Foundation
 *               authors : O. Parcollet, N. Wentzell 
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
#include "./../gf/flatten.hpp"

namespace triqs::gfs {

  using namespace triqs::arrays;

  /*------------------------------------------------------------------------------------------------------
  *                                  Mesh calculator
  *-----------------------------------------------------------------------------------------------------*/

  // general form. NB : options MUST have defaults (for traits later)
  //gf_mesh<Y> make_mesh_fourier_compatible( gf_mesh<X> const &, options ...);

  // FIXME : DOC
  inline gf_mesh<imfreq> make_mesh_fourier_compatible(gf_mesh<imtime> const &m, int n_iw = -1) {
    if (n_iw == -1) n_iw = (m.size() - 1) / 2;
    return {m.domain(), n_iw};
  }

  // FIXME : DOC
  inline gf_mesh<imtime> make_mesh_fourier_compatible(gf_mesh<imfreq> const &m, int n_tau = -1) {
    if (n_tau == -1) n_tau = 2 * (m.last_index() + 1) + 1;
    return {m.domain(), n_tau};
  }

  // FIXME : DOC
  inline gf_mesh<refreq> make_mesh_fourier_compatible(gf_mesh<retime> const &m) {
    int L       = m.size();
    double wmax = M_PI * (L - 1) / (L * m.delta());
    return {-wmax, wmax, L};
  }

  inline gf_mesh<retime> make_mesh_fourier_compatible(gf_mesh<refreq> const &m) {
    int L       = m.size();
    double tmax = M_PI * (L - 1) / (L * m.delta());
    return {-tmax, tmax, L};
  }

  // FIXME : DOC
  inline gf_mesh<brillouin_zone> make_mesh_fourier_compatible(gf_mesh<cyclic_lattice> const &m, int n_iw = -1) {
    return {m.domain(), m.periodization_matrix};
  }

  // FIXME : DOC
  inline gf_mesh<cyclic_lattice> make_mesh_fourier_compatible(gf_mesh<brillouin_zone> const &m, int n_tau = -1) {
    return {m.domain(), m.periodization_matrix};
  }

  // trait for error messages later
  template <typename V> using _mesh_fourier_image = typename decltype(make_mesh_fourier_compatible(gf_mesh<V>()))::var_t;

  /*------------------------------------------------------------------------------------------------------
            Implementation
  *-----------------------------------------------------------------------------------------------------*/

  template <typename V> using gf_vec_t = gf<V, tensor_valued<1>>;

  // matsubara
  gf_vec_t<imfreq> _fourier_impl(gf_mesh<imfreq> const &iw_mesh, gf_vec_t<imtime> &&gt, array_const_view<dcomplex, 2> mom_23 = {});
  gf_vec_t<imtime> _fourier_impl(gf_mesh<imtime> const &tau_mesh, gf_vec_t<imfreq> &&gw, array_const_view<dcomplex, 2> mom_123 = {});

  // real
  gf_vec_t<refreq> _fourier_impl(gf_mesh<refreq> const &w_mesh, gf_vec_t<retime> &&gt, arrays::array_const_view<dcomplex, 2> mom_12 = {});
  gf_vec_t<retime> _fourier_impl(gf_mesh<retime> const &t_mesh, gf_vec_t<refreq> &&gw, arrays::array_const_view<dcomplex, 2> mom_12 = {});

  // lattice
  gf_vec_t<cyclic_lattice> _fourier_impl(gf_mesh<cyclic_lattice> const &r_mesh, gf_vec_t<brillouin_zone> &&gk);
  gf_vec_t<brillouin_zone> _fourier_impl(gf_mesh<brillouin_zone> const &k_mesh, gf_vec_t<cyclic_lattice> &&gr);

  /*------------------------------------------------------------------------------------------------------
           The general Fourier function
  *-----------------------------------------------------------------------------------------------------*/

  // this function just regroups the function, and calls the vector_valued gf core implementation
  template <int N, typename V, typename V2, typename T>
  void _fourier(gf_const_view<V, T> gin, gf_view<V2, T> gout, array_const_view<dcomplex, gf<V, T>::data_rank> moments = {}) {

    auto const &mesh  = std::get<N>(gout.mesh());
    auto gout_flatten = _fourier_impl(mesh, flatten_gf_2d<N>(gin), flatten_2d(moments, N));
    if constexpr (gin.data_rank == 1)
      gout = gout_flatten;
    else {
      // inverse operation as flatten_2d, exactly
      auto _     = ellipsis();
      auto g_rot = rotate_index_view(gout.data(), N);
      auto a_0   = g_rot(0, _);
      for (auto const &mp : mesh) {
        auto g_rot_sl = g_rot(mp.linear_index(), _); // if the array is long, it is faster to precompute the view ...
        auto gout_col = gout_flatten.data()(mp.linear_index(), _);
        assign_foreach(g_rot_sl, [&gout_col, c = 0ll](auto &&... i) mutable { return gout_col(c++); });
      }
    }
  }

  /* *-----------------------------------------------------------------------------------------------------
   *
   * make_gf_from_fourier (mesh, g, options)  -> fourier_transform of g
   *
   * *-----------------------------------------------------------------------------------------------------*/

  template <int N, typename V1, typename V2, typename T>
  gf<V2, T> make_gf_from_fourier(gf_const_view<V1, T> gin, gf_mesh<V2> const &mesh, array_const_view<dcomplex, gf<V1, T>::data_rank> moments = {}) {
    static_assert(std::is_same_v<V2, _mesh_fourier_image<V1>>, "There is no Fourier transform between these two meshes");
    gf<V2, T> gout{mesh, gin.target_shape()};
    _fourier(gout(), gin, moments);
    return gout;
  }

  // to avoid the template matching issue ...
  template <typename V, typename T, typename... Args> auto make_gf_from_fourier(gf_view<V, T> const &gin, Args &&... args) {
    return make_gf_from_fourier(gin(), std::forward<Args>(args)...);
  }

  template <typename V, typename T, typename... Args> auto make_gf_from_fourier(gf<V, T> const &gin, Args &&... args) {
    return make_gf_from_fourier(gin(), std::forward<Args>(args)...);
  }

  template <typename V, typename T, typename... Args> auto make_gf_from_inverse_fourier(gf_view<V, T> const &gin, Args &&... args) {
    return make_gf_from_inverse_fourier(gin(), std::forward<Args>(args)...);
  }

  template <typename V, typename T, typename... Args> auto make_gf_from_inverse_fourier(gf<V, T> const &gin, Args &&... args) {
    return make_gf_from_inverse_fourier(gin(), std::forward<Args>(args)...);
  }
  /*------------------------------------------------------------------------------------------------------
  *                                  Lazy transformation
  *-----------------------------------------------------------------------------------------------------*/

  // internal. Keep a view on a, and the argument of the call
  template <typename GCV, typename... U> struct _fourier_lazy {
    GCV g;
    std::tuple<U...> args; // U can be a ref.
  };

  // FIXME : DOC
  template <typename G, typename... U> _fourier_lazy<typename G::const_view_type, U...> fourier(G const &g, U &&... u) {
    return {g(), {std::forward<U>(u)...}};
  }

  // FIXME : DOC. NB : backward compatibility only, or CHECK var_t ??
  template <typename G, typename... U> _fourier_lazy<typename G::const_view_type, U...> inverse_fourier(G const &g, U &&... u) {
    return {g(), {std::forward<U>(u)...}};
  }

  // realize the call for gx = fourier(gy);
  template <typename V1, typename T1, typename V2, typename T2, typename... U>
  void triqs_gf_view_assign_delegation(gf_view<V1, T1> g, _fourier_lazy<gf_const_view<V2, T2>, U...> const &lhs) {
    static_assert(std::is_same_v<T1, T2>, "Error : in gx = fourier(gy), gx and gy must have the same target");
    static_assert(std::is_same_v<V2, _mesh_fourier_image<V1>>, "Error : in gx = fourier(gy), gx and gy must have compatible meshes");
    std::apply([&](auto &&... u) { _fourier<0>(lhs.g, g, u...); }, lhs.args); // calls _fourier( g, lhs.g, lhs.args...)
  }

} // namespace triqs::gfs

// declares the function to accept the clef lazy expressions
namespace triqs::clef {
  TRIQS_CLEF_MAKE_FNT_LAZY(fourier);
  TRIQS_CLEF_MAKE_FNT_LAZY(inverse_fourier);
} // namespace triqs::clef
