
.. highlight:: python

.. _Debug: 

Bound checking and debug macros
===================================

To be fast, by default, no check are made on the indices while accessing elements or slicing.
However, checks can be activated in two ways : 

* Adding the `Tag::BoundCheck` option (in the options<...> template of the array, matrix, vector)

* Defining the debug macro TRIQS_ARRAYS_ENFORCE_BOUNDCHECK, which switches the default option from `Tag::NoBoundCheck` to `Tag::BoundCheck`
  for all arrays, matrices and vectors.

In both cases, if the indices are not within the domain of defintion, an exception triqs::arrays::key_error 
will be thrown. It's .what() returns the file and line where the exception occurs, with the stack of all in C++, 
e.g. ::

  BOX>./bound_check_nopy 

  catching key error in B 
  triqs::arrays key_error at ..../triqs_source/triqs/arrays/test/C++/./src/IndexMaps/cuboid/./cuboid_domain.hpp : 104

  Trace is :

  ./bound_check_nopy void triqs::arrays::IndexMaps::cuboid_domain<2>::assert_key_in_domain<boost::tuples::tuple<int, int> > \
  (boost::tuples::tuple<int, int> const&) const 0x181 [0x403e11]
  ./bound_check_nopy main 0x916 [0x403016]
  /lib/libc.so.6 __libc_start_main 0xfd [0x7f389e6abc4d]
  ./bound_check_nopy [0x4023c9]

  key out of domain 
  key [1] = 3 is not within [0,3[

Further information on the line of the error in the stack can be retrieved with the addr2line utility `[linux only]`
which retrieve the source file and line from the address of the function (if you compile with -g). The script in TRIQS_SOURCE_DIR/dev_tools/analyse_stack.py
does it automatically for you (when compiled with -g) ::

  BOX> ./bound_check_nopy 2>&1 | analyse_stack.py
   
  ....
  ./bound_check_nopy main 0x8a7 [0x403477]
  ---> /home/parcolle/triqs_source/triqs/arrays/test/C++/bound_check.cpp:83

   

