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
#ifndef TRIQS_ARRAYS_H5_STACK_H
#define TRIQS_ARRAYS_H5_STACK_H
#include "../array.hpp"
#include <triqs/h5.hpp>
#include "./simple_read_write.hpp"
#include <triqs/h5/base.hpp>

namespace triqs {
namespace arrays {

 // to be cleaned
 namespace h5_impl {
  template <typename A> void* __get_array_data_ptr(A& x) { return h5::get_data_ptr(&(x.storage()[0])); }

  h5::dataspace data_space_impl(array_stride_info info, bool is_complex);

  template <typename ArrayType> h5::dataspace data_space(ArrayType const& A) {
   if (!A.indexmap().is_contiguous()) TRIQS_RUNTIME_ERROR << " h5 : internal error : array not contiguous";
   return data_space_impl(array_stride_info{A}, triqs::is_complex<typename ArrayType::value_type>::value);
  }
 }

 /// The implementation class
 template <typename T, int R> class array_stack_impl {
  static const size_t dim = R;
  static const bool base_is_array = dim > 0;
  size_t bufsize_, step, _size;
  static const bool T_is_complex = triqs::is_complex<T>::value;
  static const unsigned int RANK = dim + 1 + (T_is_complex ? 1 : 0);
  utility::mini_vector<hsize_t, RANK> dims, offset, maxdims, dim_chunk, buffer_dim, zero;
  h5::dataset d_set;
  array<T, dim + 1> buffer;

  public:
  array_stack_impl(h5::group g, std::string const &name, mini_vector<size_t, dim> const &base_element_shape, size_t bufsize) {
   mini_vector<hsize_t, RANK> dim_chunk;
   bufsize_ = bufsize;
   step = 0;
   _size = 0;
   for (size_t i = 1; i <= dim; ++i) {
    dims[i] = base_element_shape[i - 1];
   }
   if (T_is_complex) {
    dims[RANK - 1] = 2;
   }
   maxdims = dims;
   buffer_dim = dims;
   dim_chunk = dims;
   dims[0] = 0;
   maxdims[0] = H5S_UNLIMITED;
   dim_chunk[0] = 1;
   buffer_dim[0] = bufsize_;
   mini_vector<size_t, dim + 1> s;
   for (size_t i = 0; i <= dim; ++i) {
    s[i] = buffer_dim[i];
   }
   buffer.resize(s);
   h5::dataspace mspace1 = H5Screate_simple(RANK, dims.ptr(), maxdims.ptr());
   h5::proplist cparms = H5Pcreate(H5P_DATASET_CREATE);
   //auto err = H5Pset_chunk(cparms, RANK, dim_chunk.ptr());
   H5Pset_chunk(cparms, RANK, dim_chunk.ptr());
   d_set = g.create_dataset(name, h5::native_type_from_C(T()), mspace1, cparms);
   if (triqs::is_complex<T>::value) h5::write_string_attribute(d_set, "__complex__", "1");
  }

  ///
  ~array_stack_impl() { flush(); }

#ifdef TRIQS_DOXYGEN
  /// A view (for an array/matrix/vector base) or a reference (for a scalar base) to the top of the stack i.e. the next element to be assigned to
  auto operator()() { return buffer(step, ellipsis()); }

  /// A view (for an array/matrix/vector base) or a reference (for a scalar base) to the top of the stack i.e. the next element to be assigned to
  auto operator()() const { return buffer(step, ellipsis()); }
#else
  auto operator()() DECL_AND_RETURN(buffer(step, ellipsis()));
  auto operator()() const DECL_AND_RETURN(buffer(step, ellipsis()));
#endif

  /// Advance the stack by one
  void operator++() {
   ++step;
   ++_size;
   if (step == bufsize_) flush();
  }

  /// Flush the buffer to the disk. Automatically called at destruction.
  void flush() {
   save_buffer();
   step = 0;
  }

  /**
   * \brief Add a element onto the stack and advance it by one.
   * S << A is equivalent to S() = A; ++S;
   */
  template <class AType> void operator<<(AType const &A) {
   (*this)() = A;
   ++(*this);
  }

  /// Current size of the stack
  size_t size() const { return _size; }

  private:
  void save_buffer() {
   if (step == 0) return;
   dims[0] += step;
   buffer_dim[0] = step;

   herr_t err= H5Dset_extent(d_set,dims.ptr());  // resize the data_space

   h5::dataspace fspace1 = H5Dget_space(d_set);
   h5::dataspace mspace = h5_impl::data_space(buffer);

   err = H5Sselect_hyperslab(fspace1, H5S_SELECT_SET, offset.ptr(), NULL, buffer_dim.ptr(), NULL);
   if (err < 0) TRIQS_RUNTIME_ERROR << "Cannot set hyperslab";
   err = H5Sselect_hyperslab(mspace, H5S_SELECT_SET, zero.ptr(), NULL, buffer_dim.ptr(), NULL);
   if (err < 0) TRIQS_RUNTIME_ERROR << "Cannot set hyperslab";

   err = H5Dwrite(d_set, h5::data_type_memory<T>(), mspace, fspace1, H5P_DEFAULT, h5_impl::__get_array_data_ptr(buffer));
   if (err < 0) TRIQS_RUNTIME_ERROR << "Error writing the array_stack buffer";
   offset[0] += step;
  }
 };

 // ------------------------- User classes ------------------------------

 // The simple case, 1d
 template <typename T> class array_stack : public array_stack_impl<T, 0> {
  static_assert((is_scalar<T>::value), "Only available for a scalar type");
  public:
  /**
   * \brief Constructor
   *  \param g The h5 group
   *  \param name The name of the hdf5 array in the file/group where the stack will be stored
   *  \param bufsize The size of the buffer
   *  \exception The HDF5 exceptions will be caught and rethrown as TRIQS_RUNTIME_ERROR (with stackstrace, cf doc).
   */
  array_stack(h5::group g, std::string const &name, size_t bufsize)
     : array_stack_impl<T, 0>{g, name, mini_vector<size_t, 0>{}, bufsize} {}
 };

 // Specialisation for The simple case, 1d
 template <typename T, int N> class array_stack<array<T, N>> : public array_stack_impl<T, N> {
  public:
   /**
    * \brief Constructor
    *  \param g The h5 group
    *  \param name The name of the hdf5 array in the file/group where the stack will be stored
    *  \param base_element_shape The shape of the base array of the stack.
    *  \param bufsize The size of the buffer
    *  \exception The HDF5 exceptions will be caught and rethrown as TRIQS_RUNTIME_ERROR (with stackstrace, cf doc).
    */
  array_stack(h5::group g, std::string const &name, mini_vector<size_t,N> const &base_element_shape, size_t bufsize)
     : array_stack_impl<T, N>{g, name, base_element_shape, bufsize} {}
 };
}
} // namespace
#endif

