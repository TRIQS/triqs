/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
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
#include <itertools/itertools.hpp>

namespace triqs::arrays {

  // For backward-compatibility
  using range    = itertools::range;
  template <typename F> void foreach (range const &r, F const &f) { itertools::foreach (r, f); }

  /**
   * Ellipsis
   *
   * Ellipsis can be provided in place of [[range]], as in python.
   * The type `ellipsis` is similar to [[range]] except that it is implicitly repeated as much as necessary.
   */
  struct ellipsis : range {
    ellipsis() : range() {}
  };
  // for the case A(i, ellipsis) where A is of dim 1...
  inline int operator*(ellipsis, int) { return 0; }
  inline int operator*(int, ellipsis) { return 0; }

} // namespace triqs::arrays
