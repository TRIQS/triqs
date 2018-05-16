#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>
#include <iostream>

using namespace triqs::gfs;
using namespace triqs::arrays;
using namespace triqs::utility;
using namespace triqs::clef;

namespace std {
} // namespace std

TEST(Gf, Fourier) { // NOLINT

  double beta = 10;
  int n_iw    = 100;

  auto gw = gf<imfreq, scalar_valued>{{beta, Fermion, n_iw}};

  placeholder<0> w_;
  gw(w_) << 1. / (w_ - 1);

  std::cout << fit_tail(gw) << "\n";

  auto gt  = make_gf_from_fourier(gw, 100 * n_iw);
  auto gw2 = make_gf_from_fourier(gt);

  std::cout << fit_tail(gw2) << "\n";
}

TEST(Gf, Tail) { // NOLINT

  double beta = 500;
  int n_iw = 3000;

  auto g = gf<imfreq, scalar_valued>{{beta, Fermion, n_iw}};

  placeholder<0> w_;
  g(w_) << (1. / (w_ - 5) + 1. / (w_ + 10)) / 2.0;

  std::cout << fit_tail(g) << "\n\n";

  auto known_moments = array<dcomplex, 1>(2);

  known_moments(0) = 0.0;
  known_moments(1) = 1.0;
  std::cout << fit_tail(g, known_moments) << "\n\n";
}
MAKE_MAIN;
