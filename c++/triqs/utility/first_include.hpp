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

// clang but not libc++
//#if defined(__clang__) and !defined(_LIBCPP_VERSION) and (__clang_major__ <= 3) and (__clang_minor__ <= 5)  and (__clang_patchlevel__ < 2)
#ifdef TRIQS_ADD_MAX_ALIGN_T_WORKAROUND
// workaround an error in gcc stdlib, but corrected in clang 3.4.2
typedef long double max_align_t;
#endif

using dcomplex = std::complex<double>;

// disable std::auto_ptr (disabled in c++17) usage in boost
#define BOOST_NO_AUTO_PTR

// boost serialization declaration...
namespace boost {
  namespace serialization {
    class access;
  }
} // namespace boost

// missing complex * int and in * complex
#include <h5/std_addons/complex.hpp>

#define CPP2PY_IGNORE  __attribute__((annotate("ignore_in_python")))
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
