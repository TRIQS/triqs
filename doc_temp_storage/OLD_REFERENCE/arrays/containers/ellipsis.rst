.. highlight:: c


.. _arr_ellipsis:

ellipsis
===============

* Ellipsis can be provided in place of `range`, as in python. The type `ellipsis` is similar to range
  except that it is implicitely repeated to as much as necessary.

* Example:

  .. triqs_example:: ./range_ell_0.cpp
* NB: there can be at most one ellipsis per expression (otherwise it would be meaningless).

* Example of usage: 
 
  Ellipsis are useful to write generic algorithms. For example, imagine that you want to sum 
  arrays on their first index :

  .. triqs_example:: ./range_ell_1.cpp
