/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2013 by O. Parcollet
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
#pragma once
#include <triqs/utility/first_include.hpp>
#include <triqs/utility/macros.hpp>
#include <triqs/utility/tuple_tools.hpp>
#include <triqs/utility/c14.hpp>
#include <tuple>
#include <type_traits>
#include <functional>
#include <memory>
#include <complex>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>

namespace triqs { namespace clef {
 using ull_t = unsigned long long;
 namespace tags { struct function_class{}; struct function{}; struct subscript{}; struct terminal{}; struct if_else{}; struct unary_op{}; struct binary_op{}; }

 // Compute the type to put in the expression tree.
 // If T is a lvalue reference, pack it into a reference_wrapper, unless force_copy_in_expr<T>::value == true
 // If T is an rvalue reference, we store it as the type (using move semantics).
 template<typename T> struct force_copy_in_expr : std::false_type{};
 template<typename T> struct force_copy_in_expr<T const> : force_copy_in_expr<T>{};

 template< class T> struct expr_storage_impl      {using type=T;};
 template< class T> struct expr_storage_impl<T&>  : std::conditional<force_copy_in_expr<T>::value, typename std::remove_const<T>::type ,std::reference_wrapper<T>>{};
 template< class T> struct expr_storage_impl<T&&> {using type=T;};
 template< class T> struct expr_storage_impl<const T&&> {using type=T;};
 template< class T> struct expr_storage_impl<const T> {using type=T;};

 template <class T> using expr_storage_t = typename expr_storage_impl<T>::type; // helper type

 /* ---------------------------------------------------------------------------------------------------
  *  Placeholder and corresponding traits
  *  --------------------------------------------------------------------------------------------------- */
 template<int i, typename T> struct pair; // forward
 template<typename Tag, typename... T> struct expr; //forward

 // a placeholder is an empty struct, labelled by an int.
 template<int N> struct _ph {
  //static_assert( (N>=0) && (N<64) , "Placeholder number limited to [0,63]");
  static_assert( (N>=0) , "Invalid placeholder range. Placeholder parameters is to [0,15] for each type of placeholder.");
  static constexpr int index = N;
  template <typename RHS> pair<N,RHS> operator = (RHS && rhs) const { return {std::forward<RHS>(rhs)};}
  template <typename... T> expr<tags::function, _ph, expr_storage_t<T>...> operator()(T&&... x) const {
   return {tags::function{}, *this, std::forward<T>(x)...};
  }
  template <typename T> expr<tags::subscript, _ph, expr_storage_t<T>> operator[](T&& x) const {
   return {tags::subscript{}, *this, std::forward<T>(x)};
  }
 };

 //
 constexpr int _ph_flatten_indices(int i, int p) { return (i <= 15 ? p * 16 + i : -1); }

 // user class
 template <int I> using placeholder = _ph<_ph_flatten_indices(I, 0)>;       // the ordinary placeholder (rank 0) with index [0,15]
 template <int I> using placeholder_prime = _ph<_ph_flatten_indices(I, 1)>; // the of placeholder rank 1 with index [0,15]

 // Given a placeholder, gets the index and rank
 template <int I> constexpr int get_placeholder_index(_ph<I>) { return I % 16; }
 template <int I> constexpr int get_placeholder_rank(_ph<I>) { return I / 16; }

 // _ph will always be copied (they are empty anyway).
 template <int N> struct force_copy_in_expr<_ph<N>> : std::true_type {};

 // represent a couple (_ph, value).
 template <int N, typename U> struct pair {
  U rhs;
  static constexpr int p = N;
  using value_type = std14::decay_t<U>;
 };

 // ph_set is a trait that given a pack of type, returns the set of _phs they contain
 // it returns a int in binary coding : bit N in the int is 1 iif at least one T is lazy and contains _ph<N>
 template<typename... T> struct ph_set;
 template<typename T0, typename... T> struct ph_set<T0,T...>{static constexpr ull_t value= ph_set<T0>::value| ph_set<T...>::value;};
 template<typename T> struct ph_set<T>                      {static constexpr ull_t value= 0;};
 template<int N>      struct ph_set<_ph<N>>         {static constexpr ull_t value= 1ull<<N;};
 template<int i, typename T> struct ph_set<pair<i,T> > : ph_set<_ph<i>>{};

