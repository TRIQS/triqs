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
 * @brief Helper functions built around `std::type_info`. 
 */

#pragma once

#include <string>
#include <typeinfo>

namespace triqs::utility {

  /**
   * @addtogroup triqs-utility-other
   * @{
   */

  /**
   * @brief Demangle a mangled C++ symbol name to a human-readable string.
   * 
   * @param name Mangled name (typically `typeid(...).name()`).
   * @return The demangled name, or the original name if demangling failed.
   */
  std::string demangle(const char *name);

  /// Overload of @ref demangle accepting an `std::string`.
  std::string demangle(std::string const &name);

  /**
   * @brief Demangle the name corresponding to an `std::type_info`.
   * 
   * @param info Type info object.
   * @return The demangled name, or the original name if demangling failed.
   */
  std::string get_name(std::type_info const &info);

  /**
   * @brief Human-readable name of the dynamic type of a given object.
   * 
   * @tparam T Type of the object.
   * @param obj Object whose type name is requested.
   * @return Demangled name of the dynamic type of the given object.
   */
  template <typename T> std::string typeid_name(T const &obj) { return get_name(typeid(obj)); }

  /**
   * @brief Human-readable name of a given static type.
   * 
   * @tparam T Type of interest.
   * @return Demangled name of `T`.
   */
  template <typename T> std::string typeid_name() { return get_name(typeid(T)); }

  /** @} */

} // namespace triqs::utility
