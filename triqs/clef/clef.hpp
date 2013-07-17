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
#ifndef TRIQS_CLEF_CORE_H
#define TRIQS_CLEF_CORE_H
#include <triqs/utility/first_include.hpp>
#include <tuple>
#include <type_traits>
#include <functional>
#include <memory>
#include <complex>
#include <assert.h>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/repetition/repeat_from_to.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>

#define TRIQS_CLEF_MAXNARGS 8

namespace triqs { namespace clef { 
 typedef unsigned long long ull_t;
 template<typename T> struct remove_cv_ref : std::remove_cv< typename std::remove_reference<T>::type> {};
 namespace tags { struct function_class{}; struct function{}; struct subscript{}; struct terminal{}; struct if_else{}; struct unary_op{}; struct binary_op{}; }

 /* ---------------------------------------------------------------------------------------------------
  *  Placeholder and corresponding traits
  *  --------------------------------------------------------------------------------------------------- */
 template<int i, typename T> struct pair { 
  static constexpr int p = i; 
  typedef typename remove_cv_ref <T>::type value_type; 
  value_type const & r;
  value_type const & rhs() const { return r;} 
  pair (T const & t) : r(t){}
 };

 template<int N> struct placeholder {
  static_assert( (N>=0) && (N<64) , "Placeholder number limited to [0,63]");
  static constexpr int index = N;
  template <typename RHS> pair<N,RHS> operator = (RHS const & rhs) { return pair<N,RHS>(rhs);} 
 };

 template<typename T> struct is_placeholder : std::false_type  {}; 
 template<int N> struct is_placeholder <placeholder <N> > : std::true_type  {}; 

 template<typename... T> struct ph_set;
 template<typename T0, typename... T> struct ph_set<T0,T...>{static constexpr ull_t value= ph_set<T0>::value| ph_set<T...>::value;};
 template<typename T> struct ph_set<T>                      {static constexpr ull_t value= 0;};
 template<int N>      struct ph_set<placeholder<N>>         {static constexpr ull_t value= 1ull<<N;};
 template<int i, typename T> struct ph_set<pair<i,T> > : ph_set<placeholder<i>>{};

 /* ---------------------------------------------------------------------------------------------------
  *  Detect if an object is a lazy expression
  *  --------------------------------------------------------------------------------------------------- */
 template<typename... Args>             struct is_any_lazy : std::false_type {};
 template<int N>                        struct is_any_lazy <placeholder <N> > : std::true_type  {}; 
 template <typename T>                  struct is_any_lazy< T > : std::false_type {};
 template <typename T>                  struct is_any_lazy< T&& > : is_any_lazy<T> {};
 template <typename T>                  struct is_any_lazy< T& > : is_any_lazy<T> {};
 template <typename T>                  struct is_any_lazy< T const > : is_any_lazy<T> {};
 template<typename T, typename... Args> struct is_any_lazy<T, Args...> : 
  std::integral_constant<bool, is_any_lazy<T>::value || is_any_lazy<Args...>::value> {}; 

 // a trait that checks the type are value i.e. no &, &&
 template<typename... T> struct has_no_ref : std::true_type {};
 template<typename T0, typename... T> struct has_no_ref<T0,T...> : std::integral_constant<bool, !(std::is_reference<T0>::value)&& has_no_ref<T...>::value> {};

