#include <fmt/format.h>
#include <triqs/mesh.hpp>

int main() {
  // initialize a logarithmic mesh with eps=0.1, w_max=10, ratio=2
  triqs::mesh::refreq_log m{0.1, 10, 2.0};

  // loop over all mesh points and print their index, data index and value
  for (int i = 0; auto mp : m)
    fmt::println("mesh point #{}: index = {}, data index = {}, value = {}", i++, mp.index(), mp.data_index(), mp.value());
}