 // in_any_lazy : trait to detect if any of Args is a lazy expression
 template<typename... Args>             struct is_any_lazy                     :  std::false_type {};
 template<int N>                        struct is_any_lazy <_ph <N> >  :  std::true_type  {};
 template <typename T>                  struct is_any_lazy< T >                :  std::false_type {};
 template <typename T>                  struct is_any_lazy< T&& >              :  is_any_lazy<T> {};
 template <typename T>                  struct is_any_lazy< T& >               :  is_any_lazy<T> {};
 template <typename T>                  struct is_any_lazy< T const >          :  is_any_lazy<T> {};
 template<typename T, typename... Args> struct is_any_lazy<T, Args...>         :
  std::integral_constant<bool, is_any_lazy<T>::value || is_any_lazy<Args...>::value> {}; 

 template<typename T>
  constexpr bool ClefExpression() { return is_any_lazy<T>::value;}

 template<typename T> struct is_clef_expression : is_any_lazy<T>{};

 /* ---------------------------------------------------------------------------------------------------
  * Node of the expression tree
  *  --------------------------------------------------------------------------------------------------- */
 template<typename Tag, typename... T> struct expr {
  // T can be U, U & (a reference or a value).
  using childs_t = std::tuple<T...>;
  childs_t childs; 
  expr(expr const & x) = default; 
  expr(expr && x) noexcept : childs(std::move(x.childs)) {}
  // a constructor with the Tag make it unambiguous with other constructors...
  template<typename... Args> expr(Tag, Args&&...args) : childs(std::forward<Args>(args)...) {} 
  // [] returns a new lazy expression, with one more layer
  template<typename Args> 
   expr<tags::subscript, expr, expr_storage_t<Args> > operator[](Args && args) const 
   { return {tags::subscript(), *this,std::forward<Args>(args)};}
  // () also ...
  template< typename... Args > 
   expr<tags::function, expr, expr_storage_t<Args>...> operator()(Args && ... args) const 
   { return {tags::function(), *this,std::forward<Args>(args)...};}
  // assignement is in general deleted
  expr & operator= (expr const &)  = delete; // no ordinary assignment
  expr & operator= (expr &&)  = default; // move assign ok 
  // however, this is ok in the case f(i,j) = expr, where f is a clef::function
  template<typename RHS, typename CH = childs_t> 
   ENABLE_IF(std::is_base_of<tags::function_class, typename std::tuple_element<0,CH>::type>)
   operator= (RHS const & rhs) { *this << rhs;}
  template<typename RHS, typename CH = childs_t> 
   DISABLE_IF(std::is_base_of<tags::function_class, typename std::tuple_element<0,CH>::type>)
   operator= (RHS const & rhs) = delete; 
 };
 // set some traits
 template<typename Tag, typename... T> struct ph_set< expr<Tag,T... > > : ph_set<T...> {};
 template<typename Tag, typename... T> struct is_any_lazy< expr<Tag,T... > >: std::true_type {};
 // if we want that subexpression are copied ?
 template<typename Tag, typename... T> struct force_copy_in_expr< expr<Tag,T... > > : std::true_type{};

 template <typename Tag, typename... T> using expr_node_t = expr<Tag, expr_storage_t<T>...>;

 /* ---------------------------------------------------------------------------------------------------
  * The basic operations put in a template.... 
  *  --------------------------------------------------------------------------------------------------- */
 template<typename Tag> struct operation;

 // a little function to clean the reference_wrapper
 template<typename U> U  _cl(U && x) { return std::forward<U>(x);}
 template<typename U> decltype(auto) _cl(std::reference_wrapper<U> x)  { return x.get();}

