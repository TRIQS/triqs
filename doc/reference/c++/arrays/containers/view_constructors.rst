.. highlight:: c

.. _arr_view_constr:

Constructors of views
================================

Note that in practice, you rarely explicitly construct a view.
Views are normally automatically built by the :ref:`() operator<arr_call>`.

Constructors of array_view
---------------------------------------

Views have `view semantics`, i.e. they **never** make a copy of the data.

  
======================================================================  =====================================================================================================
Constructors of array_view                                              Comments
======================================================================  =====================================================================================================
array_view(const array_view &)                                          Copy construction (makes a shallow copy)
array_view(const T & X)                                                 T is any type such that X.indexmap() and X.storage() can be used to construct a view.
                                                                        REF to concept here ....
======================================================================  =====================================================================================================


+-----------------------------------+---------------------------------------------------------------------------------------+
| Constructors of array_view        | Comments                                                                              |
+===================================+=======================================================================================+
| array_view(const array_view &)    | Copy construction: create a new view, viewing the same data. Does **not** copy data   |
+-----------------------------------+---------------------------------------------------------------------------------------+
| array_view(const T & X)           | T is any type such that X.indexmap() and X.storage() can be used to construct a view. |
+-----------------------------------+---------------------------------------------------------------------------------------+
| array_view(array_view &&)         | Move construction (trivial here, a copy constructor is very quick anyway).            |
+-----------------------------------+---------------------------------------------------------------------------------------+
| explicit array_view(PyObject * X) | Construct an array_view from the Python object X. Type and rank must match exactly,   |
|                                   | or an exception if thrown                                                             |
+-----------------------------------+---------------------------------------------------------------------------------------+
|                                   | .. note:: X is a borrowed reference, array does not affect its counting reference.    |
+-----------------------------------+---------------------------------------------------------------------------------------+


Constructors of matrix_view, vector_view
------------------------------------------------

Similar to array_view
