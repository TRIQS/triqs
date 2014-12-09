Template parameters of the containers and views
======================================================

.. _arr_templ_par_to:

TraversalOrder
----------------------------

* TraversalOrder is a type encoding of the optimal traversal in memory.
  Default is void, corresponding to  as regular C-style ordering (slowest index first).

* The traversal of the arrays (iterators, foreach loop) will be written and optimised for this 
  order.

* It is indeed sometimes necessary to know *at compile time* the traversal order to generate
  an optimal code. The default (C-style) is sufficient is most cases, but not always...

* More explanations here...

* Note that this notion is completely independent of the real memory layout of the array, 
  which is a runtime parameter.

  The code will be correct for any order, but may be faster for the TraversalOrder.
  
* TraversalOrder is not present for vector since there is only one possibility in 1d.

* TODO : document the various possibility beyond C style.

+--------------------------+------------------------------------------------------------+
| TraversalOrder           | Meaning                                                    |
+==========================+============================================================+
| void                     | C style traversal (lowest index first).                    |
+--------------------------+------------------------------------------------------------+
| _traversal_fortran       | Fortran style traversal (last index first)                 |
+--------------------------+------------------------------------------------------------+
| _traversal_dynamical     | Traverse in the order specified by the memory layout       |
+--------------------------+------------------------------------------------------------+
| _traversal_custom<2,1,0> | Traverse in the order specified by the permutation (2,1,0) |
+--------------------------+------------------------------------------------------------+


