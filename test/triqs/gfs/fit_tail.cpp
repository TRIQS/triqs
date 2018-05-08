#include <triqs/test_tools/gfs.hpp>
#include <triqs/gfs/singularity/fit_tail.hpp>
using triqs::arrays::make_shape;

TEST(Gf, FitTailBasic) {

 triqs::clef::placeholder<0> iom_;
 double beta = 10;
 int N = 100;

 auto gw = gf<imfreq>{{beta, Fermion, N}, {1, 1}};
 auto gw_s = gf<imfreq, scalar_valued>{{beta, Fermion, N}, {}};

 triqs::arrays::array<dcomplex, 1> c{1, 3, 5, 7, 9};

 auto known_moments = __tail<matrix_valued>(make_shape(1,1)); // all moments are set to zero
 known_moments.reset(1); // first unkown moment is 1 (set moments >= 1 to NaN)
 auto known_moments_s = __tail<scalar_valued>(make_shape()); // all moments are set to zero
 known_moments_s.reset(1); // first unkown moment is 1 (set moments >= 1 to NaN)

 gw(iom_) << c(0) / iom_ + c(1) / iom_ / iom_ + c(2) / iom_ / iom_ / iom_;
 gw_s(iom_) << c(0) / iom_ + c(1) / iom_ / iom_ + c(2) / iom_ / iom_ / iom_;

 gw.singularity().data()() = 0.0;
 gw_s.singularity().data()() = 0.0;

 int wn_min = 50; // frequency to start the fit
 int wn_max = 90; // final fitting frequency (included)
 int max_moment = 3; // number of moments in the final tail (including known ones)

 // restore tail
 fit_tail(gw, known_moments, max_moment, wn_min, wn_max);
 fit_tail(gw_s, known_moments_s, max_moment, wn_min, wn_max);

 triqs::arrays::array<dcomplex, 1> res{0, 0, 1, 3, 5};
 EXPECT_ARRAY_NEAR(res, gw.singularity().data()(range(1, 6), 0, 0));

 // erase tail
 gw.singularity().data()() = 0.0;

 // now with a known moment
 known_moments.reset(2); // first unknow moment is 2
 known_moments(1) = 1.; // set the first moment
 fit_tail(gw, known_moments, max_moment, wn_min, wn_max, true); // true replace the gf data in the fitting range by the tail values

 EXPECT_ARRAY_NEAR(res, gw.singularity().data()(range(1, 6), 0, 0));
}

// ------------------------------------------------------------------------------

TEST(Gf, FitTailReal_F_and_B) {
 // real life test: find tails of 1/(iom -1)

 triqs::clef::placeholder<0> iom_;
 double beta = 10;
 int N = 100;

 auto gw = gf<imfreq>{{beta, Fermion, N}, {1, 1}};
 auto gw_b = gf<imfreq>{{beta, Boson, N-1}, {1, 1}};
 gw(iom_) << 1 / (iom_ - 1);
 gw_b(iom_) << 1 / (iom_ - 1);

 int wn_min = 50; 
 int wn_max = 90; 
 int max_moment = 4; 
 auto known_moments = __tail<matrix_valued>(make_shape(1, 1)); // all moments are set to zero
 known_moments.reset(2); // put NaN for moments >= 2
 known_moments(1) = 1.; // this one we know
 fit_tail(gw, known_moments, max_moment, wn_min, wn_max, true); 
 fit_tail(gw_b, known_moments, max_moment, wn_min, wn_max, true); 

 auto t_exact = __tail<matrix_valued>(make_shape(1, 1)); // all moments are set to zero
 t_exact.reset(5);
 t_exact.data()(range(3,7),0,0) = triqs::arrays::array<dcomplex, 1> {1.0, 1.0, 1.0, 1.0};

 EXPECT_TAIL_NEAR(t_exact, gw.singularity(), 2e-3);
 EXPECT_TAIL_NEAR(t_exact, gw_b.singularity(), 2e-3);
}


TEST(Gf, FitTailComplex) {

 // real life test: find tails of 1/(iom -1) -- with positive and negative matsubara
 triqs::clef::placeholder<0> iom_;
 double beta = 10;
 int N = 200;

 auto gw = gf<imfreq>{{beta, Fermion, N}, {1, 1}};
 auto a = dcomplex(1.0,0.4);
 gw(iom_) << 1 / (iom_ - a);

 int wn_min = 50; 
 int wn_max = 90;
 int max_moment = 4;
 auto known_moments = __tail<matrix_valued>(make_shape(1, 1));
 known_moments.reset(2);
 known_moments(1) = 1.;
 fit_tail(gw, known_moments, max_moment, -wn_max-1, -wn_min-1, wn_min, wn_max,  true); 

 auto t_exact = __tail<matrix_valued>(make_shape(1, 1));
 t_exact.reset(5);
 t_exact.data()(range(3,7),0,0) = triqs::arrays::array<dcomplex, 1> {dcomplex(1.0,0.0), a, std::pow(a,2), std::pow(a,3)};

 EXPECT_TAIL_NEAR(t_exact, gw.singularity(), 9e-3); 
}
MAKE_MAIN;

