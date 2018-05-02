#define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
#include <triqs/test_tools/gfs.hpp>
using namespace triqs::clef;
using namespace triqs::lattice;
using triqs::clef::placeholder;
using triqs::utility::mindex;

template <typename Function, typename Mesh> auto sum_gf(Function const &f, Mesh const &m) {
 auto res = make_matrix(0 * f(*(m.begin())));
 for (auto const &x : m) //
  res = res + f(x);
 return res;
}

namespace triqs::clef {
  TRIQS_CLEF_MAKE_FNT_LAZY(sum_gf);
}

placeholder<0> k_;
placeholder<1> w_;
placeholder_prime <0> wp_;
using gf_bz_imfreq_mat = gf<cartesian_product<brillouin_zone, imfreq>, matrix_valued>;
using gf_bz_imtime_mat = gf<cartesian_product<brillouin_zone, imtime>, matrix_valued>;

auto _ = var_t{};

// ------------------------------------------------------------


// FIXME : Does not test much ...
TEST(Gf, GkOm) {

 double beta = 1;
 int n_bz    = 20;

 auto bz    = brillouin_zone{bravais_lattice{{{1, 0}, {0, 1}}}};
 auto g_eps = gf<brillouin_zone>{{bz, n_bz}, {1, 1}};

 auto G = gf_bz_imfreq_mat{{{bz, n_bz}, {beta, Fermion, 100}}, {1, 1}};
 auto Sigma = gf<imfreq, matrix_valued>{ {beta, Fermion, 100}, {1,1}};
 Sigma() =0;

 auto eps_k = -2 * (cos(k_(0)) + cos(k_(1)));
 
 G(k_, w_) << 1 / (w_ - eps_k - 1 / (w_ + 2));
 
 // broken with wp -> w 
 //G[k_, w_] <<  G(k_, w_) ;//1 / (w_ - eps_k - 1 / (w_ + 2));
 
 
 G[k_, w_] <<  G[k_, w_]  + 1 / (w_ - eps_k - 1 / (w_ + 2));

 G[k_, wp_] << 2* G[k_, wp_];
 
 G[k_, w_] << 2*Sigma(w_);
 
 G[k_, w_] << 2*Sigma[w_];

 G[k_, w_] << 1 / (w_ - eps_k - 1 / (w_ + 2));

 rw_h5(G, "ess_g_k_om.h5", "g");
}


MAKE_MAIN;
