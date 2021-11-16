#include "./cluster_mesh.hpp"

namespace triqs::mesh {
  std::array<long, 3> find_cell_dims(nda::matrix<long> const &periodization_matrix) {

    auto P     = nda::matrix<double>(periodization_matrix);
    auto inv_P = make_regular(inverse(P));
    int L      = std::floor(std::abs(nda::determinant(P)));
    clef::placeholder<0> i_;
    std::vector<nda::vector<int>> units{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    std::vector<nda::vector<int>> C{{0, 0, 0}};
    std::array<long, 3> k_res;
    for (int d = 0; d < 3; d++) {
      k_res[d] = L + 1;
      for (auto const &x : C) {
        for (int k = 1; k <= L; k++) {
          bool OK = false;
          for (int dp = 0; dp < 3; dp++) {
            double s = 0;
            for (int i : range(3)) s += inv_P(dp, i) * x(i);
            double crit = k * inv_P(dp, d) - s;
            if (std::abs(crit - int(crit)) > 1e-8) {
              OK = false;
              break;
            } else {
              OK = true;
            }

          } // dp
          if (OK and k < k_res[d]) {
            k_res[d] = k;
            break;
          }
        } // k
      }   // x

      std::vector<nda::vector<int>> Cp;
      for (auto const &x : C)
        for (int q = 0; q < k_res[d]; q++) Cp.push_back(x + q * units[d]);
      C = Cp;
    } // d

    return k_res;
  }
} // namespace triqs::mesh
