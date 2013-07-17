.. highlight:: c

array and array_view : declaration & construction
====================================================================================================================

array and array_view : 

* are the standard d-dimensional cuboid array and the corresponding view.
* allow **custom memory ordering** at compile time.
* model the :ref:`HasImmutableArrayInterface` concept.

Template parameters
----------------------------

* The classes have three template parameters.

 .. code-block:: c

   template <typename ValueType, int Rank, typename Opt= Option::Default > class array_view;
   template <typename ValueType, int Rank, typename Opt= Option::Default > class array;


 ============================    ==================================  ==========================  ====================================================================
 Template parameter              Accepted type                       Access in the class         Meaning                                    
 ============================    ==================================  ==========================  ====================================================================
 ValueType                       normally a scalar, but any default  value_type                  The type of the element of the array           
                                 constructible type (?).                                    
 Rank                            int                                 rank                        The rank of the array 
 Opt                             Option::options< ...>               opt_type                    Compile time options, see below.
 ============================    ==================================  ==========================  ====================================================================

Options template parameters are described :ref:`here <option_template>`.

.. _array_constructors:

Constructors of array
---------------------------


==========================================  ===========================================================================================
Constructors of array                       Comments
==========================================  ===========================================================================================
array()                                     - Empty array of size 0
array(const array &)                        - Copy constructor
array(array &&)                             - Move constructor
array(size_t, ...., size_t)                 - From the dimensions [number of parameters checked at compile time]. 
                                              Does **NOT** initialize the array elements to 0 ! (see compile time option)
                                              Use my_array() =0 to do it explicitely.
array(cuboid_domain<rank> const &)          - New array with the corresponding domain 
array(PyObject * X)                         - Construct a new array from the Python object X. 
                                              NB : X is a borrowed reference, array does affect its counting reference.
                                            - it takes a **copy** of the data of X (or of numpy(X)) into C++. 
                                            - X is first transformed into a numpy by the python numpy lib itself 
                                              (as if you call numpy.array(X)) if X is not a numpy array or an array of the wrong type
                                              (e.g. you construct an array<double,2> from a numpy of int....), and 
                                              copied into the array.
array(const T & X)                          - Type T models the :ref:`HasImmutableArrayInterface` concept.
                                            - X must have the appropriate domain (checked at compile time).
                                            - Constructs a new array of domain X.domain() and fills it with evaluation of X.  
==========================================  ===========================================================================================

.. warning:: 
   The constructor from the dimensions does **NOT** initialize the matrix to 0
   (because it may not be optimal).
   If needed, do it explicitely by (a if the array) `a()=0`;

 - LAYOUT 

Examples
------------

.. compileblock::

    #include <triqs/arrays.hpp>
    using triqs::arrays::array; using triqs::arrays::matrix; 
    using triqs::arrays::vector; using triqs::arrays::permutation; 
    int main(){
      
      // A 3d array of long, C ordering, no option
      array<long, 3> A3(1,2,3);
      
      // A 2d array of double, C ordering, with explicit Bound Checking
      array<double, 2> B(1,2);

      // a matrix of long
      matrix<long> M(2,2);
      
      // a vector of double
      vector<double> V(10);

      // arrays with custom TraversalOrder  

      // C-style
      array<long, 3, 0, permutation(2,1,0)> A0(2,3,4);       
      array<long, 3, 0> A0b; // same type but empty      
     
      // Fortran-style
      array<long, 3, TRAVERSAL_ORDER_FORTRAN> A4 (2,3,4);
      array<long, 3, 0, permutation(0,1,2)> A1b(); //same type but empty      

      // custom :  (i,j,k)  : index j is fastest, then k, then i
      array<long, 3, 0, permutation(1,0,2)> A2(2,3,4); 
    }
   

Constructors of array_views
----------------------------------------------

Automatic construction
^^^^^^^^^^^^^^^^^^^^^^^^^^^

array_view are normally automatically constructed by making (partial) views, ref:`Slicing`, e.g. :: 
 
   array<int,2> A(2,2);
   A(range(),2) ; // --> this makes a view...
   A() ;          // --> this makes a view over the full array

Explicit construction
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To explicitly make a view of an array, use make_view or the ()::

   array<int,2> A(2,2);
   make_view(A);       //-> a view...
   make_view(A) = 13 ; // to assign e.g. 
   A() = 13;           // same thing...


 

======================================================================  =====================================================================================================
Constructors of array_view                                              Comments
======================================================================  =====================================================================================================
array_view(const array_view &)                                          - Copy construction (shallow copy)
array_view(const T & X)                                                 - `[Advanced]` T is any type such that X.indexmap() and X.storage() can be used to construct a view.
array_view(indexmap_type const & I, S_type const &)                     - `[Advanced]` From a couple of indexmap I and storage of type S_type.                                 
======================================================================  =====================================================================================================


