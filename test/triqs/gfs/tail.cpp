#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>
#include <iostream>

using namespace triqs::gfs;
using namespace triqs::arrays;
using namespace triqs::utility;
using namespace triqs::clef;

namespace std {
  template <typename T1, typename T2> std::ostream &operator<<(std::ostream &os, std::pair<T1, T2> const &x) {
    return os << '(' << x.first << ", " << x.second << ')';
  }
} // namespace std

TEST(Gf, Exceptions) { // NOLINT

  double beta = 1;

  auto g = gf<imfreq>{{beta, Fermion, 20}, make_shape(1, 1)};

  placeholder<0> w_;
  g(w_) << 1. / (w_ - 1);

  std::cout << get_tail(g) << "\n\n";

  //g.mesh().set_tail_parameters(0.4);
  //std::cout << get_tail(g) << "\n\n";

  auto known_moments     = array<dcomplex, 3>(2, 1, 1);

  known_moments(0, 0, 0) = 0.0;
  known_moments(1, 0, 0) = 1.0;
  std::cout << get_tail(g, known_moments) << "\n\n";

  known_moments(0, 0, 0) = 1.0;
  known_moments(1, 0, 0) = 1.0;
  std::cout << get_tail(g, known_moments) << "\n\n";
}
MAKE_MAIN;
