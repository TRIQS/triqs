/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by O. Parcollet
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

 /** 
  * Regroup indices for a C array
  * Usage : group_indices_view(A, {0,1}, {2,3})
  * Precondition : 
  *   - every indices is listed in the {...} exactly once.
  *   - the indices in one group are consecutive in memory.
  * Routine is not extra-fast, since it does check the preconditions
  */ 
 template <typename A, typename ...U>
 array_view<typename A::value_type, sizeof...(U)> group_indices_view(A const& a, std::initializer_list<U>... args) {

  auto indices_groups = std::vector<std::vector<int>>{std::vector<int>(args)...};
  
  static const int Rf = sizeof...(U);
  if (Rf != indices_groups.size())
   TRIQS_RUNTIME_ERROR << "Return rank is " << Rf << " but " << indices_groups.size() << " indices are given";
  
  /// First check indices are contiguous in memory
  auto mem_pos = a.indexmap().memory_layout().get_memory_positions();
  for (auto const& gr : indices_groups) {
   auto gr2 = std::vector<int>(gr.size());
   for (int i = 0; i < int(gr.size()); ++i) gr2[i] = mem_pos[gr[i]];
   if (*std::max_element(begin(gr2), end(gr2)) - *std::min_element(begin(gr2), end(gr2)) + 1 != gr2.size())
    TRIQS_RUNTIME_ERROR << "Indices not contiguous in memory";
  }

  /// Now compute the new lengths and strides.
  mini_vector<size_t, Rf> l;
  mini_vector<std::ptrdiff_t, Rf> s;
  auto la = a.indexmap().lengths();
  l = 1;
  s = a.num_elements(); // init : superior to all strides, so a good start for the min ...
  for (int i = 0; i < Rf; ++i) {
   for (auto ind : indices_groups[i]) {
    if (la[ind] == -1) TRIQS_RUNTIME_ERROR << "Index "<< ind << "given multiple times";
    l[i] *= la [ind];
    la[ind] = -1;
    s[i] = std::min(s[i], a.indexmap().strides()[ind]);
   }
  }
  
  /// Check all indices have been used 
  for (int i = 0; i < la.size(); ++i)
   if (la[i] != -1) TRIQS_RUNTIME_ERROR << "Grouping incomplete : index " << i << " is to taken into account";
  
  /// Return the new view (indexmap, storage)
  return {{l, s, 0}, a.storage()};
 }
}
}
