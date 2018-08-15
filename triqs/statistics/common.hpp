/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2017 by O. Parcollet, T. Ayral.
 * Copyright (C) 2018 by Simons Foundation
 *   author : O. Parcollet, H. Strand, P. Dumitrescu
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
#include <triqs/utility/itertools.hpp>
#include <triqs/mpi/vector.hpp>
#include <type_traits>
#include <vector>
#include "./stacked_array.hpp"
#include "./stacked_gf.hpp"

namespace triqs::stat {

  template <typename T, typename Enable = void> struct _vec_t { using type = std::vector<T>; };

  template <typename A> struct _vec_t<A, std::enable_if_t<is_amv_value_or_view_class<A>::value>> {
    using type = stacked_array<typename A::value_type, A::rank>;
  };

  template <typename A> struct _vec_t<A, std::enable_if_t<gfs::is_gf<A>::value>> { using type = gfs::stacked_gf<A>; };

  template <typename T> using vec_t = typename _vec_t<T>::type;

  template <typename T> class accumulator;

  template <typename V> auto sum_vectorized(V const &v) {
    auto r = v[0];
    for (int i = 1; i < v.size(); ++i) r += v[i];
    return r;
  }
} // namespace triqs::stat

namespace triqs::h5 {

  // The class provides 
  // template <typename F> void h5_serialize(F &&f) { f("a", a), f("b", b, h5::as_attribute), ...
  // and 
  // friend class h5::access;
  // and the h5_read and write are automatically enabled.
  //
  struct _as_attribute_t {};
  static inline _as_attribute_t as_attribute;

  class access {
    struct _h5_writer {
      group gr;
      template <typename T> void operator()(std::string const &name, T const &x) { h5_write(gr, name, x); }
      template <typename T> void operator()(std::string const &name, T const &x, _as_attribute_t) { h5_write_attribute(gr, name, x); }
    };

    struct _h5_reader {
      group gr;
      template <typename T> void operator()(std::string const &name, T &x) { h5_read(gr, name, x); }
      template <typename T> void operator()(std::string const &name, T &x, _as_attribute_t) { h5_read_attribute(gr, name, x); }
    };

    public:
    template <typename T> static auto write(group g, T const &x) -> decltype(const_cast<T &>(x).h5_serialize(_h5_writer{g})) {
      const_cast<T &>(x).h5_serialize(_h5_writer{g});
    }
    template <typename T> static auto read(group g, T &x) -> decltype(x.h5_serialize(_h5_reader{g})) { x.h5_serialize(_h5_reader{g}); }
  };

  template <typename T> auto h5_write(h5::group g, std::string const &name, T const &x) -> decltype(access::write(g, x)) {
    auto gr = g.create_group(name);
    h5_write_attribute(gr, "TRIQS_HDF5_data_scheme", T::hdf5_scheme());
    access::write(gr, x);
  }

  template <typename T> auto h5_read(h5::group g, std::string const &name, T &x) -> decltype(access::read(g, x)) {
    auto gr = g.open_group(name);
    // Check scheme
    access::read(gr, x);
  }

} // namespace triqs::h5
