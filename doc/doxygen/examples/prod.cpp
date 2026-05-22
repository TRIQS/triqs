#include <fmt/ranges.h>
#include <triqs/mesh.hpp>

#include <numbers>

int main() {
  using std::numbers::pi;

  // initialize a mesh on a bravais lattice with a = 1 and a 2x2x1 meshpoints
  auto bl   = triqs::mesh::bravais_lattice{nda::eye<double>(3)};
  auto bl_m = triqs::mesh::cyclat{bl, {2, 2, 1}};

  // initialize a bosonic imaginary frequency mesh with 2 positive frequencies and beta = 10
  triqs::mesh::imfreq iw_m{10, triqs::mesh::Boson, 2};

  // combine the two meshes into a product mesh
  auto m = triqs::mesh::prod{bl_m, iw_m};

  // loop over all mesh points and print their index and data index tuples
  for (int i = 0; auto mp : m) { fmt::println("mesh point #{}: index = {}, data index = {}", i++, mp.index(), mp.data_index()); }
}