 /* ---------------------------------------------------------------------------------------------------
  * Node of the expression tree
  *  --------------------------------------------------------------------------------------------------- */
 template<typename Tag, typename... T> struct expr {
  static_assert( has_no_ref<T...>::value , "Internal error : expr childs can not be reference "); 
  typedef std::tuple< T ...> childs_t; 
  childs_t childs; 
  expr(expr const & x) = default; 
  expr(expr && x) : childs(std::move(x.childs)) {}
  template<typename... Args> explicit expr(Tag, Args&&...args) : childs(std::forward<Args>(args)...) {} 
  template< typename Args > 
   expr<tags::subscript, expr, typename remove_cv_ref<Args>::type > operator[](Args && args) const 
   { return expr<tags::subscript, expr, typename remove_cv_ref<Args>::type> (tags::subscript(), *this,std::forward<Args>(args));}
  template< typename... Args > 
   expr<tags::function, expr, typename remove_cv_ref<Args>::type...> operator()(Args && ... args) const 
   { return expr<tags::function, expr, typename remove_cv_ref<Args>::type...>(tags::function(), *this,std::forward<Args>(args)...);}
  // only f(i,j) = expr is allowed, in case where f is a clef::function
  // otherwise use the << operator
  template<typename RHS, typename CH = childs_t> 
   typename std::enable_if<std::is_base_of<tags::function_class, typename std::tuple_element<0,CH>::type>::value>::type
   operator= (RHS const & rhs) { *this << rhs;}
  template<typename RHS, typename CH = childs_t> 
   typename std::enable_if<!std::is_base_of<tags::function_class, typename std::tuple_element<0,CH>::type>::value>::type
   operator= (RHS const & rhs) = delete; 
  expr & operator= (expr const & )  = delete; // no ordinary copy
 };
 template<typename Tag, typename... T> struct ph_set< expr<Tag,T... > > : ph_set<T...> {};
 template<typename Tag, typename... T> struct is_any_lazy< expr<Tag,T... > >: std::true_type {};

 /* ---------------------------------------------------------------------------------------------------
  * The basic operations put in a template.... 
  *  --------------------------------------------------------------------------------------------------- */
 template<typename Tag> struct operation;
 
 /// Terminal 
 template<> struct operation<tags::terminal> { template<typename L> L operator()(L&& l) const { return std::forward<L>(l);} };

 /// Function call 
 template<> struct operation<tags::function> { 
  template<typename F, typename... Args> auto operator()(F const & f, Args const & ... args) const -> decltype(f(args...)) { return f(args...);} 
  template<typename F, typename... Args> auto operator()(std::reference_wrapper<F> const &f, Args const & ... args) const -> decltype(f.get()(args...)) { return f.get()(args...);} 
 };

 /// [ ] Call
 template<> struct operation<tags::subscript> { 
  template<typename F, typename Args> auto operator()(F const & f, Args const & args) const -> decltype(f[args]) { return f[args];} 
  template<typename F, typename Args> auto operator()(std::reference_wrapper<F> const &f, Args const & args) const -> decltype(f.get()[args]) { return f.get()[args];} 
 };

 // all binary operators....
#define TRIQS_CLEF_OPERATION(TAG,OP)\
 namespace tags { struct TAG : binary_op { static const char * name() { return BOOST_PP_STRINGIZE(OP);} };}\
 template<> struct operation<tags::TAG> {\
  template<typename L, typename R> auto operator()(L  const & l, R  const & r) const -> decltype(l OP r) { return l OP r;}\
 };\
 template<typename L, typename R>\
 typename std::enable_if<is_any_lazy<L,R>::value, expr<tags::TAG,typename remove_cv_ref<L>::type,typename remove_cv_ref<R>::type> >::type \
 operator OP (L && l, R && r) { return expr<tags::TAG,typename remove_cv_ref<L>::type,typename remove_cv_ref<R>::type> (tags::TAG(),std::forward<L>(l),std::forward<R>(r));}\

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
#define TRIQS_CLEF_OPERATION(TAG,OP)\
 namespace tags { struct TAG : unary_op { static const char * name() { return BOOST_PP_STRINGIZE(OP);} };}\
 template<> struct operation<tags::TAG> {\
  template<typename L> auto operator()(L const & l) const -> decltype(OP l) { return OP l;}\
 };\
 template<typename L>\
 typename std::enable_if<is_any_lazy<L>::value, expr<tags::TAG,typename remove_cv_ref<L>::type> >::type \
 operator OP (L && l) { return expr<tags::TAG,typename remove_cv_ref<L>::type> (tags::TAG(),std::forward<L>(l));}\

