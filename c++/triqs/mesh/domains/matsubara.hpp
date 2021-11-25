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

#include "../details/mesh_tools.hpp"
#include "../../utility/exceptions.hpp"

#include "../../utility/arithmetic_ops_by_cast.hpp"
#include "../../utility/kronecker.hpp"

namespace triqs::mesh {

  // a long with no cast, no operation
  struct _long {
    long value;
    _long(int i) : value{i} {}
    _long(long i) : value{i} {}
  };

  /**
  * A matsubara frequency, i.e.
  *   * n : int, the index
  *   * beta : double, the temperature inverse
  *   * statistic : Fermion or Boson
  *
  * * Can be casted into a complex.
  *
  * * Every operations is done by casting to complex, except addition and substraction of matsubara_freq, which return matsubara_freq
  *   and work on the index
  **/
  struct matsubara_freq : public utility::arithmetic_ops_by_cast_disable_same_type<matsubara_freq, std::complex<double>> {
    long n; // index
    long linear_index;
    std::complex<double> value;
    std::size_t mesh_hash;

    double beta;
    statistic_enum statistic;

    // operator std::complex<double>() const { return value; }

    matsubara_freq() : n(0), beta(1), statistic(Fermion) {}
    matsubara_freq(long n_, double beta_, statistic_enum stat_) : n(n_), beta(beta_), statistic(stat_) {}
    matsubara_freq(_long n_, double beta_, statistic_enum stat_) : n(n_.value), beta(beta_), statistic(stat_) {}
    using cast_t = std::complex<double>;
    operator cast_t() const { return {0, M_PI * (2 * n + statistic) / beta}; }
    operator _long() const { return {n}; }
  };

  inline std::ostream &operator<<(std::ostream &out, matsubara_freq const &y) { return out << std::complex<double>(y); }

  inline matsubara_freq operator+(matsubara_freq const &x, matsubara_freq const &y) {
    return {x.n + y.n + (x.statistic & y.statistic), x.beta, ((x.statistic ^ y.statistic) == 1 ? Fermion : Boson)};
  }

  inline matsubara_freq operator-(matsubara_freq const &x, matsubara_freq const &y) {
    return {x.n - y.n - (~x.statistic & y.statistic), x.beta, ((x.statistic ^ y.statistic) == 1 ? Fermion : Boson)};
  }

  inline matsubara_freq operator-(matsubara_freq const &mp) { return {-(mp.n + (mp.statistic == Fermion ? 1 : 0)), mp.beta, mp.statistic}; }

  inline std::complex<double> operator*(matsubara_freq const &x, matsubara_freq const &y) {
    return std::complex<double>{x} * std::complex<double>{y};
  }

  inline bool kronecker(matsubara_freq const &freq) { return freq.n == 0; }
  inline bool kronecker(matsubara_freq const &f1, matsubara_freq const &f2) { return f1.n == f2.n; }

  //---------------------------------------------------------------------------------------------------------
  // Domains

  struct matsubara_time_domain; // Advance Declaration

  struct matsubara_freq_domain {
    using point_t = std::complex<double>;

    double beta              = 0.0;
    statistic_enum statistic = Fermion;

    [[nodiscard]] bool is_in_domain(point_t const &pt) const {
      double n_guess   = pt.imag() / (2 * std::numbers::pi / beta);
      bool double_comp = (std::abs(n_guess - std::round(n_guess)) < 1.e-15); // TODO: Fix the double comparrison!!
      return (pt.real() == 0.0) && double_comp;
    }

    matsubara_freq_domain(double beta_, statistic_enum statistic_) : beta{beta_}, statistic(statistic_) {
      if (beta < 0) TRIQS_RUNTIME_ERROR << "Matsubara domain construction : beta < 0 : beta =" << beta << "\n";
    }

    matsubara_freq_domain() = default;
    explicit matsubara_freq_domain(matsubara_time_domain const &x);

    bool operator==(matsubara_freq_domain const &D) const = default;

    static std::string hdf5_format() { return "MatsubaraFreqDomain"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string const &subgroup_name, matsubara_freq_domain const &d) {
      h5::group gr = fg.create_group(subgroup_name);
      h5_write(gr, "beta", d.beta);
      h5_write(gr, "statistic", (d.statistic == Fermion ? "F" : "B"));
    }

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string const &subgroup_name, matsubara_freq_domain &d) {
      h5::group gr = fg.open_group(subgroup_name);
      std::string statistic{};
      h5_read(gr, "beta", d.beta);
      h5_read(gr, "statistic", statistic);
      d.statistic = "F" ? Fermion : Boson;
    }

    //  BOOST Serialization
    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, const unsigned int version) {
      ar &beta;
      ar &statistic;
    }

    friend std::ostream &operator<<(std::ostream &sout, matsubara_freq_domain const &d) {
      auto stat_cstr = (d.statistic == Boson ? "Boson" : "Fermion");
      return sout << fmt::format("Matsubara frequency domain with beta = {}, statistic = {}", d.beta, stat_cstr);
    }
  };

  struct matsubara_time_domain {
    using point_t = double;

    double beta              = 0.0;
    statistic_enum statistic = Fermion;

    [[nodiscard]] bool is_in_domain(point_t const &pt) const { return (pt <= beta) && (0.0 <= pt); }

    [[nodiscard]] constexpr point_t min() const { return 0.0; }
    [[nodiscard]] point_t max() const { return beta; }

    matsubara_time_domain() = default;
    matsubara_time_domain(double beta_, statistic_enum statistic_) : beta{beta_}, statistic(statistic_) {
      if (beta < 0) TRIQS_RUNTIME_ERROR << "Matsubara domain construction : beta < 0 : beta =" << beta << "\n";
    }

    explicit matsubara_time_domain(matsubara_freq_domain const &x);

    bool operator==(matsubara_time_domain const &D) const = default;

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

    //  BOOST Serialization
    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, const unsigned int version) {
      ar &beta;
      ar &statistic;
    }

    friend std::ostream &operator<<(std::ostream &sout, matsubara_time_domain const &d) {
      return sout << fmt::format("Matsubara time domain with beta = {}, statistic = {}", d.beta, d.statistic);
    }
  };

  inline matsubara_freq_domain::matsubara_freq_domain(matsubara_time_domain const &x) : matsubara_freq_domain{x.beta, x.statistic} {};
  inline matsubara_time_domain::matsubara_time_domain(matsubara_freq_domain const &x) : matsubara_time_domain{x.beta, x.statistic} {};

} // namespace triqs::mesh
