/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#include "../meshes/product.hpp"

namespace triqs {
namespace gfs {

 // forward declaration, Cf m_tail
 template <typename Var, typename... Args>
 auto evaluate(gf_const_view<Var, tail_valued<matrix_valued>> const &g, Args const &... args);
 template <typename Var, typename... Args> auto evaluate(gf<Var, tail_valued<matrix_valued>> const &g, Args const &... args);
 template <typename Var, typename... Args> auto evaluate(gf_view<Var, tail_valued<matrix_valued>> const &g, Args const &... args);

}
}

