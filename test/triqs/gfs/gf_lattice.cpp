#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/gfs.hpp>

using index_t = utility::mini_vector<int, 3>;

TEST(Gfs, cyclic_lattice) {
  double beta = 1;
  triqs::clef::placeholder<0> r_;

  auto bl  = bravais_lattice{make_unit_matrix<double>(2)};
  auto m_r = gf_mesh<cyclic_lattice>{bl, 2};
  ASSERT_EQ(m_r.size(), 4);

  auto Gr = gf<cyclic_lattice>{m_r, {2, 2}};

  // Clef Assignment
  Gr(r_) << 1. + r_(0) + r_(1);
  EXPECT_COMPLEX_NEAR(Gr(index_t{0, 0, 0})(0, 0), 1);
  Gr(r_) << exp(-r_(0));
}

TEST(Gfs, brillouin_zone) {
  double beta = 1;
  triqs::clef::placeholder<0> k_;

  auto bz = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};
  auto m_k = gf_mesh<brillouin_zone>{bz, 4};
  ASSERT_EQ(m_k.size(), 16);

  auto Gk = gf<brillouin_zone>{m_k, {2, 2}};

  // Clef Assignment
  Gk(k_) << -2 * (cos(k_(0)) + cos(k_(1)));
  ASSERT_EQ(Gk.mesh().locate_neighbours(arrays::vector<double>{0, 0, 0}), (utility::mini_vector<long, 3>({0, 0, 0})));
  auto a = Gk(index_t{0, 0, 0});
  EXPECT_COMPLEX_NEAR(a(0, 0), -4);
}
MAKE_MAIN;
