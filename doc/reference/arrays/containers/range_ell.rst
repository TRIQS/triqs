.. highlight:: c

.. _arr_range:

range
=========

  `range` mimics the Python `range`. Arguments of the constructor can be: 
  
  * no argument: it then takes the whole set of indices in the dimension (like `:` in python) ::    
    
     A(range(), 0) // take the first column of A

  * two arguments to specify a range ::

     A(range (0,3), 0)  // means  A(0,0), A(1,0), A(2,0)

    .. warning::
       the second element is excluded: range(0,3) is 0,1,2, like in Python.

  * three arguments : a range with a step ::

      A(range(0,4,2), 0) // means A(0,0), A(2,0)  
   

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
