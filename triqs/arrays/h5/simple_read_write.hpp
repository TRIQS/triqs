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
#pragma once
#include <triqs/arrays/array.hpp>
#include <triqs/arrays/vector.hpp>
#include <triqs/h5.hpp>
#include "../cache.hpp"

namespace triqs {
  namespace arrays {
    namespace h5_impl {

      struct array_stride_info {
        int R;
        size_t const *lengths;
        std::ptrdiff_t const *strides;
        template <typename A> explicit array_stride_info(A &&a) {
          R       = std::c14::decay_t<A>::rank;
          lengths = a.indexmap().domain().lengths().ptr();
          strides = a.indexmap().strides().ptr();
        }
      };

      /********************   resize or check the size ****************************************************/

      template <typename A> ENABLE_IF(is_amv_value_class<A>) resize_or_check(A &a, mini_vector<size_t, A::rank> const &dimsf) {
        a.resize(indexmaps::cuboid::domain_t<A::rank>(dimsf));
      }

      template <typename A> ENABLE_IF(is_amv_view_class<A>) resize_or_check(A const &a, mini_vector<size_t, A::rank> const &dimsf) {
        if (a.indexmap().domain().lengths() != dimsf)
          TRIQS_RUNTIME_ERROR << "Dimension error : the view cannot be resized : "
                              << "\n in file  : " << dimsf.to_string() << "\n in view  : " << a.indexmap().domain().lengths().to_string();
      }

      /*********************************** WRITE array ****************************************************************/

      template <typename T> void write_array_impl(h5::group g, std::string const &name, const T *start, array_stride_info info);

      template <typename A> void write_array(h5::group g, std::string const &name, A const &a, bool C_reorder = true) {
        if (C_reorder) {
          auto b = make_const_cache(a).view();
          write_array_impl(g, name, b.data_start(), array_stride_info{b});
        } else
          write_array_impl(g, name, a.data_start(), array_stride_info{a});
      }

      // overload : special treatment for arrays of strings (one dimension only).
      void write_array(h5::group g, std::string const &name, vector_const_view<std::string> V);
      void write_array(h5::group g, std::string const &name, array_const_view<std::string, 1> V);

      /*********************************** READ array ****************************************************************/

      std::vector<size_t> get_array_lengths(int R, h5::group g, std::string const &name, bool is_complex);
      template <typename T> void read_array_impl(h5::group g, std::string const &name, T *start, array_stride_info info);
      template <typename A> void read_array1(h5::group g, std::string const &name, A &a, bool C_reorder);

      bool is_dataset_complex(h5::group g, std::string const &name);

      template <typename A> void read_array(h5::group g, std::string const &name, A &&a, bool C_reorder = true) {
        read_array1(g, name, a, C_reorder);
      }

      template <typename A> void read_array1(h5::group g, std::string const &name, A &a, bool C_reorder) {
        constexpr bool is_complex = triqs::is_complex<typename A::value_type>::value;

        if (is_complex && !is_dataset_complex(g, name)) { // if not complex in file, we load in real and assign
          array<double, A::rank> tmp;
          read_array(g, name, tmp);
          a = tmp;
          return;
        }

        // mini_vector... : useless on 4.9 and clang, there seems to be a bug (??) on 4.8.
        resize_or_check(a, mini_vector<size_t, A::rank>(get_array_lengths(a.rank, g, name, is_complex)));
        if (C_reorder) {
          auto b = make_cache(a);
          read_array_impl(g, name, b.view().data_start(), array_stride_info{b.view()});
        } else
          read_array_impl(g, name, a.data_start(), array_stride_info{a});
      }

      // overload : special treatment for arrays of strings (one dimension only).
      void read_array(h5::group g, std::string const &name, arrays::vector<std::string> &V);
      void read_array(h5::group f, std::string const &name, arrays::array<std::string, 1> &V);

    } // namespace h5_impl

    // a trait to detect if A::value_type exists and is a scalar or a string
    // used to exclude array<array<..>>
    template <typename A, typename Enable = void> struct has_scalar_or_string_value_type : std::false_type {};
    template <typename A>
    struct has_scalar_or_string_value_type<A, std::void_t<typename A::value_type>>
       : std::integral_constant<bool, is_scalar<typename A::value_type>::value || std::is_base_of<std::string, typename A::value_type>::value> {};

    /*
  * Read an array or a view from an hdf5 file
  * ArrayType The type of the array/matrix/vector, etc..
  * g The h5 group
  * name The name of the hdf5 array in the file/group where the stack will be stored
  * A The array to be stored
  * The HDF5 exceptions will be caught and rethrown as TRIQS_RUNTIME_ERROR (with a full stackstrace, cf triqs doc).
  */
    template <typename ArrayType>
    ENABLE_IFC(is_amv_value_or_view_class<ArrayType>::value &&has_scalar_or_string_value_type<ArrayType>::value)
    h5_read(h5::group g, std::string const &name, ArrayType &A) {
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
    ENABLE_IFC(is_amv_value_or_view_class<ArrayType>::value &&has_scalar_or_string_value_type<ArrayType>::value)
    h5_write(h5::group g, std::string const &name, ArrayType const &A) {
      h5_impl::write_array(g, name, array_const_view<typename ArrayType::value_type, ArrayType::rank>(A));
    }

  } // namespace arrays
} // namespace triqs
