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
#include "./nda_test_common.hpp"
static_assert(!std::is_pod<triqs::arrays::array<long, 2>>::value, "POD pb");

#if defined(__has_feature)
#if __has_feature(address_sanitizer)
#include <sanitizer/asan_interface.h>
#endif
#endif

TEST(Array, Poison) {

  long *p;
  {
    array<long, 2> A(3, 3);
    A() = 3;
    p   = &(A(0, 0));
  }

#if defined(__has_feature)
#if __has_feature(address_sanitizer)
  EXPECT_EQ(__asan_address_is_poisoned(p), 1);
#endif
#endif
}

MAKE_MAIN;
