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
#ifndef TRIQS_ARRAYS_INDEXMAP_STORAGE_ORDER_H
#define TRIQS_ARRAYS_INDEXMAP_STORAGE_ORDER_H
#include "./mem_layout.hpp"
namespace triqs { namespace arrays { namespace indexmaps { namespace cuboid { namespace slicing_TO_order { 

#ifndef TRIQS_WORKAROUND_INTEL_COMPILER_BUGS
  template <typename... SliceArgs> struct _impl{ 
   static constexpr int n_range_ellipsis=0;
   static constexpr ull_t mask(ull_t mo, int P, int c) { return 0;}
   static constexpr ull_t smo (ull_t mo, ull_t ma, int P, int c) { return 0;}
  };
  template <typename A0, typename... SliceArgs> struct _impl< A0,SliceArgs...> { 
   typedef _impl<SliceArgs...> next_t;
   static constexpr bool is_range = std::is_base_of<range, A0>::value; // range and ellipsis (derived from range)
   static constexpr int n_range_ellipsis= next_t::n_range_ellipsis + is_range;
   static constexpr ull_t mask(ull_t mo, int P=0, int c=0)            { return ((is_range ? P+1 : 0) << (4*c)) + next_t::mask(mo,P + is_range,c+1);}
   static constexpr ull_t r2  (ull_t mo, ull_t ma, int c)             { return (ma >> (4*(permutations::apply(mo,c)))) & 0xF; } 
   static constexpr ull_t aux (ull_t mo,ull_t ma,ull_t r,int P,int c) { return (r!=0? ( (r-1) << (4*P)) : 0) + next_t::smo(mo,ma,P+(r!=0), c+1); }
   static constexpr ull_t smo (ull_t mo, ull_t ma, int P=0, int c=0)  { return aux(mo,ma,r2(mo,ma,c),P,c); }
  };
  template<typename ... Args> 
   constexpr ull_t sliced_memory_order1(ull_t mo) { return _impl<Args...>::n_range_ellipsis + 0x10*_impl<Args...>::smo(mo, _impl<Args...>::mask(mo));}
  template<typename ... Args> 
   constexpr ull_t sliced_memory_order2(ull_t mo) { return (mem_layout::is_c (mo) ? c_order(_impl<Args...>::n_range_ellipsis) : 
     ( mem_layout::is_fortran(mo) ? fortran_order(_impl<Args...>::n_range_ellipsis) : sliced_memory_order1<Args...>(mo) ));
   }
  template<ull_t mo, typename ... Args> struct sliced_memory_order { static constexpr ull_t value = sliced_memory_order1<Args...>(mo); };
#else
  template <int P, int c, typename... SliceArgs> struct mask { 
   static constexpr ull_t value = 0;
   static constexpr int n_range_ellipsis= 0;
  };
  template <int P, int c, typename A0, typename... SliceArgs> struct mask<P,c,A0,SliceArgs...> { 
   static constexpr bool is_range = std::is_base_of<range, A0>::value; 
   typedef mask<P + is_range, c+1,SliceArgs...> next_t;
   static constexpr int n_range_ellipsis= next_t::n_range_ellipsis + is_range;
   static constexpr ull_t value = ((is_range ? P+1 : 0) << (4*c)) + next_t::value;
  };
  template <ull_t mo, ull_t ma, int P, int c, typename ... A> struct _impl { static constexpr ull_t value = 0; };
  template <ull_t mo, ull_t ma, int P, int c, typename A0, typename... SliceArgs> struct _impl<mo,ma,P,c, A0,SliceArgs...> { 
   static constexpr bool is_range = std::is_base_of<range, A0>::value; 
   struct r2 { // Intel .... 
    static constexpr ull_t pn = permutations::apply(mo,c);
    static constexpr ull_t value  = (ma >> (4*(pn))) & 0xFull;
   };
   static const bool r2_nozero = (r2::value!=0);
   static constexpr ull_t value =  (r2::value!=0  ? ( (r2::value-1) << (4*P) ) : 0)+ _impl<mo,ma,P+r2_nozero,c+1,SliceArgs...>::value;
  };
  template<ull_t mo, typename ... Args> struct sliced_memory_order { 
   typedef mask<0,0,Args...> mask_t;
   static constexpr ull_t value = mask_t::n_range_ellipsis + 0x10* _impl<mo, mask_t::value,0,0,Args...>::value ;
  };
#endif
}}}}}//namespace triqs::arrays 
#endif
