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
#include <triqs/arrays/vector.hpp>
#include <iostream>

namespace tqa = triqs::arrays;

int main(int argc, char **argv) {

  std::cout << "With swap" << std::endl;
  {
    triqs::arrays::vector<double> V(3), W(4);
    ;
    V() = 3;
    W() = 4;

    auto VV = V(tqa::range(0, 2));
    auto VW = W(tqa::range(0, 2));

    std::cout << "V = " << V << " W = " << W << " V view " << VV << " W view " << VW << std::endl;
    swap(V, W);
    std::cout << "V = " << V << " W = " << W << " V view " << VV << " W view " << VW << std::endl;
    swap(VV, VW);
    std::cout << "V = " << V << " W = " << W << " V view " << VV << " W view " << VW << std::endl;
  }

  std::cout << "With std::swap" << std::endl;

  {
    triqs::arrays::vector<double> V(3), W(4);
    ;
    V() = 3;
    W() = 4;

    std::cout << "V = " << V << " W = " << W << std::endl;
    std::swap(V, W);
    std::cout << "V = " << V << " W = " << W << std::endl;

    // This does NOT compile, the std::swap specialization is deleted.
    auto VV = V(tqa::range(0, 2));
    auto VW = W(tqa::range(0, 2));
    //std::swap(VV,VW);
  }
  std::cout << "With deep_swap" << std::endl;

  {
    triqs::arrays::vector<double> V(3), W(3);
    ;
    V() = 3;
    W() = 4;

    auto VV = V(tqa::range(0, 2));
    auto VW = W(tqa::range(0, 2));

    std::cout << "V = " << V << " W = " << W << " V view " << VV << " W view " << VW << std::endl;
    deep_swap(V, W);
    std::cout << "V = " << V << " W = " << W << " V view " << VV << " W view " << VW << std::endl;
    deep_swap(VV, VW);
    std::cout << "V = " << V << " W = " << W << " V view " << VV << " W view " << VW << std::endl;
  }

  return 0;
}
