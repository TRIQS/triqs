.. highlight:: c

.. _partial_views:

Views 
==============================================================

**Synopsis**:

.. code-block:: c

    template <typename ValueType, int Rank, ull_t OptionsFlags=0, ull_t TraversalOrder=0> class array_view;
    template <typename ValueType,           ull_t OptionsFlags=0, ull_t TraversalOrder=0> class matrix_view;
    template <typename ValueType,           ull_t OptionsFlags=0>                         class vector_view;

    template <typename ValueType, int Rank, ull_t OptionsFlags=0, ull_t TraversalOrder=0> class array_const_view;
    template <typename ValueType,           ull_t OptionsFlags=0, ull_t TraversalOrder=0> class matrix_const_view;
    template <typename ValueType,           ull_t OptionsFlags=0>                         class vector_const_view;


where triqs::ull_t is the type defined by :

.. code-block:: c

    typedef unsigned long long ull_t;

* The view types of X (= array, matrix, vector) are called X_view, and X_const_view, with the same template parameters as the regular type.

* A `view` is give access to a restricted portion of the array, matrix or vector, i.e. a `partial view` of the data.
  The view can also be `complete` (i.e. show all the container).

* The views model the :ref:`MutableCuboidArray`, :ref:`MutableMatrix`, :ref:`MutableVector`, like the corresponding regular type.

* The const views are similar to view, except that theirs contents can not be modified.
  They model  the :ref:`ImmutableCuboidArray`, :ref:`ImmutableMatrix`, :ref:`ImmutableVector`, like the corresponding *const* regular type.

* Views have `view semantics`, i.e. their behave like a reference to the data, they are not regular type.
  In particular, they never make copy of the data.
  See :ref:`view_vs_regular` for a detailed discussion of the difference between a regular type and its corresponding view.

* Views are largely interoperable : it is easy and quick to take a matrix_view of an array, or vice versa.
  Cf constructors belows.

* Const views can be constructed from a view, but the reverse is not true (Cf constructors).

* **Memory Management**

  Views offers a strong guarantee of the existence of the data, like a std::shared_ptr.
  Cf :ref:`arr_view_memory` for details.

  .. toctree::
   :hidden:

   memory


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
| :ref:`OptionsFlags<arr_templ_par_opt>`  | ull_t                         | opt_flags                     | Compile time options                 |
+-----------------------------------------+-------------------------------+-------------------------------+--------------------------------------+
| :ref:`TraversalOrder<arr_templ_par_to>` | ull_t                         |                               | Traversal Order for all loops        |
+-----------------------------------------+-------------------------------+-------------------------------+--------------------------------------+

NB: Rank is only present for array, since matrix have rank 2 and vector rank 1.

.. toctree::
   :hidden:

   template_parameters

Member types 
--------------------------------------

+-----------------+-----------------------------------+
| Member type     | Definitions                       |
+=================+===================================+
| value_type      | ValueType                         |
+-----------------+-----------------------------------+
| view_type       | The corresponding view type       |
+-----------------+-----------------------------------+
| const_view_type | The corresponding const view type |
+-----------------+-----------------------------------+
| regular_type    | The corresponding regular type    |
+-----------------+-----------------------------------+

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
------------------

+-------------------------------------------+------------------------------------------+
| Member function                           | Meaning                                  |
+===========================================+==========================================+
| :ref:`(constructor)<arr_view_constr>`     |                                          |
+-------------------------------------------+------------------------------------------+
| (destructor)                              |                                          |
+-------------------------------------------+------------------------------------------+
| :ref:`operator ()<arr_call>`              | element of access/views/lazy expressions |
+-------------------------------------------+------------------------------------------+
| :ref:`operator =<arr_view_assign>`        | assigns values to the container          |
+-------------------------------------------+------------------------------------------+
| :ref:`operator +=,-=,*=,/=<arr_comp_ops>` | compound assignment operators            |
+-------------------------------------------+------------------------------------------+
| begin/cbegin                              | returns iterator to the beginning        |
+-------------------------------------------+------------------------------------------+
| end/cend                                  | returns iterator to the end              |
+-------------------------------------------+------------------------------------------+
| :ref:`rebind<arr_rebind>`                 | rebind the view                          |
+-------------------------------------------+------------------------------------------+
| bool is_empty() const                     | Is the array empty ?                     |
+-------------------------------------------+------------------------------------------+

.. note:: 
   
   views can not be resized.

.. toctree::
   :hidden:

   view_constructors
   view_assign
   comp_ops
   call 
   rebind
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
