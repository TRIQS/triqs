#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/gfs.hpp>

using namespace triqs::clef;


TEST(Gf, G_r) {
 double beta = 1;

 auto bl = bravais_lattice{make_unit_matrix<double>(2)};

 auto m_r = gf_mesh<cyclic_lattice>{2, 2};
 ASSERT_EQ(m_r.size(), 4);

 auto g_r = gf<cyclic_lattice>{m_r, {1, 1}};

 // try to assign to expression
 placeholder<0> r_;
 g_r(r_) << 1;
 using index_t = utility::mini_vector<int,3>;
 EXPECT_NEAR_COMPLEX(g_r(index_t{0,0,0})(0,0), 1);
 g_r(r_) << exp(-r_(0));
 //std::cout << g_r.data() << std::endl;
}

TEST(Gf, G_k) {
 double beta = 1;
 auto bz = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};
 auto g_k = gf<brillouin_zone>{{bz, 4}, {1, 1}};

 placeholder<0> k_;
 g_k(k_) << -2 * (cos(k_(0)) + cos(k_(1)));

 using index_t = utility::mini_vector<int,3>;
 ASSERT_EQ( g_k.mesh().size(), 16);
 ASSERT_EQ(g_k.mesh().locate_neighbours(arrays::vector<double>{0,0,0}) , (utility::mini_vector<long,3>( {0,0,0})));
 auto a = g_k(index_t{0,0,0});
 EXPECT_NEAR_COMPLEX(a(0,0), -4);
}

TEST(Gf, lattice_FT) {
 double beta = 1;
 int n_k=2;

 auto bl = bravais_lattice{make_unit_matrix<double>(2)};
 auto bz = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};
 auto m_r = gf_mesh<cyclic_lattice>{n_k, n_k};
 auto g_r = gf<cyclic_lattice, matrix_valued>{m_r, {1, 1}};
 placeholder<0> r_;
 g_r(r_) << exp(-r_(0));
 auto g_k = gf<brillouin_zone, matrix_valued>{{bz,n_k}, {1, 1}};

 g_k() = fourier(g_r);
 auto g_r_2 = gf<cyclic_lattice, matrix_valued>{m_r, {1, 1}};
 g_r_2() = inverse_fourier(g_k);
 EXPECT_ARRAY_NEAR(g_r.data(), g_r_2.data());
}
TEST(Gf, lattice_FT_throw) {
 double beta = 1;
 int n_k=2;

 auto bl = bravais_lattice{make_unit_matrix<double>(2)};
 auto bz = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};
 auto per_mat = matrix<int>{{{2, 1, 0},{-1, 2, 0},{0, 0,1}}};
 auto m_r = gf_mesh<cyclic_lattice>{bl, per_mat};
 auto g_r = gf<cyclic_lattice, matrix_valued>{m_r, {1, 1}};
 placeholder<0> r_;
 g_r(r_) << exp(-r_(0));
 auto g_k = gf<brillouin_zone, matrix_valued>{{bz,per_mat}, {1, 1}};

 EXPECT_THROW(g_k() = fourier(g_r), triqs::runtime_error);
}
MAKE_MAIN;
