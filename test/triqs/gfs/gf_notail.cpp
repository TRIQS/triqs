#include "./common.hpp"
#include <triqs/gfs.hpp>
#include <triqs/gfs/functions/functions.hpp>
using namespace triqs::gfs;
using namespace triqs::arrays;
namespace h5 = triqs::h5;

TEST(Gf, NoTail) {

 double beta = 1, a = 1;

 // construct
 auto gw_n = gf<imfreq, matrix_valued, no_tail>{{beta, Fermion}, {2, 2}};
 auto gt_n = gf<imtime, matrix_valued, no_tail>{{beta, Fermion, 10000}, {2, 2}};

 // make a view from a g with a tail
 auto gw = gf<imfreq, matrix_valued>{{beta, Fermion}, {2, 2}};
 auto gt = gf<imtime, matrix_valued>{gf_mesh<imtime>{beta, Fermion, 10000}, {2, 2}};

 auto vw = make_gf_view_without_tail(gw);
 auto vt = make_gf_view_without_tail(gt);

 triqs::clef::placeholder<0> tau_;
 // should not and does not compile : improve error message ???
 // gt(tau_) << exp(-a * tau_) / (1 + exp(-beta * a));
 vt(tau_) << exp(-a * tau_) / (1 + exp(-beta * a));

 // test hdf5
 // not ok for view
 //rw_h5(vt, "ess_g_notail.h5", "g");

 // rebuilding a new gf...
 auto g3 = make_gf_from_g_and_tail(vw, gw.singularity());
 // need to test all this....

 // test make_gw_view_from_g_and_tail
 auto g4 = gf<imfreq, matrix_valued, no_tail>{{beta, Fermion, 100}, {1, 1}};
 tail t2(1, 1);
 auto gwt = make_gf_view_from_g_and_tail(g4, t2);

 EXPECT_FALSE(g4.indices().is_empty());
 EXPECT_FALSE(gwt.indices().is_empty());

 // test antiperiodicity
 auto Gt = gf<imtime, scalar_valued, no_tail>{{beta, Fermion, 10000}, {}};
 Gt(tau_) << exp(-tau_);

 double pre = 1.e-5;
 EXPECT_NEAR_COMPLEX(Gt(0.01), 0.99005, pre);
 EXPECT_NEAR_COMPLEX(Gt(.5), 0.606531, pre);
 EXPECT_NEAR_COMPLEX(Gt(.9), 0.40657, pre);
 EXPECT_NEAR_COMPLEX(Gt(-.1), -0.40657, pre); // should be equal to line above

 // fourier
 gw_n(tau_) << 1 / (tau_ - 1.);
 auto gt_with_full_tail = make_gf_from_inverse_fourier(make_gf_from_g_and_tail(gw_n, gw.singularity()));

 tail t(2, 2);
 t(1) = 1;
 auto gt_tail_with_one_term = make_gf_from_inverse_fourier(make_gf_from_g_and_tail(gw_n, t));
}

//------------------------------------------------------

// bug : on regression
TEST(Gf, NoTailWithTail) {
 double beta = 1;
 auto g = gf<imfreq, matrix_valued, no_tail>{{beta, Fermion, 100}, {1, 1}};
 tail t(1, 1);
 auto gwt = make_gf_view_from_g_and_tail(g, t);
}


MAKE_MAIN;
