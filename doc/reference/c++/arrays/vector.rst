.. highlight:: c

vector and vector_view
============================

vector and vector_view :

* are the class for a vector and the corresponding view.
* model the :ref:`HasImmutableArrayInterface` concept.


Template parameters
----------------------------

* The classes have two template parameters.

 .. code-block:: c

     template <typename ValueType, typename Opt = Option::Default> class vector_view;
     template <typename ValueType, typename Opt = Option::Default> class vector;

 ============================    ==================================  ==========================  ====================================================================
 Template parameter              Accepted type                       Access in the class         Meaning                                    
 ============================    ==================================  ==========================  ====================================================================
 ValueType                       normally a scalar, but any default  value_type                  The type of the element of the vector
                                 constructible type (?).                                    
 Opt                             Option::options< ...>               opt_type                    Compile time options, see below.
 ============================    ==================================  ==========================  ====================================================================

Options template parameters are described :ref:`here <option_template>`.
The memory order is obviously useless here...

.. _vector_constructors:

Constructors
-----------------

Intentionally, vector and vector_view have only a few constructors : 

==========================================  ===========================================================================================
Constructors of vector                       Comments
==========================================  ===========================================================================================
vector()                                    - empty vector of size 0
vector(size_t)                              - from the dimension. Does **NOT** initialize the elements of the vector to 0 !
vector(const vector &)                      - copy construction
vector (PyObject * X)                       - Construct a new vector from the data of the Python object X. 
                                              NB : X is a borrowed reference, vector does affect its counting reference.
                                            - it takes a **copy** of the data of X (or of numpy(X)) into C++. 
                                            - X is first transformed into a numpy by the python numpy lib itself 
                                              (as if you call numpy.array(X)) if X is not a numpy array or an array of the wrong type
                                              (e.g. you construct an vector<double> from a numpy of int....).
vector(const T & X)                         - Type T models the :ref:`HasImmutableArrayInterface` concept.
                                            - X must have the appropriate domain (checked at compile time).
                                            - Constructs a new vector of domain X.domain() and fills it with evaluation of X.  
==========================================  ===========================================================================================

* Examples :: 

   vector<int> A(10,2);                    
   A()=0; // init A to 0

.. warning:: 
   The constructor from the dimensions does **NOT** initialize the vector to 0
   (because it may not be optimal).
   If needed, do it explicitely by `a()=0`;

Constructors of vector_views
----------------------------------------------

Automatic construction
^^^^^^^^^^^^^^^^^^^^^^^^^^^

vector_view are normally automatically constructed by making (partial) views, ref:`Slicing`, e.g. :: 
 
   tqa::vector<int> A(2,2);
   A(range(),2) ; // --> this makes a view...
   A() ;          // --> this makes a view over the full array


Explicit construction 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To explicitly make a view of an vector, use make_view ::

   vector<int> A(2,2);
   make_view(A) //-> a view...
   make_view(A) = 13 ; // to assign e.g. 

 

======================================================================  ===========================================================================================================
Constructors of vector_view                                             Comments
======================================================================  ===========================================================================================================
vector_view(const array_view &)                                         - Copy construction (shallow copy)
vector_view(const T & X)                                                - `[Advanced]` T is any type such that X.indexmap() and X.storage() can be used to construct a view.
======================================================================  ===========================================================================================================


