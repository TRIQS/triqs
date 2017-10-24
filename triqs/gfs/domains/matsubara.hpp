/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once
#include "../../utility/arithmetic_ops_by_cast.hpp"
#include "../../utility/kronecker.hpp"

namespace triqs::gfs {

  // a long with no cast, no operation
  struct _long {
    long value;
    _long(int i) : value{i} {}
    _long(long i) : value{i} {}
  };

  /** 
  * A small variant type for the complex frequency.
  *
  * It can be in 2 different states : 
  *
  *  - 0) a matsubara frequency : in that state, n() is the index (omega = i \pi/\beta (2n() + 1))
  *  - 1) a point at the circle of radius R : n() is such that omega = R e^{2i \pi n()}
  *  - 2) a generic point in the complex plane : n() throws.
  *
  * It has the algebra of complex number, however when possible (i.e. under addition, substraction of 2 matsubara freq)
  * it computes the index rather than as dcomplex.
  * Otherwiser it becomes a generic complex point under all other operations.
  *
  * It implicitly casts into a dcomplex.
  *
  * Data : 
  *  * In state 0) : 
  *   * n : int, the index
  *   * beta : double, the temperature inverse
  *   * statistic : Fermion or Boson
  *
  *  * In state 1) :
  *   * n : long, the index on the circle
  *   * R : the radius of the circle
  *
  *  * In state 2) :
  *   * omega : the dcomplex value
  *
  **/
  class complex_freq {
    long _n                  = 0;
    statistic_enum statistic = Boson;
    uint8_t status           = 2; // 0 : matsubara frequency, 1 : on circle, 2 : elsewhere in the complex plane
    union uni_t {
      double beta;
      double R;
      dcomplex omega;
      uni_t() { omega = 0; } // default
    } uni;

    public:
    // construct at omega =0, as a generic point in the complex plane.
    complex_freq() = default;

    // construct at a generic point in the complex plane.
    complex_freq(dcomplex omega) : uni.omega(std::move(omega)), status(2) {}

    // construct at a matsubara freq
    complex_freq(long n, double beta, statistic_enum stat) : _n(n), uni.beta(beta), statistic(stat), status(0) {}

    // construct at a matsubara freq
    complex_freq(_long n, double beta, statistic_enum stat_) : complex_freq(n.value, beta, stat) {}

    // construct on the circle
    complex_freq(long n, double R) : _n(n), uni.R(R), status(1) {}

    // construct on the circle
    complex_freq(_long n, double beta) : complex_freq(n.value, beta) {}

    // implicit cast to dcomplex:
    operator dcomplex() const {
      switch (status) {
        case (0): return {0, M_PI * (2 * n + statistic) / beta};
        case (1):
          return R * std::exp(dcomplex{0, 2 * M_PI * idx.value}); //FIXME Cache this exp ?
        default: return uni.omega;
      }
    }

    // access to indices
    long n() const {
      if (status != 0) TRIQS_RUNTIME_ERROR << "The complex freq is not a matsubara freq. Can not take its index : " << status;
      return _n;
    }

    // other acccessor to beta, R, stat if needed ??

    // it can be a long. Throws it is not a matsubara freq.
    operator _long() const { return {n()}; }

    friend complex_freq operator+(complex_freq const &, complex_freq const &);
    friend complex_freq operator-(complex_freq const &, complex_freq const &);
    friend complex_freq operator-(complex_freq const &);
  };

  inline std::ostream &operator<<(std::ostream &out, complex_freq const &y) { return out << dcomplex(y); }

  inline complex_freq operator+(complex_freq const &x, complex_freq const &y) {
    if (x.status == 0 and y.status == 0)
      return {x.n + y.n + (x.statistic & y.statistic), x.uni.beta, ((x.statistic ^ y.statistic) == 1 ? Fermion : Boson)};
    return {dcomplex{x} + dcomplex{y}};
  }

  inline complex_freq operator-(complex_freq const &x, complex_freq const &y) {
    if (x.status == 0 and y.status == 0)
      return {x.n - y.n - (~x.statistic & y.statistic), x.uni.beta, ((x.statistic ^ y.statistic) == 1 ? Fermion : Boson)};
    return {dcomplex{x} - dcomplex{y}};
  }

