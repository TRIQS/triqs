#include <triqs/test_tools/gfs.hpp>
#include <nda/clef/adapters/math.hpp>
#include <nda/nda.hpp>
#include "triqs/utility/integration/adaptive.hpp"
#include "triqs/utility/integration/integrator.hpp"

namespace triqs::utility {

  TEST(integrators, adaptive_double_1d) { // NOLINT

    // simple test to check that adaptive and ptr integrator
    // correctly integrates a simple real function

    std::pair<double, double> D{0, M_PI / 4.};
    nda::clef::placeholder<0> x_;

    auto ex             = cos(x_);
    auto int_1d_adapt   = integrate_1d_adapt<std::complex<double>>{1e-8};
    auto integral_adapt = integrate(int_1d_adapt, ex, x_ = D);

    EXPECT_COMPLEX_NEAR(1. / std::sqrt(2.), integral_adapt, 1.e-4);
  }

  TEST(integrators, adaptive_dcomplex_1d) { // NOLINT

    // simple test to check that adaptive integrator
    // correctly integrates a simple complex function
    std::pair<double, double> D{0, 1};
    nda::clef::placeholder<0> x_;

    auto ex           = 1. / (x_ + 0.1i);
    auto int_1d_adapt = integrate_1d_adapt<dcomplex>{1e-6};
    dcomplex integral = integrate(int_1d_adapt, ex, x_ = D);

    EXPECT_COMPLEX_NEAR(dcomplex{2.30757, -1.47113}, integral, 1.e-5);
  }

  TEST(integrators, adaptive_dcomplex_2d) { // NOLINT

    // simple test to check that adaptive integrator
    // correctly integrates a simple 2d complex function

    std::pair<double, double> D{0, 1};
    nda::clef::placeholder<0> x_;
    nda::clef::placeholder<1> y_;

    dcomplex answer = {2.30757, -1.47113};
    answer *= answer;

    // adaptive 2d integration
    auto ex           = 1. / (x_ + 0.1i) * 1. / (y_ + 0.1i);
    auto int_1d_adapt = integrate_1d_adapt<dcomplex>{1e-8};
    dcomplex integral = integrate(int_1d_adapt, integrate(int_1d_adapt, ex, x_ = D), y_ = D);
    EXPECT_COMPLEX_NEAR(answer, integral, 1.e-3);
  }
} // namespace triqs::utility

MAKE_MAIN;
//MPI_TEST_MAIN;
