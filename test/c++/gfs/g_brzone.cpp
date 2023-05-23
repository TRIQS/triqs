#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs.hpp>

using namespace triqs::gfs;
using namespace triqs::clef;
using namespace triqs::arrays;
using namespace triqs::lattice;

double pi = M_PI;

double f(auto &k) { return -2 * (cos(k[0]) + cos(k[1])); }

TEST(Gfk, base) {

  int N = 10;
  //int S = 2;
  placeholder<0> k_;

  auto bz = brillouin_zone{bravais_lattice{nda::eye<double>(2)}};
  auto gk = gf<brzone, scalar_valued>{{bz, N}};

  gk(k_) << -2 * (cos(k_[0]) + cos(k_[1]));

  auto m  = gk.mesh();
  auto m2 = brzone{bz, 3 * N};

  EXPECT_FALSE(m.mesh_hash() == m2.mesh_hash());
  EXPECT_DEBUG_DEATH(gk[m2[1]], "mesh_hash.* violated");

  EXPECT_EQ(m[0].index(), (std::array<long, 3>{0, 0, 0}));
  EXPECT_EQ(m[1].index(), (std::array<long, 3>{0, 1, 0}));
  EXPECT_EQ(m2[1].index(), (std::array<long, 3>{0, 1, 0}));

  EXPECT_COMPLEX_NEAR(gk[m[0] + m[1]], gk[m[1]]);
  EXPECT_COMPLEX_NEAR(gk(m[0]), f(m[0].value()));
  EXPECT_COMPLEX_NEAR(gk(m[1]), f(m[1].value()));

  auto kexp = m[0] + m[1];
  EXPECT_EQ(kexp.index(), (std::array<long, 3>{0, 1, 0}));
  PRINT(make_regular(kexp.value() / pi));
  EXPECT_COMPLEX_NEAR(gk(kexp), (-2 * (cos(0) + cos(0.2 * M_PI))), 1.e-10);

  PRINT(gk[closest_mesh_pt(m[1] + m2[1])]);
}

// --------------------------

TEST(Gfk, loopkq) {

  int N = 10;
  //int S = 2;
  placeholder<0> k_;
  double tol = 1.e-13;

  auto bz = brillouin_zone{bravais_lattice{nda::eye<double>(2)}};
  auto g  = gf<brzone, scalar_valued>{{bz, N}};

  g(k_) << -2 * (cos(k_[0]) + cos(k_[1]));

  auto m  = g.mesh();
  auto m2 = brzone{bz, 3 * N};

  for (auto [k, q] : m *m) {
    { // MP +
      auto kq_idx = m.index_modulo(k.index() + q.index());
      EXPECT_COMPLEX_NEAR(g[kq_idx], g[k + q], tol);
    }

    { // MP -
      auto kq_idx = m.index_modulo(k.index() - q.index());
      EXPECT_COMPLEX_NEAR(g[kq_idx], g[k - q], tol);
    }

    { // value +
      auto kq_idx = m.index_modulo(k.index() + q.index());
      EXPECT_COMPLEX_NEAR(g[kq_idx], g(k + q), tol);
    }

    { // value -
      auto kq_idx = m.index_modulo(k.index() - q.index());
      EXPECT_COMPLEX_NEAR(g[kq_idx], g(k - q), tol);
    }
  }
}

MAKE_MAIN;