 TRIQS_CLEF_OPERATION(negate,      -);
 TRIQS_CLEF_OPERATION(loginot,     !);
#undef TRIQS_CLEF_OPERATION

 /// the only ternary node :  expression if
 template<> struct operation<tags::if_else> { 
  template<typename C, typename A, typename B> auto operator()(C const & c, A const & a, B const & b) const -> decltype((c?a:b)) {
   //static_assert(std::is_same<typename remove_cv_ref<A>::type,typename remove_cv_ref<B>::type>::value, "Expression if : evaluation type must be the same");
   return (c ? a: b);
  } 
 };
 // operator is : if_else( Condition, A, B)
 template<typename C, typename A, typename B> 
  expr<tags::if_else,typename remove_cv_ref<C>::type,typename remove_cv_ref<A>::type,typename remove_cv_ref<B>::type>
  if_else(C && c, A && a, B && b) { 
   return  expr<tags::if_else,typename remove_cv_ref<C>::type,typename remove_cv_ref<A>::type,typename remove_cv_ref<B>::type>
    (tags::if_else(),std::forward<C>(c),std::forward<A>(a),std::forward<B>(b));}

 /* ---------------------------------------------------------------------------------------------------
  * Evaluation of the expression tree.
  *  --------------------------------------------------------------------------------------------------- */

 template<typename Tag, bool IsLazy,  typename... Args> struct operation2;
 template<typename Tag, typename... Args> struct operation2<Tag, true, Args...> { 
  typedef expr<Tag,typename remove_cv_ref<Args>::type ...> rtype;
  rtype operator()(Args && ...  args) const  {return rtype (Tag(), std::forward<Args>(args)...);} 
 };
 template<typename Tag, typename... Args> struct operation2<Tag, false, Args...> { 
  typedef typename std::remove_reference<typename std::result_of<operation<Tag>(Args...)>::type>::type rtype;
  // remove the reference because of ternary if_else in which decltype returns a ref...
  rtype operator()(Args  const & ... args) const  {return operation<Tag>()(args...); } 
 };

 // Generic case : do nothing (for the leaf of the tree except placeholder)
 template<typename T, typename ... Pairs> struct evaluator{
  typedef T rtype;
  rtype operator()( T const & k, Pairs const &... pairs) {return k;}
 };

 // placeholder
 template<int N, int i, typename T, typename... Pairs> struct evaluator< placeholder<N>, pair<i,T>, Pairs... > {
  typedef evaluator< placeholder<N>, Pairs...> eval_t;
  typedef typename eval_t::rtype rtype;
  rtype operator()(placeholder<N>, pair<i,T> const &, Pairs const& ... pairs) { return eval_t()(placeholder<N>(), pairs...);} 
 };
 template<int N, typename T, typename... Pairs> struct evaluator< placeholder<N>, pair<N,T>, Pairs... > {
  typedef typename pair<N,T>::value_type const & rtype;
  rtype operator()(placeholder<N>, pair<N,T> const & p, Pairs const& ...) { return p.r;}
 };

 // general expr node
 template<typename Tag, typename... Childs, typename... Pairs> struct evaluator<expr<Tag, Childs...>, Pairs...> {
  typedef operation2<Tag, is_any_lazy<typename evaluator<Childs, Pairs...>::rtype... >::value, typename evaluator<Childs, Pairs...>::rtype... > OPTYPE;
  typedef typename OPTYPE::rtype rtype;
#define AUX(z,p,unused)  eval(std::get<p>(ex.childs),pairs...) 
#define IMPL(z, NN, unused)  \
  template< int arity = sizeof...(Childs)>\
  typename std::enable_if< arity==NN, rtype>::type\
  operator()(expr<Tag, Childs...> const & ex, Pairs const & ... pairs) const\
  { return OPTYPE()(BOOST_PP_ENUM(NN,AUX,nil));}
  BOOST_PP_REPEAT_FROM_TO(1,BOOST_PP_INC(TRIQS_CLEF_MAXNARGS), IMPL, nil);
#undef AUX
#undef IMPL 
 }; 

