
.. highlight:: c

.. _util_exceptions:

Exceptions
=============


TRIQS defines special exceptions, with the following characteristics :

* they derives from std::exceptions 
* their method .what() returns : 

  * the date and time of the exception (useful in long QMC computations...).
  * the file and line where the exception occurred
  * an additionnal error message (see example below). The error behaves like a std::stringstream, one can accumulate any message

* their method .what() returns : 
  * a complete stack strace of the C++ code at the exception point, with demangling of the function name (on gcc and clang).

In addition, we provide a TRIQS_RUNTIME_ERROR macro to throw exception easily, Cf example below.

.. warning::
  
   For uniformity, it is highly recommended to use these macros when developing for TRIQS.

Example: 

.. literalinclude:: /documentation/manual/triqs/utilities/exceptions_0.cpp
   :language: cpp

The exception can of course be caught :

.. literalinclude:: /documentation/manual/triqs/utilities/exceptions_1.cpp
  :language: cpp

.. literalinclude:: /documentation/manual/triqs/utilities/exceptions_1.output