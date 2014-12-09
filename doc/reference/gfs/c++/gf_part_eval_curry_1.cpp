#include <triqs/gfs.hpp>
using namespace triqs::gfs;
using triqs::clef::placeholder;
int main() {
 double beta = 1, wmin = 0, wmax = 1.0;
 int n_re_freq = 100, n_im_freq = 100;

 using g_w_wn_s = gf<cartesian_product<refreq, imfreq>, scalar_valued>;

 // a scalar valued function
 auto g = g_w_wn_s{{{wmin, wmax, n_re_freq}, {beta, Fermion, n_im_freq}}};

 // put expression in it
 triqs::clef::placeholder<0> w_;
 triqs::clef::placeholder<1> wn_;
 g(w_, wn_) << 1 / (wn_ - 1) / (w_ + 3_j);

 // Currying by selecting the first variable, i.e. t -> tau -> g(t,tau)
 auto g1 = curry<0>(g);

 // Currying by selecting the second variable, i.e. tau -> t -> g(t,tau)
 auto g2 = curry<1>(g);

 // std::cout << g << g1[1]<< g2 [2] << std::endl;
 std::cout << g1[1][2] << g2[2][1] << g[{1, 2}] << std::endl;
}

