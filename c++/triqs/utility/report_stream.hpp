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

#pragma once
#include <ostream>
#include <streambuf>
#include <string>

namespace triqs::utility {

  /**
   * @brief Output stream with flexible verbosity level.
   *
   * This class behaves pretty much like a standard ostream but you can also
   * set some verbosity level.
   *
   * Example:
   *
   *     report_stream rep(std::cout, 2); // verbosity level 2
   *     rep << "Hello" << endl;          // prints Hello
   *     rep(2) << "Hello2" << endl;      // prints Hello2
   *     rep(3) << "Hello3" << endl;      // verbosity < 3, no output
   */
  class report_stream {

    std::ostream *out;
    int verbosity;

    public:
    /// Construct from a pointer to an ostream (legacy interface).
    report_stream(std::ostream *out_, int verbosity_ = 1) : out(out_), verbosity(verbosity_) {}

    /// Construct from a reference to an ostream.
    report_stream(std::ostream &out_, int verbosity_ = 1) : out(&out_), verbosity(verbosity_) {}

    report_stream operator()(int n) const { return {out, verbosity - n + 1}; }

    template <class T> report_stream &operator<<(T const &x) {
      if (verbosity > 0) (*out) << x;
      return *this;
    }

    // operator<< for manipulators like std::endl
    report_stream &operator<<(std::ostream &(*manip)(std::ostream &)) {
      if (verbosity > 0) manip(*out);
      return *this;
    }
  };

  // =============================================================================

  /**
   * @class indented_ostream
   * @brief A custom output stream that automatically indents each new line by a specified number of spaces.
   *
   * This class is useful for formatting output with consistent indentation. It wraps around an existing
   * `std::ostream` and ensures that every new line starts with a specified number of spaces.
   *
   * Example usage:
   * @code
   *   auto out = triqs::utility::indented_ostream{std::cout, 3}; // Indent all lines with 3 spaces
   *   out << "Hello, world!" << std::endl;
   *   out << "Indented text." << std::endl;
   * @endcode
   *
   * @note This class inherits from `std::ostream` and uses a custom stream buffer to handle indentation.
   */
  class indented_ostream : public std::ostream {

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
     * @brief Constructor
     *
     * @param os The underlying std::ostream to write into.
     * @param indent The number of spaces to use for indentation.
     */
    indented_ostream(std::ostream &os, int indent) : std::ostream(&buffer), buffer(os.rdbuf(), indent) {}
  };

} // namespace triqs::utility
