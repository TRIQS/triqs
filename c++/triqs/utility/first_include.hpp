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

#pragma once
#include <complex>

#if defined __GNUC__ && !defined __clang__
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

using dcomplex = std::complex<double>;

// disable std::auto_ptr (disabled in c++17) usage in boost
#define BOOST_NO_AUTO_PTR

// Workaround GCC 15 + Boost Issue for OSX Builds
#if defined(__GNUC__) && (__GNUC__ == 15) && defined(__APPLE__) && !defined(__clang__)
#define BOOST_HAS_LONG_LONG 1
#endif

#define CPP2PY_IGNORE __attribute__((annotate("ignore_in_python")))
#define CPP2PY_ARG_AS_DICT __attribute__((annotate("use_parameter_class")))

// MACRO USED only in automatic Python wrapper generator desc.
// Only on clang, other compilers : not needed,
#if defined(__clang__)
#define TRIQS_WRAP_ARG_AS_DICT CPP2PY_ARG_AS_DICT
#define TRIQS_WRAP_IGNORE CPP2PY_IGNORE
#define TRIQS_CPP2PY_IGNORE CPP2PY_IGNORE
#else
#define TRIQS_WRAP_ARG_AS_DICT
#define TRIQS_WRAP_IGNORE
#define TRIQS_CPP2PY_IGNORE
#endif