 // The general eval function for expressions
 template<typename T, typename... Pairs> 
  typename evaluator<T,Pairs...>::rtype 
  eval (T const & ex, Pairs const &... pairs) { return evaluator<T, Pairs...>()(ex, pairs...); }

#ifdef TRIQS_SHORT_CIRCUIT_NOT_IMPLEMENTED
 // A short circuit if intersection of ph and is 0, no need to evaluate the whole tree......
 template<typename T, typename... Pairs> 
  template<typename Tag, typename... Childs, typename... Pairs> 
  typename std::enable_if< (ph_set<Childs...>::value & ph_set<Pairs...>::value) !=0, typename evaluator<expr<Tag,Childs...>,Pairs...>::rtype > ::type
  eval (expr<Tag,Childs...> const & ex, Pairs const &... pairs) { return evaluator<expr<Tag,Childs...>, Pairs...>()(ex, pairs...); }

 template<typename Tag, typename... Childs, typename... Pairs> 
  typename std::enable_if< (ph_set<Childs...>::value & ph_set<Pairs...>::value) ==0, expr<Tag,Childs...> > ::type
  eval (expr<Tag,Childs...> const & ex, Pairs const &... pairs) { return ex; }
#endif

 /* ---------------------------------------------------------------------------------------------------
  * make_function : transform an expression to a function  
  *  --------------------------------------------------------------------------------------------------- */

 template< typename Expr, int... Is> struct make_fun_impl { 
  Expr ex; // keep a copy of the expression
  make_fun_impl(Expr const & ex_) : ex(ex_) {} 

  // gcc 4.6 crashes (!!!) on the first variant
#define TRIQS_WORKAROUND_GCC46BUG
#ifndef TRIQS_WORKAROUND_GCC46BUG
  template<typename... Args> 
   typename evaluator<Expr,pair<Is,Args>...>::rtype 
   operator()(Args const &... args) const 
   { return evaluator<Expr,pair<Is,Args>...>() ( ex, pair<Is,Args>(args)...); }
#else
  template<typename... Args> struct __eval { 
   typedef evaluator<Expr,pair<Is,Args>...> eval_t;
   typedef typename eval_t::rtype rtype;
   rtype operator()(Expr const &ex , Args const &... args) const { return eval_t() ( ex, pair<Is,Args>(args)...); }
  };
  template<typename... Args> 
   typename __eval<Args...>::rtype operator()(Args const &... args) const { return __eval<Args...>() ( ex, args...); } 
#endif
 };

 template<ull_t x, int... Is> struct ph_filter;
 template<ull_t x, int I0, int... Is> struct ph_filter<x,I0,Is...> { static constexpr ull_t value =  ph_filter<x,Is...>::value & (~ (1ull<<I0));};
 template<ull_t x> struct ph_filter<x> { static constexpr ull_t value = x; }; 

 template< typename Expr, int... Is> struct ph_set<make_fun_impl<Expr,Is...> >  { static constexpr ull_t value = ph_filter <ph_set<Expr>::value, Is...>::value;};
 template< typename Expr, int... Is> struct is_any_lazy<make_fun_impl<Expr,Is...> > : std::integral_constant<bool,ph_set<make_fun_impl<Expr,Is...>>::value !=0>{};

 template< typename Expr, int... Is,typename... Pairs> struct evaluator<make_fun_impl<Expr,Is...>, Pairs...> {
  typedef evaluator<Expr,Pairs...> e_t;
  typedef make_fun_impl<typename e_t::rtype, Is...> rtype;
  rtype operator()(make_fun_impl<Expr,Is...> const & f, Pairs const & ... pairs) const { return rtype( e_t()(f.ex, pairs...));}
 };

