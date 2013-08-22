.. highlight:: c

.. _util_tuple:

Tuple compile time tools
=============================

Very useful for lib developers, they fill a missing gap in the std library.
They implement various standard functional operations, at compile time, 
on tuple...

.. note::

   Simple measures have shown that these routines are **as fast as native code** (tested on gcc, clang, icc),
   due to inlining. They can therefore be used in critical parts of codes.

apply
-----------------------------------------------

*Purpose* : `apply a function on a tuple of arguments`

 Given a function object `f`, and its arguments stored in a tuple `t`, and we want to apply `f` on `t`.
 
 Python equivalent : `f(*t)`

*Synopsis* ::

  template<typename Function, typename Tuple> auto apply (Function && f, Tuple const & t);

*Solution* :

.. compileblock::
        
    #include <triqs/utility/tuple_tools.hpp>
    #include <iostream>
    int main() {
     
    auto fun= [](int i, double x, double y, int k) {return 6*k + i - 1.3*x + 2*y;};
    auto t = std::make_tuple(1,2.3,4.3,8);
   
    auto res = triqs::tuple::apply(fun,t);
    std::cout  << " f(t) =" << res << std::endl ;
    }

for_each 
-------------------------------------------------------------------------

*Purpose* : `apply a function for each element of a tuple (in order)`


 Given a function object `f`, we want to apply it to all elements of a tuple `t`.

 Python equivalent : `for x in t : f(x)`

*Synopsis* ::

  template<typename Function, typename Tuple> void for_each(Tuple const & t, Function && f);
 
*Solution* :

.. compileblock::
        
    #include <triqs/utility/tuple_tools.hpp>
    #include <iostream>
    
    struct print_t { template<typename T> void operator()(T x) { std::cout << x << " "; } };

    int main() {
     auto t = std::make_tuple(1,2.3,4.3,8, "hello");
     triqs::tuple::for_each(t, print_t());

     // C++14 solution : with generic lambda, there is no need to define a print_t ...
     // triqs::tuple::for_each(t, [](auto x) { std::cout<<x<<" ";});
    }



