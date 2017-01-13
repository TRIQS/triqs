/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2017 by O. Parcollet
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
#include <experimental/optional>

namespace std { // to be found by ADL

using std::experimental::optional;
using std::experimental::in_place;

namespace experimental {
 
 // workaround for compiler defect
 template <typename T> void reset(std::experimental::optional<T>& x) { x = std::experimental::optional<T>{}; }

 // .value does not work on old OS X compilers ... WORKAROUND
 template <typename T> T& value_of(std::experimental::optional<T>& x) { *x;}
 template <typename T> T const & value_of(std::experimental::optional<T> const & x) { *x;}
}
}

