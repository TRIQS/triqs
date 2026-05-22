#include <fmt/std.h>
#include <triqs/mesh.hpp>

int main() {
  // initialize a fermionic imaginary frequency mesh with 3 positive frequencies and beta = 10
  triqs::mesh::imfreq m{10, triqs::mesh::Fermion, 3};

  // loop over all mesh points and print their index, data index and value
  for (int i = 0; auto mp : m) {
    fmt::println("mesh point #{}: index = {}, data index = {}, value = {}", i++, mp.index(), mp.data_index(), std::complex<double>(mp.value()));
  }
}
