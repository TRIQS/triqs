#include <fmt/base.h>
#include <triqs/mesh.hpp>

int main() {
  // initialize a Legendre mesh with beta = 10, fermionic statistics and 5 polynomials
  triqs::mesh::legendre m{10, triqs::mesh::Fermion, 5};

  // loop over all mesh points and print their index and data index
  for (int i = 0; auto mp : m) fmt::println("mesh point #{}: index = {}, data index = {}", i++, mp.index(), mp.data_index());
}
