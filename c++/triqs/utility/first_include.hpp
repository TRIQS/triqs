// Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
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
// Authors: Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Compiler / platform glue and the `dcomplex` alias (must be included before any Boost header).
 */

#pragma once

#include <complex>

/**
 * @addtogroup triqs-utility-other
 * @{
 */

/// Convenience alias for `std::complex<double>`.
using dcomplex = std::complex<double>;

/** @} */

#if defined __GNUC__ && !defined __clang__
// GCC version as an integer of the form `major * 10000 + minor * 100 + patchlevel` (only defined on GCC).
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

// Disable std::auto_ptr (removed in C++17) usage in boost.
#define BOOST_NO_AUTO_PTR

// Workaround for a GCC 15 + Boost issue on macOS builds.
#if defined(__GNUC__) && (__GNUC__ == 15) && defined(__APPLE__) && !defined(__clang__)
#define BOOST_HAS_LONG_LONG 1
#endif

// Annotation consumed by the legacy cpp2py wrapper generator to skip a declaration.
#define CPP2PY_IGNORE __attribute__((annotate("ignore_in_python")))

// Annotation consumed by the legacy cpp2py wrapper generator to wrap an argument as a Python dictionary.
#define CPP2PY_ARG_AS_DICT __attribute__((annotate("use_parameter_class")))

// The annotations below are only emitted under Clang (the AST analyzer used by the wrapper generator). Under
// other compilers they expand to nothing so they do not affect the produced binary.
#if defined(__clang__)
#define TRIQS_WRAP_ARG_AS_DICT CPP2PY_ARG_AS_DICT
#define TRIQS_WRAP_IGNORE CPP2PY_IGNORE
#define TRIQS_CPP2PY_IGNORE CPP2PY_IGNORE
#else
#define TRIQS_WRAP_ARG_AS_DICT
#define TRIQS_WRAP_IGNORE
#define TRIQS_CPP2PY_IGNORE
#endif