 template< typename Expr, typename ... Phs> 
  make_fun_impl<typename std::remove_cv<typename std::remove_reference<Expr>::type>::type,Phs::index...> 
  make_function(Expr && ex, Phs...) { return {ex}; }

 namespace result_of {
  template< typename Expr, typename ... Phs> struct make_function { 
   typedef make_fun_impl<typename std::remove_cv<typename std::remove_reference<Expr>::type>::type,Phs::index...> type;
  };
 }

 /* --------------------------------------------------------------------------------------------------
  *  make_function
  *  x_ >> expression  is the same as make_function(expression,x)
  * --------------------------------------------------------------------------------------------------- */

 template <int N, typename Expr>
  make_fun_impl<Expr,N > operator >> ( placeholder<N> p, Expr&& ex) { return {ex}; } 

 /* ---------------------------------------------------------------------------------------------------
  * Auto assign for ()
  *  --------------------------------------------------------------------------------------------------- */

 // by default it is deleted = not implemented : every class has to define it...
 template<typename T, typename F> void triqs_clef_auto_assign (T,F) = delete;

 // remove the ref_wrapper, terminal ...
 template<typename T, typename F> void triqs_clef_auto_assign (std::reference_wrapper<T> R     ,F && f) { triqs_clef_auto_assign(R.get(),std::forward<F>(f));}
 template<typename T, typename F> void triqs_clef_auto_assign (expr<tags::terminal,T> const & t,F && f) { triqs_clef_auto_assign(std::get<0>(t.childs),std::forward<F>(f));}

 // auto assign of an expr ? (for chain calls) : just reuse the same operator
 template<typename Tag, typename... Childs, typename RHS> 
  void triqs_clef_auto_assign (expr<Tag,Childs...> && ex, RHS const & rhs) { ex << rhs;}

  template<typename Tag, typename... Childs, typename RHS> 
  void triqs_clef_auto_assign (expr<Tag,Childs...> const & ex, RHS const & rhs) { ex << rhs;}

 // The case A(x_,y_) = RHS : we form the function (make_function) and call auto_assign (by ADL)
 template<typename F, typename RHS, int... Is> 
  void operator<<(expr<tags::function, F, placeholder<Is>...> && ex, RHS && rhs) { 
   triqs_clef_auto_assign(std::get<0>(ex.childs), make_function(std::forward<RHS>(rhs), placeholder<Is>()...));
  }
 template<typename F, typename RHS, int... Is> 
  void operator<<(expr<tags::function, F, placeholder<Is>...> const & ex, RHS && rhs) { 
   triqs_clef_auto_assign(std::get<0>(ex.childs), make_function(std::forward<RHS>(rhs), placeholder<Is>()...));
  }

 // any other case e.g. f(x_+y_) = RHS etc .... which makes no sense : compiler will stop
 template<typename F, typename RHS, typename... T> 
  void operator<<(expr<tags::function, F, T...> && ex, RHS && rhs) = delete;
 template<typename F, typename RHS, typename... T> 
  void operator<<(expr<tags::function, F, T...> const & ex, RHS && rhs) = delete;

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
  void triqs_clef_auto_assign_subscript (expr<Tag,Childs...> const & ex, RHS const & rhs) { ex << rhs;}

 // Same thing for the  [ ]
 template<typename F, typename RHS, int... Is> 
  void operator<<(expr<tags::subscript, F, placeholder<Is>...> const & ex, RHS && rhs) { 
   triqs_clef_auto_assign_subscript(std::get<0>(ex.childs), make_function(std::forward<RHS>(rhs), placeholder<Is>()...));
  }
 template<typename F, typename RHS, typename... T> 
  void operator<<(expr<tags::subscript, F, T...> const & ex, RHS && rhs) = delete;

