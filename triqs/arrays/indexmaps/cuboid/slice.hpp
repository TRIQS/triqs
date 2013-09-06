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
#ifndef TRIQS_ARRAYS_INDEXMAP_CUBOID_SLICE_H
#define TRIQS_ARRAYS_INDEXMAP_CUBOID_SLICE_H
#include <triqs/utility/count_type_occurrence.hpp>
#include "./slice_traversal_order.hpp"
namespace triqs { namespace arrays { namespace indexmaps {
 namespace cuboid_details {
#define LISILOSO  l_type const * li, s_type const * si, l_type * lo, s_type * so
  typedef size_t         l_type;
  typedef std::ptrdiff_t s_type;

  template <bool BC> inline void _check_BC ( int N, int ind, size_t B, int ind_min =0) { }
  template <> inline void _check_BC<true> (int N, int ind, size_t B, int ind_min) { 
   if (!((ind >= ind_min) && (ind < int(B)))) TRIQS_ARRAYS_KEY_ERROR << " index "<<N<<" is out of domain: \n    " << ind <<" is not within [0,"<< B <<"[\n";
  }

  struct slice_calc { // group in a struct to avoid declaration order issue with the cross call of templates...

   template<int N, int P, bool BoundCheck>
    static void one_step(LISILOSO, std::ptrdiff_t& offset, size_t R){
     _check_BC <BoundCheck> (N, R, li[N]);
     offset += R*si[N];
    }

   template<int N, int P, bool BoundCheck>
    static void one_step(LISILOSO, std::ptrdiff_t& offset, range R){
     _check_BC<BoundCheck> (N, R.first(),li[N]);
     lo[P]  = ((R.last()==-1 ? li[N] : R.last()) - R.first() + R.step()-1 )/R.step(); // python behaviour
     _check_BC<BoundCheck> (N, R.first() + (lo[P]!=0 ? (lo[P]-1) : 0)*R.step() ,li[N], -1);
     so[P]  = si[N] * R.step();
     offset += R.first() * si[N];
    }

   template<int N, int P, bool BoundCheck, int EllipsisLength, typename Arg0, typename...  Args>
    static typename std::enable_if<((EllipsisLength==1) || (!std::is_base_of<ellipsis, Arg0 >::type::value)), void>::type
    invoke(LISILOSO, s_type & offset, Arg0 const & arg0, Args const & ... args ) {
     constexpr bool dP  = (std::is_base_of<range,Arg0>::type::value ? 1 : 0); // Arg0 is range or ellipsis
     one_step<N,P,BoundCheck>(li,si,lo,so,offset, arg0);
     invoke<N+1,P+dP,BoundCheck,EllipsisLength>(li,si,lo,so, offset, args...);
    }

   template<int N, int P, bool BoundCheck, int EllipsisLength, typename Arg0, typename...  Args>
    static typename std::enable_if<((EllipsisLength==0) && std::is_base_of<ellipsis, Arg0 >::type::value), void>::type
    invoke(LISILOSO, s_type & offset, Arg0 const & arg0, Args const & ... args ) {
     invoke<N,P,BoundCheck,EllipsisLength>(li,si,lo,so, offset, args...);
    }

   template<int N, int P, bool BoundCheck, int EllipsisLength, typename Arg0, typename...  Args>
    static typename std::enable_if<((EllipsisLength>1) && std::is_base_of<ellipsis, Arg0 >::type::value), void>::type
    invoke(LISILOSO, s_type & offset, Arg0 const & arg0, Args const & ... args ) {
     //constexpr int dP  = 1;//(std::is_base_of<range,Arg0>::type::value ? 1 : 0); // Arg0 is range or ellipsis
     one_step<N,P,BoundCheck>(li,si,lo,so,offset, arg0);
     invoke<N+1,P+1,BoundCheck,EllipsisLength-1>(li,si,lo,so, offset, arg0, args...);
    }

   template<int N, int P, bool BoundCheck, int EllipsisLength> static void invoke(LISILOSO, s_type & offset ) {}
  };
#undef LISILOSO
 }//namespace cuboid_details

 // special case of no argument : 
 template<int R, ull_t Opt, ull_t To> struct slicer < cuboid::map<R, Opt,To> >  { typedef cuboid::map < R, Opt,To > r_type; }; 

 // general case
 template<int R, ull_t Opt, ull_t To, typename... Args> struct slicer < cuboid::map<R, Opt,To>,  Args...>  { 

  static const unsigned int len = sizeof...(Args);
  static constexpr bool has_ellipsis = (count_type_occurrence<ellipsis,Args...>::value>0);
  static_assert((count_type_occurrence<ellipsis,Args...>::value < 2), "Only one ellipsis is permitted");
  static_assert((len>=R || has_ellipsis), "Too few arguments in slice");
  static_assert((len<=R+(has_ellipsis ?1:0)), "Too many arguments in slice"); // + one to allow an empty ellipsis

  typedef cuboid::map<R,Opt,To> im_t;
  static constexpr int Rf = R - count_type_occurrence_not<range,Args...>::value;
  static constexpr ull_t To_i = im_t::traversal_order;

  // compute a new traversal order, only if there is no ellipsis
  // the computation with ellipsis is not yet implemented...
  static constexpr ull_t Tof = (has_ellipsis ? 0 : cuboid::slicing_TO_order::sliced_memory_order<To_i,Args...>::value);
  typedef cuboid::map < Rf , Opt, Tof> r_type;

#ifndef TRIQS_WORKAROUND_INTEL_COMPILER_BUGS
  static_assert(( (has_ellipsis) ||  (permutations::size(Tof) == Rf)), "Mismatch between Rank and the TraversalOrder");
#endif

  static r_type invoke (im_t const & X, Args ... args) {
   typename r_type::lengths_type newlengths;
   typename r_type::strides_type newstrides;
   std::ptrdiff_t newstart= X.start_shift();
   constexpr int EllipsisLength = R -len+1;
   cuboid_details::slice_calc::invoke<0,0,flags::bound_check_trait<Opt>::value,EllipsisLength>(&X.lengths()[0],&X.strides()[0],&newlengths[0],&newstrides[0],newstart, args...);
   return r_type(std::move(newlengths),std::move(newstrides),newstart);// use move construction ?
  };
 };
}}}//namespaces triqs::arrays::indexmaps
#endif
