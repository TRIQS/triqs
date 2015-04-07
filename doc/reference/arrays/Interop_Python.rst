.. highlight:: c

Interface with Python numpy arrays
===================================================================

The array, matrix, vector and their views are fully interoperable with the numpy array objects in python.

.. warning::

   TO BE FULLY REWRITTEN


From Python to C++
--------------------------

Value and view classes can be constructed from a PyObject * (the opaque type of python object).
They follow their respective semantic :
   
    * `value classes` (array, matrix, vector) **always** make copies. 
      
       Hence they can be constructed from a python object X which is not an array, but 
       out of which numpy can make an array of the correct type.

    * `view classes` **never** make copies, they present views of the numpy array.
       
       If this is not possible (e.g. the python object is not a numpy, but a list, the type are not exactly the same)
       they throw an exception (`triqs::runtime_error`), with an explanation of the problem.


From C++ to Python
----------------------

Value and view classes have a to_python method with the following synopsis ::

  PyObject * to_python() const 

which return a **new** reference to the numpy array.

To be more precise, two cases must be distinguished.

* array_view constructed from a PyObject * . 
 
  In this case, the array_view's storage is the numpy array, and it keeps a
  (owned) reference to the python array all along its existence.
  This means that Python cannot destroy the array as long as the view exists.
  
  The to_python method simply returns a new reference to this numpy array.

* array or an array_view which was *not* constructed from a PyObject* .
  
  In this case, the storage has been allocated by C++, for example because the array
  was created in a C++ routine. There is no natural numpy array to return.
  The library returns a new numpy array which *owns* the C++ data, 
  so the usage of the class is completely transparent. 
  Python will *automatically* release the memory allocated by the C++ routine 
  when the array in no longer needed.

Examples
-----------------

Split in several files. --> also the theory above.


Memory management
-----------------
.. warning:: 
   
   TO BE WRITTEN

The reference counting system is *compatible* with the Python reference counting (but distinct),
if you compile with python support of course.

As long as you write pure C++ code, you are basically using a shared_ptr to your data block.
No python is involved.

But, if you return your view into a numpy array in python, ownership of your data 
is automatically transfered to the python interpreter::

The interpreter then take the responsability of destroying the data when needed (meaning here, long after f has returned,
when the python object returned will be cleaned).