 // Terminal 
 template<> struct operation<tags::terminal> { template<typename L> L operator()(L&& l) const { return std::forward<L>(l);} };

 // Function call 
 template<> struct operation<tags::function> {
  template <typename F, typename... Args>
  decltype(auto) operator()(F&& f, Args&&... args) const  { return _cl(std::forward<F>(f))(_cl(std::forward<Args>(args))...);}
 };

 // [ ] Call
 template<> struct operation<tags::subscript> {
  template <typename F, typename Args>
  decltype(auto) operator()(F&& f, Args&& args) const  { return _cl(std::forward<F>(f))[_cl(std::forward<Args>(args))];}
 };

 // all binary operators....
#define TRIQS_CLEF_OPERATION(TAG, OP)                                                                                            \
 namespace tags {                                                                                                                \
  struct TAG : binary_op {                                                                                                       \
   static const char* name() { return BOOST_PP_STRINGIZE(OP); }                                                                  \
  };                                                                                                                             \
 }                                                                                                                               \
 template <typename L, typename R>                                                                                               \
 std14::enable_if_t<is_any_lazy<L, R>::value, expr<tags::TAG, expr_storage_t<L>, expr_storage_t<R>>> operator OP(L&& l, R&& r) { \
  return {tags::TAG(), std::forward<L>(l), std::forward<R>(r)};                                                                  \
 }                                                                                                                               \
 template <> struct operation<tags::TAG> {                                                                                       \
  template <typename L, typename R> decltype(auto) operator()(L&& l, R&& r) const {                                              \
   return _cl(std::forward<L>(l)) OP _cl(std::forward<R>(r));                                                                    \
  }                                                                                                                              \
 };

 TRIQS_CLEF_OPERATION(plus,       +);
 TRIQS_CLEF_OPERATION(minus,      -);
 TRIQS_CLEF_OPERATION(multiplies, *);
 TRIQS_CLEF_OPERATION(divides,    /);
 TRIQS_CLEF_OPERATION(greater,    >);
 TRIQS_CLEF_OPERATION(less,       <);
 TRIQS_CLEF_OPERATION(leq,        <=);
 TRIQS_CLEF_OPERATION(geq,        >=);
 TRIQS_CLEF_OPERATION(eq,        ==);
#undef TRIQS_CLEF_OPERATION

 // all unary operators....
#define TRIQS_CLEF_OPERATION(TAG, OP)                                                                                            \
 namespace tags {                                                                                                                \
  struct TAG : unary_op {                                                                                                        \
   static const char* name() { return BOOST_PP_STRINGIZE(OP); }                                                                  \
  };                                                                                                                             \
 }                                                                                                                               \
 template <typename L> std14::enable_if_t<is_any_lazy<L>::value, expr<tags::TAG, expr_storage_t<L>>> operator OP(L&& l) {        \
  return {tags::TAG(), std::forward<L>(l)};                                                                                      \
 }                                                                                                                               \
 template <> struct operation<tags::TAG> {                                                                                       \
  template <typename L> decltype(auto) operator()(L&& l) const { return OP _cl(std::forward<L>(l)); }                            \
 };

 TRIQS_CLEF_OPERATION(negate,      -);
 TRIQS_CLEF_OPERATION(loginot,     !);
#undef TRIQS_CLEF_OPERATION

 // the only ternary node :  expression if
 template<> struct operation<tags::if_else> { 
  // A and B MUST be the same 
  template<typename C, typename A, typename B> A operator()(C const & c, A const & a, B const & b) const {return _cl(c) ? _cl(a): _cl(b);}
  //template<typename C, typename A, typename B> auto operator()(C const & c, A const & a, B const & b) const DECL_AND_RETURN (_cl(c) ? _cl(a): _cl(b));
 };
 // operator is : if_else( Condition, A, B)
 template<typename C, typename A, typename B> 
  expr<tags::if_else,expr_storage_t<C>,expr_storage_t<A>,expr_storage_t<B>>
  if_else(C && c, A && a, B && b) { return {tags::if_else(),std::forward<C>(c),std::forward<A>(a),std::forward<B>(b)};}

