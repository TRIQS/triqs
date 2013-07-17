
.. highlight:: python

.. _Debug: 

Bound checking and debug macros
===================================

To be fast, by default, no check are made on the indices while accessing elements or slicing.
However, checks can be activated in two ways : 

* Adding the `BOUND_CHECK` option 

* Defining the debug macro TRIQS_ARRAYS_ENFORCE_BOUNDCHECK, which switches the default option to `BOUND_CHECK`
  for all arrays, matrices and vectors.

In both cases, if the indices are not within the domain of defintion, an exception triqs::arrays::key_error 
will be thrown. It's .what() returns the file and line where the exception occurs, with the stack of all in C++, 
e.g. :

.. compileblock::

    #define TRIQS_ARRAYS_ENFORCE_BOUNDCHECK
    #include <triqs/arrays.hpp>
    using triqs::arrays::array;
    int main(){
        try { 
          array<double,2> A(2,2); A() = 3;   
          std::cout << A(0,3) << std::endl;            
        }
        //catch (triqs::arrays::key_error & e) { std::cout<< e.what()<< std::endl;}
        catch (std::exception & e) { std::cout<< e.what()<< std::endl;} // or like this : triqs::arrays::key_error derives from std::exception
    }


.. note:: Bound Checking is not necessarly used for debugging. You may used it for a given array, catching exception.
   Beware however that bound checking comes with some performance penalty.



