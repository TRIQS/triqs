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
#include "./bases/segment.hpp"
#include "./details/tail_fitter.hpp"

namespace triqs::mesh {

  struct refreq : segment_mesh, tail_fitter_handle {

    template <typename... T> refreq(T &&... x) : segment_mesh(std::forward<T>(x)...) {}
    // using segment_mesh::segment_mesh;

    /// Is the mesh only for positive omega
    static constexpr bool positive_only() { return false; }

    // -------------------- tail -------------------

    // First index of the mesh
    static constexpr long first_index() { return 0; }

    // Last index of the mesh
    long last_index() const { return size() - 1; }

    // Largest frequency in the mesh
    double omega_max() const { return index_to_point(last_index()); }

    // -------------------- HDF5 -------------------

    static std::string hdf5_format() { return "MeshReFreq"; }

    friend void h5_write(h5::group fg, std::string const &subgroup_name, refreq const &m) { h5_write_impl(fg, subgroup_name, m, "MeshReFreq"); }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, refreq &m) { h5_read_impl(fg, subgroup_name, m, "MeshReFreq"); }
  };

} // namespace triqs::mesh
