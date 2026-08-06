#include <fmt/format.h>
#include <triqs/mesh.hpp>

int main() {
  // initialize a Chebyshev mesh with beta = 10, fermionic statistics and 5 points
  triqs::mesh::chebyshev m{10, triqs::mesh::Fermion, 5};

  // loop over all mesh points and print their index, data index and value
  for (int i = 0; auto mp : m)
    fmt::println("mesh point #{}: index = {}, data index = {}, value = {}", i++, mp.index(), mp.data_index(), mp.value());
}
