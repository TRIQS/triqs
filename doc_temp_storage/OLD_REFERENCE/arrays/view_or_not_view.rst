.. highlight:: c

.. _view_vs_regular:

Regular type vs view type
=================================================================

A fundamental distinction in TRIQS is the difference between the `regular type` 
and the corresponding `view types`.

* The **regular types** 
  
  * Following the definition of A. Stepanov, a regular type is a *value*, much like an int, a double, 
    or a std::vector. Cf http://www.stepanovpapers.com/DeSt98.pdf (I added move semantics to the definition).

  * It has `value semantics`, more precisely ::

      T a;               // default constructible
      T a = b; T a (b);  // copy constructible
      a = b;             // assignment 
      a == b; a != b;    // Equality.

    with the following properties ::
      
      T a = b; assert(a==b);
      T a; a=b; is equivalent to  T a=b;
      T a=c; T b=c; a=d; assert(b==c);

  * When copied, assigned, it makes deep copy of the data and resizes itself if necessary (like e.g.  std::vector). 

  * Examples: 
    
    * std::vector<T> 
    * triqs::arrays::array<T,R>, triqs::arrays::matrix<T>, triqs::arrays::vector<T> 

* The **view types** 
  
  * They have a `reference semantics`.
 
  * They are just partial or complete views of the corresponding regular type.
  * They never make deep copy, either in copying, constructing, transforming to python, 
  * They cannot be resized.
  * They are useful to work on arrays, matrices, or on some part thereof, without making copies
    (e.g. on a slice of an array, a column of a matrix).
 
  * Examples: 
    
    * triqs::arrays::array_view<T,R>, triqs::arrays::matrix_view, triqs::arrays::vector_view<T> 

Many triqs containers (array, matrix, green functions) 
is therefore provided in two flavors, and it is crucial for the user
to learn and understand their difference in order to use them appropriately.


Behaviour comparison table between regular and view types
------------------------------------------------------------

A detailed comparison of the behaviour of the two type of types is provided in the following table.


===================  ======================================================================= ======================================================================================
Topics                    regular type (e.g. array, matrix, vector)                               view type (e.g. array_view, matrix_view, vector_view)  
===================  ======================================================================= ======================================================================================
Contructors          - Constructors create a new fresh memory block.                         - Constructors only make another view of the data. 
Default Contructor   - Default constructor creates an empty array                            - No default constructor (what would it view ?).   
Copy contructors     - Create a new fresh memory block.                                      - Make another view of the data (shallow copy). 
                     - Make a *true* copy of the data.                                       - *Never* copy data, so they are quite quick.   
Assignment (=)       - The assignment operator creates a new datablock if size mismatchs.    - The assignment operator just copy data into the view. 
                     - Hence, assignment never fail for size reason                            Behaviour is undefined if the size of the view is too small.
                       (unless there is a memory allocation exception of course)               (it throws if the array has option `Tag::BoundCheck` or if 
                                                                                               `TRIQS_ARRAYS_ENFORCE_BOUNDCHECK` is defined, Cf below). 
Resizing             - Can be resized, invalidating all references/pointers to the data.     - Can be not be resized.
Invalidation         - References/pointers to the data may become invalid after resize,      - References/pointers to the data  are still valid after assignment.
                       or assignment.
Data Contiguity      - The data are contiguous in memory.                                    - The data may not be contiguous in memory  (e.g. a slice).
===================  ======================================================================= ======================================================================================

