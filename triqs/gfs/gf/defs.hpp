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

namespace triqs {
namespace gfs {
 using utility::factory;
 using arrays::make_shape;
 using arrays::range;
 using arrays::array;
 using arrays::array_view;
 using arrays::matrix;
 using arrays::matrix_view;
 using arrays::matrix_const_view;
 using triqs::make_clone;
 using dcomplex = std::complex<double>;
 using arrays::make_shape;
 using arrays::range;
 using arrays::ellipsis;
 using arrays::mini_vector;

 template <typename Var, typename Target> struct gf_singularity;
 template <typename M, typename T> using gf_singularity_t = typename gf_singularity<M, T>::type;
 template <typename S> struct gf_singularity_factory;

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

 template <typename... T> closest_pt_wrap<T...> closest_mesh_pt(T &&... x) {
  return closest_pt_wrap<T...>{std::forward<T>(x)...};
 }

 /*----------------------------------------------------------
  *  HDF5
  *  Traits to read/write in hdf5 files.
  *  Can be specialized for some case (Cf block). Defined below
  *--------------------------------------------------------*/

 template <typename Var, typename Target> struct gf_h5_name;
 template <typename Var, typename Target> struct gf_h5_rw;

 /*----------------------------------------------------------
  *  Get shape of the data or of the target
  *--------------------------------------------------------*/

 template <typename G> TRIQS_DEPRECATED("use X.data_shape() instead") auto get_gf_data_shape(G const &g) {
  return g.data_shape();
 }

 template <typename G> TRIQS_DEPRECATED("use X.target_shape() instead") auto get_target_shape(G const &g) {
  return g.target_shape();
 }

 /*------------------------------------------------------------------------------------------------------
 *                                  For mpi lazy
 *-----------------------------------------------------------------------------------------------------*/

 // FIXME

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
}
}
