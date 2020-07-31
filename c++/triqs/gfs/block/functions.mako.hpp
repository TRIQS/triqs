// Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
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
// Authors: Nils Wentzell

#pragma once

namespace triqs {
  namespace gfs {

    /*mako
  %for FUNC in ['fourier']:
   %for ROOT in ['block', 'block2']:
    %for EXT in ['_gf<V, T> const&','_gf<V, T>&', '_gf_view<V, T>', '_gf_const_view<V, T>']:
    <%
      GF = ROOT + EXT
     %>
  */
    template <typename V, typename T> auto MAKO_FUNC(MAKO_GF g) {
      return make_lazy_transform([](auto &&x) { return MAKO_FUNC(x); }, g);
    };

    // mako %endfor
    // mako %endfor
    // mako %endfor

    // FIXME c++17
    //#define MAP_BGF_17(FUNC)                                                                                                         \
 //template <typename G> requires(is_block_gf<G>()) auto FUNC(G const& g) {                                                \
  //lazy_transform_t{[](auto&& x) { return FUNC(x); }, g};                                                                         \
 //}
    // MAP_BGF(fourier);

  } // namespace gfs
} // namespace triqs
