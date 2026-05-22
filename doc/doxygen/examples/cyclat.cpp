#include <fmt/ranges.h>
#include <triqs/mesh.hpp>

int main() {
  // initialize a mesh on a cubic lattice with a = 0.5 and a 2x2x3 supercell
  triqs::mesh::cyclat m{triqs::mesh::bravais_lattice{nda::eye<double>(3) * 0.5}, {2, 2, 3}};

  // loop over all mesh points and print their index, data index and value (lattice vector)
  for (int i = 0; auto mp : m) {
    fmt::println("mesh point #{}: index = {}, data index = {}, value = {}", i++, mp.index(), mp.data_index(), nda::vector<double>{mp.value()});
  }
}
