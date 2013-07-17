.. highlight:: c

A fundamental distinction : value classes vs view classes
=================================================================

A fundamental distinction in TRIQS is the difference between the `value class` 
and the corresponding `view classes`.

* The **value classes** : e.g. array, matrix, vector, ... 
  
  * They own their data. 
  * They have a `value semantic`. 
  * When copied, assigned, they make deep copy. 
  * They are default constructible.
  * They resize themselves under assignment if necessary (like e.g.  std::vector). 
  * They can be put in standard STL containers.


* The **view classes** : e.g. array_view, matrix_view, vector_view, ...
 
  * They are just partial views of the corresponding value class.
  * They do not own their data. 
  * They have a `view semantic`, with reference counting to the data.
  * They never make deep copy, either in copying, constructing, transforming to python, 
  * They can not be resized.
  * They are useful to work on arrays, matrices, or on some part thereof, without making copies
    (e.g. on a slice of an array, a column of a matrix).
 

Each class is therefore provided in two flavors, and it is crucial for the user
to learn and understand their difference in order to use them appropriately.

This distinction extends to more complex objects in TRIQS, such as Green functions.


Behaviour comparison table between value and view classes
------------------------------------------------------------

A detailed comparison of the behaviour of the two type of classes is provided in the following table.



===================  ======================================================================= ======================================================================================
Topics                    value class (e.g. array, matrix, vector0                               view (e.g. array_view, matrix_view, vector_view)  
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
STL                  - Can be used with STL container and algorithms                         - Can not be put in STL containers, but have STL compliant iterators.
Data Contiguity      - The data are contiguous in memory.                                    - The data may not be contiguous in memory  (e.g. a slice).
===================  ======================================================================= ======================================================================================

An example
--------------

To be written....

A simple example ::

 array<int,2> f(size_t n) { 
   array<int,2> res(n,n); 
   res() = 0;
   return res;
 }

 array_view <int,2> f(size_t n) { 
   array<int,2> res(n,n); 
   res() = 0;
   return res;
 }


