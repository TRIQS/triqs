#include <triqs/test_tools/gfs.hpp>

using namespace triqs::clef;

TEST(Gf, k) {
 auto bz = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};
 auto t = __tail<matrix_valued>{1, 1};
 auto G = m_tail<brillouin_zone, matrix_valued>{{bz, 100}, {1, 1}};

 placeholder<0> k_;
 G(k_) << -2 * (cos(k_(0)) + cos(k_(1))) * t;

 rw_h5(G, "ess_g_k_tail.h5", "g");
}
MAKE_MAIN;

