#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>

namespace h5 = triqs::h5;
using namespace triqs::gfs;
using namespace triqs::clef;
using namespace triqs::arrays;
using namespace triqs::lattice;

TEST(Gf, r_k) {

  int N = 10;
  int S = 2;
  placeholder<0> k_;

  auto bz = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};
  auto gk = gf<brillouin_zone>{{bz, N}, {S, S}};

  gk(k_) << -2 * (cos(k_(0)) + cos(k_(1)));

  auto gr = gf<cyclic_lattice>{{N, N}, {S, S}};

  gr() = fourier(gk); // ADD Security if gf =

  for (auto r : gr.mesh())
    if (max_element(abs(gr[r])) > 1.e-10) std::cerr << r << gr[r] << std::endl;

  // reverse transfo
  auto gk2 = gk;
  gk2()    = fourier(gr);
  EXPECT_ARRAY_NEAR(gk.data(), gk2.data());

  // check gr
  auto gr_test        = gf<cyclic_lattice>{{N, N}, {S, S}};
  gr_test[{1, 0, 0}]  = -1;
  gr_test[{-1, 0, 0}] = -1;
  gr_test[{0, 1, 0}]  = -1;
  gr_test[{0, -1, 0}] = -1;

  EXPECT_ARRAY_NEAR(gr.data(), gr_test.data());

  rw_h5(gk);
  rw_h5(gr);
}
MAKE_MAIN;
