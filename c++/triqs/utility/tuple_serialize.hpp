// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Boost.Serialization support for `std::tuple`.
 */

#pragma once

#include <tuple>

namespace boost::serialization {

  // Recursive helper that serializes the element at position `tuple_size - 1 - pos`.
  template <int pos> struct tuple_serialize_impl {
    template <typename Archive, typename T> void operator()(Archive &ar, T &t) {
      ar &std::get<std::tuple_size_v<T> - 1 - pos>(t);
      tuple_serialize_impl<pos - 1>()(ar, t);
    }
  };

  // Base case of tuple_serialize_impl — serializes the last element.
  template <> struct tuple_serialize_impl<0> {
    template <typename Archive, typename T> void operator()(Archive &ar, T &t) { ar &std::get<std::tuple_size_v<T> - 1>(t); }
  };

  /**
   * @ingroup triqs-utility-tuple
   * @brief Boost.Serialization entry point for `std::tuple`.
   * 
   * @tparam Archive Archive type.
   * @tparam Ts Tuple element types.
   * @param ar Boost archive.
   * @param t Tuple to serialize.
   * @param version Unused version required by the Boost.Serialization protocol.
   */
  template <typename Archive, typename... Ts> void serialize(Archive &ar, std::tuple<Ts...> &t, [[maybe_unused]] const unsigned int version) {
    tuple_serialize_impl<sizeof...(Ts) - 1>()(ar, t);
  }

} // namespace boost::serialization
