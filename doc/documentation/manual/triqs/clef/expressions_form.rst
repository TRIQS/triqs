.. highlight:: c

Forming CLEF expressions
===========================

In this section, we describe how to form CLEF expressions.
 
Placeholders
-------------------

Loosely speaking, a placeholder is a "variable name" used to build an expression.
Placeholders are declared as ::

  placeholder<Number> Name;

Example ::

  placeholder <1> x_; 
  placeholder <2> y_; 

Note that the only thing of significance in a placeholder is its type (i.e.
a number). A placeholder is **empty**: it contains **no value** at runtime. 
   
  .. warning:: 
    
      As a consequence, defining ::
      
        placeholder <1> y_; 

      would imply that `x_` is the same as `y_`: `x_` == `y_` will be always true.

Forming an expression
------------------------

CLEF expressions are made of: 

* Placeholders
* Binary operations on expressions `(+, -, *, /, >, <, >=, <=, ==)`
* Ternary conditional if_else expressions
* Callable objects which overload the operator () for CLEF expressions, See :ref:`callable_object`.
* Functions overloaded for CLEF expressions. For example, the header `math.hpp` contains the declaration to make 
  the basic function of std `math.h` accept CLEF_expressions.
* In fact, almost anything: the *make_expr* function can be called on any object to make it lazy. 

Examples: 

.. literalinclude:: /documentation/manual/triqs/clef/expressions_form_0.cpp
Note that: 

* Expressions do not compute anything, they just store the expression tree.
* There is no check of correctness here in general: an expression can be well formed, 
  but meaningless, e.g. ::

    auto e = cos(2*x_, 8); // !


.. highlight:: c

Storage of expressions [advanced]
-----------------------------------

CLEF expressions have a complicated (expression template) type encoding the structure of the expression
at compile time::

  auto e = x_ + 2* y_;
  // the type of e is something like   
  expr<tags::plus, placeholder<1>, expr<tags::multiplies, int, placeholder<2> >

Note that: 

* As a user, one *never* has to write such a type.
  One always use expression "on the fly", or use auto.

* Having the whole structure of the expression at compile time allows
  efficient evaluation (it is the principle of expression template: add a ref here).

* Declaring an expression does not do any computation.
  It just stores the expression tree (its structure in the type, and the leaves of the tree).

* Every object in the expression tree is captured by :

   * documentation/manual/triqs it is an lvalue.

   * value it is an rvalue: an rvalue (i.e. a temporary) is *moved* into the tree, using 
     move semantics.
 
   Exceptions: the following objects are always copied: placeholders, expression themselves.

   Example :: 

     double a = 3;
     auto e = a + 2* x_ ;  // a is stored by documentation/manual/triqs (double &), but 2 is stored by value

   The rationale is as follows: 

   * rvalue must be moved, otherwise we would keep (dangling) documentation/manual/triqs to temporaries.
   * for lvalue, keeping a documentation/manual/triqs is quicker. Of course, in the previous example, 
     it is mandatory that a live longer than e ...
    

