#ifndef TRIQS_UTILITY_COMPLEX_OPS_H
#define TRIQS_UTILITY_COMPLEX_OPS_H
#include <complex>

inline std::complex<double> operator"" _j(long double x) { return std::complex<double>(0, x); }
inline std::complex<double> operator"" _j(unsigned long long x) { return std::complex<double>(0, x); }

namespace std { // has to be in the right namespace for ADL !

  template <typename T> std::complex<T> operator+(std::complex<T> const &a, long b) { return a + T(b); }
  template <typename T> std::complex<T> operator+(long a, std::complex<T> const &b) { return T(a) + b; }
  template <typename T> std::complex<T> operator-(std::complex<T> const &a, long b) { return a - T(b); }
  template <typename T> std::complex<T> operator-(long a, std::complex<T> const &b) { return T(a) - b; }
  template <typename T> std::complex<T> operator*(std::complex<T> const &a, long b) { return a * T(b); }
  template <typename T> std::complex<T> operator*(long a, std::complex<T> const &b) { return T(a) * b; }
  template <typename T> std::complex<T> operator/(std::complex<T> const &a, long b) { return a / T(b); }
  template <typename T> std::complex<T> operator/(long a, std::complex<T> const &b) { return T(a) / b; }
} // namespace std

#endif
