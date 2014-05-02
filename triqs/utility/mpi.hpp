/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#ifndef TRIQS_UTILITY_MPI_H
#define TRIQS_UTILITY_MPI_H
#include <triqs/utility/c14.hpp>
#include <boost/mpi.hpp>

namespace triqs { namespace mpi { 
 
 using boost::mpi::communicator;
 using boost::mpi::environment;

 // transformation type -> mpi types
 template <class T> struct mpi_datatype { static constexpr bool ok=false;};
#define D(T,MPI_TY) template <> struct mpi_datatype <T>{ static MPI_Datatype invoke() { return MPI_TY;}; static constexpr bool ok=true;};
 D(int,MPI_INT) D(long,MPI_LONG) D(double,MPI_DOUBLE) D(float,MPI_FLOAT) D(std::complex<double>, MPI_DOUBLE_COMPLEX);
#undef D

 // ok that is simple ...
 inline void barrier(communicator _c) { MPI_Barrier(_c);}

 // a struct to specialize for the implementation for various types...
 template<typename T, typename Enable=void> struct mpi_impl;

 // ------------------------------
 // the final function for users
 // ------------------------------
 
 // reduce  : first the in_place version
 template<typename T> void reduce_in_place(communicator _c, T & a, int root=0) { mpi_impl<T>::reduce_in_place(_c,a,root); }

 inline void reduce_in_place_v(communicator _c) {}
 
 // try a variadic one. Does not cost much more to code...
 template<typename T0, typename ... T> void reduce_in_place_v(communicator _c, T0 & a0, T& ... a) { 
  reduce_in_place(_c,a0,0); 
  reduce_in_place_v(_c, a...);
 }

 // reduce : the regular version in term of the in place one (accept views on the fly from b).
 template<typename T, typename U> void reduce (communicator _c, T & a, U && b, int root =0) { 
  b = a; reduce_in_place(_c,b,root);
 }

 // all_reduce : first the in_place version
 template<typename T> void all_reduce_in_place(communicator _c, T & a) { mpi_impl<T>::reduce_in_place(_c,a,0); }

 // all_reduce : the regular version in term of the in place one (accept views on the fly from b).
 template<typename T, typename U> void all_reduce (communicator _c, T & a, U && b) { b = a; reduce_in_place(_c,b); }

 // BroadCast
 template<typename T> void broadcast(communicator _c, T & a, int root =0) { mpi_impl<T>::broadcast(_c,a,root); }

 // ----------------------------------------------------------------------
 // the generic implementation : using serialization for recursive action  
 // ----------------------------------------------------------------------
 template<typename T, typename Enable> struct mpi_impl { 

#define MAKE_ADAPTOR_AND_FNT(FNT)\
  struct adaptor_##FNT {\
   communicator _c; int root;\
   template<typename RHS> adaptor_##FNT & operator & (RHS & rhs) { mpi_impl<RHS>::FNT(_c, rhs, root); return *this; }\
  };\
  static void FNT (communicator _c, T & a, int root) {\
   auto ad = adaptor_##FNT{_c,root};\
   serialize(ad, a);\
  }

  MAKE_ADAPTOR_AND_FNT(reduce_in_place); 
  MAKE_ADAPTOR_AND_FNT(all_reduce_in_place); 
  MAKE_ADAPTOR_AND_FNT(broadcast); 

#undef MAKE_ADAPTOR_AND_FNT
 };

 // ------------------------------
 // overload for basic types
 // ------------------------------
 template<typename A> struct mpi_impl<A,ENABLE_IFC(std::is_arithmetic<A>::value || triqs::is_complex<A>::value)> {

  static void reduce_in_place (communicator _c, A & a, int root) {
   MPI_Reduce ((_c.rank()==root ? MPI_IN_PLACE:&a),&a,1, mpi_datatype<A>::invoke(), MPI_SUM, root, _c);
  }

  static void all_reduce_in_place (communicator _c, A & a, int root) {
   MPI_Allreduce (&a,1, mpi_datatype<A>::invoke(), MPI_SUM, _c);
  }

  static void broadcast (communicator _c, A & a, int root) { MPI_Bcast (&a,1, mpi_datatype<A>::invoke(), root, _c); }
 };

 // ------------------------------
 // a boost::mpi implementation
 // ------------------------------
 template<typename A> struct boost_mpi_impl {

  static void reduce_in_place (communicator _c, A & a, int root) {
   boost::mpi::reduce(_c,a,a, std::c14::plus<>(), root);
  }

  static void all_reduce_in_place (communicator _c, A & a, int root) {
   boost::mpi::all_reduce(_c,a,a, std::c14::plus<>(), root);
  }

  static void broadcast (communicator _c, A & a, int root) { boost::mpi::broadcast(_c,a,root);}
 };

 // ------------------------------
 // overload for arrays
 // Stragey : if not contigous, we can i) revert to boost::mpi, ii) fail !??
 // ------------------------------
 // When value_type is a basic type, we can directly call the C API
 template<typename A> struct mpi_impl<A,ENABLE_IFC(mpi_datatype<typename A::value_type>::ok && arrays::is_amv_value_or_view_class<A>::value)>  {

  typedef typename A::value_type a_t;

  static void reduce_in_place (communicator _c, A & a, int root) {
   if (!has_contiguous_data(a)) TRIQS_RUNTIME_ERROR << "Non contiguous view in mpi_reduce_in_place"; 
   auto p = a.data_start();
   MPI_Reduce ((_c.rank()==root ? MPI_IN_PLACE:p),p,a.domain().number_of_elements(), mpi_datatype<a_t>::invoke(), MPI_SUM, root, _c);
  }

  static void all_reduce_in_place (communicator _c, A & a, int root) {
   if (!has_contiguous_data(a)) TRIQS_RUNTIME_ERROR << "Non contiguous view in mpi_reduce_in_place"; 
   MPI_Allreduce (MPI_IN_PLACE, a.data_start(), a.domain().number_of_elements(), mpi_datatype<a_t>::invoke(), MPI_SUM, _c);
  }

  static void broadcast (communicator _c, A & a, int root) { 
   if (!has_contiguous_data(a)) TRIQS_RUNTIME_ERROR << "Non contiguous view in mpi_reduce_in_place"; 
   MPI_Bcast (a.data_start(),a.domain().number_of_elements(), mpi_datatype<a_t>::invoke(), root, _c); 
  }

 };

 // When value_type is NOT a basic type, we revert to boost::mpi
 template<typename A> struct mpi_impl<A,ENABLE_IFC(!mpi_datatype<typename A::value_type>::ok && arrays::is_amv_value_or_view_class<A>::value)> : boost_mpi_impl<A>{};
 
 // overload for views rvalues (created on the fly)
 template <typename V, int R, ull_t Opt, ull_t To, bool W> 
  void reduce_in_place( communicator _c, arrays::array_view<V,R,Opt,To, W> && a, int root =0) { reduce_in_place(_c,a,root);}

 template <typename A, typename V, int R, ull_t Opt, ull_t To, bool W> 
  void reduce( communicator _c, A const & a, arrays::array_view<V,R,Opt,To, W> && b, int root =0) { reduce(_c,a,b,root);}

 // to be implemented : scatter, gather for arrays

}}
#endif

