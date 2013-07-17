Utilities
##################


Exceptions
---------------

TRIQS defines special exceptions, with the following characteristics :

* they derives from std::exceptions and have the interface
* their .what() contains : 

  * the file and line where the exception occurred
  * an additionnal error message (see example below). The error behaves like a std::stringstream, 
    one can accumulate any message
  * a complete stack strace of the C++ code at the exception point, with demangling of the function name (on gcc and clang).
  * the boost python interface catches these exceptions and report all the information , for debugging.
    So instead of having "Unknown exception in C++", you have a complete trace of the problem that occurred.

.. warning::
  
   For uniformity, it is highly recommended to use these macros when developing for TRIQS.

Example of use ::

  #include <triqs/utility/exceptions.hpp>
  
  ...
  if (!condition) TRIQS_RUNTIME_ERROR <<" The value of a = "<<a<<" is not in the expected range" << R;
  ...
 
List of available exceptions : 

* TRIQS_RUNTIME_ERROR. 

  This macro is simply ::

   #define TRIQS_RUNTIME_ERROR  throw triqs::runtime_error()<<" Triqs runtime error at "<<__FILE__<< " : "<<__LINE__<<"\n\n Trace is :\n\n"<<triqs::utility::stack_trace()<<"\n"

  So it can be catched with the `triqs::runtime_error` type, e.g. ::

   #include <triqs/utility/exceptions.hpp>
   
   try { 
      ...
      if (!condition) TRIQS_RUNTIME_ERROR <<" The value of a = "<<a<<" is not in the expected range" << R;
      ...
      }
   catch (triqs::runtime_error const & e) { 
      std::cout << "caught error "<< e.what()<<std::endl;
      }



