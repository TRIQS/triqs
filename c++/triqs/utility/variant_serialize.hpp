// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
// Copyright (c) 2015 Igor Krivenko
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
// Authors: Igor Krivenko, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Boost.Serialization support for `std::variant`.
 */

#pragma once

#include <boost/serialization/split_free.hpp>

#include <variant>

namespace boost::serialization {

  /**
   * @addtogroup triqs-utility-tuple
   * @{
   */

  // Visitor that saves the held alternative to a Boost archive.
  template <typename Archive> struct variant_serialize_saver {
    Archive &ar; // NOLINT
    variant_serialize_saver(Archive &ar) : ar(ar) {}
    template <typename T> void operator()(T x) { ar << x; }
  };

  // Visitor that loads an alternative from a Boost archive (optionally placement-new-constructing the slot first).
  template <typename Archive> struct variant_serialize_loader {
    Archive &ar; // NOLINT
    bool initialize;
    variant_serialize_loader(Archive &ar, bool initialize) : ar(ar), initialize(initialize) {}
    template <typename T> void operator()(T &x) {
      if (initialize) ::new (&x) T();
      ar >> x;
    }
  };

  /**
   * @brief Serialize a `std::variant`.
   * 
   * @warning This is unused. It might be removed in the future.
   * 
   * @tparam Archive Archive type.
   * @tparam Ts Alternative types of the variant.
   * @param ar Boost archive.
   * @param v Variant to serialize.
   * @param version Unused version required by the Boost.Serialization protocol.
   */
  template <typename Archive, typename... Ts> void save(Archive &ar, std::variant<Ts...> const &v, [[maybe_unused]] const unsigned int version) {
    ar << v.type_id;
    visit(variant_serialize_saver<Archive>(ar), v);
  }

  /**
   * @brief Deserialize a `std::variant`.
   * 
   * @warning This is unused. It might be removed in the future.
   * 
   * @tparam Archive Archive type.
   * @tparam Ts Alternative types of the variant.
   * @param ar Boost archive.
   * @param v Variant to deserialize into.
   * @param version Unused version required by the Boost.Serialization protocol.
   */
  template <typename Archive, typename... Ts> void load(Archive &ar, std::variant<Ts...> &v, [[maybe_unused]] const unsigned int version) {
    int new_type_id{};
    ar >> new_type_id;
    if (v.type_id != new_type_id) {
      v.destroy();
      v.type_id = new_type_id;
      visit(variant_serialize_loader<Archive>(ar, true), v);
    } else
      visit(variant_serialize_loader<Archive>(ar, false), v);
  }

  /** @} */

} // namespace boost::serialization
