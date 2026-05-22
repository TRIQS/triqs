#include <fmt/base.h>
#include <triqs/mesh.hpp>

int main() {
  // initialize a fermionic imaginary time mesh with 5 points and beta = 10
  triqs::mesh::imtime m{10, triqs::mesh::Fermion, 5};

  // loop over all mesh points and print their index, data index and value
  for (int i = 0; auto mp : m) {
    fmt::println("mesh point #{}: index = {}, data index = {}, value = {}", i++, mp.index(), mp.data_index(), mp.value());
  }
}
