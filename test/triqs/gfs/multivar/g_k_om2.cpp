#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/gfs.hpp>

// FAIRE make_value !!
//
using namespace triqs::clef;

template <typename Function, typename Mesh> auto sum_gf(Function const &f, Mesh const &m) {
 auto res = make_matrix(0 * f(*(m.begin())));
 for (auto const &x : m) res = res + f(x);
 return res;
}

namespace triqs {
 namespace clef {
  TRIQS_CLEF_MAKE_FNT_LAZY(sum_gf);
 }
}
TEST(Gf, Experimental1) {
 double beta = 1;
 int n_k=8;

 using gf_bz_imfreq_mat_tail = gf<cartesian_product<brillouin_zone, imfreq>, matrix_valued>;
 auto bz = brillouin_zone{bravais_lattice{make_unit_matrix<double>(2)}};

 auto g_eps = gf<brillouin_zone>{{bz, n_k}, {1, 1}};

 auto G_k_iom = gf_bz_imfreq_mat_tail{{{bz, n_k}, {beta, Fermion, 100}}, {1, 1}};
 ASSERT_EQ(std::get<0>(G_k_iom.mesh().components()).size(), n_k*n_k);
 auto G_k_iom_2 = gf_bz_imfreq_mat_tail{{{bz, n_k}, {beta, Fermion, 100}}, {1, 1}};

 // try to assign to expression
 placeholder<0> k_;
 placeholder<1> w_;

 auto eps_k = -2 * (cos(k_(0)) + cos(k_(1)));
 G_k_iom(k_, w_) << 1 / (w_ - eps_k);
 
 for(auto const & k : std::get<0>(G_k_iom.mesh().components()))
 for(auto const & w : std::get<1>(G_k_iom.mesh().components()))
  G_k_iom_2[{k, w}] = 1 / (dcomplex(w) + 2*(cos(k[0])+cos(k[1])));

 EXPECT_ARRAY_NEAR(G_k_iom.data(),G_k_iom_2.data());
  
 
 auto G_loc = gf<imfreq, matrix_valued>{{beta, Fermion, 100}, {1, 1}};
 
 //auto r = G_k_iom(k_t{0, 0, 0}, matsubara_freq{0, beta, Fermion});
 auto r = G_k_iom(utility::mini_vector<int,3>{0, 0, 0}, matsubara_freq{0, beta, Fermion});
 auto r5 = sum_gf(k_ >> G_k_iom(k_, 0), g_eps.mesh());

 placeholder_prime<0> wp_;
 G_loc(wp_) << sum_gf(k_ >> G_k_iom(k_, wp_), g_eps.mesh());

 auto G_k_tau = gf<cartesian_product<brillouin_zone, imtime>, matrix_valued>{
     {{bz,  n_k}, {beta, Fermion, 201}}, {1, 1}};

 // auto r3 = partial_eval<0>(G_k_iom,0);
 // auto r4 = partial_eval<0>(G_k_tau,0);
 // auto gt = curry<0>(G_k_tau) [0];
 // auto  gw = curry<0>(G_k_iom)[0];

  curry<0>(G_k_tau) [k_] << inverse_fourier(curry<0>(G_k_iom)[k_]);

 // TEST(G_k_tau[{0,0}]);

 // hdf5
 // h5::file file("ess_g_k_om.h5", H5F_ACC_TRUNC );
 // h5_write(file, "g", G);
}
MAKE_MAIN;
