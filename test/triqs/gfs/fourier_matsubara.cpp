#include <triqs/test_tools/gfs.hpp>

TEST(Gfs, FourierMatsubaraFermionMatrix){
 double precision=10e-9;
 triqs::clef::placeholder<0> om_;
 double beta =1;
 int Nw=10000;
 double E=1;

 auto Gw1 = gf<imfreq> {{beta, Fermion, Nw}, {2,2}};
 Gw1(om_) << 1/(om_-E);

 auto Gt1 = gf<imtime> {{beta, Fermion, 2*Nw+1}, {2,2}};
 Gt1() = inverse_fourier(Gw1);

 ///verification that TF(TF^-1)=Id
 auto Gw1b = gf<imfreq> {{beta, Fermion, Nw}, {2,2}};
 Gw1b() = fourier(Gt1);
 EXPECT_GF_NEAR(Gw1, Gw1b, precision);

 // Check against the exact result
 auto Gt1_check = Gt1;
 Gt1_check()=0.0;
 for(auto const & t:Gt1.mesh()){
  Gt1_check[t]= exp(-E*t) * ( (t>=0?-1:0)+1/(1+exp(E*beta)) );
 }
 EXPECT_ARRAY_NEAR(Gt1.data(), Gt1_check.data(), precision);//not comparing GF because of tails

 // Test the factory function
 auto Gt2 = make_gf_from_inverse_fourier(Gw1);
 auto Gw2b = make_gf_from_fourier(Gt2);
 EXPECT_GF_NEAR(Gt2, Gt1, precision);
 EXPECT_GF_NEAR(Gw2b, Gw1, precision);

 ///to verify that fourier computes (??)
 auto Gw2 = gf<imfreq> {{beta, Fermion}, {2,2}};
 Gw2() = fourier(Gt1);
}

TEST(Gfs, FourierMatsubaraFermionScalar){
 double precision=10e-9;
 triqs::clef::placeholder<0> om_;
 double beta =1;
 int Nw=10000;
 double E=1;

 auto Gw1 = gf<imfreq, scalar_valued> {{beta, Fermion, Nw}};
 Gw1(om_) << 1/(om_-E);

 auto Gt1 = gf<imtime, scalar_valued> {{beta, Fermion, 2*Nw+1}};
 Gt1() = inverse_fourier(Gw1);

 ///verification that TF(TF^-1)=Id
 auto Gw1b = gf<imfreq, scalar_valued> {{beta, Fermion, Nw}};
 Gw1b() = fourier(Gt1);
 EXPECT_GF_NEAR(Gw1, Gw1b, precision);

 // Check against the exact result
 auto Gt1_check = Gt1;
 Gt1_check()=0.0;
 for(auto const & t:Gt1.mesh()){
  Gt1_check[t]= exp(-E*t) * ( (t>=0?-1:0)+1/(1+exp(E*beta)) );
 }
 EXPECT_ARRAY_NEAR(Gt1.data(), Gt1_check.data(), precision);//not comparing GF because of tails

 // Test the factory function
 auto Gt2 = make_gf_from_inverse_fourier(Gw1);
 auto Gw2b = make_gf_from_fourier(Gt2);
 EXPECT_GF_NEAR(Gt2, Gt1, precision);
 EXPECT_GF_NEAR(Gw2b, Gw1, precision);

 ///to verify that fourier computes (??)
 auto Gw2 = gf<imfreq, scalar_valued> {{beta, Fermion}};
 Gw2() = fourier(Gt1);
}

TEST(Gfs, FourierMatsubaraBoson){
 double precision=10e-9;
 triqs::clef::placeholder<0> om_;
 double beta =1;
 int Nw=10000;
 double E=1;

 auto Gw1 = gf<imfreq> {{beta, Boson, Nw}, {2,2}};
 Gw1(om_) << 1/(om_-E) - 1/(om_+E);

 auto Gt1 = gf<imtime> {{beta, Boson, 2*Nw+1}, {2,2}};
 Gt1() = inverse_fourier(Gw1);

 ///verification that TF(TF^-1)=Id
 auto Gw1b = gf<imfreq> {{beta, Boson, Nw}, {2,2}};
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
 auto Gw1 = gf<imfreq> {{beta, Fermion, Nw, matsubara_mesh_opt::positive_frequencies_only}, {2,2}};
 Gw1(om_) << 1/(om_-E);
 auto Gt1 = gf<imtime> {{beta, Fermion, 2*Nw+1}, {2,2}};
 ASSERT_THROW(Gt1() = inverse_fourier(Gw1), triqs::runtime_error);
}
MAKE_MAIN;
