#pragma once
#include <triqs/h5.hpp>
#include <triqs/h5/array_interface.hpp>

#include <algorithm>

#include "./array.hpp"

namespace triqs::arrays {

  namespace h5_details {

    using h5::v_t;
    // in cpp to diminish template instantiations
    void write(h5::group g, std::string const &name, h5::datatype ty, void *start, int rank, bool is_complex, size_t const *lens, long const *strides,
               size_t total_size);

    // FIXME almost the same code as for vector. Factorize this ?
    // For the moment, 1d only : easy to implement, just change the construction of the lengths
    template <typename A> h5::char_buf to_char_buf(A const &v) REQUIRES(is_regular_or_view_v<A>) {
      static_assert(A::rank == 1, "H5 for array<string, N> for N>1 not implemented");
      size_t s = 0;
      for (auto &x : v) s = std::max(s, x.size() + 1);
      auto len = v_t{size_t(v.size()), s};

      std::vector<char> buf;
      buf.resize(v.size() * s, 0x00);
      size_t i = 0;
      for (auto &x : v) {
        strcpy(&buf[i * s], x.c_str());
        ++i;
      }
      return {buf, len};
    }

    template <typename A> void from_char_buf(h5::char_buf const &cb, A &v) REQUIRES(is_regular_or_view_v<A>) {
      static_assert(A::rank == 1, "H5 for array<string, N> for N>1 not implemented");
      v.resize(cb.lengths[0]);
      auto len_string = cb.lengths[1];

      size_t i = 0;
      for (auto &x : v) {
        x = "";
        x.append(&cb.buffer[i * len_string]);
        ++i;
      }
    }

  } // namespace h5_details

  /*
   * Write an array or a view into an hdf5 file
   * ArrayType The type of the array/matrix/vector, etc..
   * g The h5 group
   * name The name of the hdf5 array in the file/group where the stack will be stored
   * A The array to be stored
   * The HDF5 exceptions will be caught and rethrown as TRIQS_RUNTIME_ERROR (with a full stackstrace, cf triqs doc).
   */
  template <typename A> void h5_write(h5::group g, std::string const &name, A const &a) REQUIRES(is_regular_or_view_v<A>) {

    // If we are not in C_LAYOUT copy to C_LAYOUT array and write
    if constexpr (is_matrix_or_view_v<A> or is_array_or_view_v<A>) {
      if (not a.indexmap().memory_layout().is_c()) {
        typename A::regular_type a_c_layout{a.shape()};
        a_c_layout() = a;
        h5_write(g, name, a_c_layout);
        return;
      }
    }

    // first case array of string
    if constexpr (std::is_same_v<typename A::value_type, std::string>) { // special case of string. Like vector of string

      h5_write(g, name, h5_details::to_char_buf(a));

    } else if constexpr (is_scalar_v<typename A::value_type>) { // FIXME : register types as USER DEFINED hdf5 types

      static constexpr bool is_complex = triqs::h5::is_complex_v<typename A::value_type>;
      h5_details::write(g, name, h5::hdf5_type<typename A::value_type>(), (void *)(a.data_start()), A::rank, is_complex,
                        a.indexmap().lengths().data(), a.indexmap().strides().data(), a.size());

    } else { // generic unknown type to hdf5
      if (a.is_empty()) return;
      auto g2 = g.create_group(name);
      array<int, 1> sha(A::rank);
      for (int u = 0; u < A::rank; ++u) sha(u) = a.shape()[u];
      h5_write(g2, "shape", sha);
      auto make_name = [](auto i0, auto... is) { return (std::to_string(i0) + ... + ("_" + std::to_string(is))); };
      foreach(a, [&](auto... is) { h5_write(g2, make_name(is...), a(is...)); });
    }
  }

  /*
   * Read an array or a view from an hdf5 file
   * ArrayType The type of the array/matrix/vector, etc..
   * g The h5 group
   * name The name of the hdf5 array in the file/group where the stack will be stored
   * A The array to be stored
   * The HDF5 exceptions will be caught and rethrown as std::runtime_error (with a full stackstrace, cf doc).
   */
  template <typename A> void h5_read(h5::group g, std::string const &name, A &a) REQUIRES(is_regular_or_view_v<A>) {

    // If we are not in C_LAYOUT read into C_LAYOUT array and copy
    if constexpr (is_matrix_or_view_v<A> or is_array_or_view_v<A>) {
      if (not a.indexmap().memory_layout().is_c()) {
        typename A::regular_type a_c_layout;
        h5_read(g, name, a_c_layout);
        a.resize(a_c_layout.shape());
        a() = a_c_layout;
        return;
      }
    }

    // first case array of string
    if constexpr (std::is_same_v<typename A::value_type, std::string>) { // special case of string. Like vector of string

      h5::char_buf cb;
      h5_read(g, name, cb);
      h5_details::from_char_buf(cb, a);

    } else if constexpr (is_scalar_v<typename A::value_type>) { // FIXME : register types as USER DEFINED hdf5 types

      static constexpr bool is_complex = triqs::h5::is_complex_v<typename A::value_type>;

      auto lt = h5::array_interface::get_h5_lengths_type(g, name);

      if (is_complex && !lt.has_complex_attribute) { // if not complex in file, we load in real and assign
        array<double, A::rank> tmp;
        h5_read(g, name, tmp);
        a = tmp;
        return;
      }

      int rank_in_file = lt.rank() - (is_complex ? 1 : 0);
      if (rank_in_file != A::rank)
        TRIQS_RUNTIME_ERROR << " h5 read of array : incorrect rank. In file: " << rank_in_file << "  In memory " << A::rank;
      shape_t<A::rank> L;

      for (int u = 0; u < A::rank; ++u) L[u] = lt.lengths[u]; // NB : correct for complex

      if constexpr (is_regular_v<A>) {
        a.resize(typename A::domain_type{mini_vector<size_t, A::rank>{L}});
      } else {
        if (a.shape() != L)
          TRIQS_RUNTIME_ERROR << "Error trying to read from an hdf5 file to a view. Dimension mismatch"
                              << "\n in file  : " << L << "\n in view  : " << a.shape();
      }

      h5::array_interface::h5_array_view v{h5::hdf5_type<typename A::value_type>(), (void *)(a.data_start()), A::rank, is_complex};
      for (int u = 0; u < A::rank; ++u) {
        v.slab.count[u] = L[u];
        v.L_tot[u]      = L[u];
      }
      h5::array_interface::read(g, name, v, lt);

    } else { // generic unknown type to hdf5
      auto g2 = g.open_group(name);
      auto sha2 = a.shape();
      array<int, 1> sha;
      h5_read(g2, "shape", sha);
      if (first_dim(sha) != sha2.size())
        TRIQS_RUNTIME_ERROR << " array<array<...>> load : rank mismatch. Expected " << sha2.size() << " Got " << first_dim(sha);
      for (int u = 0; u < sha2.size(); ++u) sha2[u] = sha(u);
      if (a.shape() != sha2) a.resize(sha2);
      auto make_name = [](auto i0, auto... is) { return (std::to_string(i0) + ... + ("_" + std::to_string(is))); };
      foreach(a, [&](auto... is) { h5_read(g2, make_name(is...), a(is...)); });
    }
  }

} // namespace triqs::arrays