 /* ---------------------------------------------------------------------------------------------------
  * Evaluation of the expression tree.
  *  --------------------------------------------------------------------------------------------------- */

 constexpr bool __or() { return false;}
 template <typename... B> constexpr bool __or(bool b, B... bs) { return b || __or(bs...); }

 // Generic case : do nothing (for the leaf of the tree including _ph)
 template <typename T, typename... Pairs> struct evaluator {
  static constexpr bool is_lazy = is_any_lazy<T>::value;
  T const & operator()(T const& k, Pairs const&... pairs) const { return k; }
 };

 // The general eval function for expressions : declaration only
 template <typename T, typename... Pairs> decltype(auto) eval(T const& ex, Pairs const&... pairs);

 // _ph
 template <int N, int i, typename T, typename... Pairs> struct evaluator<_ph<N>, pair<i, T>, Pairs...> {
  using eval_t = evaluator<_ph<N>, Pairs...>;
  static constexpr bool is_lazy = eval_t::is_lazy;
  decltype(auto) operator()(_ph<N>, pair<i, T> const&, Pairs const&... pairs) const { return eval_t()(_ph<N>(), pairs...);}
 };

 template <int N, typename T, typename... Pairs> struct evaluator<_ph<N>, pair<N, T>, Pairs...> {
  static constexpr bool is_lazy = false;
  T operator()(_ph<N>, pair<N, T> const& p, Pairs const&...) const { return p.rhs; }
 };

 // any object hold by reference wrapper is redirected to the evaluator of the object
 template <typename T, typename... Contexts> struct evaluator<std::reference_wrapper<T>, Contexts...> {
  static constexpr bool is_lazy = false;
  decltype(auto) operator()(std::reference_wrapper<T> const& x, Contexts const&... contexts) const {
   return eval(x.get(), contexts...);
  }
 };

 // Dispatch the operations : depends it the result is a lazy expression
 template <typename Tag, typename... Args> expr<Tag, expr_storage_t<Args>...> op_dispatch(std::true_type, Args&&... args) {
  return {Tag(), std::forward<Args>(args)...};
 }

 template <typename Tag, typename... Args> decltype(auto) op_dispatch(std::false_type, Args&&... args) {
  return operation<Tag>()(std::forward<Args>(args)...);
 }

 // the evaluator for an expression
 template <typename Tag, typename... Childs, typename... Pairs> struct evaluator<expr<Tag, Childs...>, Pairs...> {
  static constexpr bool is_lazy = __or(evaluator<Childs, Pairs...>::is_lazy...);

  template <size_t... Is>
  decltype(auto) eval_impl(std14::index_sequence<Is...>, expr<Tag, Childs...> const& ex, Pairs const&... pairs) const {
   return op_dispatch<Tag>(std::integral_constant<bool, is_lazy>{}, eval(std::get<Is>(ex.childs), pairs...)...);
  }

  decltype(auto) operator()(expr<Tag, Childs...> const& ex, Pairs const&... pairs) const {
   return eval_impl(std14::make_index_sequence<sizeof...(Childs)>(), ex, pairs...);
  }
 };

 // The general eval function for expressions
 template <typename T, typename... Pairs>
 decltype(auto) eval(T const& ex, Pairs const&... pairs)  { return evaluator<T, Pairs...>()(ex, pairs...);}

 /* ---------------------------------------------------------------------------------------------------
 * Apply a function object to all the leaves of the expression tree
 *  --------------------------------------------------------------------------------------------------- */

 template <typename F> struct apply_on_each_leaf_impl {
  F f;
  template <typename T> std::c14::enable_if_t<is_clef_expression<T>::value> operator()(T const& ex) {
   tuple::for_each(ex.childs, *this);
  }
  template <typename T> std::c14::enable_if_t<!is_clef_expression<T>::value> operator()(T const& x) { f(x); }
  template <typename T> std::c14::enable_if_t<!is_clef_expression<T>::value> operator()(std::reference_wrapper<T> const& x) {
   f(x.get());
  }
 };

