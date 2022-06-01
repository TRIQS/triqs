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

namespace triqs::mesh {

  struct retime : segment_mesh {
    using segment_mesh::segment_mesh;
    // template <typename... T> retime(T &&... x) : segment_mesh(std::forward<T>(x)...) {}

    friend std::ostream &operator<<(std::ostream &sout, retime const &m) {
      return sout << "Real Time Mesh of size " << m.size() << ", t_min: " << m.t_min() << ", t_max: " << m.t_max();
    }

    // Smallest frequency in the mesh
    double t_min() const { return x_min(); }

    // Largest frequency in the mesh
    double t_max() const { return x_max(); }

    // -------------------- HDF5 -------------------

    static std::string hdf5_format() { return "MeshReTime"; }

    friend void h5_write(h5::group fg, std::string const &subgroup_name, retime const &m) { h5_write_impl(fg, subgroup_name, m, "MeshReTime"); }

    friend void h5_read(h5::group fg, std::string const &subgroup_name, retime &m) { h5_read_impl(fg, subgroup_name, m, "MeshReTime"); }
  };
} // namespace triqs::mesh
