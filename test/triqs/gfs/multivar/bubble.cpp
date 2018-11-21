//#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/gfs.hpp>

using namespace triqs::clef;
using namespace triqs::lattice;

// UNTESTED

using two_var_gf = gf<cartesian_product<imfreq, imfreq>, scalar_valued>;
two_var_gf invert_as_matrix(two_var_gf const &g) {
  auto res = g;
  //  auto mat = make_matrix_view(group_indices_view(g.data(), {0}, {1}));
  auto mat                     = make_matrix_view(g.data());
  make_matrix_view(res.data()) = inverse(mat);
  return res;
}

int n_k     = 4; // 16; // too long, 30 s
int nw      = 3;
double beta = 20, mu = 0.;

placeholder<0> k_;
placeholder<1> q_;
placeholder<2> r_;
placeholder<3> iw_;
placeholder<4> inu_;

auto _ = all_t{};

auto bz     = brillouin_zone{bravais_lattice{{{1, 0}, {0, 1}}}};
auto eps_k_ = -2 * (cos(k_(0)) + cos(k_(1)));

// ------------------------------------------------------------

TEST(Gf, Bubble) {
  auto chi0q = gf<cartesian_product<imfreq, imfreq, brillouin_zone>>{{{beta, Fermion, nw}, {beta, Boson, nw}, {bz, n_k}}, {1, 1}};
  auto chi0r = gf<cartesian_product<imfreq, imfreq, cyclic_lattice>>{{{beta, Fermion, nw}, {beta, Boson, nw}, {n_k, n_k}}, {1, 1}};

  auto chi0q_from_r = chi0q;

  auto Gk = gf<cartesian_product<imfreq, brillouin_zone>, matrix_valued>{{{beta, Fermion, nw}, {bz, n_k}}, {1, 1}};
  auto Gr = gf<cartesian_product<imfreq, cyclic_lattice>, matrix_valued>{{{beta, Fermion, nw}, {n_k, n_k}}, {1, 1}};

  Gk(inu_, k_) << 1 / (inu_ + mu - eps_k_);

  auto k_mesh = std::get<1>(Gk.mesh());

  chi0q(inu_, iw_, q_) << sum(Gk(inu_, k_) * Gk(inu_ + iw_, k_ + q_), k_ = k_mesh) / k_mesh.size();

  for (auto const &inu : std::get<0>(Gr.mesh())) Gr[inu, _] = fourier(Gk[inu, _]);

  chi0r(inu_, iw_, r_) << Gr(inu_, r_) * Gr(inu_ + iw_, -r_);

  for (auto const &inu : std::get<0>(chi0q_from_r.mesh())) {
    for (auto const &iw : std::get<1>(chi0q_from_r.mesh())) {

      EXPECT_EQ((chi0q_from_r[inu, iw, _]).mesh().size(), 16);
      EXPECT_EQ((chi0r[inu, iw, _]).mesh().size(), 16);

      chi0q_from_r[inu, iw, _] = fourier(chi0r[inu, iw, _]);
    }
  }
  EXPECT_ARRAY_NEAR(chi0q_from_r.data(), chi0q.data());

  // hdf5
  rw_h5(chi0q, "chi0q");
  rw_h5(chi0r, "chi0r");
}

// ------------------------------------------------------------

TEST(Gf, BubbleScalar) {

  auto chi0q = gf<cartesian_product<imfreq, imfreq, brillouin_zone>, scalar_valued>{{{beta, Fermion, nw}, {beta, Boson, nw}, {bz, n_k}}};
  auto chi0r = gf<cartesian_product<imfreq, imfreq, cyclic_lattice>, scalar_valued>{{{beta, Fermion, nw}, {beta, Boson, nw}, {n_k, n_k}}};

  auto chi0q_from_r = chi0q;

  auto Gk = gf<cartesian_product<imfreq, brillouin_zone>, scalar_valued>{{{beta, Fermion, nw}, {bz, n_k}}};
  auto Gr = gf<cartesian_product<imfreq, cyclic_lattice>, scalar_valued>{{{beta, Fermion, nw}, {n_k, n_k}}};

  Gk(inu_, k_) << 1 / (inu_ + mu - eps_k_);

  auto Gmesh = std::get<1>(Gk.mesh());
  chi0q(inu_, iw_, q_) << sum(Gk(inu_, k_) * Gk(inu_ + iw_, k_ + q_), k_ = Gmesh) / Gmesh.size();

  for (auto const &inu : std::get<0>(Gr.mesh())) Gr[inu, _] = fourier(Gk[inu, _]);

  chi0r(inu_, iw_, r_) << Gr(inu_, r_) * Gr(inu_ + iw_, -r_);

  for (auto const &inu : std::get<0>(chi0q_from_r.mesh())) {
    for (auto const &iw : std::get<1>(chi0q_from_r.mesh())) { chi0q_from_r[inu, iw, _] = fourier(chi0r[inu, iw, _]); }
  }
  EXPECT_ARRAY_NEAR(chi0q_from_r.data(), chi0q.data());

  rw_h5(chi0q, "chi0q");
  rw_h5(chi0r, "chi0r");
}

// ------------------------------------------------------------

TEST(Gf, BubbleSimplif) {
  // simplified, without frequency dependence
  auto gk         = gf<brillouin_zone>{{bz, n_k}, {1, 1}};
  auto gr         = gf<cyclic_lattice>{{n_k, n_k}, {1, 1}};
  auto ggr        = gr;
  auto ggq        = gk;
  auto ggq_from_r = gk;

  gk(k_) << 1. / (M_PI / beta * 1_j - eps_k_);
  ggq(q_) << sum(gk(k_) * gk(k_ + q_), k_ = gk.mesh()) / gk.mesh().size();

  gr() = fourier(gk);
  ggr(r_) << gr(r_) * gr(r_);
  ggq_from_r() = fourier(ggr);

  EXPECT_ARRAY_NEAR(ggq_from_r.data(), ggq.data()); //<<"fourier pb";
}

// ------------------------------------------------------------

TEST(Gf, BubbleSimplifScalar) {
  // simplified, without frequency dependence
  auto gk         = gf<brillouin_zone, scalar_valued>{{bz, n_k}};
  auto gr         = gf<cyclic_lattice, scalar_valued>{{n_k, n_k}, {}};
  auto ggr        = gr;
  auto ggq        = gk;
  auto ggq_from_r = gk;

  gk(k_) << 1. / (M_PI / beta * 1_j - eps_k_);
  ggq(q_) << sum(gk(k_) * gk(k_ + q_), k_ = gk.mesh()) / gk.mesh().size();

  gr() = fourier(gk);
  ggr(r_) << gr(r_) * gr(r_);
  ggq_from_r() = fourier(ggr);

  EXPECT_ARRAY_NEAR(ggq_from_r.data(), ggq.data()); //<<"fourier pb";
}

MAKE_MAIN;
