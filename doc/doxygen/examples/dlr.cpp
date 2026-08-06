#include <fmt/format.h>
#include <triqs/mesh.hpp>

int main() {
  // initialize a fermionic DLR mesh with beta = 10, omega_max = 0.5 and epsilon = 1e-6
  triqs::mesh::dlr m{10, triqs::mesh::Fermion, 0.5, 1e-6};

  // loop over all mesh points and print their index, data index and value
  for (int i = 0; auto mp : m) {
    fmt::println("mesh point #{}: index = {}, data index = {}, value = {}", i++, mp.index(), mp.data_index(), mp.value());
  }
}
