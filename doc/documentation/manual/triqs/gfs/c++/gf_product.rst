.. highlight:: c

.. _gf_product: 

**[gf<prod<X,Y,...>>]** Multiple variables 
========================================================

Domain & mesh
----------------

The domain is a cartesian product of domains.

The mesh is a cartesian product of the corresponding meshes.

Subscript operator
------------------

In addition to the general :ref:`subscript functionality <gf_subscript>` of
the Green function, the cartesian product provides the functionality to write
expressions such as ::

        g[x1, x2, ..]

where the xi are mesh points of the respective mesh-component of the cartesian product.


Clef expressions
----------------

The subscript operator in clef expression assignment has also been extended to
allow for assignments of the kind ::

        g[_x1, _x2, ..] << g1[y1, _x1] * g2[_x2, y2] * ...

where the _xi are clef placeholders, and yi denote mesh points of the respective meshes
of the gi.


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
.. literalinclude:: /documentation/examples/triqs/gfs/gf_product_0.cpp
:language: cpp

.. literalinclude:: /documentation/examples/triqs/gfs/gf_product_0.output
