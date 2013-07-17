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
#ifndef TRIQS_ARRAYS_PERMUTATIONS2_H
#define TRIQS_ARRAYS_PERMUTATIONS2_H
namespace triqs { namespace arrays { namespace permutations { 

 typedef unsigned long long ull;

 constexpr ull apply(ull p, ull i) { return (p >> (4*(i+1))) & 0xFull;}
 constexpr ull size( ull p) { return p & 0xFull;}

 inline ull permutation_from_array ( const int * const p, int length) {
  ull res =0; 
  for (int i =length-1; i>=0; --i) res =  p[i] + 0x10*res;
  res= 0x10 * res + length;
  return res;
 }

 // icc 13.0 has a big bug in constexpr.
#ifndef TRIQS_WORKAROUND_INTEL_COMPILER_BUGS

 //  sum_{k=0}^{n-1} k a^k =  ( (n-1) * a**(n+1) - n * a**n + a)/(a-1)**2
 //  sum_{k=0}^{n-1} k a^(n-k-1) =  ( (n-1)  - n * a + a**n)/(a-1)**2
 //  with a = 16, a**n = 1<<4n
  constexpr ull _identity(ull n) { return  ( (n-1ull)*(1ull<<(4*(n+1))) +(1ull<<4)- n *(1ull<<(4*n))) /( (1ull<<4) -1ull) /((1ull<<4) -1ull); } 
 constexpr ull identity(ull n) { return n + 0x10 * _identity(n);}

 constexpr ull _ridentity(ull n) { return  ( (n-1ull) - n* (1ull<<4) + (1ull<<(4*n))) /( (1ull<<4) -1ull) /((1ull<<4) -1ull); } 
 constexpr ull ridentity(ull n) { return n + 0x10 *_ridentity(n); }

 constexpr ull make_perm_impl(){ return 0;}
 template<typename ... T> constexpr ull make_perm_impl(ull i0, T... x) { return i0 + 0x10 * make_perm_impl(x...); } 
 template<typename ... T> constexpr ull permutation(T... x) { return ull(sizeof...(T)) + 0x10* make_perm_impl(x...); } 

 constexpr ull compose_impl(ull p1, ull p2, ull c) { return apply(p2, apply(p1,c)) + 16ull * ( (c+1 <size(p1) ? compose_impl(p1,p2,c+1ull) : 0)); }
 constexpr ull compose(ull p1, ull p2) { return size(p1) + 0x10* compose_impl(p1,p2, 0);}

  constexpr ull inverse_impl(ull p, ull c) { return (c << (4*apply(p,c))) + (c>0 ? inverse_impl(p,c-1): 0 ); }
  constexpr ull inverse(ull p) { return size(p) + 0x10ull*inverse_impl(p,size(p)-1);}
 
#else

 constexpr ull _identity(ull n) { return  ( (n-1ull)*(1ull<<(4*(n+1))) +(1ull<<4)- n *(1ull<<(4*n))) /( (1ull<<4) -1ull) /((1ull<<4) -1ull); } 
 constexpr ull identity(ull n) { return n + 0x10 * _identity(n);}

 constexpr ull _ridentity(ull n) { return  ( (n-1ull) - n* (1ull<<4) + (1ull<<(4*n))) /( (1ull<<4) -1ull) /((1ull<<4) -1ull); } 
 constexpr ull ridentity(ull n) { return n + 0x10 *_ridentity(n); }

 // todo generalize with preproc...
 constexpr ull make_perm_impl(ull i0) { return i0;} 
 constexpr ull make_perm_impl(ull i0, ull i1) { return i0 + 0x10ull * i1;} 
 constexpr ull make_perm_impl(ull i0, ull i1, ull i2) { return i0 + 0x10ull *make_perm_impl(i1,i2);} 
 constexpr ull make_perm_impl(ull i0, ull i1, ull i2, ull i3) { return i0 + 0x10ull *make_perm_impl(i1,i2,i3);} 
 constexpr ull make_perm_impl(ull i0, ull i1, ull i2, ull i3, ull i4) { return i0 + 0x10ull * make_perm_impl(i1,i2,i3,i4); } 

 constexpr ull permutation(ull i0) { return 1ull +  0x10ull *make_perm_impl(i0);} 
 constexpr ull permutation(ull i0, ull i1) { return 2ull +  0x10ull *make_perm_impl(i0,i1);}
 constexpr ull permutation(ull i0, ull i1, ull i2) { return 3ull +  0x10ull *make_perm_impl(i0,i1,i2);}
 constexpr ull permutation(ull i0, ull i1, ull i2, ull i3)  { return 4ull +  0x10ull *make_perm_impl(i0,i1,i2,i3);}
 constexpr ull permutation(ull i0, ull i1, ull i2, ull i3, ull i4) { return 5ull +  0x10ull *make_perm_impl(i0,i1,i2,i3,i4);}

 template<int c> struct _compose { static constexpr ull invoke(ull p1, ull p2) { return apply(p2, apply(p1,c)) + 16ull * _compose<c+1>::invoke(p1,p2); } };
 template<> struct _compose<15> { static constexpr ull invoke(ull p1, ull p2) { return 0ull; } };
 constexpr ull compose(ull p1, ull p2) { return size(p1) + 0x10ull* (_compose<0>::invoke(p1,p2) & ((1ull<<4*size(p1)) -1ull));}

 template<int c> struct _inverse { static constexpr ull invoke(ull p) { return (ull(c) << (4*apply(p,c))) + _inverse<c+1>::invoke(p);} };
 template<> struct _inverse<15> { static constexpr ull invoke(ull p) { return 0ull; } };
 constexpr ull inverse(ull p) { return size(p) + 0x10ull*( _inverse<0>::invoke(p) - 7ull*15ull + size(p)*(size(p)-1ull)/2ull);}

#endif
 
 inline void print( std::ostream & out, ull perm) { 
  out << "(";
  for (int i =0; i< permutations::size(perm); ++i) { out << (i!=0 ? " " : "") << apply(perm,i);}
  out << ")";
 }
}
using permutations::permutation;
}}
#endif

