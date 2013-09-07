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
#ifndef TRIQS_COMPILER_DETAILS_H
#define TRIQS_COMPILER_DETAILS_H

#if defined __GNUC__  ||  defined  __clang__
#define restrict __restrict__
#endif

#if defined __GNUC__  && ! defined  __clang__
#define GCC_VERSION (__GNUC__ * 10000 \
                               + __GNUC_MINOR__ * 100 \
                               + __GNUC_PATCHLEVEL__)
#if GCC_VERSION < 40801
//#warning "gcc compiler" GCC_VERSION
#define TRIQS_COMPILER_IS_OBSOLETE
#define TRIQS_COMPILER_OBSOLETE_GCC
// we still accept gcc 4.6 and 4.7, but only the 4.8.1 and higher is a compliant c++11
#endif
#endif


#endif

