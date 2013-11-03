/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2013 by O. Parcollet
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
#ifndef TRIQS_ARRAYS_H5_LOWLEVEL_H
#define TRIQS_ARRAYS_H5_LOWLEVEL_H
#include <triqs/arrays/array.hpp>
#include <triqs/arrays/vector.hpp>
#include <triqs/h5.hpp>
#include "../cache.hpp"

namespace triqs {
namespace arrays {
 namespace h5_impl {

  template <typename A> const void* __get_array_data_cptr(A const& a) { return h5::get_data_ptr(&(a.storage()[0])); }
  template <typename A> void* __get_array_data_ptr(A& x) { return h5::get_data_ptr(&(x.storage()[0])); }

  // the dataspace corresponding to the array. Contiguous data only...
  template <typename ArrayType> H5::DataSpace data_space(ArrayType const& A) {
   if (!A.indexmap().is_contiguous()) TRIQS_RUNTIME_ERROR << " h5 : internal error : array not contiguous";
   static const unsigned int R = ArrayType::rank;
   mini_vector<hsize_t, R> S;
   auto const& S1(A.indexmap().strides());
   for (int u = 0; u < R; ++u) {
    if (S1[u] <= 0) TRIQS_RUNTIME_ERROR << " negative strides not permitted in h5";
    S[u] = 1;
   }
   static const bool is_complex = boost::is_complex<typename ArrayType::value_type>::value;
   return h5::dataspace_from_LS<R, is_complex>(A.indexmap().domain().lengths(), A.indexmap().domain().lengths(), S);
  }

  /********************   resize or check the size ****************************************************/

  template <typename A> ENABLE_IF(is_amv_value_class<A>) resize_or_check(A& a, mini_vector<size_t, A::rank> const& dimsf) {
   a.resize(indexmaps::cuboid::domain_t<A::rank>(dimsf));
  }

  template <typename A> ENABLE_IF(is_amv_view_class<A>) resize_or_check(A const& a, mini_vector<size_t, A::rank> const& dimsf) {
   if (a.indexmap().domain().lengths() != dimsf)
    TRIQS_RUNTIME_ERROR << "Dimension error : the view can not be resized : "
                        << "\n in file  : " << dimsf.to_string()
                        << "\n in view  : " << a.indexmap().domain().lengths().to_string();
  }

  /*********************************** WRITE array ****************************************************************/
  /*
   * Write an array or a view into an hdf5 file
   * 
   * f The h5 file or group of type H5::H5File or H5::Group
   * name The name of the hdf5 array in the file/group where the stack will be stored
   * A The array to be stored
   * C_reorder bool If true [default] the data will be stored in C order in the hdf5, hence making a temporary
   *        cache of the data to reorder them in memory.
   *        If false, the array is stored as it [if you know what you are doing]
   * The HDF5 exceptions will be caught and rethrown as TRIQS_RUNTIME_ERROR (with a full stackstrace, cf triqs doc).
   */
  template <typename T, int R>
  void write_array_impl(h5::group g, std::string const& name, array_const_view<T, R> A, bool C_reorder) {
   static_assert(!std::is_base_of<std::string, T>::value, " Not implemented"); // 1d is below
   if (C_reorder) {
    write_array_impl(g, name, make_const_cache(A).view(), false);
    return;
   }
   try {
    H5::DataSet ds = g.create_dataset(name, h5::data_type_file<T>(), data_space(A));
    ds.write(__get_array_data_cptr(A), h5::data_type_memory<T>(), data_space(A));
    // if complex, to be python compatible, we add the __complex__ attribute
    if (boost::is_complex<T>::value) h5::write_string_attribute(&ds, "__complex__", "1");
   }
   TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
  }

  template <typename A> void write_array(h5::group g, std::string const& name, A const& a, bool C_reorder = true) {
   write_array_impl(g, name, typename A::const_view_type{a}, C_reorder);
  }

