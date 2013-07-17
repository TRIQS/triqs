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

//#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/seq.hpp>

/**
 * Usage : 
 *  - The macro TRIQS_MAKE_TRAIT_HAS_METHOD ( ReturnType, ArgTypeList, Name, NickName )
 *    defines a trait called has_method_NickName such that : 
 *    has_method_NickName<T>::value is true iff 
 *       ReturnType T::Name(ArgTypeList)
 *    is a valid signature 
 *
 *  - The macro TRIQS_MAKE_TRAIT_HAS_CONST_METHOD ( ReturnType, ArgTypeList, Name, NickName )
 *    defines a trait called has_method_NickName such that : 
 *    has_method_NickName<T>::value is true iff 
 *       ReturnType T::Name(ArgTypeList) const
 *    is a valid signature 
 *
 *  - Where : 
 *     ArgTypeList is a boost preprocessor sequence e.g.
 *     TRIQS_MAKE_TRAIT_HAS_CONST_METHOD (void, (int)(double)(string), Truc, Truc)
 *     makes the trait to check if a object A has method  : 
 *     void A::Truc(int,double,string) const
 *
 */
#define TRIQS_MAKE_TRAIT_HAS_METHOD_AUX(r,s,p,XX) BOOST_PP_COMMA_IF(p) XX   
#define TRIQS_MAKE_TRAIT_HAS_METHOD_IMPL( ReturnType, ArgTypeList, Name, NickName, CONST )\
 template<typename T> struct has_method_##NickName {\
  typedef char yes[1]; typedef char no[2];\
  template<typename U, ReturnType (U::*)(BOOST_PP_SEQ_FOR_EACH_I (TRIQS_MAKE_TRAIT_HAS_METHOD_AUX,~,ArgTypeList)) CONST> struct SFINAE {};\
  template<typename U> static yes& Test(SFINAE<U, &U::Name>*);\
  template<typename U> static no& Test(...);\
  static const bool value = sizeof(Test<T>(0)) == sizeof(yes);\
 };
#define TRIQS_MAKE_TRAIT_HAS_METHOD( ReturnType, ArgTypeList, Name, NickName) TRIQS_MAKE_TRAIT_HAS_METHOD_IMPL(ReturnType,ArgTypeList,Name,NickName,)
#define TRIQS_MAKE_TRAIT_HAS_CONST_METHOD( ReturnType, ArgTypeList, Name, NickName) TRIQS_MAKE_TRAIT_HAS_METHOD_IMPL(ReturnType,ArgTypeList,Name,NickName,const)
//#define TRIQS_MAKE_TRAIT_HAS_METHOD_TPL( ReturnType, ArgTypeList, TPL,Name, NickName) TRIQS_MAKE_TRAIT_HAS_METHOD_IMPL(ReturnType,ArgTypeList,Name,NickName,,TPL)
//#define TRIQS_MAKE_TRAIT_HAS_CONST_METHOD_TPL( ReturnType, ArgTypeList, TPL, Name, NickName) TRIQS_MAKE_TRAIT_HAS_METHOD_IMPL(ReturnType,ArgTypeList,Name,NickName,const,TPL)


