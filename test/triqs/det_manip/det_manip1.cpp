#include <triqs/det_manip/det_manip.hpp>
#include <triqs/mc_tools/random_generator.hpp>
#include <triqs/arrays/linalg/det_and_inverse.hpp>
#include <triqs/arrays/asserts.hpp>
#include <iostream>

struct fun {

  typedef double result_type;
  typedef double argument_type;

  /*double f(double eps, double tau, double beta) const {
   bool s = (tau>0);
   tau = (s ? tau : beta + tau);
   return (s ? 1 : -1 ) * exp( - eps* (tau)) / (1 + exp(- beta *eps));
   }*/

  double operator()(double x, double y) const {
    const double pi   = acos(-1);
    const double beta = 10.0;
    const double epsi = 0.1;
    double tau        = x - y;
    bool s            = (tau > 0);
    tau               = (s ? tau : beta + tau);
    double r          = epsi + tau / beta * (1 - 2 * epsi);
    return -2 * (pi / beta) / std::sin(pi * r);

    //return - 0.5*(f(0.12, x-y, beta) + f(0.43,x-y,beta) ); //+ f(-0.7,x-y,beta));
  }
};

template <class T1, class T2> void assert_close(T1 const &A, T2 const &B, double precision) {
  if (std::abs(A - B) > precision) TRIQS_RUNTIME_ERROR << "assert_close error : " << A << "\n" << B;
}
const double PRECISION = 1.e-6;

struct test {

  fun f;
  triqs::det_manip::det_manip<fun> D;
  double det_old, detratio;

  test() : f(), D(f, 100) {}

  void check() {
#ifndef PRINT_ALL
    std::cerr << "det = " << D.determinant() << " == " << double(determinant(D.matrix())) << std::endl;
#else
    std::cerr << "det = " << D.determinant() << " == " << double(determinant(D.matrix())) << std::endl
              << D.inverse_matrix() << D.matrix() << triqs::arrays::matrix<double>(inverse(D.matrix())) << std::endl;
    std::cerr << "det_old = " << det_old << "detratio = " << detratio << " determin " << D.determinant() << std::endl;
#endif
    assert_close(D.determinant(), 1 / determinant(D.inverse_matrix()), PRECISION);
    triqs::arrays::assert_all_close(inverse(D.matrix()), D.inverse_matrix(), PRECISION, true);
    assert_close(det_old * detratio, D.determinant(), PRECISION);
  }

  void run() {
    triqs::mc_tools::random_generator RNG("mt19937", 23432);
    for (size_t i = 0; i < 100; ++i) {
      std::cerr << " ------------------------------------------------" << std::endl;
      std::cerr << " i = " << i << " size = " << D.size() << std::endl;
      // choose a move
      size_t s = D.size();
      size_t i0, j0, i1, j1;
      det_old  = D.determinant();
      detratio = 1;
      double x, y, x1, y1;
      bool do_something = true;

      switch (RNG((i > 10 ? 4 : 1))) {
        case 0:
          x = RNG(10.0), y = RNG(10.0);
          std::cerr << " x,y = " << x << "  " << y << std::endl;
          detratio = D.try_insert(RNG(s), RNG(s), x, y);
          break;
        case 1:
          if (s > 0) detratio = D.try_remove(RNG(s), RNG(s));
          break;
        case 2:
          std::cerr << " Insert2" << std::endl;
          x  = RNG(10.0);
          x1 = RNG(10.0);
          y  = RNG(10.0);
          y1 = RNG(10.0);
          i0 = RNG(s);
          i1 = RNG(s + 1);
          j0 = RNG(s);
          j1 = RNG(s + 1);
          if ((i0 != i1) && (j0 != j1)) {
            detratio = D.try_insert2(i0, i1, j0, j1, x, x1, y, y1);
          } else
            do_something = false;
          break;
        case 3:
          std::cerr << " Remove2" << std::endl;
          if (D.size() >= 2) {
            i0 = RNG(s);
            i1 = RNG(s);
            j0 = RNG(s);
            j1 = RNG(s);
            if ((i0 != i1) && (j0 != j1)) {
              detratio = D.try_remove2(i0, i1, j0, j1);
            } else
              do_something = false;
          }
          break;
        default: TRIQS_RUNTIME_ERROR << " TEST INTERNAL ERROR";
      };

      if (do_something) {
        if (std::abs(detratio * det_old) > PRECISION) {
          D.complete_operation();
          if (D.size() > 0) check();
        } else {
          std::cerr << " reject  since new det is = " << std::abs(detratio * det_old) << std::endl;
          D.reject_last_try();
        }
      }
    }
  }
};

int main(int argc, char **argv) { test().run(); }
