// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Michel Ferrero, Laura Messio, Olivier Parcollet, Nils Wentzell, tayral

#pragma once
#include "../../utility/arithmetic_ops_by_cast.hpp"
#include "../../utility/kronecker.hpp"

namespace triqs {
  namespace gfs {

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
      long n;
      double beta;
      statistic_enum statistic;
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

    //template<typename T> bool operator<(matsubara_freq const &x, T const &y) { return (x.n <y);}
    //template<typename T> bool operator<(T const &x, matsubara_freq const &y) { return (x <y.n);}
    //inline bool operator<(matsubara_freq const &x, matsubara_freq const &y) { return (x.n<y.n);}

    //template<typename T> bool operator>(matsubara_freq const &x, T const &y) { return (x.n>y);}
    //template<typename T> bool operator>(T const &x, matsubara_freq const &y) { return (x>y.n);}
    //inline bool operator>(matsubara_freq const &x, matsubara_freq const &y) { return (x.n>y.n);}

    //---------------------------------------------------------------------------------------------------------
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

      /// Write into HDF5
      friend void h5_write(h5::group fg, std::string subgroup_name, matsubara_domain const &d) {
        h5::group gr = fg.create_group(subgroup_name);
        h5_write(gr, "beta", d.beta);
        h5_write(gr, "statistic", (d.statistic == Fermion ? "F" : "B"));
      }

      /// Read from HDF5
      friend void h5_read(h5::group fg, std::string subgroup_name, matsubara_domain &d) {
        h5::group gr = fg.open_group(subgroup_name);
        double beta;
        std::string statistic = " ";
        h5_read(gr, "beta", beta);
        h5_read(gr, "statistic", statistic);
        d = matsubara_domain(beta, (statistic == "F" ? Fermion : Boson));
      }

      //  BOOST Serialization
      friend class boost::serialization::access;
      template <class Archive> void serialize(Archive &ar, const unsigned int version) {
        ar &beta;
        ar &statistic;
      }
      friend std::ostream &operator<<(std::ostream &sout, matsubara_domain const &d) {
        return sout << "Matsubara domain with beta = " << d.beta << ", statistic = " << d.statistic;
      }
    };

    using matsubara_freq_domain = matsubara_domain<true>;
    using matsubara_time_domain = matsubara_domain<false>;

    // ----- kronecker function : overload for matsubara_freq
    inline bool kronecker(matsubara_freq const &freq) { return freq.n == 0; }
    inline bool kronecker(matsubara_freq const &f1, matsubara_freq const &f2) { return f1.n == f2.n; }
  } // namespace gfs
} // namespace triqs
