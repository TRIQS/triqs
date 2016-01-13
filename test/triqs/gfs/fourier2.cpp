#include <triqs/test_tools/gfs.hpp>

TEST(Gfs, FourierMatsubaraFermion){
 double precision=10e-9;
 triqs::clef::placeholder<0> om_;
 double beta =1;
 int Nw=10000;
 double E=1;
 
 auto Gw1 = gf<imfreq> {{beta, Fermion, Nw}, {1,1}};
 Gw1(om_) << 1/(om_-E);
 
 auto Gt1 = gf<imtime> {{beta, Fermion, 2*Nw+1}, {1,1}};
 Gt1() = inverse_fourier(Gw1);
 
 ///verification that TF(TF^-1)=Id
 auto Gw1b = gf<imfreq> {{beta, Fermion, Nw}, {1,1}};
 Gw1b() = fourier(Gt1);

 EXPECT_GF_NEAR(Gw1, Gw1b, precision);

 auto Gt1_check = Gt1;
 Gt1_check()=0.0; 
 ///verification that the TF is OK
 for(auto const & t:Gt1.mesh()){
  Gt1_check[t]= exp(-E*t) * ( (t>=0?-1:0)+1/(1+exp(E*beta)) );
 }
 EXPECT_ARRAY_NEAR(Gt1.data(), Gt1_check.data(), precision);//not comparing GF because of tails
 
 ///to verify that fourier computes (??)
 auto Gw2 = gf<imfreq> {{beta, Fermion}, {1,1}};
 Gw2() = fourier(Gt1);
}

TEST(Gfs, FourierMatsubaraBoson){
 double precision=10e-9;
 triqs::clef::placeholder<0> om_;
 double beta =1;
 int Nw=10000;
 double E=1;
 
 auto Gw1 = gf<imfreq> {{beta, Boson, Nw}, {1,1}};
 Gw1(om_) << 1/(om_-E) - 1/(om_+E);
 
 auto Gt1 = gf<imtime> {{beta, Boson, 2*Nw+1}, {1,1}};
 Gt1() = inverse_fourier(Gw1);
 
 ///verification that TF(TF^-1)=Id
 auto Gw1b = gf<imfreq> {{beta, Boson, Nw}, {1,1}};
 Gw1b() = fourier(Gt1);

 EXPECT_GF_NEAR(Gw1, Gw1b, precision);
}

///check Fourier on positive-only freqs fails
TEST(Gfs, FourierMatsubaraAllFreq){
 double precision=10e-9;
 triqs::clef::placeholder<0> om_;
 double beta =1;
 int Nw=10000;
 double E=1;
 auto Gw1 = gf<imfreq> {{beta, Fermion, Nw, matsubara_mesh_opt::positive_frequencies_only}, {1,1}};
 Gw1(om_) << 1/(om_-E);
 auto Gt1 = gf<imtime> {{beta, Fermion, 2*Nw+1}, {1,1}};
 ASSERT_THROW(Gt1() = inverse_fourier(Gw1), triqs::runtime_error);
}
MAKE_MAIN;