 template <typename F, typename Expr> void apply_on_each_leaf(F&& f, Expr const& ex) {
  auto impl = apply_on_each_leaf_impl<F>{std::forward<F>(f)};
  impl(ex);
 }

 /* ---------------------------------------------------------------------------------------------------
  * make_function : transform an expression to a function
  *  --------------------------------------------------------------------------------------------------- */

 template< typename Expr, int... Is> struct make_fun_impl { 
  Expr ex; // keep a copy of the expression
  make_fun_impl(Expr const & ex_) : ex(ex_) {} 
  
  template<typename... Args> 
   decltype(auto) operator()(Args &&... args) const  { return  evaluator<Expr,pair<Is,Args>...>() ( ex, pair<Is,Args>{std::forward<Args>(args)}...);}
 };

 // values of the ph, excluding the Is ...
 template<ull_t x, int... Is> struct ph_filter;
 template<ull_t x, int I0, int... Is> struct ph_filter<x,I0,Is...> { static constexpr ull_t value =  ph_filter<x,Is...>::value & (~ (1ull<<I0));};
 template<ull_t x> struct ph_filter<x> { static constexpr ull_t value = x; }; 

 template< typename Expr, int... Is> struct ph_set<make_fun_impl<Expr,Is...> >  { static constexpr ull_t value = ph_filter <ph_set<Expr>::value, Is...>::value;};
 template< typename Expr, int... Is> struct is_any_lazy<make_fun_impl<Expr,Is...> > : std::integral_constant<bool,ph_set<make_fun_impl<Expr,Is...>>::value !=0>{};
 template< typename Expr, int... Is> struct force_copy_in_expr<make_fun_impl<Expr,Is...> > : std::true_type{};

 template< typename Expr, typename ... Phs> 
  make_fun_impl<typename remove_cv_ref <Expr>::type,Phs::index...> 
  make_function(Expr && ex, Phs...) { return {ex}; }

 namespace result_of {
  template< typename Expr, typename ... Phs> struct make_function {
   using type = make_fun_impl<typename remove_cv_ref<Expr>::type, Phs::index...>;
  };
 }

 template< typename Expr, int... Is,typename... Pairs> struct evaluator<make_fun_impl<Expr,Is...>, Pairs...> {
  using e_t = evaluator<Expr, Pairs...>;
  static constexpr bool is_lazy = (ph_set<make_fun_impl<Expr, Is...>>::value != ph_set<Pairs...>::value);
  decltype(auto) operator()(make_fun_impl<Expr,Is...> const & f, Pairs const & ... pairs) const  { return  make_function( e_t()(f.ex, pairs...),_ph<Is>()...);}
 };

 template<int ... N> struct ph_list {};
 template<int ... N> ph_list<N...> var( _ph<N> ...) { return {};}

 template<typename Expr, int ... N> 
  auto operator >> (ph_list<N...> &&, Expr const & ex) ->decltype(make_function(ex, _ph<N>()...)) { return  make_function(ex, _ph<N>()...);}
  // add trailing as a workaround around a clang bug here on xcode 5.1.1 (?)

 /* --------------------------------------------------------------------------------------------------
  *  make_function
  *  x_ >> expression  is the same as make_function(expression,x)
  * --------------------------------------------------------------------------------------------------- */

 template <int N, typename Expr>
  make_fun_impl<Expr,N > operator >> (_ph<N> p, Expr&& ex) { return {ex}; } 

 /* ---------------------------------------------------------------------------------------------------
  * Auto assign for ()
  *  --------------------------------------------------------------------------------------------------- */

 // by default it is deleted = not implemented : every class has to define it...
 //template<typename T, typename F> void triqs_clef_auto_assign (T,F) = delete;

