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
#include <sstream>

namespace triqs { namespace tuple {

 /**
  * t : a tuple
  * x : anything
  * push_back (t,x) -> returns new tuple with x append at the end
  */
 template<typename T, typename X>
 auto push_back(T const &t, X const &x) DECL_AND_RETURN ( std::tuple_cat(t,std::make_tuple(x)));

 /**
  * t : a tuple
  * x : anything
  * push_front (t,x) -> returns new tuple with x append at the first position
  */
 template<typename T, typename X>
 auto push_front(T const &t, X const &x) DECL_AND_RETURN ( std::tuple_cat(std::make_tuple(x),t));

/**
  * apply(f, t)
  * f : a callable object
  * t a tuple
  * Returns : f(t[0], t[1], ...)
  * Equivalent to f(*t) in python ....
  */
 template<int pos,  typename T> struct apply_impl {
  template<typename F, typename ... Args>
   auto operator()(F && f, T const & t, Args && ... args)
   DECL_AND_RETURN( apply_impl<pos-1,T>()(std::forward<F>(f),t, std::get<pos>(t), std::forward<Args>(args)...));
 };

 template<typename T> struct apply_impl<-1,T> {
  template<typename F, typename ... Args>
   auto operator()(F && f, T const & t, Args && ... args) DECL_AND_RETURN( std::forward<F>(f)(std::forward<Args>(args)...));
 };

 template<typename F, typename T>
  auto apply (F && f, T const & t) DECL_AND_RETURN( apply_impl<std::tuple_size<T>::value-1,T>()(std::forward<F>(f),t));

 //template <typename T, typename ClassType, typename ReturnType, typename... Args>
 //ReturnType apply(ReturnType(ClassType::*f)(Args...) const, T const & t) { return apply([f](Args const & ... args) { return (*f)(args...);} ,t);}

 template <typename T, typename ReturnType, typename... Args>
  ReturnType apply( ReturnType(*f)(Args...), T const & t) { return apply([f](Args const & ... args) { return (*f)(args...);} ,t);}

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
   void operator()(T const & t, F && f) {
    f(std::get<std::tuple_size<T>::value-1-pos>(t),std::tuple_size<T>::value-1-pos);
    for_each_impl<pos-1>()(t, f);
   }
 };

 template<>
  struct for_each_enumerate_impl<0> {
   template<typename T, typename F>
    void operator() (T const & t, F && f) { f(std::get<std::tuple_size<T>::value-1>(t), std::tuple_size<T>::value-1); }
  };

 template<typename T, typename F>
  void for_each_enumerate(T const & t, F && f) {
   for_each_enumerate_impl<std::tuple_size<T>::value-1>()(t, f);
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
  auto apply_on_tuple (F && f,T1 && t1) DECL_AND_RETURN( apply_on_tuple_impl<std::tuple_size<typename std::remove_reference<T1>::type>::value-1>()(std::forward<F>(f),std::forward<T1>(t1)));

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
  auto apply_on_zip (F && f,T1 && t1, T2 && t2) DECL_AND_RETURN( apply_on_zip_impl<std::tuple_size<typename std::remove_reference<T1>::type>::value-1>()(std::forward<F>(f),std::forward<T1>(t1),std::forward<T2>(t2)));

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
  auto apply_on_zip (F && f,T1 && t1, T2 && t2, T3 && t3) DECL_AND_RETURN( apply_on_zip3_impl<std::tuple_size<typename std::remove_reference<T1>::type>::value-1>()(std::forward<F>(f),std::forward<T1>(t1),std::forward<T2>(t2),std::forward<T3>(t3)));

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
  * Returns : f(x0,f(x1,f(....)) on the tuple
  */
 template<int N, int pos, typename T> struct fold_impl {
  template<typename F, typename R>
   auto operator()(F && f, T & t, R && r )
   DECL_AND_RETURN( fold_impl<N,pos-1,T>()(std::forward<F>(f),t, f(std::get<N-1-pos>(t), std::forward<R>(r))));
 };

 template<int N, typename T> struct fold_impl<N, -1,T> {
  template<typename F, typename R> R operator()(F && f, T & t, R && r) {return std::forward<R>(r);}
 };

 template<typename F, typename T, typename R>
  auto fold (F && f,T & t, R && r) DECL_AND_RETURN( fold_impl<std::tuple_size<T>::value,std::tuple_size<T>::value-1,T>()(std::forward<F>(f),t,std::forward<R>(r)));
 template<typename F, typename T, typename R>
  auto fold (F && f,T const & t, R && r) DECL_AND_RETURN( fold_impl<std::tuple_size<T>::value,std::tuple_size<T>::value-1,T const>()(std::forward<F>(f),t,std::forward<R>(r)));

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

#endif

