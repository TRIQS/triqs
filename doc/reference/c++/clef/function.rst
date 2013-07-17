
.. highlight:: c

Clef expressions vs functions
=====================================================================

Clef expressions are **NOT** functions. In short, 

* clef expressions are *evaluated* (the order of argument *does not* matter) ::

   eval( expr, x_=1, y_=2, ...);

* while functions are *called*, as usual (the order of argument *does* matter !) ::

   f(1,2)

It is however possible to transform expressions into functions, *as soon as you specify the order of the placeholders*,
and back.

Function to clef expressions
-------------------------------------

This is immediate, if the function accept lazy arguments, cf :ref:`callable_object`::

  auto e1 = f(x_);


Transforming clef expressions into functions
----------------------------------------------------

make_function
.....................

Given any expression with placeholder `x_`, `y_`, `z_`, ..., `make_function`
transform them into a regular function. If we say ::

  auto f = make_function( clef_expression, placeholder_1, placeholder_2, placeholder_3, ...)

then f is :: 
  
  a function (placeholder_1, placeholder_2, placeholder_3, ...) --> RESULT

where RESULT is : 

* the result of the complete evaluation of the expression if the list of placeholder exhausts the placeholders of the expression.
* otherwise a clef_expression of the remaining placeholders, returning a **function**.

Examples :

* With one variable::

   auto e1 = 2*x_ + 1;
   auto f = make_function( e1, x_);
   f(3) == 7; // ok
   std::function<double(double)> F(f); // ok

* With two variables ::
  
   auto e2 = 2*x_ + y_ + 1;
   auto f = make_function( e2, x_, y_);
   f(3,4) == 11; // ok
   std::function<double(double,double)> F(f); // ok

* Make a function partially ::

   auto f = make_function( 2*x_ + y_ + 1, x_);
   // f is a lazy expression expression with placeholder y_, returning a function...  
   auto f1 = eval (f, y_=1); // f1 is a function x-> 2*x + 2
   f1 (10) == 22; 

* Currifying a function ::
   auto f = make_function ( make_function( 2*x\_ + y\_ + 1, x\_), y\_);
   // f a function y-> x-> 2x+y+1
   // f(y) returns a function x-> 2x+y+1


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

         
