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
#include "./mini_vector.hpp"

namespace triqs {
namespace utility {

 class index3_generator {
  mini_vector<long, 3> i, d; // rely on mini_vector initialization
  long i_flat =0;
  bool _at_end = false;

  public:
  index3_generator() = default;
  index3_generator(mini_vector<long, 3> const& dims, mini_vector<long, 3> const &i) : d(dims),i(i) {}
  void advance() {
   ++i_flat;
   ++i[2];
   if (i[2] < d[2]) return;
   i[2] = 0;
   ++i[1];
   if (i[1] < d[1]) return;
   i[1] = 0;
   ++i[0];
   if (i[0] < d[0]) return;
   // i[0]=0;
   _at_end = true;
  }
  AUTO_DECL index() const { return i; }
  long linear_index() const { return i_flat;}
  bool at_end() const { return _at_end; }
  void reset() {
   _at_end = false;
   i_flat =0;
   i[0] = 0;
   i[1] = 0;
   i[2] = 0;
  }
 };
}
}

