/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2015 by O. Parcollet
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
#include <triqs/utility/is_complex.hpp>
#include <mpi.h>

// forward declare in case we do not include boost.
namespace boost { namespace mpi { class communicator; }}

namespace triqs {
namespace mpi {

 inline bool is_initialized() noexcept {
  int flag;
  MPI_Initialized(&flag);
  return flag;
 }

 // ------------------------------------------------------------

 struct environment {

  // MPICH does not allow Init without argc, argv, so we do not allow default constructors
  // for portability, cf #133
  environment(int argc, char *argv[]) {
   if (!is_initialized()) MPI_Init(&argc, &argv);
  }
  ~environment() { MPI_Finalize(); }
 };

 // ------------------------------------------------------------

 /// The communicator. Todo : add more constructors.
 class communicator {
  MPI_Comm _com = MPI_COMM_WORLD;

  public:
  communicator() = default;

  communicator(MPI_Comm c) : _com(c) {}

  MPI_Comm get() const { return _com; }

  // defined in boost.hpp if included
  inline communicator(boost::mpi::communicator);

  /// Cast to the boost mpi communicator:  defined in boost.hpp
  inline operator boost::mpi::communicator() const;

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

  communicator split(int color, int key = 0) const {
   communicator c;
   MPI_Comm_split(_com, color, key, &c._com);
   return c;
  }

  void barrier() const { MPI_Barrier(_com); }
 };

 // ------------------------------------------------------------

 namespace tag {
  struct reduce {};
  struct scatter {};
  struct gather {};
 }

 // A small lazy tagged class
 template <typename Tag, typename T> struct lazy {
  T const &rhs;
  communicator c;
  int root;
  bool all;
 };

 template <typename T> struct lazy<tag::reduce, T> {
  T const &rhs;
  communicator c;
  int root;
  bool all;
  MPI_Op op;
 };

// ----------------------------------------
// ------- general functions -------
// ----------------------------------------

 template <typename T> auto mpi_all_reduce(T &x, communicator c = {}, int root = 0, MPI_Op op = MPI_SUM) { return mpi_reduce(x, c, root, true, op); }
 template <typename T> auto mpi_all_gather(T &x, communicator c = {}, int root = 0) { return mpi_gather(x, c, root, true); }

 // backward compatibility. Do not document.
 template <typename... T> void broadcast(T &&... x) { mpi_broadcast(std::forward<T>(x)...); }
 template <typename... T> auto reduce(T &&... x) { return mpi_reduce(std::forward<T>(x)...); }

 /* -----------------------------------------------------------
  *   transformation type -> mpi types
  * ---------------------------------------------------------- */

 template <class T> inline MPI_Datatype mpi_datatype();
#define D(T, MPI_TY)                                                                                                             \
 template <> inline MPI_Datatype mpi_datatype<T>() { return MPI_TY; }
 D(int, MPI_INT)
 D(long, MPI_LONG)
 D(long long, MPI_LONG_LONG)
 D(double, MPI_DOUBLE)
 D(float, MPI_FLOAT)
 D(std::complex<double>, MPI_DOUBLE_COMPLEX);
 D(unsigned long, MPI_UNSIGNED_LONG);
 D(unsigned int, MPI_UNSIGNED);
 D(unsigned long long, MPI_UNSIGNED_LONG_LONG);
#undef D

 //------------ Some helper function

 // Given a range [first, last], slice it regularly for a node of rank 'rank' among n_nodes.
 // If the range is not dividable in n_nodes equal parts,
 // the first nodes have one more elements than the last ones.
 inline std::pair<long, long> slice_range(long first, long last, int n_nodes, int rank) {
  long chunk = (last - first + 1) / n_nodes;
  long n_large_nodes = (last - first + 1) - n_nodes * chunk;
  if (rank <= n_large_nodes - 1) // first, larger nodes, use chunk + 1
   return {first + rank * (chunk + 1), first + (rank + 1) * (chunk + 1) - 1};
  else // others nodes : shift the first by 1*n_large_nodes, used chunk
   return {first + n_large_nodes + rank * chunk, first + n_large_nodes + (rank + 1) * chunk - 1};
 }

 inline long slice_length(long imax, int n_nodes, int rank) {
  auto r = slice_range(0, imax, n_nodes, rank);
  return r.second - r.first + 1;
 }

 /* -----------------------------------------------------------
  *  basic types
  * ---------------------------------------------------------- */

 template <typename T>
 struct is_basic : std::integral_constant<bool, std::is_arithmetic<T>::value || triqs::is_complex<T>::value> {};

#define REQUIRES_IS_BASIC(T, U) std14::enable_if_t<is_basic<T>::value, U>

 template <typename T> REQUIRES_IS_BASIC(T, void) mpi_broadcast(T &a, communicator c = {}, int root = 0) {
  MPI_Bcast(&a, 1, mpi_datatype<T>(), root, c.get());
 }

 // Here send, recv

 template <typename T> REQUIRES_IS_BASIC(T, T) mpi_reduce(T a, communicator c = {}, int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
  T b;
  auto d = mpi_datatype<T>();
  if (!all)
   MPI_Reduce(&a, &b, 1, d, op, root, c.get());
  else
   MPI_Allreduce(&a, &b, 1, d, op, c.get());
  return b;
 }

#undef REQUIRES_IS_BASIC

}
}
