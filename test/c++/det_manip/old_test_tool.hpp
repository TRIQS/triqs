#include <nda/nda.hpp>

namespace nda {

  template <class T> inline double assert_abs(T z) { return std::abs(z); }

  template <class ArrayType1, class ArrayType2>
  void assert_all_close(ArrayType1 const &A, ArrayType2 const &B, double precision, bool relative = false) {
    typedef typename ArrayType1::value_type F;
    auto Abs = map(std::function<double(F)>(assert_abs<F>));
    auto r   = max_element(Abs(A - B));
    auto r2  = max_element(Abs(A) + Abs(B));
    if (r > (relative ? precision * r2 : precision))
      TRIQS_RUNTIME_ERROR << "assert_all_close error : \n\n"
                          << ".. A = " << A << "\n\n"
                          << ".. B= " << B << "\n\n"
                          << ".. Residue is r = " << r;
  }

  template <typename T1, typename T2> void assert_equal(T1 const &x1, T2 const &x2) {
    if (!(x1 == x2)) TRIQS_RUNTIME_ERROR << "Test assert_equal : failed";
  }

  void assert_close(double x1, double x2) {
    if (std::abs(x1 - x2) > 1.e-13) TRIQS_RUNTIME_ERROR << "Test assert_equal : failed";
  }
  void assert_close(std::complex<double> x1, std::complex<double> x2) {
    if (std::abs(x1 - x2) > 1.e-13) TRIQS_RUNTIME_ERROR << "Test assert_equal : failed";
  }

  // Put here a macro, to print the expression, the line, etc...
  void assert_is_true(bool b) {
    if (!b) TRIQS_RUNTIME_ERROR << "Test assert_is_true : failed";
  }

  void assert_is_false(bool b) {
    if (b) TRIQS_RUNTIME_ERROR << "Test assert_is_false : failed";
  }
} // namespace nda
