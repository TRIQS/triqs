/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by O. Parcollet
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
#ifndef TRIQS_CLEF_ADAPTER_MATH_H 
#define TRIQS_CLEF_ADAPTER_MATH_H
#include "../clef.hpp"
#include <math.h>
#include <boost/preprocessor/seq/for_each.hpp>
#include <complex>

#define TRIQS_CLEF_STD_MATH_FNT_TO_MAKE_LAZY (cos)(sin)(tan)(cosh)(sinh)(tanh)(acos)(asin)(atan)(exp)(log)(sqrt)(abs)(floor)(pow)(conj)

namespace triqs { namespace clef { 

#define TRIQS_CLEF_MAKE_STD_FNT_LAZY(name) using std::name; TRIQS_CLEF_MAKE_FNT_LAZY(name) 
#define AUX(r, data, elem) TRIQS_CLEF_MAKE_STD_FNT_LAZY(elem)
 BOOST_PP_SEQ_FOR_EACH(AUX , nil , TRIQS_CLEF_STD_MATH_FNT_TO_MAKE_LAZY);
#undef AUX  
//#undef TRIQS_CLEF_STD_MATH_FNT_TO_MAKE_LAZY
 
}}

#endif

