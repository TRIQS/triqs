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
#ifndef TRIQS_ARRAYS_INDEXMAP_MEMORY_LAYOUT_H
#define TRIQS_ARRAYS_INDEXMAP_MEMORY_LAYOUT_H
#include "../permutation.hpp"
#include "../../impl/flags.hpp"

namespace triqs { namespace arrays {

 namespace indexmaps { namespace mem_layout {
  /* The storage order is given by a permutation P stored in a ull_t (unsigned long long) as in permutations::..
   *   P[0] : the fastest index,
   *   P[RANK-1] : the slowest index
   *   Example : 
   *   012 : Fortran, the first index is the fastest
   *   210:  C the last index is the fastest
   *   120 : storage (i,j,k) is : index j is fastest, then k, then i 
   * 
   * index_to_memory_rank  : i ---> r : to the index (0,1, ... etc), associates the rank in memory
   *                         e.g. r=0 : fastest index, r = RANK-1 : the slowest
   * memory_rank_to_index  : the inverse mapping : r---> i : 
   *                         0-> the fastest index, etc..
   *
   * All these computations can be done *at compile time* (constexpr)
   */

#ifndef TRIQS_WORKAROUND_INTEL_COMPILER_BUGS

  constexpr int memory_rank_to_index(ull_t p, int r) { return permutations::apply(p, r);} 
  constexpr int index_to_memory_rank(ull_t p, int r) { return permutations::apply(permutations::inverse(p), r);} 

  constexpr bool is_fortran (ull_t p){ return p == permutations::identity(permutations::size(p));}
  constexpr bool is_c       (ull_t p){ return p == permutations::ridentity(permutations::size(p));}

  constexpr ull_t fortran_order (int n){ return permutations::identity(n);}
  constexpr ull_t c_order       (int n){ return permutations::ridentity(n);}
 
  template<int n> struct fortran_order_tr { static constexpr ull_t value = permutations::identity(n);};
  template<int n> struct c_order_tr       { static constexpr ull_t value = permutations::ridentity(n);};

 // From the flag in the template definition to the real traversal_order
  // 0 -> C order
  // 1 -> Fortran Order
  // Any other number interpreted as a permutation ?
  
  constexpr ull_t _get_traversal_order (int rank, ull_t fl, ull_t to) { return (flags::traversal_order_c(fl) ? c_order(rank) : 
    (flags::traversal_order_fortran(fl)  ? fortran_order(rank) : (to==0 ? c_order(rank) : to )));}
  
  template< int rank, ull_t fl, ull_t to> struct get_traversal_order { static constexpr ull_t value = _get_traversal_order (rank,fl,to); };
#else

  constexpr int memory_rank_to_index(ull_t p, int r) { return permutations::apply(p, r);} 
  constexpr int index_to_memory_rank(ull_t p, int r) { return permutations::apply(permutations::inverse(p), r);} 

  template<ull_t p, int r> struct index_to_memory_rank_tr {  static constexpr ull_t value = permutations::apply(permutations::inverse(p), r);};

  constexpr bool is_fortran (ull_t p){ return p == permutations::identity(permutations::size(p));}
  constexpr bool is_c       (ull_t p){ return p == permutations::ridentity(permutations::size(p));}

  constexpr ull_t fortran_order (int n){ return permutations::identity(n);}
  constexpr ull_t c_order       (int n){ return permutations::ridentity(n);}

  template<int n> struct fortran_order_tr { static constexpr ull_t value = permutations::identity(n);};
  template<int n> struct c_order_tr       { static constexpr ull_t value = permutations::ridentity(n);};

  template< int rank, ull_t fl, ull_t to> struct get_traversal_order {
   static constexpr ull_t value = (flags::traversal_order_c<fl>::value ? c_order_tr<rank>::value :
     (flags::traversal_order_fortran<fl>::value ?  fortran_order_tr<rank>::value : (to==0 ?  c_order_tr<rank>::value : to )));
  };
#endif
 }}



 struct memory_layout_fortran {};
 struct memory_layout_c {};

#define FORTRAN_LAYOUT (triqs::arrays::memory_layout_fortran())
#define C_LAYOUT (triqs::arrays::memory_layout_fortran())

 // stores the layout == order of the indices in memory
 // wrapped into a little type to make constructor unambigous.
 template<int Rank>
  struct memory_layout { 
   ull_t value; 
   explicit memory_layout (ull_t v) : value(v) {assert((permutations::size(v)==Rank));} 
   explicit memory_layout (char ml='C') {
    assert( (ml=='C') || (ml == 'F'));
    value = (ml=='F' ? indexmaps::mem_layout::fortran_order(Rank) : indexmaps::mem_layout::c_order(Rank));
   }
   memory_layout (memory_layout_fortran) { value = indexmaps::mem_layout::fortran_order(Rank); }
   memory_layout (memory_layout_c) { value = indexmaps::mem_layout::c_order(Rank); }
   template<typename ... INT>
    explicit memory_layout(int i0, int i1, INT ... in) : value (permutations::permutation(i0,i1,in...)){
     static_assert( sizeof...(in)==Rank-2, "Error");
    }
   memory_layout (const memory_layout & C) = default; 
   memory_layout (memory_layout && C) { *this = std::move(C);}
   friend void swap( memory_layout & a, memory_layout & b){ std::swap(a.value,b.value);}
   memory_layout & operator =( memory_layout const &) = default;
   memory_layout & operator =( memory_layout && x) { swap(*this,x); return *this;}

   bool operator ==( memory_layout const & ml) const { return value == ml.value;}
   bool operator !=( memory_layout const & ml) const { return value != ml.value;}

   friend std::ostream & operator <<( std::ostream & out, memory_layout const &  s) { permutations::print(out,s.value); return out;}
  };

 //template<int Rank> void swap( memory_layout<Rank> & a, memory_layout<Rank> & b) { std::swap(a.value,b.value);}

}}//namespace triqs::arrays 
#endif
