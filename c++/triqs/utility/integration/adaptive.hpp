#pragma once
#include <nda/clef/placeholder.hpp>
#include <nda/clef/utils.hpp>
#include <nda/nda.hpp>

using dcomplex = std::complex<double>;

namespace triqs::utility {

  // set up for the recursive call
  template <typename T> class integrate_1d_adapt {

    mutable double tolerance = 1e-6; // TODO set a minimum possible tolerance
    // abscissas for GLK quadrature
    double alpha = sqrt(2.0 / 3.0);
    double beta  = 1.0 / sqrt(5.0);
    double x1    = 0.942882415695480;
    double x2    = 0.641853342345781;
    double x3    = 0.236383199662150;

    mutable std::vector<double> all_x_midpoints;
    mutable std::vector<T> all_int;

    // store the evaluations for the 13 pt formula
    static constexpr std::array<double, 4> coefficients = {11. / 210., 432.0 / 1470.0, 125. / 294., 16. / 35.};
    // 13 pt Konrod / 4pt Gauss abscissa
    std::vector<double> x_init = {-x1, -alpha, -x2, -beta, -x3, 0.0, x3, beta, x2, alpha, x1};

    public:
    /**
      * @brief Simple constructor for 1d adaptive integrator on 0,1 domain
      *
      * @param tolerance the tolerance to stop the adaptive integration
      *
    */
    integrate_1d_adapt(double tolerance = 1e-6) : tolerance{tolerance} {}

    auto operator()(auto f, const std::pair<double, double> &domain) const
      requires(nda::clef::is_lazy<decltype(f)>)
    {
      return nda::clef::make_expr_call(*this, std::move(f), domain);
    }

    /**
      * @brief Perform an adaptive integration in 1d
      *
      * @param f function to be integrated
      * @param domain a pair of start and end points for the domain
      *
      * @return value of the integral
      */
    auto operator()(auto const &f, const std::pair<double, double> &domain) const
      requires(not nda::clef::is_lazy<decltype(f)>)
    {
      // denominator of the function
      auto h = 0.5 * (domain.second - domain.first);
      // midpoint -- to shift from domain to zero-centered domain
      auto m = 0.5 * (domain.first + domain.second);

      // fill in all the function evaluations
      std::vector<T> fx(11);
      for (int i = 0; i < 11; ++i) fx[i] = f(m + x_init[i] * h);

      T fa = f(domain.first);
      T fb = f(domain.second);

      // 13 pt Kronrod -- done once for error estimate
      T is = h
         * (0.0158271919734802 * (fa + fb) + 0.0942738402188500 * (fx[0] + fx[10]) + 0.155071987336585 * (fx[1] + fx[9])
            + 0.188821573960182 * (fx[2] + fx[8]) + 0.199773405226859 * (fx[3] + fx[7]) + 0.224926465333340 * (fx[4] + fx[6])
            + 0.242611071901408 * fx[5]);

      // Could set up a relative error and expose to users -- for now we use only absolute for simplicity
      //double error1   = abs(i1 - is);
      //double error2   = abs(i2 - is);
      //double relError = 0;
      //relError = (error2 != 0.0) ? error1/error2 : 1.0;
      //if (error2 == 0) {
      //  relError = 1.0;
      //} else {
      //  relError = error1 / error2;
      //}

      // if i1 is very small, increase the tolerance?
      //tolerance = (relError > 0.0 && relError < 1.0) ? tolerance / relError : tolerance;

      // scalar or elementwise check for a zero value of is
      if constexpr (nda::Scalar<T>) {
        if (std::abs(is) == 0.0) { is = domain.second - domain.first; }
      } else {
        nda::for_each(is.shape(), [&is, &domain](auto... args) {
          if (std::abs(is(args...)) == 0) is(args...) = domain.second - domain.first;
        });
      }
      // start the recursive call
      return adaptive_recursion(f, domain.first, domain.second, fx[0], fx[10], abs(is));
    }

    private:
    /**
      * @brief Perform successive recursive adaptive integration calls
      *
      * @param f function to be integrated
      * @param xa start of the domain of integration
      * @param xb end of the domain of integration
      * @param fa value of function at domain start point
      * @param fb value of function at domain end point
      * @param is 13 pt Kronrod integration for estimate of error
      *
      * @return value of the integral
      */
    T adaptive_recursion(auto &f, const double xa, const double xb, const T fa, const T fb, const T is) const {

      // step spacing
      double h = 0.5 * (xb - xa);
      // midpoint -- to shift from domain to zero-centered domain
      double midpoint = 0.5 * (xa + xb);

      // array of x and fx points on the segment
      // 2 pts left and right of midpoint at sqrt(2/3) and 1/sqrt(5)
      std::array<double, 5> x; //NOLINT
      std::array<T, 5> fx;
      x[0] = midpoint - alpha * h;
      x[1] = midpoint - beta * h;
      x[2] = midpoint;
      x[3] = midpoint + beta * h;
      x[4] = midpoint + alpha * h;

      // 5 pt function evaluations (+ endpts provided to the function )
      for (int i = 0; i < 5; ++i) fx[i] = f(x[i]);

      // 4 pt Gauss-Lobatto
      T i2 = (1. / 6. * h) * (fa + fb + 5.0 * (fx[1] + fx[3]));
      // 7 pt Kronrod
      T i1 = h * (coefficients[0] * (fa + fb) + coefficients[1] * (fx[0] + fx[4]) + coefficients[2] * (fx[1] + fx[3]) + coefficients[3] * fx[2]);

      // if the two are close in value, end the iteration
      bool tolerance_met = false;
      if constexpr (nda::Scalar<T>) {
        tolerance_met = std::abs(i1 - i2) <= std::abs(tolerance * is);
      } else {
        tolerance_met = nda::max_element(nda::abs(i1 - i2)) <= nda::max_element(nda::abs(tolerance * is));
      }
      if (tolerance_met || x[0] <= xa || xb <= x[4]) {
        if ((midpoint <= xa || xb <= midpoint)) {
          std::cerr << "Cannot subdivide integration domain further; however, desired acccuracy was not reached." << std::endl;
        }
        return i1; // recursive base case
      }

      return adaptive_recursion(f, xa, x[0], fa, fx[0], is) + adaptive_recursion(f, x[0], x[1], fx[0], fx[1], is)
         + adaptive_recursion(f, x[1], x[2], fx[1], fx[2], is) + adaptive_recursion(f, x[2], x[3], fx[2], fx[3], is)
         + adaptive_recursion(f, x[3], x[4], fx[3], fx[4], is) + adaptive_recursion(f, x[4], xb, fx[4], fb, is);
    }
  };

} // namespace triqs::utility
