/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015-2016 by O. Parcollet
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
#include <triqs/gfs/block/map.hpp>

namespace triqs {
namespace gfs {

 //mako %for FntToMap in ['inverse', 'reinterpret_scalar_valued_gf_as_matrix_valued'] : 

 //mako %for BGF in ['block_gf<M,T> &' , 'block_gf<M,T> const &' , 'block_gf_view<M,T>', 'block_gf_const_view<M,T>'] : 

 template <typename M, typename T> auto MAKO_FntToMap(MAKO_BGF g) { 
   auto l= [](auto&&x) { return MAKO_FntToMap(x);};
   return map_block_gf(l, g);   
 }

 //mako %endfor
 //mako %endfor

}
}

