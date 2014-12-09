.. highlight:: c

array, matrix & vector 
===========================================

**Synopsis**:

.. code-block:: c

    template <typename ValueType, int Rank, typename TraversalOrder=void> class array;
    template <typename ValueType,           typename TraversalOrder=void> class matrix;
    template <typename ValueType>                                         class vector;


* The library provides three basic containers: 

   * array: general (rectangular) `N`-dimensionnal array; models :ref:`MutableCuboidArray` concept.
   * matrix: models the :ref:`MutableMatrix` concept.
   * vector: models the :ref:`MutableVector` concept.

  and the corresponding view classes: array_view, matrix_view, vector_view (Cf :ref:`partial_views`).

* The matrix and vector are very similar to an array of dimension 2 and 1 respectivily,
  except for their algebra. Array form an array algebra, where operation are done element-wise, while matrix and vector 
  form the usual algebra and vector space of linear algebra.

Template parameters
----------------------------

+-----------------------------------------+-------------------------------+-------------------------------+--------------------------------------+
| Template parameter                      | Accepted type                 | Access in the class           | Meaning                              |
+=========================================+===============================+===============================+======================================+
| ValueType                               | any regular type (typically a | value_type                    | The type of the element of the array |
|                                         | scalar).                      |                               |                                      |
+-----------------------------------------+-------------------------------+-------------------------------+--------------------------------------+
| Rank                                    | int                           | rank                          | The rank of the array                |
+-----------------------------------------+-------------------------------+-------------------------------+--------------------------------------+
| :ref:`TraversalOrder<arr_templ_par_to>` | ull_t                         |                               | Traversal Order for loops and        |
|                                         |                               |                               | iterators                            |
+-----------------------------------------+-------------------------------+-------------------------------+--------------------------------------+

NB: Rank is only present for array, since matrix have rank 2 and vector rank 1.

.. toctree::
   :hidden:

   template_parameters
 
Member types 
--------------------------------------

+--------------+----------------------------------------------------------+
| Member type  | Definitions                                              |
+==============+==========================================================+
| value_type   | ValueType                                                |
+--------------+----------------------------------------------------------+
| view_type    | The corresponding view type                              |
+--------------+----------------------------------------------------------+
| regular_type | The corresponding regular type i.e. the container itself |
+--------------+----------------------------------------------------------+

Member constexpr
--------------------------------------

+--------+------+-------------------------------+
| Member | Type | Definitions                   |
+========+======+===============================+
| rank   | int  | Rank of the container (Rank   |
|        |      | for array), 2 for matrix, 1   |
|        |      | for vector                    |
+--------+------+-------------------------------+

Member functions
---------------------

+-------------------------------------------+------------------------------------------+
| Member function                           | Meaning                                  |
+===========================================+==========================================+
| :ref:`(constructor)<arr_reg_constr>`      |                                          |
+-------------------------------------------+------------------------------------------+
| (destructor)                              |                                          |
+-------------------------------------------+------------------------------------------+
| :ref:`operator =<arr_reg_assign>`         | assigns values to the container          |
+-------------------------------------------+------------------------------------------+
| :ref:`operator +=,-=,*=,/=<arr_comp_ops>` | compound assignment operators            |
+-------------------------------------------+------------------------------------------+
| :ref:`operator ()<arr_call>`              | element of access/views/lazy expressions |
+-------------------------------------------+------------------------------------------+
| begin/cbegin                              | returns iterator to the beginning        |
+-------------------------------------------+------------------------------------------+
| end/cend                                  | returns iterator to the end              |
+-------------------------------------------+------------------------------------------+
| :ref:`resize<arr_resize>`                 | resize the container                     |
+-------------------------------------------+------------------------------------------+
| bool is_empty() const                     | Is the array empty ?                     |
+-------------------------------------------+------------------------------------------+


.. toctree::
   :hidden:

   reg_constructors
   reg_assign
   compound_ops
   call 
   resize
   STL 

Non-member functions
------------------------


+---------------------------------+-------------------------------------------+
| Member function                 | Meaning                                   |
+=================================+===========================================+
| :ref:`swap<arr_swap>`           | Swap of 2 containers                      |
+---------------------------------+-------------------------------------------+
| :ref:`deep_swap<arr_deep_swap>` | Deep swap of the data of 2 containers ??? |
+---------------------------------+-------------------------------------------+
| :ref:`operator\<\<<arr_stream>` | Writing to stream                         |
+---------------------------------+-------------------------------------------+


.. toctree::
   :hidden:

   stream
   swap
