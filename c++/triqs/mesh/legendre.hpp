// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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
#include "./domains/legendre.hpp"
#include "./discrete.hpp"
#include "../utility/legendre.hpp"

namespace triqs::mesh {

  ///
  struct legendre : discrete<legendre_domain> {
    using B = discrete<legendre_domain>;

    legendre() = default;

    /**
     *  Construct a Mesh of Legendre polynomials with degrees in the interval [0,max_n]
     *
     *  @param beta Inverse temperature
     *  @param S Statistic, Fermion or Boson
     *  @param max_n Largest degree
     */
    legendre(double beta, statistic_enum S, size_t max_n) : B(typename B::domain_t(beta, S, max_n)) {}

    static std::string hdf5_format() { return "MeshLegendre"; }

    // Issue with return type, needs to be a nda::vector<std::pair<long, double>> of length max_n
    nda::vector<double> get_interpolation_data(double tau) const {

      auto res = nda::zeros<double>(size());

      utility::legendre_generator L{};
      L.reset(2 * tau / domain().beta - 1);
      for (auto l : range(size())) { res[l] = std::sqrt(2 * l + 1) * L.next() / domain().beta; }

      return res;
    }

    friend void h5_write(h5::group fg, std::string const &subgroup_name, legendre const &m) { h5_write_impl(fg, subgroup_name, m, "MeshLegendre"); }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, legendre &m) { h5_read_impl(fg, subgroup_name, m, "MeshLegendre"); }
  };
} // namespace triqs::mesh
