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
#ifndef TRIQS_ARRAYS_EXPRESSION_MAP_H
#define TRIQS_ARRAYS_EXPRESSION_MAP_H
#include "../impl/common.hpp"
#include <functional>
namespace triqs { namespace arrays { 
 
 template<class F, int arity=F::arity> class map_impl;

 /** 
  * Given a function f : arg_type -> result_type, map(f) is the function promoted to arrays
  * map(f) : array<arg_type, N, Opt> --> array<result_type, N, Opt> 
  */
 template<class F> map_impl<F,1> map (F const & f) { return map_impl<F,1>(f,true); }
 template<class F> map_impl<F,2> map2 (F const & f) { return map_impl<F,2>(f,true); }

 // ----------- implementation  -------------------------------------

 // NB The bool is to make constructor not ambiguous
 // clang on os X with lib++ has a pb otherwise (not clear what the pb is)
 template<class F> class map_impl<F,1>  { 
  F f;
  public :   
//  map_impl(F const & f_):f(f_) {}
  map_impl(F const & f_, bool):f(f_) {}
  map_impl(F && f_, bool):f(f_) {}
  map_impl(map_impl const &) = default;
  //map_impl(map_impl &&) = default;

  template<typename A, typename Enable = void> class m_result;

  template<typename A> class m_result<A,typename boost::enable_if<ImmutableCuboidArray<A> >::type> : TRIQS_MODEL_CONCEPT(ImmutableCuboidArray) { 
    public:
     typedef typename std::result_of<F(typename A::value_type)>::type value_type;
     typedef typename A::domain_type domain_type;
     A const & a; F f;
     m_result(F const & f_, A const & a_):a(a_),f(f_) {}
     domain_type domain() const { return a.domain(); } 
     template<typename ... Args> value_type operator() (Args const & ... args) const { return f(a(args...)); }
     //value_type operator[] ( typename domain_type::index_value_type const & key) const { return f(a[key]); }
     friend std::ostream & operator<<(std::ostream & out, m_result const & x){ return out<<"lazy matrix resulting of a mapping";}
   };
  
  template<typename A> class m_result<A,typename boost::enable_if<ImmutableMatrix<A> >::type> : TRIQS_MODEL_CONCEPT(ImmutableMatrix) { 
    public:
     typedef typename std::result_of<F(typename A::value_type)>::type value_type;
     typedef typename A::domain_type domain_type;
     A const & a; F f;
     m_result(F const & f_, A const & a_):a(a_),f(f_) {}
     domain_type domain() const { return a.domain(); } 
     template<typename ... Args> value_type operator() (Args const & ... args) const { return f(a(args...)); }
     //value_type operator[] ( typename domain_type::index_value_type const & key) const { return f(a[key]); }
     friend std::ostream & operator<<(std::ostream & out, m_result const & x){ return out<<"lazy matrix resulting of a mapping";}
   };

  template<typename A> class m_result<A,typename boost::enable_if<ImmutableVector<A> >::type> : TRIQS_MODEL_CONCEPT(ImmutableVector) { 
    public:
     typedef typename std::result_of<F(typename A::value_type)>::type value_type;
     typedef typename A::domain_type domain_type;
     A const & a; F f;
     m_result(F const & f_, A const & a_):a(a_),f(f_) {}
     domain_type domain() const { return a.domain(); } 
     size_t size() const { return a.size();}
     template<typename ... Args> value_type operator() (Args const & ... args) const { return f(a(args...)); }
     //value_type operator[] ( typename domain_type::index_value_type const & key) const { return f(a[key]); }
     value_type operator() ( size_t i) const { return f(a(i)); }
     friend std::ostream & operator<<(std::ostream & out, m_result const & x){ return out<<"lazy matrix resulting of a mapping";}
   };
  

  template<typename Sig> struct result;
  template<typename This, typename A> struct result<This(A)> { typedef m_result<A> type;};

  template< class A > m_result<A> operator()(A const & a) const { 
    //static_assert( (ImmutableArray<A>::value), "map : A does not model ImmutableArray");
    return m_result<A>(f,a);
   } 

  friend std::ostream & operator<<(std::ostream & out, map_impl const & x){ return out<<"map("<<"F"<<")";}
 };

 // ----------- // TO DO : make this with preprocessor ....
 template<class F> class map_impl<F,2>  { 
  F f;
  public : 
  map_impl(F const & f_, bool):f(f_) {}
  map_impl(F && f_, bool):f(f_) {}
  map_impl(map_impl const &) = default;
  //map_impl(map_impl &&) = default;

  template<class A, class B> class m_result : TRIQS_MODEL_CONCEPT(ImmutableArray) { 
    static_assert( (std::is_same<typename  A::domain_type, typename  B::domain_type>::value), "type mismatch");
   public:
    typedef typename std::result_of<F(typename A::value_type,typename B::value_type)>::type value_type;
    typedef typename A::domain_type domain_type;
    A const & a; B const & b; F f;
    m_result(F const & f_, A const & a_, B const & b_):a(a_),b(b_),f(f_) {
     if (a.domain() != b.domain()) TRIQS_RUNTIME_ERROR<<"map2 : domain mismatch";
    }
    domain_type domain() const { return a.domain(); } 
    template<typename ... Args> value_type operator() (Args const & ... args) const { return f(a(args...),b(args...)); }
    //value_type operator[] ( typename domain_type::index_value_type const & key) const { return f(a[key],b[key]); }
    friend std::ostream & operator<<(std::ostream & out, m_result const & x){ return out<<"lazy matrix resulting of a mapping";}
  };

  template<typename Sig> struct result;
  template<typename This, typename A,typename B> struct result<This(A,B)> { typedef m_result<A,B> type;};
 
  template< class A, class B > m_result<A,B> operator()(A const & a, B const & b) { 
    static_assert( (ImmutableArray<A>::value), "map1 : A does not model ImmutableArray");
    static_assert( (ImmutableArray<B>::value), "map1 : B does not model ImmutableArray");
    return m_result<A,B>(f,a,b);
   } 

  friend std::ostream & operator<<(std::ostream & out, map_impl const & x){ return out<<"map("<<"F"<<")";}
 };
}}//namespace triqs::arrays
#endif

