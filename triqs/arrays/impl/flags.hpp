/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2013 by O. Parcollet
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
#ifndef TRIQS_ARRAYS_FLAGS_H
#define TRIQS_ARRAYS_FLAGS_H
#include "../indexmaps/permutation.hpp"
namespace triqs { namespace arrays {

 typedef unsigned long long ull_t;

 namespace Tag {struct no_init {}; struct default_init {};}

 // Flags is a 64 bit unsigned int.
 // 0 is the default option.
 // Meaning of the bits :
 // 0   -> Const
 // 1,2 -> Predefined order : 
 //    0 : None, 1 : C, 2 : Fortran 
 // 3 -> Init the array 
 // 5 -> Boundcheck

 constexpr ull_t ConstView              = 1ull << 0;
 constexpr ull_t TraversalOrderC        = 1ull << 1;
 constexpr ull_t TraversalOrderFortran  = 1ull << 2;
 constexpr ull_t DefaultInit            = 1ull << 3;
 constexpr ull_t BoundCheck             = 1ull << 5;

#define BOUND_CHECK              triqs::arrays::BoundCheck
#define TRAVERSAL_ORDER_C        triqs::arrays::TraversalOrderC
#define TRAVERSAL_ORDER_FORTRAN  triqs::arrays::TraversalOrderFortran
#define DEFAULT_INIT             triqs::arrays::DefaultInit

 // NB : flags MUST be insensitive to slicing ...
 // i.e. when I slice, the flags does not change.
 
 namespace flags {

  constexpr ull_t get(ull_t f, ull_t a)   { return  ((f & (1ull<<a)) >> a);}
  constexpr ull_t get2(ull_t f, ull_t a)  { return  ((f & ((1ull<<a) + (1ull<< (a+1ull)))) >> a);}

  constexpr bool is_const (ull_t f) { return get (f,0) !=0;}

#ifdef TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
  constexpr bool bound_check      (ull_t f) { return true;}
#else
  constexpr bool bound_check      (ull_t f) { return get (f, 5)!=0;}
#endif

  constexpr bool traversal_order_c         (ull_t f) { return get (f,1ull)!=0ull;}
  constexpr bool traversal_order_fortran   (ull_t f) { return get (f,2ull)!=0ull;}

  template< ull_t F> struct bound_check_trait { static constexpr bool value = bound_check(F); }; 

  constexpr ull_t init_mode        (ull_t f) { return get (f,3);}

  template<ull_t F> struct init_tag1;
  template<> struct init_tag1<0> { typedef Tag::no_init type;};
  template<> struct init_tag1<1> { typedef Tag::default_init type;};

  // for the init_tag, we pass the *whole* option flag.
  template<ull_t F> struct init_tag : init_tag1 < init_mode(F)> {};

  template<ull_t F, ull_t To> struct assert_make_sense {
   static constexpr bool bc = bound_check(F);
   static constexpr bool is_c = traversal_order_c(F);
   static constexpr bool is_f = traversal_order_fortran(F);
   static_assert ( (!( is_c && is_f)), "You asked C and Fortran traversal order at the same time...");
   static_assert ( (!( (is_c || is_f) && To )), "You asked C or Fortran traversal order and gave a traversal order ...");
  };

 }
}}//namespace triqs::arrays 
#endif
