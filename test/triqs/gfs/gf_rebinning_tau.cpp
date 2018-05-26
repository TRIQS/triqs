#include <triqs/test_tools/gfs.hpp>

TEST(Gf, RebinTau) {

 static_assert(is_block_gf_or_view<block_gf<imfreq>>::value, "oops");

 double beta = 1;

 auto gt1 = gf<imtime>{{beta, Fermion, 9}, {1, 1}};
 auto gt2 = gf<imtime>{{beta, Fermion, 10}, {1, 1}};

 triqs::clef::placeholder_prime<0> tau_;
 gt1(tau_) << -0.25 - 0.5 * tau_;
 gt2(tau_) << -0.25 - 0.5 * (beta - tau_);

 auto gt1_new = rebinning_tau(gt1, 3);
 auto gt2_new = rebinning_tau(gt2, 3);

 auto res1 = array<dcomplex, 1>{-0.28125, -0.46875, -0.6875};
 auto res2 = array<dcomplex, 1>{-0.6944444444444, -0.5, -0.305555555555};

 EXPECT_ARRAY_NEAR( res1, gt1_new.data()(range(),0,0));
 EXPECT_ARRAY_NEAR( res2, gt2_new.data()(range(),0,0), 1.);

 // Apply rebinning_tau to all blocks in a full GF
 auto bgf = make_block_gf({"up", "dn"}, {gt1, gt2});
 auto rebin = [](gf_const_view<imtime> const& g) { return rebinning_tau(g, 3); };
 auto res = map(rebin, bgf);

 EXPECT_EQ(res.block_names(), (std::vector<std::string>{"up", "dn"}));
 EXPECT_GF_NEAR(gt1_new, res[0]);
 EXPECT_GF_NEAR(gt2_new, res[1]);

}
MAKE_MAIN;

