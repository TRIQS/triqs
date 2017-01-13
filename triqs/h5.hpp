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
#ifndef TRIQS_H5_FULL_H
#define TRIQS_H5_FULL_H
#include "./h5/base_public.hpp"
#include "./h5/file.hpp"
#include "./h5/group.hpp"
#include "./h5/scalar.hpp"
#include "./h5/string.hpp"
#include "./h5/vector.hpp"
#include "./h5/map.hpp"
#include "./h5/optional.hpp"

// traits to detect h5_read/h5_write is overloaded. Unused currently. Kept since
// it can useful and is it not so simple to do...
namespace triqs {
namespace h5 {
 template <typename T, typename Enable = void> struct has_h5_read : std::false_type {};
 template <typename T>
 struct has_h5_read<T, decltype(h5_read(std::declval<h5::group>(), std::string(), *(std::declval<T*>())))> : std::true_type {};

 template <typename T, typename Enable = void> struct has_h5_write : std::false_type {};
 template <typename T>
 struct has_h5_write<T, decltype(h5_write(std::declval<h5::group>(), std::string(), std::declval<const T>()))> : std::true_type {
 };

// A generic read
 template <typename T> T h5_read(group gr, std::string const& name) {
  T x;
  h5_read(gr, name, x);
  return x;
 }

 /// Returns the attribute name of obj, and "" if the attribute does not exist.
 template <typename T> T h5_read_attribute(hid_t id, std::string const& name) {
  using triqs::h5::h5_read_attribute;
  T x;
  h5_read_attribute(id, name, x);
  return x;
 }
}
}

// in some old version of hdf5 (Ubuntu 12.04 e.g.), the macro is not yet defined.
#ifndef H5_VERSION_GE

#define H5_VERSION_GE(Maj, Min, Rel)                                                                                             \
 (((H5_VERS_MAJOR == Maj) && (H5_VERS_MINOR == Min) && (H5_VERS_RELEASE >= Rel)) ||                                              \
  ((H5_VERS_MAJOR == Maj) && (H5_VERS_MINOR > Min)) || (H5_VERS_MAJOR > Maj))

#endif

#endif


