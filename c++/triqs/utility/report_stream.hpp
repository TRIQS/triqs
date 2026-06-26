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
 * @brief Verbosity-controlled output stream and an auto-indenting `std::ostream`.
 */

#pragma once

#include <ostream>
#include <streambuf>
#include <string>

namespace triqs::utility {

  /**
   * @addtogroup triqs-utility-io
   * @{
   */

  /**
   * @brief Output stream with a configurable verbosity level.
   *
   * @details It behaves like a `std::ostream` but each operator<<() call is conditional on the current verbosity level.
   * It only emits output when the verbosity level is greater than zero. The verbosity level is set at construction time 
   * and can be (temporarily) reduced using operator()().
   */
  class report_stream {
    std::ostream *out;
    int verbosity;

    public:
    /**
     * @brief Construct a report stream from a pointer to an ostream and a verbosity level.
     * 
     * @param out_ Pointer to the underlying `std::ostream`.
     * @param verbosity_ Verbosity level.
     */
    report_stream(std::ostream *out_, int verbosity_ = 1) : out(out_), verbosity(verbosity_) {}

    /**
     * @brief Construct a report stream from a reference to an ostream and a verbosity level.
     * 
     * @param out_ Reference to the underlying `std::ostream`.
     * @param verbosity_ Verbosity level.
     */
    report_stream(std::ostream &out_, int verbosity_ = 1) : out(&out_), verbosity(verbosity_) {}

    /**
     * @brief Return a child report stream whose verbosity has been reduced by \f$ n - 1 \f$.
     * 
     * @param n Amount by which to lower the verbosity threshold.
     * @return A report stream writing to the same ostream with the reduced verbosity.
     */
    report_stream operator()(int n) const { return {out, verbosity - n + 1}; }

    /**
     * @brief Streaming operator to write the given argument if the current verbosity is greater than zero.
     * 
     * @tparam T Type to be written.
     * @param x Object to write to the underlying ostream.
     * @return Reference to `*this` to allow chaining.
     */
    template <class T> report_stream &operator<<(T const &x) {
      if (verbosity > 0) (*out) << x;
      return *this;
    }

    /**
     * @brief Streaming operator that accepts manipulators like `std::endl`.
     * 
     * @param manip Stream manipulator applied to the underlying ostream when verbosity is greater than zero.
     * @return Reference to `*this` to allow chaining.
     */
    report_stream &operator<<(std::ostream &(*manip)(std::ostream &)) {
      if (verbosity > 0) manip(*out);
      return *this;
    }
  };

  /**
   * @brief Adapter for `std::ostream` that prepends a fixed-width indentation to every new line.
   *
   * @details It inherits from `std::ostream` and substitutes a custom `std::streambuf` that injects a given number of
   * spaces at the start of every line. Useful for nesting verbose output without manually padding each string.
   */
  class indented_ostream : public std::ostream {
    // Custom streambuf that prepends a fixed number of spaces to every line.
    class indented_streambuf : public std::streambuf {
      std::streambuf *dest;
      std::string head;
      bool at_line_start = true;

      public:
      indented_streambuf(std::streambuf *dest, int indent) : dest(dest), head(indent, ' ') {}

      protected:
      int_type overflow(int_type c) override {
        if (c == EOF) return !EOF;
        if (at_line_start && c != '\n') dest->sputn(head.c_str(), long(head.size()));
        at_line_start = (c == '\n');
        return dest->sputc(c); // NOLINT
      }
      int sync() override { return dest->pubsync(); }
    };
    indented_streambuf buffer;

    public:
    /**
     * @brief Construct an indented stream on top of an existing ostream with a given indentation width.
     * 
     * @param os Underlying `std::ostream` that receives the indented output.
     * @param indent Number of spaces inserted at the start of each line.
     */
    indented_ostream(std::ostream &os, int indent) : std::ostream(&buffer), buffer(os.rdbuf(), indent) {}
  };

  /** @} */

} // namespace triqs::utility
