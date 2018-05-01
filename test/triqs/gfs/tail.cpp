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

TEST(Gf, Fourier) { // NOLINT

  double beta = 1;
  int n_iw    = 20;

  auto gw = gf<imfreq, scalar_valued>{{beta, Fermion, n_iw}};

  placeholder<0> w_;
  gw(w_) << 1. / (w_ - 1);

  std::cout << get_tail(gw) << "\n";

  auto gt  = make_gf_from_inverse_fourier(gw);
  //auto gw2 = make_gf_from_fourier(gt);

  //std::cout << get_tail(gw2) << "\n";
}

TEST(Gf, Tail) { // NOLINT

  double beta = 1;

  auto g = gf<imfreq, scalar_valued>{{beta, Fermion, 20}};

  placeholder<0> w_;
  g(w_) << 1. / (w_ - 1);

  std::cout << get_tail(g) << "\n\n";

  //g.mesh().set_tail_parameters(0.4);
  //std::cout << get_tail(g) << "\n\n";

  auto known_moments = array<dcomplex, 3>(2);

  known_moments(0) = 0.0;
  known_moments(1) = 1.0;
  std::cout << get_tail(g, known_moments) << "\n\n";

  known_moments(0) = 1.0;
  known_moments(1) = 1.0;
  std::cout << get_tail(g, known_moments) << "\n\n";
}
MAKE_MAIN;
