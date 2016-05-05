#include <triqs/test_tools/gfs.hpp>

TEST(Gf, Curry1) {

 double beta = 1.;
 double tmin = 0.;
 double tmax = 1.0;
 int n_re_time = 100;
 int n_im_time = 100;

 double wmin = 0.;
 double wmax = 1.0;
 int n_re_freq = 100;
 int n_im_freq = 100;

 triqs::clef::placeholder<0> w_;
 triqs::clef::placeholder<1> wn_;
 triqs::clef::placeholder<2> tau_;

 auto G_w = gf<refreq, scalar_valued>{{wmin, wmax, n_re_freq}};

 auto G_t_tau = gf<cartesian_product<retime, imtime>, scalar_valued>{
     {gf_mesh<retime>(tmin, tmax, n_re_time), gf_mesh<imtime>(beta, Fermion, n_im_time)}};

 auto G_w_wn = gf<cartesian_product<refreq, imfreq>, scalar_valued>{
     {gf_mesh<refreq>(wmin, wmax, n_re_freq), gf_mesh<imfreq>(beta, Fermion, n_im_freq)}};

 auto G_w_tau = gf<cartesian_product<refreq, imtime>, scalar_valued>{
     {gf_mesh<refreq>(wmin, wmax, n_re_freq), gf_mesh<imtime>(beta, Fermion, n_im_time)}};

 auto G_w_wn_sl0_a = partial_eval<0>(G_w_wn(), 8);

 G_w_wn(w_, wn_) << 1 / (wn_ - 1) / (w_*w_*w_);
 G_w_tau(w_, tau_) << exp(-2 * tau_) / (w_ * w_ + 1);

 int index = n_re_freq / 3;
 double tau = std::get<1>(G_w_tau.mesh().components())[index];

 G_w(w_) << exp(-2 * tau) / (w_ * w_ + 1);
 G_w.singularity().data()() = 0;
 // the singularity must be removed as it is inexistent in re_im_time, to give the same TF.

 dcomplex ref = G_w_wn.on_mesh(8, 3);

 auto G_w_wn_curry0 = curry<0>(G_w_wn);
 auto G_w_wn_curry1 = curry<1>(G_w_wn);
 auto G_w_wn_view2 = G_w_wn();

 EXPECT_CLOSE(ref, G_w_wn_curry0[8][3]);
 EXPECT_CLOSE(ref, G_w_wn_curry1[3][8]);
 EXPECT_CLOSE(ref, G_w_wn_view2.on_mesh(8, 3));
 EXPECT_CLOSE(ref, G_w_wn_sl0_a[3]);

 rw_h5(G_t_tau);
 rw_h5(G_w_wn);
 rw_h5(G_w_tau);

 static_assert(std::is_same<std14::remove_reference_t<decltype(G_w_wn_sl0_a.mesh())>, const gf_mesh<imfreq>>::value, "oops");
 static_assert(std::is_same<std14::remove_reference_t<decltype(G_w_wn_curry0[0].mesh())>, const gf_mesh<imfreq>>::value, "oops");
 static_assert(std::is_same<std14::remove_reference_t<decltype(G_w_wn_curry0.mesh())>, const gf_mesh<refreq>>::value, "oops");
}
MAKE_MAIN;

