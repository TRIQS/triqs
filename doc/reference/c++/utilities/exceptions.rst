
.. highlight:: c

.. _util_exceptions:

Exceptions
=============


TRIQS defines special exceptions, with the following characteristics :

* they derives from std::exceptions 
* their .what() contains : 

  * the file and line where the exception occurred
  * an additionnal error message (see example below). The error behaves like a std::stringstream, 
    one can accumulate any message
  * a complete stack strace of the C++ code at the exception point, with demangling of the function name (on gcc and clang).

.. warning::
  
   For uniformity, it is highly recommended to use these macros when developing for TRIQS.

Example : 

.. compileblock::

    // automatically included in e.g. arrays, gfs, any triqs library...
    #include <triqs/utility/exceptions.hpp> 
    
    int main() { 
     if (2!=3) TRIQS_RUNTIME_ERROR <<" The condition is false because "<< 2 << "!=" << 3;
    }

The exception can of course be caught :

.. compileblock::

    // automatically included in e.g. arrays, gfs, any triqs library...
    #include <triqs/utility/exceptions.hpp>
    #include <iostream> 
    
    void f() { 
     try { 
      if (2!=3) TRIQS_RUNTIME_ERROR <<" The condition is false because "<< 2 << "!=" << 3;
     }
     catch (triqs::runtime_error const & e) { 
      std::cout << "caught error "<< e.what()<<std::endl;
     }
    } 
    
    int main() { 
     f();
    }



