#include <triqs/test_tools/gfs.hpp>

TEST(Gf, PartialEval) {

 double beta   = 1.;
 double tmin   = 0.;
 double tmax   = 1.0;
 int n_re_time = 10;
 int n_im_time = 10;
 double wmin   = 0.;
 double wmax   = 1.0;
 int n_re_freq = 10;
 int n_im_freq = 10;

 triqs::clef::placeholder<0> w_;
 triqs::clef::placeholder<1> wn_;
 triqs::clef::placeholder<2> tau_;

 auto G_w     = gf<refreq, scalar_valued>{{wmin, wmax, n_re_freq}};
 auto G_t_tau = gf<cartesian_product<retime, imtime>, scalar_valued>{{{tmin, tmax, n_re_time}, {beta, Fermion, n_im_time}}};
 auto G_w_wn  = gf<cartesian_product<refreq, imfreq>, scalar_valued>{{{wmin, wmax, n_re_freq}, {beta, Fermion, n_im_freq}}};
 auto G_w_tau = gf<cartesian_product<refreq, imtime>, scalar_valued>{{{wmin, wmax, n_re_freq}, {beta, Fermion, n_im_time}}};

 G_w_wn(w_, wn_) << 1 / (wn_ - 1) / (w_ * w_ * w_);
 G_w_tau(w_, tau_) << exp(-2 * tau_) / (w_ * w_ + 1);

 int index  = n_re_freq / 3;
 double tau = std::get<1>(G_w_tau.mesh().components())[index];

 G_w(w_) << exp(-2 * tau) / (w_ * w_ + 1);

 auto G_w_wn_sl0_a = G_w_wn[8, all_t()];
 auto G_w_wn_sl0_b = G_w_wn[all_t(), 3];
 
 static_assert(std::is_same<std14::remove_reference_t<decltype(G_w_wn_sl0_a.mesh())>, const gf_mesh<imfreq>>::value, "oops");

 EXPECT_CLOSE(G_w_wn.on_mesh(8, 3), G_w_wn_sl0_a[3]);
 EXPECT_CLOSE(G_w_wn.on_mesh(8, 3), G_w_wn_sl0_b[8]);

 rw_h5(G_t_tau);
 rw_h5(G_w_wn);
 rw_h5(G_w_tau);

}

MAKE_MAIN;
