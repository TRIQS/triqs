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

#include <numbers>
#include <fmt/core.h>
#include <h5/h5.hpp>

#include "../utils.hpp"
#include "../../utility/exceptions.hpp"
#include "../../utility/kronecker.hpp"

namespace triqs::mesh {

  /**
  * A matsubara frequency, i.e.
  *   * n : int, the index
  *   * beta : double, the temperature inverse
  *   * statistic : Fermion or Boson
  *
  * * Can be cast into a complex.
  *
  * * Every operation is done by casting to complex, except addition and substraction of matsubara_freq, which return matsubara_freq
  *   and work on the index
  **/
  struct matsubara_freq {
    long n                   = 0; // Matsubara Index
    double beta              = 0.0;
    statistic_enum statistic = Fermion;
    long index               = n;

    matsubara_freq() = default;
    matsubara_freq(long n_, double beta_, statistic_enum stat_) : n(n_), beta(beta_), statistic(stat_) {}

    using cast_t = std::complex<double>;
    operator cast_t() const { return std::complex<double>{0, std::numbers::pi * (2 * n + statistic) / beta}; }
  };

  inline std::ostream &operator<<(std::ostream &out, matsubara_freq const &y) { return out << std::complex<double>(y); }

  inline matsubara_freq operator+(matsubara_freq const &x, matsubara_freq const &y) {
    return {x.n + y.n + (x.statistic & y.statistic), x.beta, ((x.statistic ^ y.statistic) == 1 ? Fermion : Boson)};
  }

  inline matsubara_freq operator-(matsubara_freq const &x, matsubara_freq const &y) {
    return {x.n - y.n - (~x.statistic & y.statistic), x.beta, ((x.statistic ^ y.statistic) == 1 ? Fermion : Boson)};
  }

  inline matsubara_freq operator-(matsubara_freq const &mp) { return {-(mp.n + (mp.statistic == Fermion ? 1 : 0)), mp.beta, mp.statistic}; }

  inline std::complex<double> operator/(matsubara_freq const &x, matsubara_freq const &y) {
     return std::complex<double>{x} * std::complex<double>{y};
  }

  inline std::complex<double> operator*(matsubara_freq const &x, matsubara_freq const &y) {
    return std::complex<double>{x} * std::complex<double>{y};
  }

#define IMPL_OP(OP)                                                                                                                                  \
  template <typename T>                                                                                                                              \
    requires(not std::is_base_of_v<matsubara_freq, T>)                                                                                               \
  inline auto operator OP(matsubara_freq const &iw, T const &y) {                                                                                    \
    return dcomplex(iw) OP y;                                                                                                                        \
  }                                                                                                                                                  \
  template <typename T>                                                                                                                              \
    requires(not std::is_base_of_v<matsubara_freq, T>)                                                                                               \
  inline auto operator OP(T const &x, matsubara_freq const &iw) {                                                                                    \
    return x OP dcomplex(iw);                                                                                                                        \
  }
  IMPL_OP(+);
  IMPL_OP(-);
  IMPL_OP(*);
  IMPL_OP(/);
#undef IMPL_OP

  inline bool kronecker(matsubara_freq const &freq) { return freq.n == 0; }
  inline bool kronecker(matsubara_freq const &f1, matsubara_freq const &f2) { return f1.n == f2.n; }

  //---------------------------------------------------------------------------------------------------------
  // Domains

  struct matsubara_time_domain; // Forward Declaration

  struct [[deprecated("matsubara_freq_domain is deprecated")]] matsubara_freq_domain {
    using value_t = matsubara_freq;

    double beta              = 0.0;
    statistic_enum statistic = Fermion;

    [[nodiscard]] bool contains(value_t const &pt) const { return (pt.beta == beta) && (pt.statistic == statistic); }

    matsubara_freq_domain() = default;
    matsubara_freq_domain(double beta_, statistic_enum statistic_) : beta{beta_}, statistic(statistic_) {
      if (beta < 0) TRIQS_RUNTIME_ERROR << "Matsubara domain construction : beta < 0 : beta =" << beta << "\n";
    }
    explicit matsubara_freq_domain(matsubara_time_domain const &x);

    bool operator==(matsubara_freq_domain const &) const = default;
    bool operator!=(matsubara_freq_domain const &) const = default;

    [[nodiscard]] static std::string hdf5_format() { return "MatsubaraFreqDomain"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string const &subgroup_name, matsubara_freq_domain const &d) {
      h5::group gr = fg.create_group(subgroup_name);
      h5_write(gr, "beta", d.beta);
      h5_write(gr, "statistic", (d.statistic == Fermion) ? "F" : "B");
    }

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string const &subgroup_name, matsubara_freq_domain &d) {
      h5::group gr          = fg.open_group(subgroup_name);
      std::string statistic = " ";
      h5_read(gr, "beta", d.beta);
      h5_read(gr, "statistic", statistic);
      d.statistic = statistic == "F" ? Fermion : Boson;
    }

    friend std::ostream &operator<<(std::ostream &sout, matsubara_freq_domain const &d) {
      auto stat_cstr = (d.statistic == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("Matsubara frequency domain with beta = {}, statistic = {}", d.beta, stat_cstr);
    }
  };

  struct [[deprecated("matsubara_time_domain is deprecated")]] matsubara_time_domain {
    using value_t = double;

    double beta              = 0.0;
    statistic_enum statistic = Fermion;

    [[nodiscard]] bool contains(value_t const &pt) const { return (0 <= pt) && (pt <= beta); }

    [[nodiscard]] constexpr value_t min() const { return 0.0; }
    [[nodiscard]] value_t max() const { return beta; }

    matsubara_time_domain() = default;
    matsubara_time_domain(double beta_, statistic_enum statistic_) : beta{beta_}, statistic(statistic_) {
      if (beta < 0) TRIQS_RUNTIME_ERROR << "Matsubara domain construction : beta < 0 : beta =" << beta << "\n";
    }
    explicit matsubara_time_domain(matsubara_freq_domain const &x);

    bool operator==(matsubara_time_domain const &) const = default;
    bool operator!=(matsubara_time_domain const &) const = default;

    static std::string hdf5_format() { return "MatsubaraTimeDomain"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string const &subgroup_name, matsubara_time_domain const &d) {
      h5::group gr = fg.create_group(subgroup_name);
      h5_write(gr, "beta", d.beta);
      h5_write(gr, "statistic", (d.statistic == Fermion ? "F" : "B"));
    }

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string const &subgroup_name, matsubara_time_domain &d) {
      h5::group gr = fg.open_group(subgroup_name);
      double beta;
      std::string statistic{};
      h5_read(gr, "beta", beta);
      h5_read(gr, "statistic", statistic);
      d = matsubara_time_domain(beta, (statistic == "F" ? Fermion : Boson));
    }

    friend std::ostream &operator<<(std::ostream &sout, matsubara_time_domain const &d) {
      auto stat_cstr = (d.statistic == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("Matsubara time domain with beta = {}, statistic = {}", d.beta, stat_cstr);
    }
  };

  inline matsubara_freq_domain::matsubara_freq_domain(matsubara_time_domain const &x) : matsubara_freq_domain{x.beta, x.statistic} {};
  inline matsubara_time_domain::matsubara_time_domain(matsubara_freq_domain const &x) : matsubara_time_domain{x.beta, x.statistic} {};

} // namespace triqs::mesh
