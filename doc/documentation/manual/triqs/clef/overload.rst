.. highlight:: c

Overloading functions and methods for CLEF arguments
=====================================================

.. _overload_function:

Overloading functions 
----------------------------

Given a function, it is possible to overload it for CLEF expression arguments, returning a CLEF expression
using the `CLEF_MAKE_FNT_LAZY` macro.

**Synopsis** ::

 namespace triqs { namespace clef { 
 CLEF_MAKE_FNT_LAZY (function_to_make_lazy);
 }}

For example:

.. literalinclude:: /documentation/manual/triqs/clef/overload_0.cpp
   :language: cpp

.. literalinclude:: /documentation/manual/triqs/clef/overload_0.output

Note that: 
 
* This overload **must** be defined in the nda::clef namespace, since it is found by ADL.
* The function `foo` can have many overloads.
* The function `bar` can be a template, BUT then the template must be disabled for lazy expressions.
* The overload is already defined by clef for usual functions: 

.. literalinclude:: /documentation/manual/triqs/clef/overload_1.cpp
   :language: cpp

.. literalinclude:: /documentation/manual/triqs/clef/overload_1.output

.. _callable_object:

Overloading operator() and other methods 
---------------------------------------------------


Similarly to functions, classes can define an `operator()` for CLEF expressions arguments (or any other method).
It is an ordinary operator() that must: 

* Be enabled only when one argument is a CLEF expression
* Return a CLEF expression.

Example: 

.. literalinclude:: /documentation/manual/triqs/clef/overload_2.cpp
   :language: cpp

.. literalinclude:: /documentation/manual/triqs/clef/overload_2.output

**NB**  When the method or the non CLEF operator() is already a template, 
        it must be disabled for clef expression argument, using the trait ::

           clef::is_clef_expression<T...> // true iif one of the T is a clef expression

        as the `bar` function above.


