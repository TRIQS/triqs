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

#if __cplusplus > 201402L 
#include <optional>
#else
#include <experimental/optional>
namespace std { // to be found by ADL

 using std::experimental::optional;
 using std::experimental::in_place;
}
#endif

namespace std::experimental{

 // workaround for compiler defect
 template <typename T> void reset(std::optional<T>& x) { x = std::optional<T>{}; }

 // .value does not work on old OS X compilers ... WORKAROUND
 template <typename T> T& value_of(std::optional<T>& x) { *x; }
 template <typename T> T const& value_of(std::optional<T> const& x) { *x; }
}
