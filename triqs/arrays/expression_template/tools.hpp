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
#include <triqs/utility/expression_template_tools.hpp>
namespace triqs { namespace arrays {
 using utility::is_in_ZRC;

 /*
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
*/

 // Wrapping the scalar in a little struct to recognize it
 // In a matrix expression, the evaluation differ (it is a scalar matrix ....)
 template<typename S, bool IsMatrix> struct _scalar_wrap;

 // First the scalar for an array expression ...
 template<typename S> struct _scalar_wrap<S,false> {
  typedef typename std::remove_reference<S>::type value_type;
  S s; 
  template<typename T> _scalar_wrap(T && x):s(std::forward<T>(x)){}  
  template<typename KeyType> S operator[](KeyType&& key) const {return s;}
  template<typename ... Args> inline S operator()(Args && ... args) const { return s;}
  friend std::ostream &operator <<(std::ostream &sout, _scalar_wrap const &expr){return sout << expr.s; }
 };

 // Second the scalar for a matrix expression ...
 template<typename S> struct _scalar_wrap<S,true> {
  static_assert(!std::is_same<double &, S>::value, "lll");
  typedef typename std::remove_reference<S>::type value_type;
  S s; value_type zero; 
  template<typename T> _scalar_wrap(T && x):s(std::forward<T>(x)), zero{} {}  
  template<typename KeyType> S operator[](KeyType&& key) const {return (key[0]==key[1] ? s : S());}
  template<typename A1, typename A2> inline S operator()(A1  const & a1, A2 const & a2) const { return (a1==a2 ? s : zero);}
  friend std::ostream &operator <<(std::ostream &sout, _scalar_wrap const &expr){return sout << expr.s; }
 };

 // type of the node
 template<typename T, bool IsMatrix> struct node_t : 
  std::conditional<utility::is_in_ZRC<T>::value, _scalar_wrap<typename std::remove_reference<T>::type,IsMatrix>, typename utility::remove_rvalue_ref<T>::type> {};
  //std::conditional<utility::is_in_ZRC<T>::value, _scalar_wrap<typename std::add_const<T>::type,IsMatrix>, typename utility::remove_rvalue_ref<T>::type> {};

 // get the rank of something ....
 template<typename T> struct get_rank { static constexpr int value = std::remove_reference<T>::type::domain_type::rank;};
 template<typename S, bool IsMatrix> struct get_rank<_scalar_wrap<S,IsMatrix>> { static constexpr int value =0;};
 
 //
 //template<typename T, bool IsMatrix=false> struct keeper_type : boost::mpl::if_<is_in_ZRC<T>, _scalar_wrap<T,IsMatrix>, typename view_type_if_exists_else_type<T>::type> {};

 // Combine the two domains of LHS and RHS : need to specialize where there is a scalar
 struct combine_domain {
  template<typename L, typename R> 
   inline auto operator() (L const & l, R const & r) const -> decltype(l.domain()) { 
    if (l.domain().lengths() != r.domain().lengths()) TRIQS_RUNTIME_ERROR << "Domain size mismatch : "<< l.domain().lengths()<<" vs" <<r.domain().lengths();
    return l.domain();
   }
  template<typename S, bool IsMatrix, typename R> auto operator() (_scalar_wrap<S,IsMatrix> const & w, R const & r) const -> decltype(r.domain()) { return r.domain();}
  template<typename S, bool IsMatrix, typename L> auto operator() (L const & l, _scalar_wrap<S,IsMatrix> const & w) const -> decltype(l.domain()) { return l.domain();}
 };

  
}}//namespace triqs::arrays
#endif
