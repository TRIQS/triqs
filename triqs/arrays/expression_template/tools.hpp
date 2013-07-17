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
#ifndef TRIQS_ARRAYS_EXPRESSION_TOOLS_H
#define TRIQS_ARRAYS_EXPRESSION_TOOLS_H
#include <type_traits>
namespace triqs { namespace arrays {

 namespace tags { struct plus{}; struct minus{}; struct multiplies{}; struct divides{}; } 

// The basic operations put in a template.... 
 template<typename Tag> struct operation;
 template<> struct operation<tags::plus> { 
  template<typename L, typename R> auto operator()(L const & l, R const & r) const -> decltype(l+r) { return l+r;} 
  static const char name = '+';
 };
 template<> struct operation<tags::minus> { 
  template<typename L, typename R> auto operator()(L const & l, R const & r) const -> decltype(l-r) { return l-r;} 
  static const char name = '-';
 };
 template<> struct operation<tags::multiplies> { 
  template<typename L, typename R> auto operator()(L const & l, R const & r) const -> decltype(l*r) { return l*r;} 
  static const char name = '*';
 };
 template<> struct operation<tags::divides> { 
  template<typename L, typename R> auto operator()(L const & l, R const & r) const -> decltype(l/r) { return l/r;} 
  static const char name = '/';
 };

// The scalar ...
 template<typename T> struct is_in_ZRC : boost::is_arithmetic<T>  {};
 template<> struct is_in_ZRC<bool> : mpl::true_ {};
 template<typename T> struct is_in_ZRC<std::complex<T> > :  mpl::true_ {};

 // Wrapping the scalar in a little struct to recognize it
 // In a matrix expression, the evaluation differ (it is a scalar matrix ....)
 template<typename S, bool IsMatrix> struct scalar_wrap;

 // First the scalar for an array expression ...
 template<typename S> struct scalar_wrap<S,false> {
  typedef S value_type; 
  S s; scalar_wrap(S const &s_):s(s_){} 
  template<typename KeyType> value_type operator[](KeyType&& key) const {return s;}
  template<typename ... Args> inline value_type operator()(Args && ... args) const { return s;}
  friend std::ostream &operator <<(std::ostream &sout, scalar_wrap const &expr){return sout << expr.s; }
 };

 // Second the scalar for a matrix expression ...
 template<typename S> struct scalar_wrap<S,true> {
  typedef S value_type; 
  S s; scalar_wrap(S const &s_):s(s_){} 
  template<typename KeyType> value_type operator[](KeyType&& key) const {return (key[0]==key[1] ? s : S());}
  template<typename A1, typename A2> inline value_type operator()(A1 && a1, A2 && a2) const { return (a1==a2 ? s : S());}
  friend std::ostream &operator <<(std::ostream &sout, scalar_wrap const &expr){return sout << expr.s; }
 };

 // get the rank of something ....
 template<typename T> struct get_rank { static constexpr int value = std::remove_reference<T>::type::domain_type::rank;};
 template<typename S, bool IsMatrix> struct get_rank<scalar_wrap<S,IsMatrix>> { static constexpr int value =0;};
 
 //
 template<typename T, bool IsMatrix=false> struct keeper_type : boost::mpl::if_<is_in_ZRC<T>, scalar_wrap<T,IsMatrix>, typename view_type_if_exists_else_type<T>::type> {};

 // Combine the two domains of LHS and RHS : need to specialize where there is a scalar
 struct combine_domain {
  template<typename L, typename R> 
   inline auto operator() (L const & l, R const & r) const -> decltype(l.domain()) { 
    if (l.domain().lengths() != r.domain().lengths()) TRIQS_RUNTIME_ERROR << "Domain size mismatch : "<< l.domain().lengths()<<" vs" <<r.domain().lengths();
    return l.domain();
   }
  template<typename S, bool IsMatrix, typename R> auto operator() (scalar_wrap<S,IsMatrix> const & w, R const & r) const -> decltype(r.domain()) { return r.domain();}
  template<typename S, bool IsMatrix, typename L> auto operator() (L const & l, scalar_wrap<S,IsMatrix> const & w) const -> decltype(l.domain()) { return l.domain();}
 };

 template<typename A,  typename B> 
  struct type_of_mult{
   //typedef typename boost::remove_reference<A>::type T1; typedef typename boost::remove_reference<B>::type T2;
   //typedef BOOST_TYPEOF_TPL( (*(pseudo_default_construct<T1>())) * (*(pseudo_default_construct<T2>()))) type;
   typedef decltype ( std::declval<typename std::remove_reference<A>::type>() * std::declval<typename std::remove_reference<B>::type>() ) type;
  };
 
}}//namespace triqs::arrays
#endif
