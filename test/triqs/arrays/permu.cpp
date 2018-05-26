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
#include <iostream>
#include <triqs/arrays/indexmaps/permutation.hpp>

using namespace triqs::arrays;
using namespace triqs::arrays::permutations;

template <ull F> struct P {
  static constexpr ull value = F;
  friend std::ostream &operator<<(std::ostream &out, P const &s) {
    //out << "Permutation of size " << permutations::size(s.value) << " : "<< std::hex;
    out << std::hex;
    s.print(out, std::integral_constant<ull, 0>());
    return out << std::dec;
  }

  template <ull c> void print(std::ostream &out, std::integral_constant<ull, c>) const {
    out << apply(this->value, c);
    print(out, std::integral_constant<ull, c + 1>());
  }
  void print(std::ostream &out, std::integral_constant<ull, size(F)>) const {}
};

int main(int argc, char **argv) {

  constexpr auto p0 = permutation(0, 1);
  constexpr auto p  = permutation(0, 2, 1);
  constexpr auto p2 = permutation(2, 1, 0, 3);
  constexpr auto pc = permutation(1, 2, 3, 0);

  std::cout << P<permutation(0, 1)>() << std::endl;
  std::cout << P<p0>() << std::endl;
  std::cout << P<p>() << std::endl;
  std::cout << P<p2>() << std::endl;
  std::cout << P<pc>() << std::endl;

#define COMPOSE(P1, P2) std::cout << " composing " << P<P1>() << " and " << P<P2>() << " = " << P<permutations::compose(P1, P2)>() << std::endl;

#define INVERSE(P1)                                                                                                                                  \
  std::cout << "inverse of " << P<P1>() << " = " << P<permutations::inverse(P1)>() << std::endl;                                                     \
  std::cout << " checking " << P<permutations::compose(permutations::inverse(P1), P1)>() << std::endl;
  COMPOSE(p, p);
  COMPOSE(p2, p2);
  INVERSE(p);
  INVERSE(p2);
  INVERSE(pc);

#define ID(n)                                                                                                                                        \
  std::cout << " identity  :" << P<identity(n)>() << std::endl;                                                                                      \
  std::cout << " ridentity :" << P<ridentity(n)>() << std::endl << std::endl;

  ID(1);
  ID(2);
  ID(3);
  ID(4);
  ID(5);
}
