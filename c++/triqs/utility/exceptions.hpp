// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2022 Simons Foundation
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
 * @brief TRIQS exception hierarchy and related macros.
 */

#pragma once

#include "./macros.hpp"

#include <mpi/mpi.hpp>

#include <cstdlib>
#include <exception>
#include <sstream>
#include <string>

/**
 * @ingroup triqs-utility-macros
 * @{
 */

/// Throw an exception of type `CLASS` whose message starts with `Triqs <NAME>` and the current source location.
#define TRIQS_ERROR(CLASS, NAME) throw CLASS() << "Triqs " << NAME << "\n    at " << __FILE__ << " : " << __LINE__ << "\n\n"

/// Throw a triqs::runtime_error with the current source location.
#define TRIQS_RUNTIME_ERROR TRIQS_ERROR(triqs::runtime_error, "runtime error")

/// Throw a triqs::keyboard_interrupt with the current source location.
#define TRIQS_KEYBOARD_INTERRUPT TRIQS_ERROR(triqs::keyboard_interrupt, "Ctrl-C")

/// Throw a triqs::runtime_error if the boolean expression `X` evaluates to false.
#define TRIQS_ASSERT(X)                                                                                                                              \
  if (!(X)) TRIQS_RUNTIME_ERROR << AS_STRING(X);

/// Like @ref TRIQS_ASSERT but lets the caller add a custom message.
#define TRIQS_ASSERT2(X, ...)                                                                                                                        \
  if (!(X)) TRIQS_RUNTIME_ERROR << AS_STRING(X) << "\n " << __VA_ARGS__;

/** @} */

namespace triqs {

  /**
   * @addtogroup triqs-utility-macros
   * @{
   */

  /**
   * @brief Base class for exceptions thrown in **TRIQS**.
   *
   * @details A triqs::exception carries a `std::stringstream` accumulator that the user can extend with operator<<(). 
   * When what() is called, the accumulated message is returned, followed by the rank of the throwing MPI process and 
   * (if the environment variable `TRIQS_SHOW_EXCEPTION_TRACE` is set) by the C++ stack trace captured at construction.
   */
  class exception : public std::exception {
    std::stringstream acc;
    std::string _trace;
    mutable std::string _what;

    public:
    /// Default constructor ceates an exception with an empty diagnostic message and an empty stack trace.
    exception() noexcept : std::exception() {}

    /**
     * @brief Copy constructor copies the accumulated diagnostic message, the captured stack trace and the cached 
     * what() string from another exception.
     *
     * @param e Exception to copy from.
     */
    exception(exception const &e) noexcept : acc(e.acc.str()), _trace(e._trace), _what(e._what) {}

    /// Virtual default destructor.
    virtual ~exception() noexcept {}

    /**
     * @brief Append a value to the accumulated diagnostic message.
     *
     * @details The value is streamed into the internal `std::stringstream` accumulator.
     *
     * @tparam T Type streamable into a `std::ostream`.
     * @param x Value to append to the message.
     * @return Reference to `*this` to allow chaining of multiple operator<<() calls.
     */
    template <typename T> exception &operator<<(T const &x) {
      acc << x;
      return *this;
    }

    /**
     * @brief Append a C string to the accumulated diagnostic message.
     *
     * @param mess Null-terminated C string to append to the message.
     * @return Reference to `*this` to allow chaining of multiple operator<<() calls.
     */
    exception &operator<<(const char *mess) {
      (*this) << std::string(mess);
      return *this;
    }

    /**
     * @brief Build and return the full diagnostic message.
     *
     * @details Concatenates the accumulated message, the rank of the throwing MPI process (when MPI is initialized) 
     * and, if the environment variable `TRIQS_SHOW_EXCEPTION_TRACE` is set, the captured C++ stack trace. The result is 
     * cached in a member so the returned pointer stays valid for the lifetime of the exception object.
     *
     * @return Null-terminated string owned by the exception object.
     */
    virtual const char *what() const noexcept {
      std::stringstream out;
      out << acc.str() << "\nException was thrown on node ";
      if (mpi::is_initialized()) out << mpi::communicator().rank() << "\n";
      if (getenv("TRIQS_SHOW_EXCEPTION_TRACE")) out << ".. C++ trace is : " << trace() << "\n";
      _what = out.str();
      return _what.c_str();
    }

    /**
     * @brief Get the captured stack trace.
     * @return Null-terminated C string holding the C++ stack trace (empty if none was captured).
     */
    virtual const char *trace() const noexcept { return _trace.c_str(); }
  };

  /**
   * @brief Generic runtime error thrown in **TRIQS**.
   *
   * @details Specialization of triqs::exception used for the most common error case. It only overrides operator<<() so
   * that chained calls return a triqs::runtime_error reference instead of a triqs::exception reference.
   */
  class runtime_error : public exception {
    public:
    /// Default constructor creates a runtime error with an empty diagnostic message.
    runtime_error() noexcept : exception() {}

    /// Virtual default destructor.
    virtual ~runtime_error() noexcept {}

    /**
     * @brief Append a value to the accumulated diagnostic message.
     *
     * @tparam T Type streamable into a `std::ostream`.
     * @param x Value to append to the message.
     * @return Reference to `*this` to allow chaining of multiple operator<<() calls.
     */
    template <typename T> runtime_error &operator<<(T const &x) {
      exception::operator<<(x);
      return *this;
    }
  };

  /**
   * @brief Exception raised when the user interrupts the program with Ctrl-C.
   *
   * @details Specialization of triqs::exception. It only overrides operator<<() so that chained calls return a 
   * triqs::keyboard_interrupt reference instead of a triqs::exception reference.
   */
  class keyboard_interrupt : public exception {
    public:
    /// Default constructor creates a keyboard interrupt with an empty diagnostic message.
    keyboard_interrupt() noexcept : exception() {}

    /// Virtual default destructor.
    virtual ~keyboard_interrupt() noexcept {}

    /**
     * @brief Append a value to the accumulated diagnostic message.
     *
     * @tparam T Type streamable into a `std::ostream`.
     * @param x Value to append to the message.
     * @return Reference to `*this` to allow chaining of multiple operator<<() calls.
     */
    template <typename T> keyboard_interrupt &operator<<(T const &x) {
      exception::operator<<(x);
      return *this;
    }
  };

  /** @} */

} // namespace triqs
