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
#include <triqs/utility/tuple_tools.hpp>

#define TRIQS_MPI_IMPLEMENTED_AS_TUPLEVIEW using triqs_mpi_as_tuple = void;
namespace triqs {
namespace mpi {

 template <typename Tag, typename T> struct mpi_lazy {
  T const &ref;
  int root;
  communicator c;
 };

 /** ------------------------------------------------------------
  *  Type which are recursively treated by reducing them to a tuple
  *  of smaller objects.
  *  ----------------------------------------------------------  **/
 template <typename T> struct mpi_impl_tuple {

  mpi_impl_tuple() = default;
  template <typename Tag> static mpi_lazy<Tag, T> invoke(Tag, communicator c, T const &a, int root) {
   return {a, root, c};
  }

#ifdef __cpp_generic_lambdas
  static void reduce_in_place(communicator c, T &a, int root) {
   tuple::for_each(view_as_tuple(a), [c, root](auto &x) { triqs::mpi::reduce_in_place(x, c, root); });
  }

  static void broadcast(communicator c, T &a, int root) {
   tuple::for_each(view_as_tuple(a), [c, root](auto &x) { triqs::mpi::broadcast(x, c, root); });
  }

  template <typename Tag> static T &complete_operation(T &target, mpi_lazy<Tag, T> laz) {
   auto l = [laz](auto &t, auto &s) { t = triqs::mpi::mpi_impl<std::decay_t<decltype(s)>>::invoke(Tag(), laz.c, s, laz.root); };
   triqs::tuple::for_each_zip(l, view_as_tuple(target), view_as_tuple(laz.ref));
   return target;
  }
#else

  struct aux1 {
   communicator c;
   int root;

   template <typename T1> void operator()(T1 &x) const { triqs::mpi::reduce_in_place(c, x, root); }
  };

  static void reduce_in_place(communicator c, T &a, int root) {
   tuple::for_each(aux1{c, root}, view_as_tuple(a));
  }

  struct aux2 {
   communicator c;
   int root;

   template <typename T2> void operator()(T2 &x) const { triqs::mpi::broadcast(c, x, root); }
  };

  static void broadcast(communicator c, T &a, int root) {
   tuple::for_each(aux2{c, root}, view_as_tuple(a));
  }

  template <typename Tag> struct aux3 {
   mpi_lazy<Tag, T> laz;

   template <typename T1, typename T2> void operator()(T1 &t, T2 &s) const {
    t = triqs::mpi::mpi_impl<T2>::invoke(Tag(), laz.c, laz.s);
   }
  };

  template <typename Tag> static void complete_operation(T &target, mpi_lazy<Tag, T> laz) {
   auto l = aux3<Tag>{laz};
   triqs::tuple::for_each_zip(l, view_as_tuple(target), view_as_tuple(laz.ref));
  }
#endif
 };

 // If type T has a mpi_implementation nested struct, then it is mpi_impl<T>.
 template <typename T> struct mpi_impl<T, typename T::triqs_mpi_as_tuple> : mpi_impl_tuple<T> {};
}
} // namespace

