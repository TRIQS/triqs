.. highlight:: c

.. _gf_eval_curry: 

Partial evaluation 
===================

.. warning::

Multivariable Green functions can be both partially evalued.

Partial evaluation 
--------------------------

**Purpose** :

  Having a function (x,y) -> f(x,y), fix x = x0 in order to obtain a function of y only.

**Synopsis** ::

  If g is a multivar function, and 
     auto _ = var_t{};

     g[w][_] // is the gf partially evaluated for the first mesh.

When a Green function is evaluated with [][], and there is at least once var_t as an argument, 
the result is a view of the partially evaluated gf with : 

* a mesh corresponding of the mesh components of the _ variables.

* the tag type is computed accordingly

* the target is unchanged.

* the data is a sliced view of the original function

* singularity : in the case where the singularity is non trivial, is it also partially evaluated
  (in practice, G(k,omega). EXPLAIN).


Example : 

.. literalinclude:: /documentation/examples/triqs/gfs/gf_part_eval_0.cpp
   :language: cpp

.. literalinclude:: /documentation/examples/triqs/gfs/gf_part_eval_0.output
