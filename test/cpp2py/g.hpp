#include <triqs/gfs.hpp>
using namespace triqs::gfs;
using namespace triqs;

// ==== scalar gf ====

gf<imfreq, scalar_valued> make_gf(double a) {
  double beta = 1;
  auto g      = gf<imfreq, scalar_valued>{{beta, Fermion}};
  g()         = a;

  h5::file file("test_wrap_gf.h5", 'a');
  h5_write(file, "gout", g);

  return g;
}

void pass_gf(gf_view<imfreq, scalar_valued> g) {
  h5::file file("test_wrap_gf.h5", 'a');
  h5_write(file, "gin", g);
}

// ==== block gf ====

block_gf<imfreq> make_bgf(double a) {

  double beta = 1;
  auto g      = gf<imfreq>{{beta, Fermion}, {2, 2}};
  g()         = a;

  auto bg = make_block_gf<imfreq>(3, g);

  h5::file file("test_wrap_gf.h5", 'a');
  h5_write(file, "bgout", bg);

  return bg;
}

void pass_bgf(block_gf_view<imfreq> g) {

  h5::file file("test_wrap_gf.h5", 'a');
  h5_write(file, "bgin", g);
}

// ==== pair of gf ====

std::pair<gf<imfreq, scalar_valued>, gf<imfreq, scalar_valued>> make_pair_gf(double a) {
  double beta = 1;
  auto g1     = gf<imfreq, scalar_valued>{{beta, Fermion}};
  auto g2     = gf<imfreq, scalar_valued>{{beta, Fermion}};
  g1()        = a;
  g2()        = 2 * a;

  h5::file file("test_wrap_gf.h5", 'a');
  h5_write(file, "g1out", g1);
  h5_write(file, "g2out", g2);

  return {g1, g2};
}

void pass_two_gf(gf_view<imfreq, scalar_valued> g1, gf_view<imfreq, scalar_valued> g2) {
  h5::file file("test_wrap_gf.h5", 'a');
  h5_write(file, "g1in", g1);
  h5_write(file, "g2in", g2);
}
