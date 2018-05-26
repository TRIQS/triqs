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
// This test does not like the macro check , we only check memory_layout, no data here
#ifdef TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#undef TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#endif

#include "./common.hpp"
#include <iostream>
#include <triqs/arrays.hpp>

using namespace triqs::arrays;
using namespace triqs::arrays::indexmaps;

template <typename ML1, typename ML2, typename... Args> void test(ML1 ml1, ML2 ml2, Args... x) {

  try {

    cuboid::map<ML2::rank> m(ml2);
    slicer<cuboid::map<ML2::rank>, Args...> S;
    auto m2 = S.invoke(m, x...);
    std::cout << m.memory_layout() << std::endl;
    std::cout << m2.memory_layout() << std::endl;

    if (m2.memory_layout() != ml1) {
      std::cout << "error " << m2.memory_layout() << "  " << ml1 << std::endl;
      exit(1);
    }
    std::cout << "----------------------" << std::endl;

  } catch (std::exception const &e) { std::cout << " error " << e.what() << std::endl; }
}

int main(int argc, char **argv) {

  std::cout << " C order " << std::endl;
  test(make_memory_layout(0, 1), make_memory_layout(0, 1, 2, 3), 0, range(), 0, range());
  test(make_memory_layout(0, 1, 2), make_memory_layout(0, 1, 2, 3), range(), range(), 0, range());
  test(make_memory_layout(0, 1, 2, 3), make_memory_layout(0, 1, 2, 3), range(), range(), range(), range());
  test(make_memory_layout(), make_memory_layout(0, 1, 2, 3), 0, 0, 0, 0);

  std::cout << " F order " << std::endl;
  test(make_memory_layout(1, 0), make_memory_layout(3, 2, 1, 0), 0, range(), 0, range());
  test(make_memory_layout(2, 1, 0), make_memory_layout(3, 2, 1, 0), 0, range(), range(), range());
  test(make_memory_layout(2, 1, 0), make_memory_layout(3, 2, 1, 0), range(), 0, range(), range());
  test(make_memory_layout(2, 1, 0), make_memory_layout(3, 2, 1, 0), range(), range(), 0, range());
  test(make_memory_layout(2, 1, 0), make_memory_layout(3, 2, 1, 0), range(), range(), range(), 0);
  test(make_memory_layout(3, 2, 1, 0), make_memory_layout(3, 2, 1, 0), range(), range(), range(), range());
  test(make_memory_layout(), make_memory_layout(3, 2, 1, 0), 0, 0, 0, 0);

  test(make_memory_layout(0), make_memory_layout(0, 1), 0, range());

  std::cout << " custom order " << std::endl;
  test(make_memory_layout(1, 0), make_memory_layout(0, 3, 1, 2), 0, range(), 0, range());
  test(make_memory_layout(2, 0, 1), make_memory_layout(0, 3, 1, 2), 0, range(), range(), range());
  test(make_memory_layout(0, 2, 1), make_memory_layout(0, 3, 1, 2), range(), 0, range(), range());
  test(make_memory_layout(0, 2, 1), make_memory_layout(0, 3, 1, 2), range(), range(), 0, range());
  test(make_memory_layout(0, 1, 2), make_memory_layout(0, 3, 1, 2), range(), range(), range(), 0);
  test(make_memory_layout(0, 3, 1, 2), make_memory_layout(0, 3, 1, 2), range(), range(), range(), range());
  test(make_memory_layout(), make_memory_layout(0, 3, 1, 2), 0, 0, 0, 0);

  std::cout << " ----------- custom order ------------- " << std::endl;

  std::cout << " ---- 0 0 " << std::endl;
  test(make_memory_layout(2, 0, 3, 1), make_memory_layout(2, 0, 3, 1), range(), range(), range(), range());

  std::cout << " ---- 1 0 " << std::endl;
  test(make_memory_layout(1, 2, 0), make_memory_layout(2, 0, 3, 1), 0, range(), range(), range());
  test(make_memory_layout(1, 0, 2), make_memory_layout(2, 0, 3, 1), range(), 0, range(), range());
  test(make_memory_layout(0, 2, 1), make_memory_layout(2, 0, 3, 1), range(), range(), 0, range());
  test(make_memory_layout(2, 0, 1), make_memory_layout(2, 0, 3, 1), range(), range(), range(), 0);

  std::cout << " ---- 2 0 " << std::endl;
  test(make_memory_layout(0, 1), make_memory_layout(2, 0, 3, 1), 0, 0, range(), range());
  test(make_memory_layout(1, 0), make_memory_layout(2, 0, 3, 1), 0, range(), 0, range());
  test(make_memory_layout(1, 0), make_memory_layout(2, 0, 3, 1), 0, range(), range(), 0);
  test(make_memory_layout(0, 1), make_memory_layout(2, 0, 3, 1), range(), 0, 0, range());
  test(make_memory_layout(1, 0), make_memory_layout(2, 0, 3, 1), range(), 0, range(), 0);
  test(make_memory_layout(0, 1), make_memory_layout(2, 0, 3, 1), range(), range(), 0, 0);

  std::cout << " ---- 3 0 " << std::endl;
  test(make_memory_layout(0), make_memory_layout(2, 0, 3, 1), range(), 0, 0, 0);
  test(make_memory_layout(0), make_memory_layout(2, 0, 3, 1), 0, range(), 0, 0);
  test(make_memory_layout(0), make_memory_layout(2, 0, 3, 1), 0, 0, range(), 0);
  test(make_memory_layout(0), make_memory_layout(2, 0, 3, 1), 0, 0, 0, range());

  std::cout << " ---- 4 0 " << std::endl;
  test(make_memory_layout(), make_memory_layout(2, 0, 3, 1), 0, 0, 0, 0);
  std::cout << " OK " << std::endl;
}
