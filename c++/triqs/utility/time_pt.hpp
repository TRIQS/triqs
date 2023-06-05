// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
// Copyright (c) 2016 Igor Krivenko
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
// Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Priyanka Seth, Hugo U. R. Strand, Nils Wentzell

#pragma once
#include <triqs/utility/first_include.hpp>
#include <triqs/utility/macros.hpp>
#include <h5/h5.hpp>
#include <limits>
#include <iostream>
#include <cmath>

namespace triqs {
  namespace utility {

    struct time_segment;

    /**
  * A point in imaginary time, i.e. $\tau \in [0,\beta]$, but defined on a very thin grid.
  *
  * * Regular type.
  *
  * * **Rationale**: the position in the segment is given by an uint64_t,
  *   i.e. a very long integer.
  *   This allows exact comparisons, which notoriously dangerous on floating point number.
  *
  * * Each time point is constructed from a segment, Cf time_segment class.
  * * It can be casted to a double, but the reverse is wrong.
  *
  */
    struct time_pt {

      friend struct time_segment;

      /// Default constructor: $\tau=0$
      time_pt() {
        beta = 0;
        val  = 0;
        n    = 0;
      }

      public:
      time_pt(uint64_t n_, double beta_) : beta(beta_), n(n_), val(beta_ * (double(n_) / Nmax)) {}

      /// Forbidden
      time_pt &operator=(double v) = delete;

      /// Comparisons (using integer, so it is safe)
      auto operator<=>(time_pt const &tp) const { return n <=> tp.n; }
      bool operator==(time_pt const &tp) const { return n == tp.n; }

      /// adding and substracting is cyclic on [0, beta]
      inline friend time_pt operator+(time_pt const &a, time_pt const &b) {
        bool wrapped = ((Nmax - std::max(a.n, b.n)) < std::min(a.n, b.n));
        if (!wrapped)
          return time_pt((a.n + b.n) % Nmax, a.beta);
        else
          return time_pt(((a.n + b.n) + 1) % Nmax, a.beta);
      }
      inline friend time_pt operator-(time_pt const &a, time_pt const &b) {
        uint64_t n = (a.n >= b.n ? (a.n - b.n) % Nmax : Nmax - (b.n - a.n));
        return time_pt(n, a.beta);
      }

      /// unary -
      friend time_pt operator-(time_pt const &a) { return time_pt(Nmax - a.n, a.beta); }

      /// division by integer
      friend time_pt div_by_int(time_pt const &a, size_t b) { return time_pt(a.n / b, a.beta); }

      /// Multiplication by int
      friend time_pt mult_by_int(time_pt const &a, size_t b) { return time_pt(a.n * b, a.beta); }

      /// floor_div(x,y) = floor (x/y), but computed on the grid.
      friend size_t floor_div(time_pt const &a, time_pt const &b) { return a.n / b.n; }

      /// To cast to double, but it has to be done explicitly.
      explicit operator double() const { return val; } // cast to a double

      /// Stream insertion
      friend std::ostream &operator<<(std::ostream &out, time_pt const &p) {
        return out << p.val << " [time_pt : beta = " << p.beta << " n = " << p.n << "]";
      }

      /// Nmax
      static constexpr uint64_t Nmax = std::numeric_limits<uint64_t>::max();

      private:
      uint64_t n;
      double val, beta;

      friend class boost::serialization::access;
      template <class Archive> void serialize(Archive &ar, const unsigned int version) {
        ar &n;
        ar &val;
        ar &beta;
      }

      /// Write into HDF5
      friend void h5_write(h5::group fg, std::string const &subgroup_name, time_pt const &g) {
        auto gr = fg.create_group(subgroup_name);
        h5_write(gr, "beta", g.beta);
        h5_write(gr, "val", g.val);
        h5_write(gr, "n", g.n);
      }

      /// Read from HDF5
      friend void h5_read(h5::group fg, std::string const &subgroup_name, time_pt &g) {
        auto gr = fg.open_group(subgroup_name);
        h5_read(gr, "beta", g.beta);
        h5_read(gr, "val", g.val);
        h5_read(gr, "n", g.n);
      }
    };

    /// all operations below decay to double
    inline double operator*(time_pt const &a, time_pt const &b) { return double(a) * double(b); }
    inline double operator/(time_pt const &a, time_pt const &b) { return double(a) / double(b); }

#define IMPL_OP(OP)                                                                                                                                  \
  inline double operator OP(time_pt const &x, double y) { return static_cast<double>(x) OP y; }                                                      \
  inline double operator OP(double y, time_pt const &x) { return y OP static_cast<double>(x); }
    IMPL_OP(+);
    IMPL_OP(-);
    IMPL_OP(*);
    IMPL_OP(/);
#undef IMPL_OP

    /// Time segment [0, beta]
    struct time_segment {

      /// Beta
      const double beta;

      /// Construction from $\beta$
      time_segment(double beta_) : beta(beta_) {}

      /// Get a random point in $[0, tp[$
      template <typename RNG> time_pt get_random_pt(RNG &rng, time_pt tp) const { return time_pt(rng(tp.n), beta); }

      /// Get a random point in $[0,\beta[$
      template <typename RNG> time_pt get_random_pt(RNG &rng) const { return time_pt(rng(time_pt::Nmax), beta); }

      // Get a random point in $[tp1, tp2[$
      template <typename RNG> time_pt get_random_pt(RNG &rng, time_pt tp1, time_pt tp2) const { return time_pt(rng(tp2.n - tp1.n) + tp1.n, beta); }

      /// Get maximum point (i.e. $\tau =\beta$)
      time_pt get_upper_pt() const { return time_pt(time_pt::Nmax, beta); }

      /// Get minimum point (i.e. $\tau =0$)
      time_pt get_lower_pt() const { return time_pt(0, beta); }

      /// Get epsilon, defined as $\epsilon = \beta /N_max$
      time_pt get_epsilon() const { return time_pt(1, beta); }

      /// ??
      time_pt make_time_pt(double x) const {
        EXPECTS(0 <= x && x <= beta);
        uint64_t n = time_pt::Nmax * std::min(1.0, (x / beta));
        return time_pt(n, beta);
      }
    };
  } // namespace utility
} // namespace triqs
