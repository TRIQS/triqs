/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2016 by O. Parcollet
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
#include <triqs/arrays/array.hpp>
#include <algorithm>

namespace triqs {
namespace arrays {


 // FIXME  generalize
 template <typename A>
 std14::conditional_t<A::is_const, array_const_view<typename A::value_type, A::rank + 2>,
                      array_view<typename A::value_type, A::rank + 2>>
 reinterpret_array_add_1x1(A const &d) {
  auto &imap = d.indexmap();
  typename array_view<typename A::value_type, A::rank + 2>::indexmap_type index_map(
      join(imap.lengths(), make_shape(1, 1)), join(imap.strides(), make_shape(1, 1)), imap.start_shift());
  return {index_map, d.storage()};
 };
}
}
