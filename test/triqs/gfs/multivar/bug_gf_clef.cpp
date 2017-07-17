
#include <complex>
#include <triqs/test_tools/gfs.hpp>
using namespace triqs::clef;
using namespace triqs::lattice;

TEST(Gf, AutoAssignMatrixGf2) {

 double beta = 2.3;
 auto g2 = gf<cartesian_product<imfreq, imfreq>, matrix_valued>{{{beta, Fermion, 10}, {beta, Fermion, 10}}, {2, 2}};

 placeholder<0> i_;
 placeholder<1> j_;
 placeholder<3> om_;
 placeholder<4> nu_;

 g2(om_, nu_)(i_, j_) << i_ + j_ + 2 * om_ - 0.3 * nu_;

 // CHECK
 for (int i = 0; i < 2; ++i)
  for (int j = 0; j < 2; ++j)
   for (int om = 0; om < 10; ++om)
    for (int nu = 0; nu < 10; ++nu) {
     auto xom = ((2 * om + 1) * M_PI * 1_j / beta);
     auto xnu = ((2 * nu + 1) * M_PI * 1_j / beta);
     EXPECT_CLOSE(g2.data()(10+om, 10 +nu, i, j), i + j + 2 * xom - 0.3 * xnu);
    }
}

TEST(Gf, AutoAssignImagTime) {

 int ntau = 10;
 double beta = 2.3;
 auto g = gf<cartesian_product<imtime, imtime, imtime>, matrix_valued>{{
     {beta, Fermion, ntau}, {beta, Fermion, ntau}, {beta, Fermion, ntau}}, {1, 1}};

 placeholder_prime<0> t1;
 placeholder_prime<1> t2;
 placeholder_prime<3> t3;

 placeholder<4> a;
 placeholder<5> b;

 g() = 0.0;

 g(t1, t2, t3)(a, b) << a + b + 2*t1 - 0.3*t2 - 10.*t3;

 // CHECK
 for (auto const &t1 : std::get<0>(g.mesh())) {
  for (auto const &t2 : std::get<1>(g.mesh())) {
   for (auto const &t3 : std::get<2>(g.mesh())) {
    double ref_val = 2*t1 - 0.3*t2 - 10.*t3;
    double val = g[closest_mesh_pt(t1, t2, t3)](0,0).real();
    //std::cout << t1 << ", " << t2 << ", " << t3 << ", " << val << " -- " << ref_val << "\n";
    EXPECT_CLOSE(val, ref_val);
   }
  }
 }
}

TEST(Gf, AutoAssignAccumulateImagTime) {

 int ntau = 10;
 double beta = 2.3;
 auto g = gf<cartesian_product<imtime, imtime, imtime>, matrix_valued>{{
     {beta, Fermion, ntau}, {beta, Fermion, ntau}, {beta, Fermion, ntau}}, {1, 1}};

 placeholder_prime<0> t1;
 placeholder_prime<1> t2;
 placeholder_prime<3> t3;

 placeholder<4> a;
 placeholder<5> b;

 g() = 0.0;

 // We want to be able to accumulate to a three time gf using:
 // g << g + a*b, (instead of g += a*b)
 
 // This does not compile
 g(t1, t2, t3)(a, b) << g(t1, t2, t3)(a, b) + a + b + 2*t1 - 0.3*t2 - 10.*t3;

 // CHECK
 for (auto const &t1 : std::get<0>(g.mesh())) {
  for (auto const &t2 : std::get<1>(g.mesh())) {
   for (auto const &t3 : std::get<2>(g.mesh())) {
    double ref_val = 2*t1 - 0.3*t2 - 10.*t3;
    double val = g[closest_mesh_pt(t1, t2, t3)](0,0).real();
    //std::cout << t1 << ", " << t2 << ", " << t3 << ", " << val << " -- " << ref_val << "\n";
    EXPECT_CLOSE(val, ref_val);
   }
  }
 }
}

TEST(Gf, MixedIndexClefImFreq) {

 int nw = 10;
 double beta = 2.3;
 auto g = gf<cartesian_product<imfreq, imfreq, imfreq>, matrix_valued>{{
     {beta, Boson, nw}, {beta, Fermion, nw}, {beta, Fermion, nw}}, {1, 1}};

 placeholder<0> a;
 placeholder<1> b;

 g() = 0.0;

 placeholder<2> Omega;
 placeholder<3> n;
 placeholder<4> np;
 
 g(Omega, n, np)(a, b) << kronecker(n + np, Omega) * (a + b + 2*Omega - 0.2*n);
 
 // CHECK
 for (auto const &Omega : std::get<0>(g.mesh())) {
  for (auto const &n : std::get<1>(g.mesh())) {
   for (auto const &np : std::get<2>(g.mesh())) {
    std::complex<double> ref_val = 0.0;
    if(std::abs(std::complex<double>(n + np - Omega)) < 1e-6)
     ref_val = 2*Omega - 0.2*n;
    std::complex<double> val = g(Omega, n, np)(0,0);
    //std::cout << Omega << ", " << n << ", " << np << ", " << val << " -- " << ref_val << "\n";
    EXPECT_CLOSE(val, ref_val);
   }
  }
 }
}

TEST(Gf, MixedIndexClefAssignImFreq) {

 int nw = 10;
 double beta = 2.3;
 auto g = gf<cartesian_product<imfreq, imfreq, imfreq>, matrix_valued>{{
     {beta, Boson, nw}, {beta, Fermion, nw}, {beta, Fermion, nw}}, {1, 1}};

 g() = 0.0;

 auto Omega = std::get<0>(g.mesh())[4];
 auto n = std::get<1>(g.mesh())[5];

 placeholder<0> a;
 placeholder<1> b;

 g[{Omega, n, Omega - n}](a, b) << a + b + 2*Omega - 0.2*n; // Does not compile

 std::complex<double> ref_val = 2*Omega - 0.2*n;
 std::complex<double> val = g[{Omega, n, Omega - n}](0,0); // Does not compile
 
 EXPECT_CLOSE(val, ref_val);

}


MAKE_MAIN;

