#include <triqs/test_tools/gfs.hpp>

TEST(Gf, Real) { 

 double beta =1;
 double tmax=10;
 double wmax=10;
 double it0=0.3352*beta;
 double t0=0.3352*tmax;
 double w0=0.3352*wmax;
 int N=1000;
double precision=10e-12;
 
 auto Gw = gf<refreq> {{-wmax, wmax, N},{2,2}};
 auto Gt = gf<retime> {{0, tmax, N}, {2,2}};
 auto Gw2 = gf<refreq,scalar_valued> {{-wmax, wmax, N}};
 auto Gt2 = gf<retime,scalar_valued> {{0, tmax, N}};
 auto Giw = gf<imfreq> {{beta, Fermion,N}, {2,2}};
 auto Git = gf<imtime> {{beta, Fermion,N},  {2,2}};
 auto Giw2 = gf<imfreq,scalar_valued> {{beta, Fermion, N}};
 auto Git2 = gf<imtime,scalar_valued> {{beta, Fermion, N}};

 rw_h5(Gw);
 rw_h5(Gt);
 rw_h5(Git);
 rw_h5(Giw);

 for (auto & t : Gt.mesh()) Gt[t] = 1.0*t;
 for (auto & w : Gw.mesh()) Gw[w] = 1.0*w;
 for (auto & it : Git.mesh()) Git[it] = 1.0*it;
 for (auto & iw : Giw.mesh()) Giw[iw] = 1.0*iw;
 
 triqs::clef::placeholder<0> t_;
 triqs::clef::placeholder<1> w_;
 Gt2( t_) << 1.0*t_;
 Gw2( w_) << 1.0*w_;
 Git2( t_) << 1.0*t_;
 Giw2( w_) << 1.0*w_;
 
 EXPECT_CLOSE( Gt(t0)(0,0), t0);
 EXPECT_CLOSE(Gt2(t0), t0);

 EXPECT_CLOSE(Gw(w0)(0, 0), w0);
 EXPECT_CLOSE(Gw2(w0), w0);
 EXPECT_CLOSE(Git(it0)(0, 0), it0);
 EXPECT_CLOSE(Git2(it0),it0 );

 //verification of windowing for imaginary times outside from [0,beta]
 EXPECT_CLOSE(Gw2.on_mesh(N / 3), Gw2[N / 3]);
 EXPECT_CLOSE(Gt2.on_mesh(N / 3), Gt2[N / 3]);
 EXPECT_CLOSE(Git2.on_mesh(N / 3), Git2[N / 3]);

}
MAKE_MAIN;
