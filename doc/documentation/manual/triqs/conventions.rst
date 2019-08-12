.. _conventions:

C++11/14, notations and documentation conventions
====================================================

C++11/C++14
---------------

TRIQS is a C++11 library, and as such, it *requires* a last generation C++ compiler (Cf :ref:`require_cxx_compilers`).
C++11 compliant compilers (amongst which gcc and clang) are now widely available.

Indeed, the development of C++ has been very dynamic recently.
The language and its usage is changing very profoundly with the introduction 
of several notions (e.g. move semantics, type deduction, lambda, variadic 
templates, etc.).

A major consequence of this evolution is that writing libraries
has become much more accessible, at a *much* lower cost in development time,
with clearer, shorter and more readable code that is hence maintainable.
Efficient techniques which were considered before as complex and reserved to professional C++ experts
are now becoming simple to implement, such as e.g. expression templates.

The implementation of most of the TRIQS library (e.g. clef, arrays) would be either impossible or at least 
much more complex and time consuming (with a lot of abstruse boost-like constructions)
in previous versions of C++.

Besides, this evolution is not over (in fact it seems to accelerate !).
The new coming standard, C++14, expected to be adopted and implemented very soon, 
will bring significant improvements. In particular, the concept support (template constraints)
will hopefully solve the most problematic issue with metaprogramming techniques, namely, the lack of concept
check at compile time, resulting in long and obscur error messages from the compiler when *using* the library,
which can leave the non-C++-expert user quite clueless...
Hence, TRIQS will move to C++14 as soon as compilers are available.

Notation
----------

In the documentation, we make use in particular of the C++14 concept notation for template.
For example ::

  template<ImmutableArray A> void f(A const & a);

means that f is a template ::
  
  template<typename A> void f(A const & a);
 
which is enabled or valid only for types A which models the `ImmutableArray` concept.


Documentation conventions for c++ code
--------------------------------------------

The c++ source code is documented with `Doxygen <http://www.doxygen.org>`_-like conventions, with the following keywords, preceded by @:
 - **param**: parameter of a function
 - **tparam**: template parameter
 - **return**: description of the return type
 - **note**: a note on the class/functions
 - **warning**: a warning
 
Latex code is written between dollar signs (e.g $f(x)=x$), and links to other classes or functions are to be put between double brackets (e.g. [[imfreq]]).

For the rest, we use the `RST <http://sphinx-doc.org/rest.html>`_ syntax.

Here is one example::

 /// an interesting function to add two elements
 /**
   This function implements the addition of $a$ and $b$: $a+b$
   @tparam T1 any type with a + operator
   @param a the first element
   @param b the second element
   @return the sum of a and b, $a+b$
   @note This is a very simple function
   @warning Still alpha...
  */
 template<typename T1>
 T1 add(T1 const & a, T1 const & b);

This generates:

.. highlight:: c

.. code-block:: c

    #include <test.hpp>

add
*****

**Synopsis**:

.. code-block:: c

    template<typename T1>
    T1 add (const T1 & a, const T1 & b) ;

an interesting function to add two elements

   This function implements the addition of :math:`a` and :math:`b`: :math:`a+b`


Template parameters
-----------------------

* **T1**: any type with a + operator
Parameters
-------------

* **a**: the first element
* **b**: the second element

Return value
--------------

the sum of a and b, :math:`a+b`



.. note::
     This is a very simple function
.. warning::
     Still alpha...