 // remove the ref_wrapper, terminal ...
 template <typename T, typename F> void triqs_clef_auto_assign(std::reference_wrapper<T> R, F&& f) {
  triqs_clef_auto_assign(R.get(), std::forward<F>(f));
 }
 template <typename T, typename F> void triqs_clef_auto_assign(expr<tags::terminal, T> const& t, F&& f) {
  triqs_clef_auto_assign(std::get<0>(t.childs), std::forward<F>(f));
 }

 // auto assign of an expr ? (for chain calls) : just reuse the same operator
 template <typename Tag, typename... Childs, typename RHS>
 void triqs_clef_auto_assign(expr<Tag, Childs...>&& ex, RHS const& rhs) {
  ex << rhs;
 }

 template <typename Tag, typename... Childs, typename RHS>
 void triqs_clef_auto_assign(expr<Tag, Childs...> const& ex, RHS const& rhs) {
  ex << rhs;
 }

 // The case A(x_,y_) = RHS : we form the function (make_function) and call auto_assign (by ADL)
 template <typename F, typename RHS, int... Is> void operator<<(expr<tags::function, F, _ph<Is>...>&& ex, RHS&& rhs) {
  triqs_clef_auto_assign(std::get<0>(ex.childs), make_function(std::forward<RHS>(rhs), _ph<Is>()...));
 }
 template <typename F, typename RHS, int... Is>
 void operator<<(expr<tags::function, F, _ph<Is>...> const& ex, RHS&& rhs) {
  triqs_clef_auto_assign(std::get<0>(ex.childs), make_function(std::forward<RHS>(rhs), _ph<Is>()...));
 }
 template <typename F, typename RHS, int... Is> void operator<<(expr<tags::function, F, _ph<Is>...>& ex, RHS&& rhs) {
  triqs_clef_auto_assign(std::get<0>(ex.childs), make_function(std::forward<RHS>(rhs), _ph<Is>()...));
 }

 // any other case e.g. f(x_+y_) = RHS etc .... which makes no sense : compiler will stop
 template <typename F, typename RHS, typename... T> void operator<<(expr<tags::function, F, T...>&& ex, RHS&& rhs) = delete;
 template <typename F, typename RHS, typename... T> void operator<<(expr<tags::function, F, T...>& ex, RHS&& rhs) = delete;
 template <typename F, typename RHS, typename... T> void operator<<(expr<tags::function, F, T...> const& ex, RHS&& rhs) = delete;

 /* ---------------------------------------------------------------------------------------------------
  * Auto assign for []
  *  --------------------------------------------------------------------------------------------------- */

 // by default it is deleted = not implemented : every class has to define it...
 template<typename T, typename F> void triqs_clef_auto_assign_subscript (T,F) = delete;

 // remove the ref_wrapper, terminal ...
 template<typename T, typename F> void triqs_clef_auto_assign_subscript (std::reference_wrapper<T> R     ,F && f) 
 { triqs_clef_auto_assign_subscript(R.get(),std::forward<F>(f));}
 template<typename T, typename F> void triqs_clef_auto_assign_subscript (expr<tags::terminal,T> const & t,F && f) 
 { triqs_clef_auto_assign_subscript(std::get<0>(t.childs),std::forward<F>(f));}

 // auto assign of an expr ? (for chain calls) : just reuse the same operator
 template<typename Tag, typename... Childs, typename RHS> 
  void triqs_clef_auto_assign_subscript (expr<Tag,Childs...> && ex, RHS const & rhs) { ex << rhs;}

 template<typename Tag, typename... Childs, typename RHS> 
  void triqs_clef_auto_assign_subscript (expr<Tag,Childs...> const & ex, RHS const & rhs) { ex << rhs;}

