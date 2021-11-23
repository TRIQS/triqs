// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once

#include <algorithm>
#include <complex>
#include <string>
#include <fmt/core.h>
#include <h5/h5.hpp>
#include "product.hpp"

namespace triqs::mesh {

  // Overload isnan for complex and arrays
  namespace impl {
    template <std::floating_point F> constexpr bool isnan(std::complex<F> const &p) { return std::isnan(p.real()) || std::isnan(p.imag()); }
    template <std::floating_point F, std::size_t N> constexpr bool isnan(std::array<std::complex<F>, N> const &p) {
      return std::any_of(p.begin(), p.end(), [](F const &f) { return isnan(f); });
    }
    template <std::floating_point F, std::size_t N> constexpr bool isnan(std::array<F, N> const &p) {
      return std::any_of(p.begin(), p.end(), [](F const &f) { return std::isnan(f); });
    }
  } // namespace impl

  // Generic Implemenation for Numerical Types
  template <typename T> struct numerical_type_domain_base {
    using point_t = T;

    bool is_in_domain(point_t const &pt) {
      using std::isnan;
      using triqs::mesh::impl::isnan;
      return !isnan(pt);
    }

    bool operator==(numerical_type_domain_base const &) const { return true; }

    friend void h5_write(h5::group, std::string_view, numerical_type_domain_base const &) {}
    friend void h5_read(h5::group, std::string_view, numerical_type_domain_base &) {}

    // friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &, const unsigned int) {}
  };

  // ---------------------------------------------

  struct real_domain : numerical_type_domain_base<double> {
    static std::string hdf5_format() { return "real_domain"; }
  };

  struct complex_domain : numerical_type_domain_base<std::complex<double>> {
    static std::string hdf5_format() { return "complex_domain"; }
  };

  template <int N> struct real_N_domain : numerical_type_domain_base<std::array<double, N>> {
    static std::string hdf5_format() { return fmt::format("real_N_domain_{:d}", N); } // labels
  };

  template <int N> struct complex_N_domain : numerical_type_domain_base<std::array<std::complex<double>, N>> {
    static std::string hdf5_format() { return fmt::format("complex_N_domain_{:d}", N); } // labels
  };

} // namespace triqs::mesh
