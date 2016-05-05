#include <triqs/test_tools/gfs.hpp>

using namespace triqs::gfs;

TEST(Gf, SimpleAssign) {

 using A = triqs::arrays::array_proxy<triqs::arrays::array<std::complex<double>, 3, void> &, long>;
 
 static_assert(std::is_constructible<std::complex<double>, matsubara_freq>::value, "oops");
 static_assert(triqs::arrays::is_scalar_or_convertible<matsubara_freq>::value, "oops2");
 static_assert(triqs::arrays::is_scalar_for<std::complex<double>, A>::value, "oops2");
 static_assert(triqs::arrays::is_scalar_for<matsubara_freq, A>::value, "oops2");

 triqs::clef::placeholder<0> om_;
 auto g = gf<imfreq>{{10, Fermion, 10}, {2, 2}};
 auto g2 = g;
 auto g3 = g;

 g(om_) << om_ + 0.0;
 g2(om_) << om_;
 g3(om_) << om_ + om_;

 EXPECT_ARRAY_NEAR(g.data(), g2.data());
 EXPECT_ARRAY_NEAR(g.data(), g3.data() / 2);

auto g4 = gf<imfreq, tensor_valued<3>>{};
auto g5 = gf<imfreq, tensor_valued<3>>{{10,Fermion},{3,3,3}};

rw_h5(g5);
}
MAKE_MAIN;
