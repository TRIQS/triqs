// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
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
// Authors: Olivier Parcollet, Nils Wentzell

#pragma once
#include <triqs/gfs/block/map.hpp>

namespace triqs {
  namespace gfs {

    //mako %for FntToMap in ['inverse', 'reinterpret_scalar_valued_gf_as_matrix_valued', 'make_gf_from_fourier', 'make_gf_from_inverse_fourier'] :

    //mako %for BGF in ['block_gf<M,T> &' , 'block_gf<M,T> const &' , 'block_gf_view<M,T>', 'block_gf_const_view<M,T>', 'block2_gf<M,T> &' , 'block2_gf<M,T> const &' , 'block2_gf_view<M,T>', 'block2_gf_const_view<M,T>'] :
    template <typename M, typename T> auto MAKO_FntToMap(MAKO_BGF g) {
      auto l = [](auto &&x) { return MAKO_FntToMap(x); };
      return map_block_gf(l, g);
    }

    //mako %endfor

    //mako %endfor

  } // namespace gfs
} // namespace triqs
