.. highlight:: c

.. _gf_product: 

**[gf<cartesian_product<X,Y,...>>]** Multiple variables 
========================================================

.. warning::

   This part of the library is still in alpha, in development.
 
Domain & mesh
----------------

The domain is a cartesian product of domains.

The mesh is a cartesian product of the corresponding meshes.


Singularity
-------------

None.

Interpolation method
---------------------

A multidimensional linear interpolation is automatically
built from the linear interpolation of the domains.

Data storage
---------------

Like single variables gf, the first index of the array is a flattening 
of the indices of the composite mesh.


HDF5 storage convention
---------------------------

For convenience, in hdf5 files, the arrays has higher dimension,
so that the first indices are *not* flatten. EXPLAIN.

Functional techniques
------------------------

See: 

.. toctree::
  
   :maxdepth: 1

   gf_part_eval_curry

 
Examples
---------
.. triqs_example:: ./gf_product_0.cpp
