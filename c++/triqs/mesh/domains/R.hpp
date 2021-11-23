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

#include <h5/h5.hpp>

namespace triqs::mesh {

  /// The domain
  struct R_domain {
    using point_t = double;
    bool operator==(R_domain const &D) const { return true; }
    friend void h5_write(h5::group fg, std::string subgroup_name, R_domain const &d) {}
    friend void h5_read(h5::group fg, std::string subgroup_name, R_domain &d) {}
    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, const unsigned int version) {}
  };

  struct real_domain {
    using point_t = double;

    bool is_in_domain(point_t const & pt) { return !std::isnan(pt); }; // Q: -Inf & Inf
    bool operator==(real_domain const &) const { return true; }

    static std::string hdf5_format() { return "real_domain"; }
    friend void h5_write(h5::group, std::string_view, real_domain const &) {}
    friend void h5_read(h5::group, std::string_view, real_domain &) {}

    // friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &, const unsigned int) {}
  };

  // type_domain. Domain convered by type (but nan, infs, ...)

  // // Various instantiations
  // struct R_domain : point_domain<double, true> {};
  // struct C_domain : point_domain<std::complex<double>, true> {};
  // template <int N> struct RN_domain : point_domain<std::array<double, N>, N == 1> {};
  // template <int N> struct CN_domain : point_domain<std::array<std::complex<double>, N>, false> {};

} // namespace triqs::mesh
