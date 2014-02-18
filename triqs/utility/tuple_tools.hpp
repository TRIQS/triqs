/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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
#ifndef TRIQS_UTILITY_TUPLE_TOOLS_H
#define TRIQS_UTILITY_TUPLE_TOOLS_H
#include<triqs/utility/macros.hpp>
#include <tuple>
#include "./c14.hpp"
#include <sstream>

// adding to the std lib the reversed lazy tuple...
// overloading & specializing only the functions needed here.
namespace std {
  template<typename TU> struct _triqs_reversed_tuple {TU _x;};

  template<typename ... T> _triqs_reversed_tuple<std::tuple<T...>>        reverse(std::tuple<T...> && x) { return {move(x)};}
  template<typename ... T> _triqs_reversed_tuple<std::tuple<T...>&>       reverse(std::tuple<T...> & x) { return {x};}
  template<typename ... T> _triqs_reversed_tuple<std::tuple<T...>const &> reverse(std::tuple<T...> const & x) { return {x};}

  template<int pos, typename TU> auto get(_triqs_reversed_tuple<TU> const & t) 
   DECL_AND_RETURN(std::get<std::tuple_size<typename std::remove_const<typename std::remove_reference<TU>::type>::type>::value-1-pos>(t._x));
  
  template<int pos, typename TU> auto get(_triqs_reversed_tuple<TU> & t)       
   DECL_AND_RETURN(std::get<std::tuple_size<typename std::remove_const<typename std::remove_reference<TU>::type>::type>::value-1-pos>(t._x));
  
  template<int pos, typename TU> auto get(_triqs_reversed_tuple<TU> && t)      
   DECL_AND_RETURN(std::get<std::tuple_size<typename std::remove_const<typename std::remove_reference<TU>::type>::type>::value-1-pos>(move(t)._x));

  template<typename TU> class tuple_size<_triqs_reversed_tuple<TU>>         : public tuple_size<typename std::remove_const<typename std::remove_reference<TU>::type>::type>{};
}


namespace triqs { namespace tuple {

 /**
  * t : a tuple
  * x : anything
  * push_back (t,x) -> returns new tuple with x append at the end
  */
 template<typename T, typename X>
  auto push_back(T && t, X &&x) DECL_AND_RETURN ( std::tuple_cat(std::forward<T>(t),std::make_tuple(std::forward<X>(x))));

 /**
  * t : a tuple
  * x : anything
  * push_front (t,x) -> returns new tuple with x append at the first position
  */
 template<typename T, typename X>
  auto push_front(T && t, X &&x) DECL_AND_RETURN ( std::tuple_cat(std::make_tuple(std::forward<X>(x)),std::forward<T>(t)));

 /**
  * apply(f, t)
  * f : a callable object
  * t a tuple
  * Returns : f(t[0], t[1], ...)
  * Equivalent to f(*t) in python ....
  */
 template<int pos> struct apply_impl {
  template<typename F, typename T, typename ... Args>
   auto operator()(F && f, T const & t, Args && ... args)
   DECL_AND_RETURN( apply_impl<pos-1>()(std::forward<F>(f),t, std::get<pos>(t), std::forward<Args>(args)...));
 };

 template<> struct apply_impl<-1> {
  template<typename F, typename T, typename ... Args>
   auto operator()(F && f, T const & t, Args && ... args) DECL_AND_RETURN( std::forward<F>(f)(std::forward<Args>(args)...));
 };

 template<typename F, typename T>
  auto apply (F && f, T const & t) DECL_AND_RETURN( apply_impl<std::tuple_size<T>::value-1>()(std::forward<F>(f),t));

 //template <typename T, typename ReturnType, typename... Args>
 //ReturnType apply( ReturnType(*f)(Args...), T const & t) { return apply([f](Args const & ... args) { return (*f)(args...);} ,t);}

 /**
  * apply_construct<F>(t)
  * F : a class 
  * t a tuple
  * Returns : F { t[0], t[1]} 
  */
 template<int pos, typename F, typename T> struct apply_construct_impl {
  template<typename ... Args>
   auto operator()(T const & t, Args && ... args)
   DECL_AND_RETURN( apply_construct_impl<pos-1,F,T>()(t, std::get<pos>(t), std::forward<Args>(args)...));
 };

