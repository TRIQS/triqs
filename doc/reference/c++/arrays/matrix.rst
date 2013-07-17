.. highlight:: c

matrix and matrix_view
============================

matrix and matrix_view : 

* are the class for matrix and the corresponding view.
* allow **custom memory ordering** (C or Fortran) at compile time.
* model the :ref:`HasImmutableArrayInterface` concept.

Template parameters
----------------------------

* The classes have two template parameters.

 .. code-block:: c

     template <typename ValueType, typename Opt = Option::Default> class matrix_view;
     template <typename ValueType, typename Opt = Option::Default> class matrix;

 ============================    ==================================  ==========================  ====================================================================
 Template parameter              Accepted type                       Access in the class         Meaning                                    
 ============================    ==================================  ==========================  ====================================================================
 ValueType                       normally a scalar, but any default  value_type                  The type of the element of the array           
                                 constructible type (?).                                    
 Opt                             Option::options< ...>               opt_type                    Compile time options, see below.
 ============================    ==================================  ==========================  ====================================================================

Options template parameters are described :ref:`here <option_template>`.
The memory order must obviously be only C or Fortran (memory_order is fine, but useless here...).

.. _matrix_constructors:

Constructors
-----------------

Intentionally, matrix and matrix_view have only a few constructors : 

==========================================  ===========================================================================================
Constructors of matrix                       Comments
==========================================  ===========================================================================================
matrix()                                    - empty matrix of size 0
matrix(size_t, size_t)                      - from the dimensions. Does NOT initialize the elements of matrix to 0 !
matrix(const matrix &)                      - copy construction
matrix (PyObject * X)                       - Construct a new matrix from the data of the Python object X. 
                                              NB : X is a borrowed reference, matrix does affect its counting reference.
                                            - it takes a **copy** of the data of X (or of numpy(X)) into C++. 
                                            - X is first transformed into a numpy by the python numpy lib itself 
                                              (as if you call numpy.array(X)) if X is not a numpy array or an array of the wrong type
                                              (e.g. you construct an matrix<double> from a numpy of int....).
matrix(const T & X)                         - Type T models the :ref:`HasImmutableArrayInterface` concept.
                                            - X must have the appropriate domain dimension (checked at compile time).
                                            - Constructs a new matrix of domain X.domain() and fills it with evaluation of X.  
==========================================  ===========================================================================================

* Examples :: 

   matrix<double> Mc(2,2);
   matrix<double,Option::Fortran> Mf(2,2);
 
.. warning:: 
   The constructor from the dimensions does **NOT** initialize the matrix to 0
   (because it may not be optimal).
   If needed, do it explicitely by `a()=0`;



Constructors of matrix_views
----------------------------------------------

Automatic construction
^^^^^^^^^^^^^^^^^^^^^^^^^^^

matrix_view are normally automatically constructed by making (partial) views, ref:`Slicing`, e.g. :: 
 
   matrix<int> A(2,2);
   A(range(),2) ; // --> this makes a view...
   A() ;          // --> this makes a view over the full array


Explicit construction 
^^^^^^^^^^^^^^^^^^^^^^^^

To explicitly make a view of an matrix, use make_view or () ::

   matrix<int> A(2,2);
   make_view(A)        //-> a view...
   make_view(A) = 13 ; // to assign e.g. 
   A() = 13;           // idem

  

======================================================================  ===========================================================================================================
Constructors of matrix_view                                              Comments
======================================================================  ===========================================================================================================
matrix_view(const matrix_view &)                                         - Copy construction  (shallow copy)
matrix_view(const T & X)                                                 - `[Advanced]` T is any type such that X.indexmap() and X.storage() can be used to construct a view.
======================================================================  ===========================================================================================================

 
