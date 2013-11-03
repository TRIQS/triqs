.. highlight:: c

h5::array_stack : stacking arrays or scalars 
================================================================

h5::array_stack writes a sequences of arrays of the same shape (or of scalars) into an hdf5 array with one more dimension, unlimited in the stacking direction.

It is typically used to store a Monte-Carlo data series for later analysis.

* If the base of the stack is an array of rank R, the resulting hdf5 array will be of rank R+1.

* If the base of the stack is a simple number (double, int, ...), R=0.

* The syntax is simple : 

 * The << operator piles up an array/scalar onto the stack.
 * The ++ operator advances by one slice in the stack.
 * The () operator returns a view on the current slice of the stack.

* The stack is bufferized in memory (`bufsize` parameter), so that the file access does not happen too often.

* NB : beware to complex numbers ---> REF TO COMPLEX

Reference 
------------

Here is the :doxy:`full C++ documentation<triqs::arrays::array_stack>` for this class.

.. :
    Breathe Documentation 
  --------------------------
  
  .. doxygenclass:: triqs::arrays::array_stack
   :project: arrays
   :members:
 
 

Tutorial
-----------

A simple example with a stack of double: 

.. literalinclude:: examples_code/h5_stack_ex_sca.cpp

A simple example with a stack of array of rank 2 :

.. literalinclude:: examples_code/h5_stack_ex.cpp