 template<typename F, typename T> struct apply_construct_impl<-1,F,T> {
  template<typename ... Args>
   auto operator()(T const & t, Args && ... args) DECL_AND_RETURN( F{std::forward<Args>(args)...});
 };

 template<typename F, typename T>
  auto apply_construct (T const & t) DECL_AND_RETURN( apply_construct_impl<std::tuple_size<T>::value-1,F,T>()(t));


 /**
  * for_each(f, t)
  * f: a callable object
  * t: a tuple
  * calls f on all tuple elements: f(x) for all x in t
  */
 template<int pos> struct for_each_impl {
  template<typename T, typename F>
   void operator()(T const & t, F && f) {
    f(std::get<std::tuple_size<T>::value-1-pos>(t));
    for_each_impl<pos-1>()(t, f);
   }
 };

 template<>
  struct for_each_impl<0> {
   template<typename T, typename F>
    void operator() (T const & t, F && f) { f(std::get<std::tuple_size<T>::value-1>(t)); }
  };

 template<typename T, typename F>
  void for_each(T const & t, F && f) {
   for_each_impl<std::tuple_size<T>::value-1>()(t, f);
  }

 /* for_each_enumerate(f, t)
  * f: a callable object
  * t: a tuple
  * calls f on all tuple elements: f(x,n) for all x in t
  */
 template<int pos> struct for_each_enumerate_impl {
  template<typename T, typename F>
   void operator()(T & t, F && f) {
    f(std::get<std::tuple_size<std::c14::decay_t<T>>::value-1-pos>(t),std::tuple_size<T>::value-1-pos);
    for_each_enumerate_impl<pos-1>()(t, f);
   }
 };

 template<>
  struct for_each_enumerate_impl<0> {
   template<typename T, typename F>
    void operator() (T & t, F && f) { f(std::get<std::tuple_size<std::c14::decay_t<T>>::value-1>(t), std::tuple_size<std::c14::decay_t<T>>::value-1); }
  };

 template<typename T, typename F>
  void for_each_enumerate(T & t, F && f) {
   for_each_enumerate_impl<std::tuple_size<std::c14::decay_t<T>>::value-1>()(t, f);
  }

 /**
  * apply_on_tuple(f, t1,t2)
  * f : a callable object
  * t1, t2 two tuples of the same size
  * Returns : [f(i,j) for i,j in zip(t1,t2)]
  */
 template<int pos> struct apply_on_tuple_impl {
  template<typename F, typename T1, typename ... Args>
   auto operator()(F && f, T1 && t1, Args && ... args)
   DECL_AND_RETURN( apply_on_tuple_impl<pos-1>()(std::forward<F>(f),std::forward<T1>(t1),  f(std::get<pos>(t1)), std::forward<Args>(args)...));
 };

 template<> struct apply_on_tuple_impl<-1> {
  template<typename F, typename T1, typename ... Args>
   auto operator()(F && f, T1 && t1, Args && ... args) DECL_AND_RETURN( std::make_tuple(std::forward<Args>(args)...));
 };

 template<typename F, typename T1>
  auto apply_on_tuple (F && f,T1 && t1) DECL_AND_RETURN( apply_on_tuple_impl<std::tuple_size<typename std::remove_const<typename std::remove_reference<T1>::type>::type>::value-1>()(std::forward<F>(f),std::forward<T1>(t1)));

 /**
  * apply_on_zip(f, t1,t2)
  * f : a callable object
  * t1, t2 two tuples of the same size
  * Returns : [f(i,j) for i,j in zip(t1,t2)]
  */
 template<int pos> struct apply_on_zip_impl {
  template<typename F, typename T1, typename T2, typename ... Args>
   auto operator()(F && f, T1 && t1, T2 && t2,  Args && ... args)
   DECL_AND_RETURN( apply_on_zip_impl<pos-1>()(std::forward<F>(f),std::forward<T1>(t1), std::forward<T2>(t2),  f(std::get<pos>(t1),std::get<pos>(t2)), std::forward<Args>(args)...));
 };

