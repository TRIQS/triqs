// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

#pragma once
#include "bases/linear.hpp"
#include "domains/real_complex.hpp"

namespace triqs::mesh {

  struct retime : linear<retime, double> {

    // -------------------- Constructors -------------------

    /**
     *  Construct a Mesh of real times
     *
     *  Times are evenly distributed in the interval [t_min, t_max]
     *  including values at both edges.
     *
     *  @param t_min Smallest time
     *  @param t_max Largest time
     *  @param n_t Number of time-points
     */
    retime(double t_min = 0.0, double t_max = 0.0, long n_t = 2) : linear(t_min, t_max, n_t) {}

    retime(std::pair<double, double> window, int n_t) : retime(std::get<0>(window), std::get<1>(window), n_t) {}

    // -------------------- Accessors -------------------

    /// Smallest time in the mesh
    double t_min() const { return xmin; }

    /// Largest time in the mesh
    double t_max() const { return xmax; }

    // -------------------- HDF5 -------------------

    [[nodiscard]] static std::string hdf5_format() { return "MeshReTime"; }

    friend void h5_write(h5::group fg, std::string const &subgroup_name, retime const &m) { h5_write_impl(fg, subgroup_name, m, "MeshReTime"); }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, retime &m) { h5_read_impl(fg, subgroup_name, m, "MeshReTime"); }

    // -------------------- Print -------------------

    friend std::ostream &operator<<(std::ostream &sout, retime const &m) {
      return sout << fmt::format("Real Time Mesh with t_min = {}, t_max = {}, n_t = {}", m.xmin, m.xmax, m.L);
    }
  };

  inline auto evaluate(retime const &m, auto const &f, double x) { return evaluate(static_cast<linear<retime, double>>(m), f, x); }

  static_assert(Mesh<retime>);

} // namespace triqs::mesh
