.. highlight:: c

.. _arr_view_constr:

Constructors of views
================================

Note that in practice, you rarely explicitly construct a view.
Views are normally automatically built by the :ref:`() operator<arr_call>`.

Constructors of array_view
---------------------------------------

Views have `view semantics`, i.e. they **never** make a copy of the data.

  
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


Constructors of array_const_view
---------------------------------------

+------------------------------------------------------------------------+---------------------------------------------------------------------------------------+
| Constructors of array_const_view<T,R,OptionsFlags, TraversalOrder>     | Comments                                                                              |
+========================================================================+=======================================================================================+
| array_const_view(array_const_view const &)                             | Create a new view, viewing the same data. Does **not** copy data. (copy constructor)  |
+------------------------------------------------------------------------+---------------------------------------------------------------------------------------+
| array_const_view(array_view<T,R,OptionsFlags, TraversalOrder> const &) | Create a new view, viewing the same data. Does **not** copy data                      |
+------------------------------------------------------------------------+---------------------------------------------------------------------------------------+
| array_const_view(const T & X)                                          | T is any type such that X.indexmap() and X.storage() can be used to construct a view. |
+------------------------------------------------------------------------+---------------------------------------------------------------------------------------+
| array_const_view(array_const_view &&)                                  | Move construction (trivial here, a copy constructor is very quick anyway).            |
+------------------------------------------------------------------------+---------------------------------------------------------------------------------------+
| explicit array_const_view(PyObject * X)                                | Construct an array_view from the Python object X. Type and rank must match exactly,   |
|                                                                        | or an exception if thrown                                                             |
+------------------------------------------------------------------------+---------------------------------------------------------------------------------------+
|                                                                        | .. note:: X is a borrowed reference, array does not affect its counting reference.    |
+------------------------------------------------------------------------+---------------------------------------------------------------------------------------+


Constructors of matrix_view, vector_view, matrix_const_view, vector_const_view
-------------------------------------------------------------------------------

Similar to array_view, array_const_view
