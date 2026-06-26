// Copyright (c) 2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2020 Simons Foundation
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
// Authors: Igor Krivenko, Nils Wentzell

/**
 * @file
 * @brief Small helpers for working with `std::variant` types.
 */

#pragma once

#include <nda/stdutil/array.hpp>

#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace triqs::utility {

  /**
   * @ingroup triqs-utility-tuple
   * @brief Lambda-overload helper for `std::visit`.
   *
   * @details Combines several callables into a single object whose `operator()` is overloaded for each input.
   * 
   * @tparam Fs Callable types being combined.
   */
  template <typename... Fs> struct overloaded : Fs... {
    using Fs::operator()...;
  };

} // namespace triqs::utility

namespace std {

  /**
   * @addtogroup triqs-utility-tuple
   * @{
   */

  /**
   * @brief Write a `std::variant` to an output stream by visiting the held alternative and forwarding it to the stream.
   * 
   * @tparam T First alternative type.
   * @tparam Ts Remaining alternative types.
   * @param os Output stream.
   * @param v Variant to write.
   * @return Reference to the output stream.
   */
  template <typename T, typename... Ts> std::ostream &operator<<(std::ostream &os, std::variant<T, Ts...> const &v) {
    visit([&os](auto const &x) { os << x; }, v);
    return os;
  }

  /**
   * @brief Write a vector of `std::variant` as a comma-separated list of its elements.
   * 
   * @tparam T First alternative type.
   * @tparam Ts Remaining alternative types.
   * @param os Output stream.
   * @param vec Vector of variants to write.
   * @return Reference to the output stream.
   */
  template <typename T, typename... Ts> std::ostream &operator<<(std::ostream &os, std::vector<std::variant<T, Ts...>> const &vec) {
    int u = 0;
    for (auto const &i : vec) {
      if (u++) os << ",";
      os << i;
    }
    return os;
  }

  /// Identity overload for `std::string`.
  inline string to_string(string const &str) { return str; }

  /**
   * @brief Convert a variant into a string.
   * 
   * @details It uses a stringstream and the std::operator<<(std::ostream&, std::variant<T, Ts...> const&) overload.
   * 
   * @tparam T First alternative type.
   * @tparam Ts Remaining alternative types.
   * @param var Variant to convert.
   * @return String representation of the variant.
   */
  template <typename T, typename... Ts> inline string to_string(variant<T, Ts...> const &var) {
    stringstream ss;
    ss << var;
    return ss.str();
  }

  /** @} */

} // namespace std
