
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

.. triqs_example:: ./exceptions_0.cpp
The exception can of course be caught :

.. triqs_example:: ./exceptions_1.cpp