 // Same thing for the  [ ]
 template<typename F, typename RHS, int... Is> 
  void operator<<(expr<tags::subscript, F, _ph<Is>...> const & ex, RHS && rhs) { 
   triqs_clef_auto_assign_subscript(std::get<0>(ex.childs), make_function(std::forward<RHS>(rhs), _ph<Is>()...));
  }
 template<typename F, typename RHS, int... Is> 
  void operator<<(expr<tags::subscript, F, _ph<Is>...> && ex, RHS && rhs) { 
   triqs_clef_auto_assign_subscript(std::get<0>(ex.childs), make_function(std::forward<RHS>(rhs), _ph<Is>()...));
  }

 template <typename F, typename RHS, typename... T> void operator<<(expr<tags::subscript, F, T...>&& ex, RHS&& rhs) = delete;
 template <typename F, typename RHS, typename... T> void operator<<(expr<tags::subscript, F, T...>& ex, RHS&& rhs) = delete;
 template <typename F, typename RHS, typename... T> void operator<<(expr<tags::subscript, F, T...> const& ex, RHS&& rhs) = delete;

 /* --------------------------------------------------------------------------------------------------
  * Create a terminal node of an object. the from clone version force copying the object  
  * --------------------------------------------------------------------------------------------------- */

 // make a node with the ref, unless it is an rvalue (which is moved).
 template<typename T> expr<tags::terminal,expr_storage_t<T> >
  make_expr(T && x){ return {tags::terminal(), std::forward<T>(x)};}

 // make a node from a copy of the object
 template<typename T> expr<tags::terminal,typename remove_cv_ref<T>::type >
  make_expr_from_clone(T && x){ return {tags::terminal(), std::forward<T>(x)};}

 /* --------------------------------------------------------------------------------------------------
  * Create a call node of an object
  * The object can be kept as a : a ref, a copy, a view
  * --------------------------------------------------------------------------------------------------- */

 template<typename T> struct arity { static constexpr int value =-1;};

 namespace _result_of { 
  template< typename Obj, typename... Args > struct make_expr_call : 
   std::enable_if< is_any_lazy<Args...>::value, expr<tags::function,expr_storage_t<Obj>, expr_storage_t<Args> ...> > {
    static_assert (((arity<Obj>::value==-1) || (arity<Obj>::value == sizeof...(Args))), "Object called with a wrong number of arguments"); 
   };
 }
 template< typename Obj, typename... Args >
  typename _result_of::make_expr_call<Obj,Args...>::type 
  make_expr_call(Obj&& obj, Args &&... args) { return {tags::function(),std::forward<Obj>(obj), std::forward<Args>(args)...};}

 template< typename Obj, typename... Args> using make_expr_call_t = typename _result_of::make_expr_call<Obj,Args...>::type;

 /* --------------------------------------------------------------------------------------------------
  * Create a [] call (subscript) node of an object
  * The object can be kept as a : a ref, a copy, a view
  * --------------------------------------------------------------------------------------------------- */

 namespace _result_of { 
  template< typename Obj, typename Arg> struct make_expr_subscript : 
   std::enable_if< is_any_lazy<Arg>::value, expr<tags::subscript,expr_storage_t<Obj>, expr_storage_t<Arg>> > {};
 }
 template< typename Obj, typename Arg>
  typename _result_of::make_expr_subscript<Obj,Arg>::type 
  make_expr_subscript(Obj&& obj, Arg && arg) { return {tags::subscript(),std::forward<Obj>(obj), std::forward<Arg>(arg)};}

 template< typename Obj, typename... Args> using make_expr_subscript_t = typename _result_of::make_expr_subscript<Obj,Args...>::type;

 /* --------------------------------------------------------------------------------------------------
  *  function class : stores any expression polymorphically
  *  f(x_,y_ ) = an expression associates this expression dynamically to f, which 
  *  can then be used as a std::function of the same signature...
  * --------------------------------------------------------------------------------------------------- */
 template<typename F> class function;

