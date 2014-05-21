/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2014 by O. Parcollet
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
#include "./simple_read_write.hpp"

using dcomplex = std::complex<double>;
namespace triqs {
namespace arrays {
 namespace h5_impl {

  // the dataspace corresponding to the array. Contiguous data only...
  H5::DataSpace data_space_impl(array_stride_info info, bool is_complex) { 
   hsize_t L[info.R], S[info.R];
   for (int u = 0; u < info.R; ++u) {
    if (info.strides[u] <= 0) TRIQS_RUNTIME_ERROR << " negative strides not permitted in h5";
    S[u] = 1;
    L[u] = info.lengths[u];
   }
   return h5::dataspace_from_LS(info.R, is_complex, L, L, S);
  }

  /// --------------------------- WRITE ---------------------------------------------
  
  template <typename T> void write_array_impl(h5::group g, std::string const& name, const T* start, array_stride_info info) {
   static_assert(!std::is_base_of<std::string, T>::value, " Not implemented"); // 1d is below
   bool is_complex = triqs::is_complex<T>::value;
   try {
    H5::DataSet ds = g.create_dataset(name, h5::data_type_file<T>(), data_space_impl(info, is_complex));
    ds.write(h5::get_data_ptr(start), h5::data_type_memory<T>(), data_space_impl(info, is_complex));
    // if complex, to be python compatible, we add the __complex__ attribute
    if (is_complex) h5::write_string_attribute(&ds, "__complex__", "1");
   }
   TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
  }

  template void write_array_impl<int>(h5::group g, std::string const& name, const int* start, array_stride_info info);
  template void write_array_impl<long>(h5::group g, std::string const& name, const long* start, array_stride_info info);
  template void write_array_impl<double>(h5::group g, std::string const& name, const double* start, array_stride_info info);
  template void write_array_impl<dcomplex>(h5::group g, std::string const& name, const dcomplex* start, array_stride_info info);


  // overload : special treatment for arrays of strings (one dimension only).
  void write_array(h5::group g, std::string const& name, vector_const_view<std::string> V) {
   std::vector<std::string> tmp(V.size());
   std::copy(begin(V), end(V), begin(tmp));
   h5_write(g, name, tmp);
  }

  void write_array(h5::group g, std::string const& name, array_const_view<std::string, 1> V) {
   std::vector<std::string> tmp(first_dim(V));
   std::copy(begin(V), end(V), begin(tmp));
   h5_write(g, name, tmp);
  }

  /// --------------------------- READ ---------------------------------------------
 
 
/* template <typename ArrayType1> void read_array(h5::group g, std::string const& name, ArrayType1&& A, bool C_reorder = true) {
   typedef typename std::remove_reference<ArrayType1>::type ArrayType;
   static_assert(!std::is_base_of<std::string, typename ArrayType::value_type>::value, " Not implemented"); // 1d is below
   try {
    H5::DataSet ds = g.open_dataset(name);
    H5::DataSpace dataspace = ds.getSpace();
    static const unsigned int Rank = ArrayType::rank + (triqs::is_complex<typename ArrayType::value_type>::value ? 1 : 0);
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
*/
 
  std::vector<size_t> get_array_lengths(int R, h5::group g, std::string const& name, bool is_complex) { 
   try {
    H5::DataSet ds = g.open_dataset(name);
    H5::DataSpace dataspace = ds.getSpace();
    int Rank = R + (is_complex ? 1 : 0);
    int rank = dataspace.getSimpleExtentNdims();
    if (rank != Rank)
     TRIQS_RUNTIME_ERROR << "triqs::array::h5::read. Rank mismatch : the array has rank = " << Rank
                         << " while the array stored in the hdf5 file has rank = " << rank;
    std::vector<size_t> d2(R);
    hsize_t dims_out[rank];
    dataspace.getSimpleExtentDims(&dims_out[0], NULL);
    for (int u = 0; u < R; ++u) d2[u] = dims_out[u];
    return d2;
   }
   TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
  }

  template <typename T> void read_array_impl(h5::group g, std::string const& name, T* start, array_stride_info info) { 
   static_assert(!std::is_base_of<std::string, T>::value, " Not implemented"); // 1d is below
   bool is_complex = triqs::is_complex<T>::value;
   try {
    H5::DataSet ds = g.open_dataset(name);
    H5::DataSpace dataspace = ds.getSpace();
    ds.read(h5::get_data_ptr(start), h5::data_type_memory<T>(), data_space_impl(info, is_complex), dataspace);
   }
   TRIQS_ARRAYS_H5_CATCH_EXCEPTION;
  }

  template void read_array_impl<int>(h5::group g, std::string const& name, int* start, array_stride_info info);
  template void read_array_impl<long>(h5::group g, std::string const& name, long* start, array_stride_info info);
  template void read_array_impl<double>(h5::group g, std::string const& name, double* start, array_stride_info info);
  template void read_array_impl<dcomplex>(h5::group g, std::string const& name, dcomplex* start, array_stride_info info);

  void read_array(h5::group g, std::string const& name, arrays::vector<std::string>& V) {
   std::vector<std::string> tmp;
   h5_read(g, name, tmp);
   V.resize(tmp.size());
   std::copy(begin(tmp), end(tmp), begin(V));
  }

  // I can not use the generic code, just because the resize of the array take a shape,  not a size_t as std::vector and vector
  void read_array(h5::group f, std::string const& name, arrays::array<std::string, 1>& V) {
   arrays::vector<std::string> res;
   read_array(f, name, res);
   V = res;
  }
 }
}
}
