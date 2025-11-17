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

// FIXME : MERGE : replace triqs::report_steam with ostream_with_verbosity -> merge + alias

namespace triqs::utility {

  /**
  * \brief Output stream with flexible verbosity level.
  *
  * This class behaves pretty much like a standard ostream but you can also
  * set some verbosity level
  *
  * Example:
  *
  * report_stream rep(2);        // this creates a report stream w/ verbosity 2
  * rep << "Hello" << endl;      // this prints Hello
  * rep(2) << "Hello2" << endl;   // this prints Hello2
  * rep(3) << "Hello3" << endl;   // this doesn't print anything because the verbosity < 3
  *
  */
  class report_stream {

    std::ostream *out;
    int verbosity;

    public:
    report_stream(std::ostream *out_, int verbosity_ = 1) : out(out_), verbosity(verbosity_) {}

    report_stream operator()(int n) { return report_stream(out, verbosity - n + 1); }

    template <class T> report_stream &operator<<(T const &x) {
      if (verbosity > 0) (*out) << x;
      return *this;
    }

    // this is the type of std::cout
    typedef std::basic_ostream<char, std::char_traits<char>> CoutType;

    // this is the function signature of std::endl
    typedef CoutType &(*StandardEndLine)(CoutType &);

    // define an operator<< to take in std::endl
    report_stream &operator<<(StandardEndLine manip) {
      // call the function, but we cannot return it's value
      if (verbosity > 0) manip(*out);
      return *this;
    }
  };

  /// FIXME : MERGE : replace triqs::report_steam, -> merge + alias
  /**
   * @brief A stream wrapper that conditionally forwards output to an underlying std::ostream
   *        based on a verbosity level.
   *
   * The stream_with_verbosity class allows selective printing to an output stream
   * depending on a user-defined verbosity threshold.
   *
   * Usage:
   * @code
   * stream_with_verbosity out(std::cout, verbosity);
   * out << "This message is printed if verbosity > 0." << std::endl;
   * out(3) << "This will print only if verbosity >= 3." << std::endl;
   * @endcode
   */
  class ostream_with_verbosity {

    std::ostream *out_;
    int verbosity_;

    public:
    /**
     * @brief Construct a stream_with_verbosity.
     * @param out Reference to the output stream (e.g., std::cout).
     * @param verbosity Initial verbosity level (default is 1).
     */
    ostream_with_verbosity(std::ostream &out, int verbosity = 1) : out_(&out), verbosity_(verbosity) {}

    /**
     * @brief Create a derived stream with adjusted verbosity.
     * @param n The minimum verbosity required for output.
     * @return A new stream_with_verbosity with reduced verbosity.
     */
    ostream_with_verbosity operator()(int n) const { return {*out_, verbosity_ - n + 1}; }

    /**
     * @brief Stream insertion operator for generic types.
     * @tparam T Any type supporting operator<< with std::ostream.
     * @param value The value to write to the stream.
     * @return Reference to this stream_with_verbosity instance.
     */
    template <typename T> ostream_with_verbosity &operator<<(T const &value) {
      if (verbosity_ > 0) *out_ << value;
      return *this;
    }

    /**
     * @brief Stream insertion for manipulators like std::endl.
     * @param manip The manipulator function (e.g., std::endl).
     * @return Reference to this stream_with_verbosity instance.
     */
    ostream_with_verbosity &operator<<(std::ostream &(*manip)(std::ostream &)) {
      if (verbosity_ > 0) manip(*out_);
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
   *   auto out = triqs::indented_ostream{std::cout, 3}; // Indent all lines with 3 spaces
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
        return dest->sputc(c); //NOLINT
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
