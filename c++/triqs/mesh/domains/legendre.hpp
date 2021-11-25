// TRIQS: a Toolbox for Research in Interacting Quantum Systems
//
// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2021 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell, Philipp Dumitrescu

#pragma once

#include <h5/h5.hpp>
#include "../details/mesh_tools.hpp"

namespace triqs::mesh {

  struct legendre_domain {
    using point_t = long;

    double beta              = 0.0;
    statistic_enum statistic = Fermion;
    size_t max_n             = 1; // Is this inclusive order not??

    // Do we need this ctor??
    legendre_domain() = default;

    /**
     *  Construct a Mesh of Legendre polynomials with degrees in the interval [0,n_l]
     *
     *  @param beta Inverse temperature
     *  @param S Statistic, Fermion or Boson
     *  @param n_l Largest degree
     */
    legendre_domain(double beta_, statistic_enum statistic_, point_t max_n_) : beta{beta_}, statistic{statistic_}, max_n{max_n_} {};

    [[nodiscard]] bool is_in_domain(point_t const &pt) const { return (0 <= pt) && (pt <= max_n); };

    [[nodiscard]] size_t size() const { return max_n; };

    bool operator==(legendre_domain const &D) const {
      return ((std::abs(beta - D.beta) < 1.e-15) && (statistic == D.statistic) && (max_n == D.max_n));
    }
    bool operator!=(legendre_domain const &) const = default;

    [[nodiscard]] std::string hdf5_format() const { return "LegendreDomain"; }

    /// Write into HDF5
    friend void h5_write(h5::group fg, std::string const &subgroup_name, legendre_domain const &d) {
      h5::group gr = fg.create_group(subgroup_name);
      h5_write(gr, "max_n", d.max_n);
      h5_write(gr, "beta", d.beta);
      h5_write(gr, "statistic", (d.statistic == Fermion ? "F" : "B"));
    }

    /// Read from HDF5
    friend void h5_read(h5::group fg, std::string const &subgroup_name, legendre_domain &d) {
      h5::group gr = fg.open_group(subgroup_name);
      std::string statistic{};
      if (not h5_try_read(gr, "max_n", d.max_n)) h5_read(gr, "n_max", d.max_n);
      h5_read(gr, "beta", d.beta);
      h5_read(gr, "statistic", statistic);
      d.statistic = "F" ? Fermion : Boson;
    }

    //  BOOST Serialization
    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, const unsigned int version) {
      ar &max_n;
      ar &beta;
      ar &statistic;
    }
  };
} // namespace triqs::mesh
