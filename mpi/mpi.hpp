/*******************************************************************************
 * 
 * Copyright (C) 2015-2018, O. Parcollet
 * Copyright (C) 2019, The Simons Foundation
 *   author : N. Wentzell
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#pragma once

#include <itertools/itertools.hpp>

#include <mpi.h>
#include <complex>
#include <type_traits>

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

#ifdef BOOST_MPI_HPP
    // Conversion to and from boost communicator, Keep for backward compatibility
    inline operator boost::mpi::communicator() const { return boost::mpi::communicator(_com, boost::mpi::comm_duplicate); }
    inline communicator(boost::mpi::communicator c) : _com(c) {}
#endif

    void barrier() const { MPI_Barrier(_com); }
  };

  // ------------------------------------------------------------

  namespace tag {
    struct reduce {};
    struct scatter {};
    struct gather {};
  } // namespace tag

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

  template <typename T>[[gnu::always_inline]] inline decltype(auto) broadcast(T &&x, communicator c = {}, int root = 0) {
    return mpi_broadcast(std::forward<T>(x), c, root);
  }
  template <typename T>
  [[gnu::always_inline]] inline decltype(auto) reduce(T &&x, communicator c = {}, int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
    return mpi_reduce(std::forward<T>(x), c, root, all, op);
  }
  template <typename T>[[gnu::always_inline]] inline decltype(auto) scatter(T &&x, mpi::communicator c = {}, int root = 0) {
    return mpi_scatter(std::forward<T>(x), c, root);
  }
  template <typename T>[[gnu::always_inline]] inline decltype(auto) gather(T &&x, mpi::communicator c = {}, int root = 0, bool all = false) {
    return mpi_gather(std::forward<T>(x), c, root, all);
  }
  template <typename T>[[gnu::always_inline]] inline decltype(auto) all_reduce(T &&x, communicator c = {}, int root = 0, MPI_Op op = MPI_SUM) {
    return reduce(std::forward<T>(x), c, root, true, op);
  }
  template <typename T>[[gnu::always_inline]] inline decltype(auto) all_gather(T &&x, communicator c = {}, int root = 0) {
    return gather(std::forward<T>(x), c, root, true);
  }
  template <typename T>
  [[gnu::always_inline]] [[deprecated("mpi_all_reduce is deprecated, please use mpi::all_reduce instead")]] inline decltype(auto)
  mpi_all_reduce(T &&x, communicator c = {}, int root = 0, MPI_Op op = MPI_SUM) {
    return reduce(std::forward<T>(x), c, root, true, op);
  }
  template <typename T>
  [[gnu::always_inline]] [[deprecated("mpi_all_gather is deprecated, please use mpi::all_gather instead")]] inline decltype(auto)
  mpi_all_gather(T &&x, communicator c = {}, int root = 0) {
    return gather(std::forward<T>(x), c, root, true);
  }

  /* -----------------------------------------------------------
  *   transformation type -> mpi types
  * ---------------------------------------------------------- */

  template <class T> inline MPI_Datatype datatype();
#define D(T, MPI_TY)                                                                                                                                 \
  template <> inline MPI_Datatype datatype<T>() { return MPI_TY; }
  D(char, MPI_CHAR)
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
  inline long chunk_length(long end, int n_nodes, int rank) {
    auto [node_begin, node_end] = itertools::chunk_range(0, end, n_nodes, rank);
    return node_end - node_begin;
  }

  /**
    * Function to chunk a range, distributing it uniformly over all MPI ranks.
    *
    * @tparam T The type of the range
    *
    * @param range The range to chunk
    * @param comm The mpi communicator
    */
  template <typename T> auto chunk(T &&range, communicator comm = {}) {
    auto total_size           = std::distance(std::cbegin(range), std::cend(range));
    auto [start_idx, end_idx] = itertools::chunk_range(0, total_size, comm.size(), comm.rank());
    return itertools::slice(std::forward<T>(range), start_idx, end_idx);
  }

  /* -----------------------------------------------------------
  *  basic types
  * ---------------------------------------------------------- */

  template <typename T> struct is_complex : std::false_type {};
  template <typename T> struct is_complex<std::complex<T>> : std::true_type {};
  template <typename T> struct is_basic : std::integral_constant<bool, std::is_arithmetic<T>::value || is_complex<T>::value> {};

#define REQUIRES_IS_BASIC(T, U) std::enable_if_t<is_basic<T>::value, U>

  // NOTE: We keep the naming mpi_XXX for the actual implementation functions
  // so they can be defined in other namespaces and the mpi::reduce(T,...) function
  // can find them via ADL
  template <typename T> REQUIRES_IS_BASIC(T, void) mpi_broadcast(T &a, communicator c = {}, int root = 0) {
    MPI_Bcast(&a, 1, datatype<T>(), root, c.get());
  }

  template <typename T> REQUIRES_IS_BASIC(T, T) mpi_reduce(T a, communicator c = {}, int root = 0, bool all = false, MPI_Op op = MPI_SUM) {
    T b;
    auto d = datatype<T>();
    if (!all)
      MPI_Reduce(&a, &b, 1, d, op, root, c.get());
    else
      MPI_Allreduce(&a, &b, 1, d, op, c.get());
    return b;
  }

#undef REQUIRES_IS_BASIC

#define MPI_TEST_MAIN                                                                                                                                \
  int main(int argc, char **argv) {                                                                                                                  \
    mpi::environment env(argc, argv);                                                                                                                \
    ::testing::InitGoogleTest(&argc, argv);                                                                                                          \
    return RUN_ALL_TESTS();                                                                                                                          \
  }

} // namespace mpi
