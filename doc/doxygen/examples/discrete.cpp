#include <fmt/format.h>
#include <triqs/mesh.hpp>

int main() {
  // initialize a discrete mesh with 5 points
  triqs::mesh::discrete m{5};

  // loop over all mesh points and print their index and data index
  for (int i = 0; auto mp : m) fmt::println("mesh point #{}: index = {}, data index = {}", i++, mp.index(), mp.data_index());
}