  /*********************************** READ array ****************************************************************/
  /*
   * Read an array or a view from an hdf5 file
   * ArrayType The type of the array/matrix/vector, etc..
   * f The h5 file or group of type H5::H5File or H5::Group
   * name The name of the hdf5 array in the file/group where the stack will be stored
   * A The array to be stored
   * C_reorder bool If true [default] the data will be stored in C order in the hdf5, hence making a temporary
   *        cache of the data to reorder them in memory. If false, the array is stored as it [if you know what you are doing]
   * The HDF5 exceptions will be caught and rethrown as TRIQS_RUNTIME_ERROR (with a full stackstrace, cf triqs doc).
   */
  template <typename ArrayType1> void read_array(h5::group g, std::string const& name, ArrayType1&& A, bool C_reorder = true) {
   typedef typename std::remove_reference<ArrayType1>::type ArrayType;
   static_assert(!std::is_base_of<std::string, typename ArrayType::value_type>::value, " Not implemented"); // 1d is below
   try {
    H5::DataSet ds = g.open_dataset(name);
    H5::DataSpace dataspace = ds.getSpace();
    static const unsigned int Rank = ArrayType::rank + (boost::is_complex<typename ArrayType::value_type>::value ? 1 : 0);
    int rank = dataspace.getSimpleExtentNdims();
    if (rank != Rank)
     TRIQS_RUNTIME_ERROR << "triqs::array::h5::read. Rank mismatch : the array has rank = " << Rank
                         << " while the array stored in the hdf5 file has rank = " << rank;
    mini_vector<hsize_t, Rank> dims_out;
    dataspace.getSimpleExtentDims(&dims_out[0], NULL);
    mini_vector<size_t, ArrayType::rank> d2;
    for (size_t u = 0; u < ArrayType::rank; ++u) d2[u] = dims_out[u];
    resize_or_check(A, d2);
    if (C_reorder) {
     read_array(g, name, make_cache(A).view(), false);
     //read_array(g, name, cache<ArrayType, typename ArrayType::regular_type>(A).view(), false);
    } else
     ds.read(__get_array_data_ptr(A), h5::data_type_memory<typename ArrayType::value_type>(), data_space(A), dataspace);
   }
   TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
  }

  // overload : special treatment for arrays of strings (one dimension only).
  inline void write_array(h5::group f, std::string const& name, vector_const_view<std::string> V) {
   h5::detail::write_1darray_vector_of_string_impl(f, name, V);
  }

  inline void write_array(h5::group f, std::string const& name, array_const_view<std::string, 1> V) {
   write_array(f, name, vector_const_view<std::string>(V));
  }

  inline void read_array(h5::group f, std::string const& name, arrays::vector<std::string>& V) {
   h5::detail::read_1darray_vector_of_string_impl(f, name, V);
  }

  // I can not use the generic code, just because the resize of the array take a shape,  not a size_t as std::vector and vector
  inline void read_array(h5::group f, std::string const& name, arrays::array<std::string, 1>& V) {
   arrays::vector<std::string> res;
   read_array(f, name, res);
   V = res;
  }

 } // namespace h5impl

 // a trait to detect if A::value_type exists and is a scalar or a string
 // used to exclude array<array<..>>
 template <typename A, typename Enable = void> struct has_scalar_or_string_value_type : std::false_type {};
 template <typename A>
 struct has_scalar_or_string_value_type<
     A, decltype(nop(std::declval<
            typename A::value_type>()))> : std::integral_constant<bool, is_scalar<typename A::value_type>::value ||
                                                                            std::is_base_of<std::string,
                                                                                            typename A::value_type>::value> {};
 // get_triqs_hdf5_data_scheme 
 template <typename ArrayType>
 TYPE_ENABLE_IFC(std::string, is_amv_value_or_view_class<ArrayType>::value) get_triqs_hdf5_data_scheme(ArrayType const&) {
  using triqs::get_triqs_hdf5_data_scheme; // for the basic types, not found by ADL
  std::stringstream fs;
  fs << "array<" << get_triqs_hdf5_data_scheme(typename ArrayType::value_type()) << "," << ArrayType::rank << ">";
  return fs.str();
 }

 /*
  * Read an array or a view from an hdf5 file
  * ArrayType The type of the array/matrix/vector, etc..
  * g The h5 group
  * name The name of the hdf5 array in the file/group where the stack will be stored
  * A The array to be stored
  * The HDF5 exceptions will be caught and rethrown as TRIQS_RUNTIME_ERROR (with a full stackstrace, cf triqs doc).
  */
 template <typename ArrayType>
 ENABLE_IFC(is_amv_value_or_view_class<ArrayType>::value&& has_scalar_or_string_value_type<ArrayType>::value)
     h5_read(h5::group g, std::string const& name, ArrayType& A) {
  h5_impl::read_array(g, name, A);
 }

 /*
  * Write an array or a view into an hdf5 file
  * ArrayType The type of the array/matrix/vector, etc..
  * g The h5 group
  * name The name of the hdf5 array in the file/group where the stack will be stored
  * A The array to be stored
  * The HDF5 exceptions will be caught and rethrown as TRIQS_RUNTIME_ERROR (with a full stackstrace, cf triqs doc).
  */
 template <typename ArrayType>
 ENABLE_IFC(is_amv_value_or_view_class<ArrayType>::value&& has_scalar_or_string_value_type<ArrayType>::value)
     h5_write(h5::group g, std::string const& name, ArrayType const& A) {
  h5_impl::write_array(g, name, array_const_view<typename ArrayType::value_type, ArrayType::rank>(A));
 }
}
}
#endif

