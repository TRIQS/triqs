.. highlight:: c

Introduction
===========================

The first step consists in forming lazy expressions, before evaluating them.
 
Placeholders
----------------

Loosely speaking, a placeholder is a "variable name" used to build an expression.
Technically, it is a trivial object, with a type but containing no data.
Placeholders are declared as ::

  placeholder<Number> Name;

This declares a placeholder called Name (an empty object for C++). 

Example ::

  placeholder <1> x_; 
  placeholder <2> y_; 

Note that the only thing of significance in a placeholder is its type, since it contains no data.
In other words, a placeholder is **empty**. It contains **no value** at runtime. 
   
  .. warning:: 
    
      As a consequence, defining ::
      
        placeholder <1> y_; 

      would imply that `x_` is the same as `y_` : `x_` == `y_` will be always true.

Forming an expression
-------------------------

Using simple operators, one can form a more complex expression, e.g.::

  auto e = x_ + 2* y_;

e has a complicated type (it is an expression template), which encodes the structure of the expression.
Typically here something like ::
 
  expr<tags::plus, placeholder<1>, expr<tags::multiplies, int, placeholder<2> >

It is worth noting that : 

* As a user, one *never* has to write such a type
  One always use expression "on the fly", or use auto.

* Having the whole structure of the expression at compile time allows
  efficient evaluation (it is the principle of expression template : add a ref here).

* Declaring an expression does not do any computation, hence the name of the library (lazy ...).
  It just stores the expression tree (its structure in the type, and the leaves of the tree).

What is allowed in clef expressions ? 
==========================================

Expressions are composed of :

* Placeholders
* Binary operations on expressions `(+, -, *, /, >, <, >=, <=, ==)`
* Conditional if_else expressions
* Callable objects (see below) called on expressions

Callable objects
--------------------

* Objects can overload the operator () for lazy expressions in order to build more complex
  expressions.

* For example, the header `math.hpp` contains the declaration to make 
  the basic function of std `math.h` accept lazy_expressions.
  
 .. compileblock::
 
   #include <triqs/clef.hpp>
   int main () { 
    triqs::clef::placeholder <1> x_; 

    auto e1 = cos(2*x_+1);
    auto e2 = abs(2*x_-1);
    auto e3 = floor(2*x_-1);
    auto e4 = pow(2*x_+1,2);
   }

* To make your object callable, or to overload a function to accept lazy argument,  see :ref:`callable_object`.

*lazy* function
-------------------

The *lazy* function can be called on any object to make it lazy, e.g. 

.. compileblock::
 
   #include <triqs/clef.hpp>
   #include <vector>
   namespace tql = triqs::clef;
   int main () { 
    std::vector<int> V;
    tql::placeholder<1> i_;
    auto e1 = tql::lazy(V)[i_];
   }

Copy policy in building expressions
---------------------------------------------------

A central question when forming expressions is whether the object at the leaves of the expressions tree
(scalar, placeholders, various callable objects, etc...) should be captured by value or by reference.

In the clef library, the choice has been made to capture them **by value**, i.e. : 

  *By default, all objects appearing in a clef expression are* **copied**, *rather than captured by reference*.

This is necessary to store expressions (with auto like above) for future reuse, transform them into new expressions
(e.g. make partial evaluation). Expressions are ordinary objects. 
If the leaves of the expression tree were captured by reference, a guarantee would have to be made that 
they will live at least as long as the expression itself, or one gets dangling references.

The drawback of this approach is that it can generate useless copies of large objects.
There are two simple solutions to this issue : 

* If you *know* that the object `A` will survive the expression, so using a reference is not a problem
  and use std::reference_wrapper<A> instead of A in the tree.

* If the object has a compagnon view object (like array, array_view). In this case, 
  one wishes to put a view of the object rather than a copy in the expression.



