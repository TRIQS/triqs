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
// Authors: Olivier Parcollet, Nils Wentzell

#pragma once

#include "../../arrays.hpp"

namespace triqs {
  namespace gfs {
    using arrays::array;
    using arrays::array_view;
    using arrays::make_shape;
    using arrays::matrix;
    using arrays::matrix_const_view;
    using arrays::matrix_view;
    using itertools::range;
    using triqs::make_clone;
    using utility::factory;
    using dcomplex = std::complex<double>;
    using arrays::ellipsis;
    using arrays::make_shape;
    using arrays::mini_vector;
    using itertools::range;

    /*----------------------------------------------------------
  *   Useful metafunctions, traits
  *--------------------------------------------------------*/

    // Get the number of variables
    template <typename Var> struct get_n_variables { static const int value = 1; };

    /*----------------------------------------------------------
  *  Evaluator
  *--------------------------------------------------------*/

    // gf_evaluator regroup functions to evaluate the function.
    template <typename Var, typename Target> struct gf_evaluator;

    /*----------------------------------------------------------
  *  closest_point mechanism
  *  This trait will contain the specialisation to get
  *  the closest point ...
  *--------------------------------------------------------*/
    template <typename Var, typename Target> struct gf_closest_point;

    // implementation
    template <typename... T> struct closest_pt_wrap;

    template <typename T> struct closest_pt_wrap<T> : tag::mesh_point {
      T value;
      template <typename U> explicit closest_pt_wrap(U &&x) : value(std::forward<U>(x)) {}
    };

    template <typename T1, typename T2, typename... Ts> struct closest_pt_wrap<T1, T2, Ts...> : tag::mesh_point {
      std::tuple<T1, T2, Ts...> value_tuple;
      template <typename... U> explicit closest_pt_wrap(U &&... x) : value_tuple(std::forward<U>(x)...) {}
    };

    template <typename... T> closest_pt_wrap<T...> closest_mesh_pt(T &&... x) { return closest_pt_wrap<T...>{std::forward<T>(x)...}; }

    /*----------------------------------------------------------
  *  HDF5
  *  Traits to read/write in hdf5 files.
  *  Can be specialized for some case (Cf block). Defined below
  *--------------------------------------------------------*/

    template <typename Var, typename Target> struct gf_h5_rw;

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
  } // namespace gfs
} // namespace triqs
