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
#include "./tail_fitter.hpp"

namespace triqs::mesh {

  struct refreq : linear<refreq, double>, tail_fitter_handle {

    // -------------------- Constructors -------------------

    /**
     *  Construct a Mesh of real frequencies evenly distributed in the interval
     *  [w_min, w_max] including values at both edges.
     *
     *  @param w_min Smallest frequency
     *  @param w_max Largest frequency
     *  @param n_w Number of frequencies
     */
    refreq(double w_min = 0.0, double w_max = 0.0, long n_w = 2) : linear(w_min, w_max, n_w) {}

    refreq(std::pair<double, double> window, int n_w) : refreq(std::get<0>(window), std::get<1>(window), n_w) {}

    // -------------------- Accessors -------------------

    /// Is the mesh only for positive omega
    static constexpr bool positive_only() { return false; }

    /// First index of the mesh
    static constexpr long first_idx() { return 0; }

    /// Last index of the mesh
    long last_idx() const { return size() - 1; }

    /// Smallest frequency in the mesh
    double w_min() const { return xmin; }

    /// Largest frequency in the mesh
    double w_max() const { return xmax; }

    // -------------------- HDF5 -------------------

    [[nodiscard]] static std::string hdf5_format() { return "MeshReFreq"; }

    friend void h5_write(h5::group fg, std::string const &subgroup_name, refreq const &m) { h5_write_impl(fg, subgroup_name, m, "MeshReFreq"); }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, refreq &m) { h5_read_impl(fg, subgroup_name, m, "MeshReFreq"); }

    // -------------------- Print -------------------

    friend std::ostream &operator<<(std::ostream &sout, refreq const &m) {
      return sout << fmt::format("Real Freq Mesh with w_min = {}, w_max = {}, n_w = {}", m.xmin, m.xmax, m.L);
    }
  };

  inline auto evaluate(refreq const &m, auto const &f, double x) { return evaluate(static_cast<linear<refreq, double>>(m), f, x); }

  static_assert(Mesh<refreq>);

} // namespace triqs::mesh