 template<typename ReturnType, typename... T> class function<ReturnType(T...)> : tags::function_class  {
  using std_function_type = std::function<ReturnType(T...)>;
  mutable std::shared_ptr <void>  _exp; // CLEAN THIS MUTABLE ?
  mutable std::shared_ptr < std_function_type > _fnt_ptr;
  public:
  function():_fnt_ptr{std::make_shared<std_function_type> ()}{}

  template <typename Expr, typename... X>
  explicit function(Expr const& _e, X... x)
     : _exp(std::make_shared<Expr>(_e)), _fnt_ptr(new std_function_type(make_function(_e, x...))) {}

  ReturnType operator()(T const &... t) const { return (*_fnt_ptr)(t...);}

  template< typename... Args>
   auto operator()( Args&&... args ) const DECL_AND_RETURN(make_expr_call (*this,std::forward<Args>(args)...));

  template<typename RHS> friend void triqs_clef_auto_assign (function const & x, RHS rhs) {
   * (x._fnt_ptr) =  std_function_type (rhs);
   x._exp = std::make_shared<typename std::remove_cv<decltype(rhs.ex)>::type> (rhs.ex);
  }

 };
 template<typename F> struct force_copy_in_expr <function<F>> : std::true_type{};

 /* --------------------------------------------------------------------------------------------------
  *  The macro to make any function lazy
  *  TRIQS_CLEF_MAKE_FNT_LAZY (Arity,FunctionName ) : creates a new function in the triqs::lazy namespace 
  *  taking expressions (at least one argument has to be an expression) 
  *  The lookup happens by ADL, so IT MUST BE USED IN THE triqs::lazy namespace
  * --------------------------------------------------------------------------------------------------- */
#define TRIQS_CLEF_MAKE_FNT_LAZY(name)                                                                                           \
 struct name##_lazy_impl {                                                                                                       \
  template <typename... A> decltype(auto) operator()(A&&... a) const  { return name(std::forward<A>(a)...);}                         \
 };                                                                                                                              \
 template <typename... A> auto name(A&&... a) DECL_AND_RETURN(make_expr_call(name##_lazy_impl(), std::forward<A>(a)...));

#define TRIQS_CLEF_EXTEND_FNT_LAZY(FUN, TRAIT)                                                                                   \
 template <typename A>                                                                                                           \
 std::c14::enable_if_t<TRAIT<A>::value, clef::expr_node_t<clef::tags::function, clef::FUN##_lazy_impl, A>> FUN(A&& a) {          \
  return {clef::tags::function{}, clef::FUN##_lazy_impl{}, std::forward<A>(a)};                                                  \
 }

#define TRIQS_CLEF_IMPLEMENT_LAZY_METHOD(TY, name)                                                                               \
 struct __clef_lazy_method_impl_##TY##_##name {                                                                                  \
  template <typename X, typename... A> decltype(auto) operator()(X&& x, A&&... a) const  { return x.name(std::forward<A>(a)...);}    \
  friend std::ostream& operator<<(std::ostream& out, __clef_lazy_method_impl_##TY##_##name const& x) {                           \
   return out << "apply_method:"<< BOOST_PP_STRINGIZE(name);                                                      \
  }                                                                                                                              \
 };                                                                                                                              \
 template <typename... A>                                                                                                        \
 auto name(A&&... a) DECL_AND_RETURN(make_expr_call(__clef_lazy_method_impl_##TY##_##name{}, *this, std::forward<A>(a)...));

#define TRIQS_CLEF_IMPLEMENT_LAZY_CALL(...)                                                                                      \
 template <typename... Args>                                                                                                     \
 auto operator()(Args&&... args) const& DECL_AND_RETURN(make_expr_call(*this, std::forward<Args>(args)...));                     \
                                                                                                                                 \
 template <typename... Args>                                                                                                     \
     auto operator()(Args&&... args) & DECL_AND_RETURN(make_expr_call(*this, std::forward<Args>(args)...));                      \
                                                                                                                                 \
 template <typename... Args>                                                                                                     \
     auto operator()(Args&&... args) && DECL_AND_RETURN(make_expr_call(std::move(*this), std::forward<Args>(args)...));

}} //  namespace triqs::clef
