// Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
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
 * @brief A conditional output stream that only emits when a user-supplied predicate returns `true`.
 */

#pragma once

#include <functional>
#include <ostream>

namespace triqs::utility {

  /**
   * @ingroup triqs-utility-io
   * @brief Output stream wrapper that emits only when a condition returns `true` and only in debug builds.
   *
   * @details Behaves like a `std::ostream` but streaming is guarded both by `#ifdef TRIQS_DEBUG` and by a user 
   * predicate. 
   * 
   * Useful for sparse, conditional logging in hot loops (e.g. printing only every 100 Monte Carlo configurations).
   *
   * @warning This is unused. It might be removed in the future.
   */
  class debug_stream {
    std::ostream *out; // NOLINT
    std::function<bool()> condition;

    public:
    /**
     * @brief Construct a debug stream with a condition predicate.
     * 
     * @param out_ Pointer to the underlying output stream.
     * @param condition Predicate that determines whether to emit output.
     */
    debug_stream(std::ostream *out_, std::function<bool()> condition) : out(out_), condition(condition) {}

    /**
     * @brief Construct a debug stream without a condition.
     * @param out_ Pointer to the underlying output stream.
     */
    debug_stream(std::ostream *out_) : out(out_) {}

    /**
     * @brief Write an object to the underlying ostream if the condition() holds and `TRIQS_DEBUG` is defined.
     * 
     * @tparam T Type of the object to write.
     * @param x Object to write.
     * @return Reference to `*this`.
     */
    template <class T> debug_stream &operator<<([[maybe_unused]] T const &x) {
#ifdef TRIQS_DEBUG
      if (condition && condition()) (*out) << x;
#endif
      return *this;
    }

    /// Output stream type.
    using CoutType = std::basic_ostream<char, std::char_traits<char>>;

    /// Type of standard manipulators like `std::endl`.
    using StandardEndLine = CoutType &(*)(CoutType &);

    /**
     * @brief Overload of operator<<() that accepts manipulators like `std::endl`.
     * 
     * @param manip Manipulator function.
     * @return Reference to `*this`.
     */
    debug_stream &operator<<([[maybe_unused]] StandardEndLine manip) {
#ifdef TRIQS_DEBUG
      if (condition && condition()) manip(*out);
#endif
      return *this;
    }
  };

} // namespace triqs::utility