 template<> struct apply_on_zip_impl<-1> {
  template<typename F, typename T1, typename T2, typename ... Args>
   auto operator()(F && f, T1 && t1, T2 && t2, Args && ... args) DECL_AND_RETURN( std::make_tuple(std::forward<Args>(args)...));
 };

 template<typename F, typename T1, typename T2>
  auto apply_on_zip (F && f,T1 && t1, T2 && t2) DECL_AND_RETURN( apply_on_zip_impl<std::tuple_size<typename std::remove_const<typename std::remove_reference<T1>::type>::type>::value-1>()(std::forward<F>(f),std::forward<T1>(t1),std::forward<T2>(t2)));

 /**
  * apply_on_zip(f, t1,t2,t3)
  * f : a callable object
  * t1, t2 two tuples of the same size
  * Returns : [f(i,j) for i,j in zip(t1,t2)]
  */
 template<int pos> struct apply_on_zip3_impl {
  template<typename F, typename T1, typename T2, typename T3, typename ... Args>
   auto operator()(F && f, T1 && t1, T2 && t2, T3 && t3, Args && ... args)
   DECL_AND_RETURN( apply_on_zip3_impl<pos-1>()(std::forward<F>(f),std::forward<T1>(t1), std::forward<T2>(t2), std::forward<T3>(t3), 
      f(std::get<pos>(t1),std::get<pos>(t2),std::get<pos>(t3)), std::forward<Args>(args)...));
 };

 template<> struct apply_on_zip3_impl<-1> {
  template<typename F, typename T1, typename T2, typename T3,typename ... Args>
   auto operator()(F && f, T1 && t1, T2 && t2,  T3 && t3, Args && ... args) DECL_AND_RETURN( std::make_tuple(std::forward<Args>(args)...));
 };

 template<typename F, typename T1, typename T2, typename T3>
  auto apply_on_zip (F && f,T1 && t1, T2 && t2, T3 && t3) DECL_AND_RETURN( apply_on_zip3_impl<std::tuple_size<typename std::remove_const<typename std::remove_reference<T1>::type>::type>::value-1>()(std::forward<F>(f),std::forward<T1>(t1),std::forward<T2>(t2),std::forward<T3>(t3)));

 /**
  * call_on_zip(f, t1,t2,t3)
  * f : a callable object
  * t1, t2, t3 three tuples of the same size
  * Returns : void
  * Effect : calls f(i,j,k) for all(i,j,k) in zip(t1,t2,t3)]
  */
 template<int pos> struct call_on_zip3_impl {
  template<typename F, typename T1, typename T2, typename T3>
   void operator()(F && f, T1 && t1, T2 && t2, T3 && t3) { 
    f(std::get<pos>(std::forward<T1>(t1)),std::get<pos>(std::forward<T2>(t2)),std::get<pos>(std::forward<T3>(t3)));
    call_on_zip3_impl<pos-1>()(std::forward<F>(f),std::forward<T1>(t1), std::forward<T2>(t2), std::forward<T3>(t3));
   } 
 };

 template<> struct call_on_zip3_impl<-1> {
  template<typename F, typename T1, typename T2, typename T3> void operator()(F && f, T1 && t1, T2 && t2,  T3 && t3){}
 };

 template<typename F, typename T1, typename T2, typename T3>
  void call_on_zip (F && f,T1 && t1, T2 && t2, T3 && t3) { 
   call_on_zip3_impl<std::tuple_size<typename std::remove_reference<T1>::type>::value-1>()(std::forward<F>(f),std::forward<T1>(t1),std::forward<T2>(t2),std::forward<T3>(t3));
  }

 /**
  * fold(f, t1, init)
  * f : a callable object
  * t a tuple
  * Returns : f(xN,f(x_N-1,...f(x0,r)) on the tuple
  */
 template<int N, int pos, typename T> struct fold_impl {
  template<typename F, typename R>
   auto operator()(F && f, T && t, R && r )
   DECL_AND_RETURN( fold_impl<N,pos-1,T>()(std::forward<F>(f),std::forward<T>(t), f(std::get<N-1-pos>(t), std::forward<R>(r))));
 };

 template<int N, typename T> struct fold_impl<N, -1,T> {
  template<typename F, typename R> R operator()(F && f, T && t, R && r) {return std::forward<R>(r);}
 };

