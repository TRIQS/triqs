/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2014 by M. Ferrero, O. Parcollet
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
#include <triqs/arrays/vector.hpp>
#include "./../gf/flatten.hpp"

namespace triqs { namespace gfs {

 using namespace triqs::arrays;

 enum class fftw_direction_t{ direct, inverse };
 void _fourier_base(array_const_view<dcomplex, 2> in, array_view<dcomplex, 2> out, int rank, int* dims, int fftw_count, fftw_direction_t d);

 namespace tags { struct fourier{}; }

 /*------------------------------------------------------------------------------------------------------
  *                                  For lazy transformation
  *-----------------------------------------------------------------------------------------------------*/

 template <typename Tag, typename G> struct tagged_cview { typename G::const_view_type g; };

 /// FIXME : remove this 
 template <typename Tag, typename D, typename Target = matrix_valued> struct gf_keeper { gf_const_view<D, Target> g; };

  // We need a mechanism to tell which meshes are related by Fourier op, e.g. imtime, imfreq...
  // Or the error will happens as _fourier is not implemented, which is not clear...
  // A little constexpr will do the job
  template <typename Var> constexpr int _fourier_category() {
    if constexpr (std::is_same_v<Var, imtime>) return 0;
    if constexpr (std::is_same_v<Var, imfreq>) return 0;
    if constexpr (std::is_same_v<Var, retime>) return 1;
    if constexpr (std::is_same_v<Var, refreq>) return 1;
    // continue with cyclic///
    return -1;
  }

  // this function just regroups the function, and calls the vector_valued gf core implementation
  template <int N, typename Var, typename Var2, typename Target, int R = get_n_variables<Var>::value - 1 + Target::rank>
  void _fourier(gf_view<Var2, Target> gout, gf_const_view<Var, Target> gin, array_const_view<dcomplex, R + 1> moment_two_three = {}) {
    static_assert(_fourier_category<Var>() == _fourier_category<Var2>(), "There is no Fourier transform between these two meshes");
    auto const &mesh = std::get<N>(gout.mesh());
    auto gout_flatten = _fourier_impl(mesh, flatten_2d<N>(gin), flatten_2d<N>(moment_two_three));
    if constexpr (R == 0)
      gout = gout_flatten;
    else {
      // inverse operation as flatten_2d, exactly
      auto g_data_view = rotate_index_view(gout.data(), N);
      auto a_0         = g_data_view(0, ellipsis());
      for (auto const &mp : mesh)
        foreach (a_0, [&g_data_view, &gout_flatten, &mp, c = long{0} ](auto &&... i) mutable { g_data_view(mp.linear_index(), i...) = gout_flatten.data()(mp.linear_index(), c++); })
          ;
    }
  }

  // implements the maker of the fourier transform
  template <int N, typename Var, typename Var2, typename Target, int R = get_n_variables<Var>::value - 1 + Target::rank>
  gf<Var2, Target> _make_fourier(gf_const_view<Var, Target> gin, gf_mesh<Var2> const &mesh,
                                      array_const_view<dcomplex, R + 1> moment_two_three = {}) {
    static_assert(_fourier_category<Var>() == _fourier_category<Var2>(), "There is no Fourier transform between these two meshes");
    gf<Var2, Target> gout{mesh, gin.target_shape()};
    _fourier(gout(), gin, moment_two_three);
    return gout;
  }

  template <typename Var, typename T, typename... Args>
  gf<Var, T> make_gf_from_fourier(gf_view<Var, T> const& gin, Args&&... args) {
   return make_gf_from_fourier(gin(), std::forward<Args>(args)...);
  }

  template <typename Var, typename T, typename... Args>
  gf<Var, T> make_gf_from_fourier(gf<Var, T> const& gin, Args&&... args) {
   return make_gf_from_fourier(gin(), std::forward<Args>(args)...);
  }

  template <typename Var, typename T, typename... Args>
  gf<Var, T> make_gf_from_inverse_fourier(gf_view<Var, T> const& gin, Args&&... args) {
   return make_gf_from_inverse_fourier(gin(), std::forward<Args>(args)...);
  }

  template <typename Var, typename T, typename... Args>
  gf<Var, T> make_gf_from_inverse_fourier(gf<Var, T> const& gin, Args&&... args) {
   return make_gf_from_inverse_fourier(gin(), std::forward<Args>(args)...);
  }

 // -------------------------------------------------------------------

 // second part of the implementation
 template <typename X, typename Y, typename T>
 void triqs_gf_view_assign_delegation(gf_view<X, T> g, gf_keeper<tags::fourier, Y, T> const &L) {
  _fourier(g, L.g);
  }

 template <typename X, typename G, typename T>
 void triqs_gf_view_assign_delegation(gf_view<X, T> g, tagged_cview<tags::fourier, G> const &L) {
  _fourier(g, L.g);
  }
}}
