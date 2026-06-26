// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2023 Simons Foundation
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
// Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Common macros used in **TRIQS**.
 */

#pragma once

#include "./first_include.hpp"
#include "./traits.hpp"

#include <boost/utility/enable_if.hpp>
#include <nda/macros.hpp>

#include <type_traits>

/**
 * @ingroup triqs-utility-macros
 * @{
 */

/// Trailing-return-type convenience: expands to `-> decltype(...) { return ...; }`.
#define DECL_AND_RETURN(...)                                                                                                                         \
  ->decltype(__VA_ARGS__) { return __VA_ARGS__; }

/// Macro to catch exceptions, print the error message and return 1.
#define TRIQS_CATCH_AND_ABORT                                                                                                                        \
  catch (std::exception const &e) {                                                                                                                  \
    std::cout << e.what() << std::endl;                                                                                                              \
    return 1;                                                                                                                                        \
  }

// Unused, outdated macros. Should be removed.
#define TYPE_ENABLE_IF(Type, ...) typename boost::enable_if<__VA_ARGS__, Type>::type
#define TYPE_ENABLE_IFC(Type, ...) typename boost::enable_if_c<__VA_ARGS__, Type>::type
#define TYPE_DISABLE_IF(Type, ...) typename boost::disable_if<__VA_ARGS__, Type>::type
#define TYPE_DISABLE_IFC(Type, ...) typename boost::disable_if_c<__VA_ARGS__, Type>::type

/// Mark a function or type as deprecated with a custom message.
#define TRIQS_DEPRECATED(Message) __attribute__((deprecated(AS_STRING(Message))))

/// Macro that prints the name and value of a variable together with the source location.
#define TRIQS_PRINT(X) std::cerr << AS_STRING(X) << " = " << (X) << "      at " << __FILE__ << ":" << __LINE__ << '\n'

/// Force-inline attribute portable across GCC and Clang.
#define FORCEINLINE __inline__ __attribute__((always_inline))

/// Macro that prints a message together with the source location and calls `std::terminate()`.
#define TERMINATE(X)                                                                                                                                 \
  std::cerr << "Terminating at " << __FILE__ << ":" << __LINE__ << "\n";                                                                             \
  std::cerr << (X);                                                                                                                                  \
  std::terminate();                                                                                                                                  \
  }

// Fallback definitions for the c2py annotation macros: when this header is not included via the clair-c2py
// binding generator, the annotations expand to nothing so the production binary is unaffected.
#ifndef C2PY_INCLUDED
#define C2PY_IGNORE
#define C2PY_WRAP_AS_METHOD
#define C2PY_MODULE_INIT
#define C2PY_NOGIL
#define C2PY_RENAME(X)
#define C2PY_PROPERTY_GET(X)
#define C2PY_PROPERTY_SET(X)
#define C2PY_DEPRECATED_PARAMETER_NAME(...)
#endif // C2PY_INCLUDED

/** @} */