 template<typename F, typename T, typename R>
  auto fold (F && f,T && t, R && r) DECL_AND_RETURN( fold_impl<std::tuple_size<std::c14::decay_t<T>>::value,std::tuple_size<std::c14::decay_t<T>>::value-1,T>()(std::forward<F>(f),std::forward<T>(t),std::forward<R>(r)));
 //template<typename F, typename T, typename R>
 // auto fold (F && f,T const & t, R && r) DECL_AND_RETURN( fold_impl<std::tuple_size<T>::value,std::tuple_size<T>::value-1,T const>()(std::forward<F>(f),t,std::forward<R>(r)));

 /**
  * fold_on_zip(f, t1, t2, init)
  * f : a callable object
  * t1, t2 two tuples of the same size
  * Returns : f(x0,y0,f(x1,y1,,f(....)) for t1 = (x0,x1 ...) and t2 = (y0,y1...).
  */
 template<int pos, typename T1, typename T2> struct fold_on_zip_impl {
  template<typename F, typename R>
   auto operator()(F && f, T1 const & t1, T2 const & t2, R && r )
   DECL_AND_RETURN( fold_on_zip_impl<pos-1,T1,T2>()(std::forward<F>(f),t1,t2, f(std::get<pos>(t1), std::get<pos>(t2), std::forward<R>(r))));
 };

 template<typename T1, typename T2> struct fold_on_zip_impl<-1,T1,T2> {
  template<typename F, typename R> R operator()(F && f, T1 const & t1, T2 const & t2, R && r) {return std::forward<R>(r);}
 };

 template<typename F, typename T1, typename T2, typename R>
  auto fold_on_zip (F && f,T1 const & t1, T2 const & t2, R && r) DECL_AND_RETURN( fold_on_zip_impl<std::tuple_size<T1>::value-1,T1,T2>()(std::forward<F>(f),t1,t2,std::forward<R>(r)));

 /** 
  * filter<int ... I>(t) : 
  *  Given a tuple t, and integers, returns the tuple where the elements at initial position I are dropped.
  */

 // pos = position in the tuple, c : counter tuplesize-1 ->0, I : position to filter
 template<int pos, int c, int ... I> struct filter_impl;

 // default case where pos != the first I : increase pos
 template<int pos, int c, int ... I> struct filter_impl : filter_impl<pos+1, c-1, I...> {};

 // when pos == first I
 template<int pos, int c, int ... I> struct filter_impl<pos, c, pos, I...> {
  template<typename TupleIn, typename TupleOut> auto operator() (TupleIn const & t, TupleOut && out) const
   DECL_AND_RETURN( filter_impl<pos+1,c-1, I...>() ( t, push_back(std::forward<TupleOut>(out),std::get<pos>(t))));
 };

 template<int pos, int ... I> struct filter_impl <pos,-1, I...> {
  template<typename TupleIn, typename TupleOut>  TupleOut operator() (TupleIn const & t, TupleOut && out) const {return out;}
 };

 template<int ...I, typename Tu>
  auto filter(Tu const & tu) DECL_AND_RETURN ( filter_impl<0,std::tuple_size<Tu>::value-1, I...>()(tu, std::make_tuple()));

 template<typename Tu,int ...I> struct filter_t_tr : std::result_of< filter_impl<0,std::tuple_size<Tu>::value-1, I...>( Tu, std::tuple<>)>{};

 template<typename Tu,int ...I> using filter_t = typename filter_t_tr<Tu,I...>::type;

 /** 
  * filter_out<int ... I>(t) : 
  *  Given a tuple t, and integers, returns the tuple where the elements at initial position I are dropped.
  */

 // pos = position in the tuple, c : counter tuplesize-1 ->0, I : position to filter
 template<int pos, int c, int ... I> struct filter_out_impl;

 // default case where pos != the first I : increase pos
 template<int pos, int c, int ... I> struct filter_out_impl<pos, c, pos, I...> : filter_out_impl<pos+1, c-1, I...> {};

