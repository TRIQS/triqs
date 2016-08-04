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

 int size = 0; // means we don't know any moments
 int order_min = 1; // means that the first moment in the final tail will be the first moment
 auto known_moments = __tail<matrix_valued>(1,1, size, order_min); // length is 0, first moment to fit is order_min
 auto known_moments_s = __tail<scalar_valued>(size, order_min); // length is 0, first moment to fit is order_min

 gw(iom_) << c(0) / iom_ + c(1) / iom_ / iom_ + c(2) / iom_ / iom_ / iom_;
 gw_s(iom_) << c(0) / iom_ + c(1) / iom_ / iom_ + c(2) / iom_ / iom_ / iom_;

 gw.singularity().data()() = 0.0;
 gw_s.singularity().data()() = 0.0;

 int wn_min = 50; // frequency to start the fit
 int wn_max = 90; // final fitting frequency (included)
 int n_moments = 3; // number of moments in the final tail (including known ones)

 // restore tail
 fit_tail(gw, known_moments, n_moments, wn_min, wn_max);
 fit_tail(gw_s, known_moments_s, n_moments, wn_min, wn_max);

 triqs::arrays::array<dcomplex, 1> res{0, 0, 1, 3, 5};
 EXPECT_ARRAY_NEAR(res, gw.singularity().data()(range(1, 6), 0, 0));

 // erase tail
 gw.singularity().data()() = 0.0;

 // now with a known moment
 size = 1; // means that we know one moment
 order_min = 1; // means that the first moment in the final tail will be the first moment
 known_moments = __tail<matrix_valued>(1, 1, size, order_min); // length is 0, first moment to fit is order_min
 known_moments(1) = 1.; // set the first moment
 fit_tail(gw, known_moments, n_moments, wn_min, wn_max, true); // true replace the gf data in the fitting range by the tail values

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
 int n_moments = 4; 
 int size = 1; 
 int order_min = 1;
 auto known_moments = __tail<matrix_valued>(1, 1, size, order_min);
 known_moments(1) = 1.; 
 fit_tail(gw, known_moments, n_moments, wn_min, wn_max, true); 
 fit_tail(gw_b, known_moments, n_moments, wn_min, wn_max, true); 

 tail t(make_shape(1,1),4,-1);
 t.data()(range(1,7),0,0) =triqs::arrays::array<dcomplex, 1> {0.0, 0.0, 1.0, 1.0, 0.999251, 0.998655};

 EXPECT_TAIL_NEAR(t, gw.singularity());
 t.data()(range(1,7),0,0) =triqs::arrays::array<dcomplex, 1> {0.0, 0.0, 1.0, 1.0, 0.999236, 0.998631};
 EXPECT_TAIL_NEAR(t, gw_b.singularity());
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
 int n_moments = 4; 
 auto known_moments = __tail<matrix_valued>(1, 1, 1, 1);
 known_moments(1) = 1.; 
 fit_tail(gw, known_moments, n_moments, -wn_max, -wn_min, wn_min, wn_max,  true); 

 tail t(make_shape(1,1),-2,4);
 t.data()(range(0,7),0,0) = triqs::arrays::array<dcomplex, 1> {dcomplex(0.0,0.0),dcomplex(0.0,0.0), dcomplex(0.0,0.0), dcomplex(1.0,0.0), a, std::pow(a,2), std::pow(a,3)};

 EXPECT_TAIL_NEAR(t, gw.singularity(), 8e-3); 
}
MAKE_MAIN;

