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
#ifndef TRIQS_ARRAYS_FIRST_INCLUDE_PYTHON_H
#define TRIQS_ARRAYS_FIRST_INCLUDE_PYTHON_H

// clang but not libc++
#if defined(__clang__) and !defined(_LIBCPP_VERSION)
// workaround an error in gcc stdlib
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

#endif
