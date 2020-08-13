// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2019 Simons Foundation
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

// ---- TRIQS Specific Macros ----

#ifndef _TRIQS_MACROS_GUARD_H
#define _TRIQS_MACROS_GUARD_H

#ifdef TRIQS_DEBUG
#undef NDEBUG
#endif

#define TRIQS_PRINT(X) std::cerr << AS_STRING(X) << " = " << X << "      at " << __FILE__ << ":" << __LINE__ << '\n'

#include <boost/utility/enable_if.hpp>

#define TYPE_ENABLE_IF(Type, ...) typename boost::enable_if<__VA_ARGS__, Type>::type
#define TYPE_ENABLE_IFC(Type, ...) typename boost::enable_if_c<__VA_ARGS__, Type>::type
#define TYPE_DISABLE_IF(Type, ...) typename boost::disable_if<__VA_ARGS__, Type>::type
#define TYPE_DISABLE_IFC(Type, ...) typename boost::disable_if_c<__VA_ARGS__, Type>::type

#define ENABLE_IF(...) typename boost::enable_if<__VA_ARGS__, void>::type
#define ENABLE_IFC(...) typename boost::enable_if_c<__VA_ARGS__, void>::type
#define DISABLE_IF(...) typename boost::disable_if<__VA_ARGS__, void>::type
#define DISABLE_IFC(...) typename boost::disable_if_c<__VA_ARGS__, void>::type

#define DECL_AND_RETURN(...)                                                                                                                         \
  ->decltype(__VA_ARGS__) { return __VA_ARGS__; }

#define TRIQS_CATCH_AND_ABORT                                                                                                                        \
  catch (std::exception const &e) {                                                                                                                  \
    std::cout << e.what() << std::endl;                                                                                                              \
    return 1;                                                                                                                                        \
  }

#include <type_traits>
namespace triqs {
  template <typename T> struct remove_cv_ref : std::remove_cv<typename std::remove_reference<T>::type> {};

  /// Tag the views
  struct is_view_tag {};
  template <typename T> struct is_view : std::is_base_of<is_view_tag, T> {};
}

#endif

// ---- General Macros ----

#ifndef _CCQ_MACROS_GUARD_H
#define _CCQ_MACROS_GUARD_H

// CCQ, TRIQS general macros
// GUARD IT do not use pragma once
// hence one can simply include them in every projects

// --- Stringify macros -----

#define AS_STRING(...) AS_STRING2(__VA_ARGS__)
#define AS_STRING2(...) #__VA_ARGS__

#define PRINT(X) std::cerr << AS_STRING(X) << " = " << X << "      at " << __FILE__ << ":" << __LINE__ << '\n'
#define NDA_PRINT(X) std::cerr << AS_STRING(X) << " = " << X << "      at " << __FILE__ << ":" << __LINE__ << '\n'

// --- Concept macros -----

#if (__cplusplus > 201703L)

// C++20
// REQUIRES17 : only in 17, same for 20
// REQUIRES : in both

#define AUTO(X) X auto
#define CONCEPT(X) X

#define REQUIRES17(...)
#define REQUIRES requires
#define REQUIRES20 requires

// C++20 explicit(bool) : degrade it NOTHING in c++17, we can not check easily
#define EXPLICIT explicit

// WARNING : it is critical for our doctools to have REQUIRES as requires, NOT a (...) with __VA_ARGS__
// It is the same effect, but raises unnecessary complications in traversing the AST in libtooling with macros.

#else

// C++17 backward compat mode

#define AUTO(X) auto
#define CONCEPT(X) typename
#define REQUIRES20(...)

#define EXPLICIT(...)

#ifdef __clang__
#define REQUIRES17(...) __attribute__((enable_if(__VA_ARGS__, AS_STRING(__VA_ARGS__))))
#define REQUIRES(...) __attribute__((enable_if(__VA_ARGS__, AS_STRING(__VA_ARGS__))))
#elif __GNUC__
// with the -fconcepts TS only. A degraded concept mode, not exactly the C++20. We return to C++17 + basic require
#define REQUIRES17 requires
#define REQUIRES requires
#endif

#endif

// -----------------------------------------------------------

#define FORCEINLINE __inline__ __attribute__((always_inline))

#ifdef NDEBUG

#define EXPECTS(X)
#define ASSERT(X)
#define ENSURES(X)
#define EXPECTS_WITH_MESSAGE(X, ...)
#define ASSERT_WITH_MESSAGE(X, ...)
#define ENSURES_WITH_MESSAGE(X, ...)

#else

#include <iostream>

#define EXPECTS(X)                                                                                                                                   \
  if (!(X)) {                                                                                                                                        \
    std::cerr << "Precondition " << AS_STRING(X) << " violated at " << __FILE__ << ":" << __LINE__ << "\n";                                          \
    std::terminate();                                                                                                                                \
  }
#define ASSERT(X)                                                                                                                                    \
  if (!(X)) {                                                                                                                                        \
    std::cerr << "Assertion " << AS_STRING(X) << " violated at " << __FILE__ << ":" << __LINE__ << "\n";                                             \
    std::terminate();                                                                                                                                \
  }
#define ENSURES(X)                                                                                                                                   \
  if (!(X)) {                                                                                                                                        \
    std::cerr << "Postcondition " << AS_STRING(X) << " violated at " << __FILE__ << ":" << __LINE__ << "\n";                                         \
    std::terminate();                                                                                                                                \
  }

#define EXPECTS_WITH_MESSAGE(X, ...)                                                                                                                 \
  if (!(X)) {                                                                                                                                        \
    std::cerr << "Precondition " << AS_STRING(X) << " violated at " << __FILE__ << ":" << __LINE__ << "\n";                                          \
    std::cerr << "Error message : " << __VA_ARGS__ << std::endl;                                                                                     \
    std::terminate();                                                                                                                                \
  }
#define ASSERT_WITH_MESSAGE(X, ...)                                                                                                                  \
  if (!(X)) {                                                                                                                                        \
    std::cerr << "Assertion " << AS_STRING(X) << " violated at " << __FILE__ << ":" << __LINE__ << "\n";                                             \
    std::cerr << "Error message : " << __VA_ARGS__ << std::endl;                                                                                     \
    std::terminate();                                                                                                                                \
  }
#define ENSURES_WITH_MESSAGE(X, ...)                                                                                                                 \
  if (!(X)) {                                                                                                                                        \
    std::cerr << "Postcondition " << AS_STRING(X) << " violated at " << __FILE__ << ":" << __LINE__ << "\n";                                         \
    std::cerr << "Error message : " << __VA_ARGS__ << std::endl;                                                                                     \
    std::terminate();                                                                                                                                \
  }

#endif

#endif
