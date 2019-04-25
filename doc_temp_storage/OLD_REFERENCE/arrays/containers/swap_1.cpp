#include <triqs/arrays.hpp>
#include <iostream>
using itertools::range;
using triqs::arrays::vector;
int main() {
  triqs::arrays::vector<double> V(3), W(3);
  V()     = 3;
  W()     = 5; // initialize
  auto VV = V(range(0, 2));
  auto VW = W(range(0, 2));

  std::cout << "V = " << V << " W = " << W << " V view " << VV << " W view " << VW << std::endl;
  deep_swap(V, W);
  std::cout << "V = " << V << " W = " << W << " V view " << VV << " W view " << VW << std::endl;
  deep_swap(VV, VW);
  std::cout << "V = " << V << " W = " << W << " V view " << VV << " W view " << VW << std::endl;
}