 // when pos == first I
 template<int pos, int c, int ... I> struct filter_out_impl {
  template<typename TupleIn, typename TupleOut> auto operator() (TupleIn const & t, TupleOut && out) const
   DECL_AND_RETURN( filter_out_impl<pos+1,c-1, I...> ()( t, push_back(std::forward<TupleOut>(out),std::get<pos>(t))));
 };

 template<int pos, int ... I> struct filter_out_impl <pos,-1, I...> {
  template<typename TupleIn, typename TupleOut> TupleOut operator() (TupleIn const & t, TupleOut && out) const {return out;}
 };

 template<int ...I, typename Tu>
  auto filter_out(Tu const & tu) DECL_AND_RETURN ( filter_out_impl<0,std::tuple_size<Tu>::value-1, I...>()(tu, std::make_tuple()));

 template<typename Tu,int ...I> struct filter_out_t_tr : std::result_of< filter_out_impl<0,std::tuple_size<Tu>::value-1, I...>( Tu, std::tuple<>)>{};

 template<typename Tu,int ...I> using filter_out_t = typename filter_out_t_tr<Tu,I...>::type;

 /** 
  * inverse_filter<int L, int ... I>(t,x)  
  *  Given a tuple t, and integers, returns the tuple R of size L such that filter<I...>(R) == t 
  *  and the missing position are filled with object x.
  *  Precondition (static_assert : sizeof...(I)==size of t)
  *  and max (I) < L
  */

 // pos = position in the tuple, c : counter tuplesize-1 ->0, I : position to filter
 template<int pos, int pos_in, int c, int ... I> struct inverse_filter_impl;

 // default case where pos != the first I
 template<int pos, int pos_in, int c, int ... I> struct inverse_filter_impl<pos, pos_in, c, pos, I...> {
  template<typename TupleIn, typename TupleOut, typename X> auto operator() (TupleIn const & t, TupleOut && out, X const & x) const
   DECL_AND_RETURN( inverse_filter_impl<pos+1,pos_in+1,c-1, I...> ()( t, push_back(std::forward<TupleOut>(out),std::get<pos_in>(t)),x));
 };

 // when pos == first I
 template<int pos, int pos_in, int c, int ... I> struct inverse_filter_impl {
  template<typename TupleIn, typename TupleOut, typename X> auto operator() (TupleIn const & t, TupleOut && out, X const & x) const
   DECL_AND_RETURN( inverse_filter_impl<pos+1,pos_in,c-1, I...> ()( t, push_back(std::forward<TupleOut>(out),x), x));
 };

 template<int pos, int pos_in, int ... I> struct inverse_filter_impl <pos,pos_in, -1, I...> {
  template<typename TupleIn, typename TupleOut, typename X> TupleOut operator() (TupleIn const &, TupleOut && out, X const &) const {return out;}
 };

 // put out for clearer error message
 template< typename Tu, typename X, int L, int ...I> struct inverse_filter_r_type { 
  static_assert(sizeof...(I) == std::tuple_size<Tu>::value, "inverse filter : the # of int must be equal to the tuple size !!");
  typedef inverse_filter_impl<0,0,L-1, I...> type;
 };

 template<int L, int ...I, typename Tu, typename X>
  auto inverse_filter(Tu const & tu, X const &x) DECL_AND_RETURN ( typename inverse_filter_r_type<Tu, X, L,  I...>::type ()(tu, std::make_tuple(),x));

 /** 
  * inverse_filter_out<int ... I>(t,x)  
  *  Given a tuple t, and integers, returns the tuple R such that filter_out<I...>(R) == t 
  *  and the missing position are filled with object x.
  */

 // pos = position in the tuple, c : counter tuplesize-1 ->0, I : position to filter
 template<int pos, int pos_in, int c, int ... I> struct inverse_filter_out_impl;

 // default case where pos != the first I
 template<int pos, int pos_in, int c, int ... I> struct inverse_filter_out_impl { 
  template<typename TupleIn, typename TupleOut, typename X> auto operator() (TupleIn const & t, TupleOut && out, X const & x) const
   DECL_AND_RETURN( inverse_filter_out_impl<pos+1,pos_in+1,c-1, I...> ()( t, push_back(std::forward<TupleOut>(out),std::get<pos_in>(t)),x));
 };

