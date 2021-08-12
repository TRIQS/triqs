.. highlight:: c

.. _util_tuple:

Tuple tools
=============================

Various standard functional operations on tuple (which should be in the STL but are not ...).

.. 
  .. note::
 
    Simple measures have shown that these routines are **as fast as native code** (tested on gcc, clang, icc),
    due to inlining. They can therefore be used in critical parts of codes.

apply
-----------------------------------------------

*Purpose*: `apply a function on a tuple of arguments`

 Given a function object `f`, and its arguments stored in a tuple `t`, and we want to apply `f` on `t`.
 
*Python equivalent*: 
 
 .. code-block::  python

    def apply(f,t): 
      return f(*t)

*Synopsis* ::

  template<typename Function, typename Tuple> auto apply (Function && f, Tuple const & t);
 
*Example* :

.. literalinclude:: tupletools_0.cpp
   :language: cpp

for_each 
-------------------------------------------------------------------------

*Purpose*: `apply a function for each element of a tuple (in order)`


 Given a function object `f`, we want to apply it to all elements of a tuple `t`.

*Python equivalent*: 
 
 .. code-block::  python
   
    def for_each(t,f): 
      for x in t: 
        f(x)

*Synopsis* ::

  template<typename Function, typename Tuple> void for_each(Tuple const & t, Function && f);
 
*Example* :

.. literalinclude:: tupletools_1.cpp 
    :language: cpp

for_each_zip
-------------------------------------------------------------------------

*Purpose*: `apply a function for each element of tuple zip (in order)`


*Python equivalent*: 
 
 .. code-block::  python
   
    def for_each(f,t0,t1): 
      for x0,x1 in itertools.zip(t0,t1): 
        f(x0,x1)

*Synopsis* ::

  template <typename F, typename T0, typename T1>              void for_each_zip(F &&f, T0 &&t0, T1 &&t1);
  template <typename F, typename T0, typename T1, typename T2> void for_each_zip(F &&f, T0 &&t0, T1 &&t1, T2 &&t2);
 
*Example* :

.. literalinclude:: tupletools_2.cpp
   :language: cpp

map
-------------------------------------------------------------------------

*Purpose*: `map a function on a tuple to create a new tuple`

*Python equivalent*: 
 
 .. code-block::  python
   
    def map(f,t): 
      return (f(x) for x in t)

*Synopsis* ::

  template <typename T, typename F> auto map(F &&f, T &&t);

*Returns*: 
  
  The result is a tuple, of the same length as T, made of the evaluation of f on the elements on T 
  
*Example* :

.. literalinclude:: tupletools_3.cpp
   :language: cpp

fold
-------------------------------------------------------------------------

*Purpose*: `reduction of a tuple with a function`

*Python equivalent*: 
 
 .. code-block::  python
   
    def fold(f,t,r): 
      return reduce(f,t,r)

*Synopsis* ::

  (1) template <typename F, typename T, typename R> 
         decltype(auto) fold(F &&f, T &&t, R &&r);                         
  
  (2) template <typename F, typename T0, typename T1, typename R> 
         decltype(auto) fold(F &&f, T0 &&t0, T1 &&t1, R &&r);

*Returns*:: 
  
    f(get<N>(t),
       f(get<N-1>(t),
          ...,
            f(get<0>(t),r)))     (1) 
     
    f(get<N>(t0), get<N>(t1), 
        f(get<N-1>(t0), get<N-1>(t1), 
           ...,
            f(get<0>(t0), get<0>(t1), r)))         (2) 
 
*Parameters* :
  
 * f: a callable object of signature :: 
  
      f(x, r)    -> r'      (1)
      f(x, y, r) -> r'      (2)
   
   The return type of f must be a valid last parameter for f (at least for one overload).
   
 * t: a tuple

 * t0,t1: two tuples of the same size 

 * r: anything that can be a last parameter for f.

 * Precondition: everything so that the resulting expression is valid (in particular, f must be called on each tuple elements, 
   with its return type as last parameter.

.. warning::
  
  The type of the result is not necessarly R: it is automatically deduced from this expression.  Cf example. 


*Example* :

.. literalinclude:: tupletools_4.cpp
   :language: cpp

reverse
-------------------------------------------------------------------------

*Purpose*: `lazy reverse of a tuple`

*Python equivalent*:  None.
 
*Synopsis* ::

  namespace std {
   template<typename ... T> TU  reverse(std::tuple<T...> && x);
   template<typename ... T> TU  reverse(std::tuple<T...> & x);
   template<typename ... T> TU  reverse(std::tuple<T...> const& x);
  }

.. warning::

   reverse is declared in std:: to benefit from ADL (a bit dangerous, but ok here).
     
*Returns*: 
  
  TU is a tuple like type, that :
  
    * Contains a ref of the original tuple, or the tuple if a && was passed.
    * Hence, no copy is ever made.
    * Accepts std::get and std::tuple_size, like tuple.  
 
  reverse(t) can therefore be used in place of a regular tuple in the algorithms of this section.
   
*Example* :

.. literalinclude:: tupletools_reverse.cpp
   :language: cpp

called_on_tuple
-------------------------------------------------------------------------

*Purpose*: `Adapting a function to call with a tuple argument and flatten it`

*Python equivalent*: 
  
  .. code-block::  python
   
     def called_on_tuple(f): 
         return lambda x: f(*x)
 
*Synopsis* ::

 template <typename F> F2 called_on_tuple(F &&f);

*Returns*: 
  
  F2 is a function object which adapts the function f for calling on a tuple.
  
  The following call are therefore equivalent::
   
    called_on_tuple(f)( std::tie(x0,x1,x2))
    
    f(x0,x1,x2)
   
*Example* :

.. literalinclude:: tupletools_called.cpp
   :language: cpp

.. literalinclude:: tupletools_called.output

*Implementation* :

 The C++ is simple in fact :: 

   template <typename F> struct _called_on_tuple {
    F _f;
    template <typename Tu> decltype(auto) operator()(Tu &&tu) {
      return apply(_f, std::forward<Tu>(tu));
    }
   };
     
   template <typename F> _called_on_tuple<F> called_on_tuple(F &&f) {
    return {std::forward<F>(f)};
   }

