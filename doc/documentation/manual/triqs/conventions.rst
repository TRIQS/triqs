.. _conventions:

Notations and documentation conventions
=======================================

Notation
--------

In the documentation, we make use in particular of the C++20 concept notation for template.
For example ::

  template<ImmutableArray A> void f(A const & a);

means that f is a template ::

  template<typename A> void f(A const & a);

which is enabled or valid only for types A which models the `ImmutableArray` concept.


Documentation conventions for C++ code
--------------------------------------

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
***

**Synopsis**:

.. code-block:: c

    template<typename T1>
    T1 add (const T1 & a, const T1 & b) ;

an interesting function to add two elements

   This function implements the addition of :math:`a` and :math:`b`: :math:`a+b`


Template parameters
-------------------

* **T1**: any type with a + operator

Parameters
----------

* **a**: the first element
* **b**: the second element

Return value
------------

the sum of a and b, :math:`a+b`

.. note::
     This is a very simple function
.. warning::
     Still alpha...
