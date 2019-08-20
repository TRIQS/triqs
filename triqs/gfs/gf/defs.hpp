/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2015 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
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
  using mesh::tuple_com;

  // BACKWARD COMPAT !
  template <typename M, typename Requires = std::enable_if_t<mesh::models_mesh_concept_v<M>>>
  using gf_mesh [[deprecated("mesh::X is deprecated since TRIQS 2.3. Replace simply by M. Cf documentation.")]] = M;

  /*----------------------------------------------------------
  *   Useful metafunctions, traits
  *--------------------------------------------------------*/

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

  /*----------------------------------------------------------
  *  Get shape of the data or of the target
  *--------------------------------------------------------*/

  template <typename G> TRIQS_DEPRECATED("use X.data_shape() instead") auto get_gf_data_shape(G const &g) { return g.data_shape(); }

  template <typename G> TRIQS_DEPRECATED("use X.target_shape() instead") auto get_target_shape(G const &g) { return g.target_shape(); }

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
