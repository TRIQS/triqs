.. highlight:: c

Forming lazy expressions
===========================

Expressions are composed from :

* Placeholders
* Binary operations on expressions `(+, -, *, /, >, <, >=, <=, ==)`
* Callable objects (see below) called on expressions
* Conditional if_else expressions
 
Placeholders
----------------

Placeholders are only defined by their type, they contain no data ::

  triqs::lazy_expressions::placeholder <DOM,1> x_; 
  triqs::lazy_expressions::placeholder <DOM,2> y_; 

.. warning:: 
   As a consequence, if you define::
  
     triqs::clef::placeholder <DOM,2> y_; 

  then `x_` is the same as `y_` : `x_` == `y_` will be always true.


Callable objects
--------------------

* The header `math.hpp` contains the declaration to make 
  the basic function of std `math.h` accept lazy_expressions.
  Example ::
 
   #include <triqs/clef/math.hpp>
   cos(2*x_+1) (x_=2) ;
   abs(2*x_-1) (x_=2) ;
   floor(2*x_-1) (x_=2.3) ;
   pow(2*x_+1,2) (x_=2) ;


* To make your object callable, or to overload a function to accept lazy argument,  see :ref:`callable_object`.


Copy policy
--------------------

.. warning:: All objects appearing in a lazy expressions are copied, rather than captured by reference.

This is necessary to store expressions, make partial evaluation.
In order to avoid unncessary copies for large objects, it is necessary to use view type objects ( or shared_ptr).

**NB** : for array, matrix, vector of the triqs::arrays library, this is automatic : triqs::clef 
takes a *view* of the objects when building the expression tree.

Evaluating lazy expressions
===============================

Complete evaluation
--------------------

Expressions are evaluated used named arguments, following the pattern ::

 expression ( placeholder_1 = value_1, placeholder_2 = value_2, ...)

Example ::

  (x_  + 2*y_) ( x_=1, y_ = 2)

  // or (in C++11)
  auto e = x_  + 2*y_;
  auto r = e ( x_=1, y_ = 2);

Note that : 

* The order of placeholder does not matter
* It is an error to put the same placeholder twice.

Partial evaluation
--------------------

The evaluation can also be partial, in which case the compiler replaces the placeholder whose value is provided
and rebuild a new expression tree.

Example ::

 auto expr = x_  + 2*y_;
 expr (x_ =1); // ---> 1 + (2 * y_)
 expr (x_ =x_ + y_); // ---> ((x_ + y_) + (2 * y_))
 expr(x_ =x_ + y_)( x_ = 1, y_ = 2)); //---> 7

During the partial evaluation, all subtrees that can evaluated are evaluated.
For example ::

 (x_ + f(y_)) (y_=1); // --> x + f(1)

Assignment 
------------

Some objects are `lazy-assignable`, which means that calling them on placeholders is a possible LHS.
Expressions like ::

 F(x_,y_) = an_expression_of(x_,y_);

are rewritten by the compiler into ::

 F.set_from( make_function( an_expression_of(x_,y_), x_, y_) );





