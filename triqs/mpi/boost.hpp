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
#include "./base.hpp"
#include <boost/mpi.hpp>

#define TRIQS_MPI_IMPLEMENTED_VIA_BOOST using triqs_mpi_via_boost = void;

namespace triqs {
namespace mpi {

 // implement the communicator cast
 inline communicator::operator boost::mpi::communicator() const {
  return boost::mpi::communicator(_com, boost::mpi::comm_duplicate); 
  // duplicate policy : cf http://www.boost.org/doc/libs/1_56_0/doc/html/boost/mpi/comm_create_kind.html
 }

 // reverse : construct (implicit) the communicator from the boost one.
 inline communicator::communicator(boost::mpi::communicator c) :_com(c) {}

 /** ------------------------------------------------------------
   *  Type which we use boost::mpi
   *  ----------------------------------------------------------  **/

 template <typename T> struct mpi_impl_boost_mpi {

  static T invoke(tag::reduce, communicator c, T const &a, int root) {
   T b;
   boost::mpi::reduce(c, a, b, std::c14::plus<>(), root);
   return b;
  }

  static T invoke(tag::all_reduce, communicator c, T const &a, int root) {
   T b;
   boost::mpi::all_reduce(c, a, b, std::c14::plus<>());
   return b;
  }

  static void reduce_in_place(communicator c, T &a, int root) { boost::mpi::reduce(c, a, a, std::c14::plus<>(), root); }
  static void broadcast(communicator c, T &a, int root) { boost::mpi::broadcast(c, a, root); }

  static void scatter(communicator c, T const &, int root) = delete;
  static void gather(communicator c, T const &, int root) = delete;
  static void allgather(communicator c, T const &, int root) = delete;
 };

 // If type T has a mpi_implementation nested struct, then it is mpi_impl<T>.
 template <typename T> struct mpi_impl<T, typename T::triqs_mpi_via_boost> : mpi_impl_boost_mpi<T> {};

}}//namespace




