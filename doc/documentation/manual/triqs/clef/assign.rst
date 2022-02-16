.. highlight:: c

.. _clef_auto_assign:

Automatic assignment of containers
===================================

Another use of expression is the automatic assignment of containers.

**Synopsis**:

 If C is a container, ::

  C(x_)        << some_expression_of_x_
  C[i_]        << some_expression_of_i_
  C(x_)(i_,j_) << some_expression_of_x_i_j
  C[x_](i_,j_) << some_expression_of_x_i_j

depending of course of the operator that the container support.
The Right Hand Side (RHS) of the << statement can be any expression,
while Left Hand Side (LHS) of the << sign is a container supporting the operation (see below).

This statement simply will be rewritten by the CLEF library as ::

  triqs_clef_auto_assign (C, x_ -> some_expression_of_x_);                  // pseudo code
  triqs_clef_auto_assign (C, [](auto x_) {return some_expression_of_x_;});  // or in C++ lambda syntax

The function `triqs_clef_auto_assign` has to be overloaded for the container and the correct
functional form, and it is expected to fill C by evaluating this function.

Such a function is provided for TRIQS objects (arrays, matrix, Green function),
and also for some STL container like std::vector.

Example :

.. literalinclude:: /documentation/manual/triqs/clef/assign_0.cpp
   :language: cpp

.. literalinclude:: /documentation/manual/triqs/clef/assign_1.output

**Details**

The synopsis of the `triqs_clef_auto_assign` functions is ::

   template<typename Fnt> void triqs_clef_auto_assign (Obj & x, Fnt f);

The compiler will then rewrite ::

   obj(x_,y_, ...) = expression

into ::

   triqs_clef_auto_assign (obj, make_function( expression, x_, y_, ....))

The function must be found by ADL. It is therefore useful to implement it e.g. as a friend function.

Similarly, for [ ], adding a function ::

   template<typename Fnt> void triqs_clef_auto_assign_subscript (Obj & x, Fnt f);

The compiler will rewrite ::

   obj[i_] = expression

into ::

   triqs_clef_auto_assign_subscript (obj, make_function( expression, i_))



A complete example:

.. literalinclude:: /documentation/manual/triqs/clef/assign_1.cpp
   :language: cpp

.. literalinclude:: /documentation/manual/triqs/clef/assign_1.output