  inline complex_freq operator-(complex_freq const &mp) {
    if (mp.status == 0)
      return {-(mp.n + (mp.statistic == Fermion ? 1 : 0)), mp.uni.beta, mp.statistic};
    else
      return -dcomplex{mp};
  }

  inline complex_freq operator*(complex_freq const &x, complex_freq const &y) { return dcomplex{x} * dcomplex{y}; }

  inline complex_freq operator/(complex_freq const &x, complex_freq const &y) { return dcomplex{x} / dcomplex{y}; }

  template <typename T> complex_freq operator+(T const &x, complex_freq const &y) { return x + dcomplex{y}; }
  template <typename T> complex_freq operator+(complex_freq const &x, T const &y) { return dcomplex{x} + y; }

  template <typename T> complex_freq operator-(T const &x, complex_freq const &y) { return x - dcomplex{y}; }
  template <typename T> complex_freq operator-(complex_freq const &x, T const &y) { return dcomplex{x} - y; }

  template <typename T> complex_freq operator*(T const &x, complex_freq const &y) { return x * dcomplex{y}; }
  template <typename T> complex_freq operator*(complex_freq const &x, T const &y) { return dcomplex{x} * y; }

  template <typename T> complex_freq operator/(T const &x, complex_freq const &y) { return x / dcomplex{y}; }
  template <typename T> complex_freq operator/(complex_freq const &x, T const &y) { return dcomplex{x} / y; }

  //---------------------------------------------------------------------------------------------------------

  template <bool B> void h5_write(h5::group fg, std::string subgroup_name, matsubara_domain<B> const &d);
  template <bool B> void h5_read(h5::group fg, std::string subgroup_name, matsubara_domain<B> &d);

  /// The domain
  template <bool IsFreq> struct matsubara_domain {
    using point_t = typename std::conditional<IsFreq, std::complex<double>, double>::type;
    double beta;
    statistic_enum statistic;
    matsubara_domain(double beta, statistic_enum s) : beta(beta), statistic(s) {
      if (beta < 0) TRIQS_RUNTIME_ERROR << "Matsubara domain construction :  beta <0 : beta =" << beta << "\n";
    }
    matsubara_domain() : matsubara_domain(1, Fermion) {}
    matsubara_domain(matsubara_domain const &) = default;
    matsubara_domain(matsubara_domain<!IsFreq> const &x) : matsubara_domain(x.beta, x.statistic) {}
    bool operator==(matsubara_domain const &D) const { return ((std::abs(beta - D.beta) < 1.e-15) && (statistic == D.statistic)); }
    bool operator!=(matsubara_domain const &x) const { return !(operator==(x)); }

    //  BOOST Serialization
    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, const unsigned int version) {
      ar &TRIQS_MAKE_NVP("beta", beta);
      ar &TRIQS_MAKE_NVP("statistic", statistic);
    }
  };

  template <bool B> std::ostream &operator<<(std::ostream &sout, matsubara_domain<B> const &d) {
    return sout << "Matsubara domain with beta = " << d.beta << ", statistic = " << d.statistic;
  }

  /// Write into HDF5
  template <bool B> void h5_write(h5::group fg, std::string subgroup_name, matsubara_domain<B> const &d) {
    h5::group gr = fg.create_group(subgroup_name);
    h5_write(gr, "beta", d.beta);
    h5_write(gr, "statistic", (d.statistic == Fermion ? "F" : "B"));
  }

  /// Read from HDF5
  template <bool B> void h5_read(h5::group fg, std::string subgroup_name, matsubara_domain<B> &d) {
    h5::group gr = fg.open_group(subgroup_name);
    double beta;
    std::string statistic = " ";
    h5_read(gr, "beta", beta);
    h5_read(gr, "statistic", statistic);
    d = matsubara_domain(beta, (statistic == "F" ? Fermion : Boson));
  }

  using matsubara_freq_domain = matsubara_domain<true>;
  using matsubara_time_domain = matsubara_domain<false>;

  // ----- kronecker function : overload for matsubara_freq
  //inline bool kronecker(matsubara_freq const & freq) { return freq.n == 0; }
  //inline bool kronecker(matsubara_freq const & f1, matsubara_freq const &f2) { return f1.n == f2.n; }
} // namespace triqs::gfs
