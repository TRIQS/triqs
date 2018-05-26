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
#include "./common.hpp"
#include <triqs/arrays/array.hpp>
#include <iostream>

using namespace triqs::arrays;

auto bad(array<long, 1> const &a) DECL_AND_RETURN(a(range(0, 3)));

auto bad2(array<long, 1> const &a) DECL_AND_RETURN(a(0));

array_const_view<long, 1> good(array<long, 1> const &a) { return a(range(0, 3)); }

int main(int argc, char **argv) {

  try {

    array<long, 1> A = {1, 2, 3, 4};

    std::cerr << "A is " << A << std::endl;

    {
      auto r = A(range(0, 3));
      TEST_ASSERT(r.storage().is_weak);
    }

    {
      auto r = array<long, 1>{1, 2, 3, 4}(range(0, 3));
      TEST_ASSERT(!r.storage().is_weak);
      std::cerr << r << r.storage().is_weak << std::endl;
    }

    {
      auto r = array<long, 1>{1, 2, 3, 4}(range(0, 3))();
      std::cerr << r << r.storage().is_weak << std::endl;
      TEST_ASSERT(!r.storage().is_weak);
    }

    {
      auto r = good({1, 2, 3, 4});
      std::cerr << " good " << r << r.storage().is_weak << std::endl;
    }

// rm or sanitizer will detect this ...
#ifdef MAKE_IT_WRONG
    {
      auto &r = bad2({1, 2, 3, 4});
      std::cerr << " bad2 " << r << std::endl;
    }

    {
      auto r = bad({1, 2, 3, 4});
      std::cerr << " bad " << r << r.storage().is_weak << std::endl;
    }
#endif
  }
  TRIQS_CATCH_AND_ABORT;
}
