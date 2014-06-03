/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2014 by O. Parcollet
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
#include <triqs/utility/c14.hpp>
//#include <triqs/utility/tuple_tools.hpp>
#include <mpi.h>

namespace triqs {
namespace mpi {

 /// Environment
 struct environment {
  environment(int argc, char *argv[]) { MPI_Init(&argc, &argv); }
  ~environment() { MPI_Finalize(); }
 };

 /// The communicator. Todo : add more constructors.
 class communicator {
  MPI_Comm _com = MPI_COMM_WORLD;

  public:
  communicator() = default;

  MPI_Comm get() const { return _com; }

  int rank() const {
   int num;
   MPI_Comm_rank(_com, &num);
   return num;
  }

  int size() const {
   int num;
   MPI_Comm_size(_com, &num);
   return num;
  }

  void barrier() const { MPI_Barrier(_com); }
 };

 /// a tag for each operation
 namespace tag {
  struct reduce {};
  struct allreduce {};
  struct scatter {};
  struct gather {};
  struct allgather {};
 }

 /// The implementation of mpi ops for each type
 template <typename T, typename Enable = void> struct mpi_impl;

 // ----------------------------------------
 // ------- top level functions -------
 // ----------------------------------------

 // ----- functions that can be lazy -------

 template <typename T>
 AUTO_DECL reduce(T const &x, communicator c = {}, int root = 0) RETURN(mpi_impl<T>::invoke(tag::reduce(), c, x, root));
 template <typename T>
 AUTO_DECL scatter(T const &x, communicator c = {}, int root = 0) RETURN(mpi_impl<T>::invoke(tag::scatter(), c, x, root));
 template <typename T>
 AUTO_DECL gather(T const &x, communicator c = {}, int root = 0) RETURN(mpi_impl<T>::invoke(tag::gather(), c, x, root));
 template <typename T>
 AUTO_DECL allreduce(T const &x, communicator c = {}, int root = 0) RETURN(mpi_impl<T>::invoke(tag::allreduce(), c, x, root));
 template <typename T>
 AUTO_DECL allgather(T const &x, communicator c = {}, int root = 0) RETURN(mpi_impl<T>::invoke(tag::allgather(), c, x, root));

 // ----- functions that cannot be lazy -------

 template <typename T> void reduce_in_place(T &x, communicator c = {}, int root = 0) { mpi_impl<T>::reduce_in_place(c, x, root); }
 template <typename T> void broadcast(T &x, communicator c = {}, int root = 0) { mpi_impl<T>::broadcast(c, x, root); }

 // transformation type -> mpi types
 template <class T> struct mpi_datatype;
#define D(T, MPI_TY)                                                                                                             \
 template <> struct mpi_datatype<T> {                                                                                            \
  static MPI_Datatype invoke() { return MPI_TY; }                                                                                \
 };
 D(int, MPI_INT) D(long, MPI_LONG) D(double, MPI_DOUBLE) D(float, MPI_FLOAT) D(std::complex<double>, MPI_DOUBLE_COMPLEX);
 D(unsigned long, MPI_UNSIGNED_LONG);
#undef D

 /** ------------------------------------------------------------
   *  basic types
   *  ----------------------------------------------------------  **/

 template <typename T> struct mpi_impl_basic {

  static MPI_Datatype D() { return mpi_datatype<T>::invoke(); }

  static T invoke(tag::reduce, communicator c, T a, int root) {
   T b;
   MPI_Reduce(&a, &b, 1, D(), MPI_SUM, root, c.get());
   return b;
  }

  static T invoke(tag::allreduce, communicator c, T a, int root) {
   T b;
   MPI_Allreduce(&a, &b, 1, D(), MPI_SUM, root, c.get());
   return b;
  }

  static void reduce_in_place(communicator c, T &a, int root) {
   MPI_Reduce((c.rank() == root ? MPI_IN_PLACE : &a), &a, 1, D(), MPI_SUM, root, c.get());
  }

  static void allreduce_in_place(communicator c, T &a, int root) {
   MPI_Allreduce(MPI_IN_PLACE, &a, 1, D(), MPI_SUM, root, c.get());
  }

  static void broadcast(communicator c, T &a, int root) { MPI_Bcast(&a, 1, D(), root, c.get()); }
 };

 // mpl_impl_basic is the mpi_impl<T> is T is a number (including complex)
 template <typename T>
 struct mpi_impl<T, std14::enable_if_t<std::is_arithmetic<T>::value || triqs::is_complex<T>::value>> : mpi_impl_basic<T> {};

 //------------ Some helper function
 inline long slice_length(size_t imax, communicator c, int r) {
  auto imin = 0;
  long j = (imax - imin + 1) / c.size();
  long i = imax - imin + 1 - c.size() * j;
  auto r_min = (r <= i - 1 ? imin + r * (j + 1) : imin + r * j + i);
  auto r_max = (r <= i - 1 ? imin + (r + 1) * (j + 1) - 1 : imin + (r + 1) * j + i - 1);
  return r_max - r_min + 1;
 };
}
}
