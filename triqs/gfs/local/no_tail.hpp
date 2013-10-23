/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013-2014 by O. Parcollet
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
#include "../tools.hpp"
#include "../gf.hpp"
#include "./tail.hpp"

namespace triqs {
namespace gfs {

 struct no_tail {};

 template <typename Variable, typename Target, bool V, bool C>
 gf_view<Variable, Target, no_tail, C> make_gf_view_without_tail(gf_impl<Variable, Target, void, V, C> const &g) {
  return {g.mesh(), g.data(), {}, g.symmetry()};
 }

 template <typename Variable, typename Target, bool V, bool C>
 gf_view<Variable, Target> make_gf_from_g_and_tail(gf_impl<Variable, Target, no_tail, V, C> const &g, local::tail t) {
  auto g2 = gf<Variable, Target, no_tail>{g}; // copy the function without tail
  return {std::move(g2.mesh()), std::move(g2.data()), std::move(t), g2.symmetry()};
 }

 template <typename Variable, typename Target, bool V, bool C>
 gf_view<Variable, Target, void, C> make_gf_view_from_g_and_tail(gf_impl<Variable, Target, no_tail, V, C> const &g,
                                                                 local::tail_view t) {
  return {g.mesh(), g.data(), t, g.symmetry()};
 }
}
}
