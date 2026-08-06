#include <fmt/format.h>
#include <triqs/mesh.hpp>

int main() {
  // initialize a real frequency mesh with 5 points on the interval [-5, 5]
  triqs::mesh::refreq m{-5, 5, 5};

  // loop over all mesh points and print their index, data index and value
  for (int i = 0; auto mp : m) {
    fmt::println("mesh point #{}: index = {}, data index = {}, value = {}", i++, mp.index(), mp.data_index(), mp.value());
  }
}
