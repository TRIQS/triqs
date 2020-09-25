// Copyright (c) 2016-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2016-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
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

#include "../../arrays.hpp"

namespace triqs::gfs {

  /// FIXME CLEAN THIS
  using dcomplex = std::complex<double>;

  using arrays::array;
  using arrays::array_view;
  using arrays::ellipsis;
  using arrays::make_shape;
  using arrays::matrix;
  using arrays::matrix_const_view;
  using arrays::matrix_view;
  using arrays::memory_layout_t;
  using arrays::mini_vector;

  using itertools::range;
  using triqs::make_clone;
  using utility::factory;

  // Using from mesh namespace
  using mesh::_long;
  using mesh::all_t;
  using mesh::Boson;
  using mesh::closest_mesh_pt;
  using mesh::closest_pt_wrap;
  using mesh::Fermion;
  using mesh::get_n_variables;
  using mesh::matsubara_freq;
  using mesh::statistic_enum;

  /*----------------------------------------------------------
   *  Evaluator
   *--------------------------------------------------------*/

  // gf_evaluator regroup functions to evaluate the function.
  template <typename Mesh, typename Target> struct gf_evaluator;

  /*----------------------------------------------------------
   *  HDF5
   *  Traits to read/write in hdf5 files.
   *  Can be specialized for some case (Cf block). Defined below
   *--------------------------------------------------------*/

  template <typename Mesh, typename Target> struct gf_h5_rw;

  /*------------------------------------------------------------------------------------------------------
   *                                  For mpi lazy
   *-----------------------------------------------------------------------------------------------------*/

  // A small lazy tagged class
  template <typename Tag, typename T> struct mpi_lazy {
    T rhs;
    mpi::communicator c;
    int root;
    bool all;
  };

  template <typename T> struct mpi_lazy<mpi::tag::reduce, T> {
    T rhs;
    mpi::communicator c;
    int root;
    bool all;
    MPI_Op op;
  };
} // namespace triqs::gfs
