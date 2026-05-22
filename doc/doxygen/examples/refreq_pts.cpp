#include <fmt/base.h>
#include <triqs/mesh.hpp>

int main() {
  // initialize a refreq_pts mesh from a vector of points
  triqs::mesh::refreq_pts m{std::vector<double>{-5.0, -1.0, 0.0, 1.0, 5.0}};

  // loop over all mesh points and print their index, data index and value
  for (int i = 0; auto mp : m)
    fmt::println("mesh point #{}: index = {}, data index = {}, value = {}", i++, mp.index(), mp.data_index(), mp.value());
}