 // when pos == first I
 template<int pos, int pos_in, int c, int ... I> struct inverse_filter_out_impl <pos, pos_in, c, pos, I...> {
  template<typename TupleIn, typename TupleOut, typename X> auto operator() (TupleIn const & t, TupleOut && out, X const & x) const
   DECL_AND_RETURN( inverse_filter_out_impl<pos+1,pos_in,c-1, I...> ()( t, push_back(std::forward<TupleOut>(out),x), x));
 };

 template<int pos, int pos_in, int ... I> struct inverse_filter_out_impl <pos,pos_in, -1, I...> {
  template<typename TupleIn, typename TupleOut, typename X> TupleOut operator() (TupleIn const &, TupleOut && out, X const &) const {return out;}
 };

 template<int ...I, typename Tu, typename X>
  auto inverse_filter_out(Tu const & tu, X const &x) DECL_AND_RETURN ( inverse_filter_out_impl<0,0,std::tuple_size<Tu>::value+sizeof...(I)-1, I...>()(tu, std::make_tuple(),x));

 /** 
  * replace<int ... I>(t,r)  
  *  Given a tuple t, and integers, returns the tuple where the elements at initial position I are replaced by r
  */

 // pos = position in the tuple, c : counter tuplesize-1 ->0, I : position to filter
 template<int pos, int c, int ... I> struct replace_impl;

 // default case where pos != the first I : increase pos
 template<int pos, int c, int ... I> struct replace_impl<pos, c, pos, I...> {
  template<typename TupleIn, typename TupleOut, typename R> auto operator() (TupleIn const & t, TupleOut && out, R const & r) const
   DECL_AND_RETURN( replace_impl<pos+1,c-1, I...> ()( t, push_back(std::forward<TupleOut>(out),r),r));
 };

 // when pos == first I
 template<int pos, int c, int ... I> struct replace_impl {
  template<typename TupleIn, typename TupleOut, typename R> auto operator() (TupleIn const & t, TupleOut && out, R const & r) const
   DECL_AND_RETURN( replace_impl<pos+1,c-1, I...> ()( t, push_back(std::forward<TupleOut>(out),std::get<pos>(t)), r));
 };

 template<int pos, int ... I> struct replace_impl <pos,-1, I...> {
  template<typename TupleIn, typename TupleOut, typename R> TupleOut operator() (TupleIn const & t, TupleOut && out, R const & r) const {return out;}
 };

 template<int ...I, typename Tu, typename R>
  auto replace(Tu const & tu, R const &r) DECL_AND_RETURN ( replace_impl<0,std::tuple_size<Tu>::value-1, I...>()(tu, std::make_tuple(),r));


 /*
  * print a tuple 
  */
 template<int a, int b> struct __s {};
 template<int L, typename T> void print_tuple_impl (std::ostream& os, T const& t, std::integral_constant<int,-1> ) {}
 template<int L, int rpos, typename T> void print_tuple_impl (std::ostream& os, T const& t, std::integral_constant<int,rpos> ) {
  os << std::get<L-rpos-1>(t); 
  if (rpos>0) os << ',';
  print_tuple_impl<L>(os, t, std::integral_constant<int,rpos-1>());
 }
}}

namespace std { 
 template<typename ... T> std::ostream & operator << (std::ostream & os, std::tuple<T...> const & t) { 
  os << "(";
  constexpr int L = sizeof...(T);
  triqs::tuple::print_tuple_impl<L>(os,t,std::integral_constant<int,L-1>()); 
  return os << ")";
 }
}

namespace std {
namespace c14 {

   // a little helper class to wait for the correction that tuple construct is NOT explicit
   template <typename... Args> class tuple : public std::tuple<Args...> {
    public:
    template <typename... Args2> tuple(Args2 &&... args2) : std::tuple<Args...>(std::forward<Args2>(args2)...) {}
   };
  }

  // minimal hack to get the metaprogramming work with this tuple too....
  template <int i, typename... Args>
  auto get(c14::tuple<Args...> const &t) DECL_AND_RETURN(std::get<i>(static_cast<std::tuple<Args...>>(t)));

  template <typename... Args> class tuple_size<c14::tuple<Args...>> : public tuple_size<std::tuple<Args...>> {};

}

#endif

