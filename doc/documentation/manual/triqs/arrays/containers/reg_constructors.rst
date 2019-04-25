
.. highlight:: c

.. _arr_reg_constr:

Constructors
====================

Constructors of array
---------------------------

* The value classes array, matrix and vector have very similar constructors. 
  We will therefore describe only for the array, and mention the difference for matrix and vector below.

* array, matrix, vector are regular types (with value semantics).
  Hence, all these constructors make a **copy** of the data, except of course the move constructor. 

+---------------------------------+------------------------------------------------------------------------------------------+
| Constructors of array           | Comments                                                                                 |
+=================================+==========================================================================================+
| array()                         | Empty array of size 0                                                                    |
+---------------------------------+------------------------------------------------------------------------------------------+
| array(size_t, ...., size_t)     | From the lengths of the array dimensions, with Rank arguments (checked at compile time). |
+---------------------------------+------------------------------------------------------------------------------------------+
| array(const array &)            | Copy construction                                                                        |
+---------------------------------+------------------------------------------------------------------------------------------+
| array(array &&)                 | Move construction                                                                        |
+---------------------------------+------------------------------------------------------------------------------------------+
| array(const T & X)              | Type T models the :ref:`ImmutableCuboidArray` concept.                                   |
|                                 | - X must have the appropriate domain (checked at compile time).                          |
|                                 | - Constructs a new array of domain X.domain() and fills it with evaluation of X.         |
+---------------------------------+------------------------------------------------------------------------------------------+
| array(shape_t const &)          | New array with the corresponding shape (as returned by the function shape. See example   |
|                                 | in section XXX).                                                                         |
+---------------------------------+------------------------------------------------------------------------------------------+
| array(std::initializer_list<T>) | Initialization from a initializer_list of T. Enable for rank==1 only                     |
+---------------------------------+------------------------------------------------------------------------------------------+
| explicit array(PyObject * X)    | Construct a new array from the Python object X.                                          |
|                                 | - it takes a **copy** of the data of X (or of numpy(X)) into C++.                        |
|                                 | - X is first transformed into a numpy by the python numpy lib itself                     |
|                                 | (as if you call numpy.array(X)) if X is not a numpy array or an array of the wrong type  |
|                                 | (e.g. you construct an array<double,2> from a numpy of int....), and                     |
|                                 | copied into the array.                                                                   |
|                                 | .. note: X is a borrowed reference, array does not affect its counting reference.        |
+---------------------------------+------------------------------------------------------------------------------------------+

.. warning:: 
   The constructor from the dimensions does **NOT** initialize the array to 0
   (because it may not be optimal).
   If needed, do it explicitely by (a if the array) `a()=0`

Constructors of matrix
---------------------------

* matrix have similar constructors  

==========================================  =======================================================================================================
Constructors of matrix                       Comments
==========================================  =======================================================================================================
matrix()                                    empty matrix of size 0
matrix(size_t, size_t)                      from the dimensions. Does NOT initialize the elements of matrix to 0 !
matrix(const matrix &)                      copy construction
matrix(matrix &&)                           move construction
matrix (PyObject * X)                       Construct a new matrix from the Python object X.
matrix(const T & X)                         Type T models the :ref:`ImmutableMatrix` or :ref:`ImmutableCuboidArray` (with rank 2). Same as array.
==========================================  =======================================================================================================

Constructors of vector
---------------------------

==========================================  =======================================================================================================
Constructors of vector                       Comments
==========================================  =======================================================================================================
vector()                                    empty vector of size 0
vector(size_t)                              from the dimensions. Does NOT initialize the elements of vector to 0 !
vector(const vector &)                      copy construction
vector(vector &&)                           move construction
vector (PyObject * X)                       Construct a new vector from the Python object X.
vector(const T & X)                         Type T models the :ref:`ImmutableVector` or :ref:`ImmutableCuboidArray` (with rank 1). Same as array.
==========================================  =======================================================================================================

Examples
------------

.. literalinclude:: ./reg_constructors_0.cpp
