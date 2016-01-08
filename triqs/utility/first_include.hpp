/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once

// general debug macro
#ifdef TRIQS_DEBUG
#define TRIQS_EXCEPTION_SHOW_CPP_TRACE
#define TRIQS_ARRAYS_DEBUG
#endif

#if defined __GNUC__  ||  defined  __clang__
#define restrict __restrict__
#endif

#if defined __GNUC__  && ! defined  __clang__
#define GCC_VERSION (__GNUC__ * 10000 \
                               + __GNUC_MINOR__ * 100 \
                               + __GNUC_PATCHLEVEL__)
#endif

// clang but not libc++
//#if defined(__clang__) and !defined(_LIBCPP_VERSION) and (__clang_major__ <= 3) and (__clang_minor__ <= 5)  and (__clang_patchlevel__ < 2) 
#ifdef TRIQS_ADD_MAX_ALIGN_T_WORKAROUND
// workaround an error in gcc stdlib, but corrected in clang 3.4.2
typedef long double max_align_t;
#endif

#ifdef TRIQS_BUILDING_LIBRARY
#include <triqs_config.h>
#else
#include <triqs/triqs_config.h>
#endif

// FOR DEBUG ONLY : detection of the version of numpy is automatic.
// Uncomment to use the old API ANYWAY
//#define TRIQS_NUMPY_VERSION_LT_17

#ifndef TRIQS_NUMPY_VERSION_LT_17
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#endif

// including python first remove some warning
#ifdef TRIQS_WITH_PYTHON_SUPPORT
#include "Python.h"
#endif

// MACRO USED only in automatic Python wrapper generator desc.
// Only on clang, other compilers : not needed, 
#if defined(__clang__)
#define TRIQS_WRAP_ARG_AS_DICT  __attribute__((annotate("use_parameter_class")))
#define TRIQS_WRAP_IGNORE  __attribute__((annotate("ignore_in_python")))
#define TRIQS_CPP2PY_IGNORE  __attribute__((annotate("ignore_in_python")))
#else
#define TRIQS_WRAP_ARG_AS_DICT
#define TRIQS_WRAP_IGNORE
#define TRIQS_CPP2PY_IGNORE
#endif

// boost serialization declaration...
namespace boost { namespace serialization { class access; }} //forward
#define TRIQS_MAKE_NVP(NAME,X) X

// missing complex * int and in * complex
#include <triqs/utility/complex_ops.hpp>
