
.. highlight:: c

Transform CLEF expressions into functions
===============================================

Clef expressions are **NOT** functions. In short, 

* clef expressions are *evaluated* (the order of argument *does not* matter) ::

   eval( expr, x_=1, y_=2, ...);

* while functions are *called*, as usual (the order of argument *does* matter !) ::

   f(1,2)

It is however possible to transform expressions into functions, *as soon as you specify the order of the placeholders*.
(the opposite is true, if the function accept lazy arguments, cf :ref:`overload_function`).


make_function 
---------------

Given any expression with placeholder `x_`, `y_`, `z_`, ..., `make_function`
transform them into a regular function. If we say ::

  auto f = make_function( clef_expression, placeholder_1, placeholder_2, placeholder_3, ...)

then f is :: 
  
  a function (x1,x2,x3) --> RESULT

where RESULT is: 

* the result of the complete evaluation of the expression if the list of placeholder exhausts the placeholders of the expression.
* otherwise a clef_expression of the remaining placeholders, returning a **function**.


Short notation with >> operator
.....................................

For function of *one* variable, the make_function notation can be simplified into ::

    // same
    auto f = make_function( 2*x_ + y_ + 1, x_);
    auto f = x_ >> 2*x_ + y_ + 1; 

    // same
    auto f = make_function ( make_function( 2*x_ + y_ + 1, y_), x_);
    auto f = x_ >> (y_ >> 2*x_ + y_ + 1) ; 

.. warning:: 
   The notation ::
   
     `x_` >> `y_` >> expression 
   
   is banned because it conflicts with the standard priority of >>. 
   Use parenthesis.

clef::function
--------------------------

The class nda::clef::function stored a function of a given signature 
It is similar to std::function but 
it can be constructed from an expression and an ordered list of placeholders.

clef::function can be assigned with the = operator, Cf example below.

.. note::
  Like std::function, it stores the expression polymorphically, by erasing its type. 
  This might lead to some performance penalty in some case, even though tests do not show that at present...


Examples
---------

  .. literalinclude:: ./function_0.cpp