 /* --------------------------------------------------------------------------------------------------
  * Create a terminal node of an object. The reference is wrapped in a reference_wrapper... 
  * --------------------------------------------------------------------------------------------------- */

 template<typename T> expr<tags::terminal,typename remove_cv_ref<T>::type >
  make_expr(T && x){ return expr<tags::terminal,typename remove_cv_ref<T>::type >(tags::terminal(), std::forward<T>(x));}

 template<typename T> auto lazy (T && x) -> decltype (make_expr(std::ref(x))) { return make_expr(std::ref(x)); }

 /* --------------------------------------------------------------------------------------------------
  * Create a call node of an object
  * The object can be kept as a : a ref, a copy, a view
  * --------------------------------------------------------------------------------------------------- */

 namespace result_of { 
  template< typename Obj, typename... Args > struct make_expr_call : 
   std::enable_if< is_any_lazy<Args...>::value, expr<tags::function,typename remove_cv_ref<Obj>::type, typename remove_cv_ref<Args>::type ...> > {};
 }
 template< typename Obj, typename... Args >
  typename result_of::make_expr_call<Obj,Args...>::type 
  make_expr_call(Obj&& obj, Args &&... args) 
  { return typename result_of::make_expr_call<Obj,Args...>::type (tags::function(),std::forward<Obj>(obj), std::forward<Args>(args)...);}

 /* --------------------------------------------------------------------------------------------------
  *  function class : stores any expression polymorphically
  *  f(x_,y_ ) = an expression associates this expression dynamically to f, which 
  *  can then be used as a std::function of the same signature...
  * --------------------------------------------------------------------------------------------------- */
 template<typename F> class function;

 template<typename ReturnType, typename... T> class function<ReturnType(T...)> : tags::function_class  {
  typedef std::function<ReturnType(T...)> std_function_type;
  mutable std::shared_ptr <void>  _exp; // CLEAN THIS MUTABLE ?
  mutable std::shared_ptr < std_function_type > _fnt_ptr;
  public:
  function():_fnt_ptr(new std_function_type ()){}

  template<typename Expr, typename...  X>
   explicit function(Expr const & _e, X... x) : _exp(new Expr(_e)),_fnt_ptr(new std_function_type(make_function(_e, x...))){} 

  ReturnType operator()(T const &... t) const { return (*_fnt_ptr)(t...);}

  template< typename... Args>
   typename triqs::clef::result_of::make_expr_call<function,Args...>::type
   operator()( Args&&... args ) const { return  triqs::clef::make_expr_call (*this,args...);}

  template<typename RHS> friend void triqs_clef_auto_assign (function const & x, RHS rhs) {
   * (x._fnt_ptr) =  std_function_type (rhs);
   x._exp = std::shared_ptr <void> (new typename std::remove_cv<decltype(rhs.ex)>::type (rhs.ex));
  }

 }; 
 /* --------------------------------------------------------------------------------------------------
  *  The macro to make any function lazy
  *  TRIQS_CLEF_MAKE_FNT_LAZY (Arity,FunctionName ) : creates a new function in the triqs::lazy namespace 
  *  taking expressions (at least one argument has to be an expression) 
  *  The lookup happens by ADL, so IT MUST BE USED IN THE triqs::lazy namespace
  * --------------------------------------------------------------------------------------------------- */
#define TRIQS_CLEF_MAKE_FNT_LAZY(name)\
 struct name##_lazy_impl { \
  template<typename... A> auto operator()(A&&... a) const -> decltype (name(std::forward<A>(a)...)) { return name(std::forward<A>(a)...); }\
 };\
 template< typename... A> \
 typename triqs::clef::result_of::make_expr_call<name##_lazy_impl,A...>::type name( A&& ... a) { return make_expr_call(name##_lazy_impl(),a...);}

}} //  namespace triqs::clef

#endif 
