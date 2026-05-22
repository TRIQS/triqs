#include <fmt/ranges.h>
#include <triqs/mesh.hpp>

#include <numbers>

int main() {
  using std::numbers::pi;

  // initialize a mesh on a cubic BZ with a = 2pi and a 2x2x3 k-point grid
  triqs::mesh::brzone m{triqs::mesh::brillouin_zone{triqs::mesh::bravais_lattice{nda::eye<double>(3) * 2 * pi}}, {2, 2, 3}};

  // loop over all mesh points and print their index, data index and value (k-point)
  for (int i = 0; auto mp : m) {
    fmt::println("mesh point #{}: index = {}, data index = {}, value = {::.4f}", i++, mp.index(), mp.data_index(), mp.value());
  }
}
