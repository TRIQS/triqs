.. highlight:: c

.. _gf_imtime: 

**[gf<imtime>]** Matsubara imaginary time
==========================================================

This is a specialisation of :ref:`gf<gf_and_view>` for imaginary Matsubara time.

Synopsis
------------

.. code::

  gf<imtime, Target, Opt>

The *Target* template parameter can take the following values: 
 
+-------------------------+-----------------------------------------------------+
| Target                  | Meaning                                             |
+=========================+=====================================================+
| scalar_valued           | The function is scalar valued (double, complex...). |
+-------------------------+-----------------------------------------------------+
| matrix_valued [default] | The function is matrix valued.                      |
+-------------------------+-----------------------------------------------------+

Domain & mesh
----------------

The domain is  the set of real numbers between 0 and :math:`\beta`
since the function is periodic (resp. antiperiodic) for bosons (resp. fermions),  i.e.

* :math:`G(\tau+\beta)=-G(\tau)`  for fermions
* :math:`G(\tau+\beta)=G(\tau)` for bosons.

The domain is implemented in 

The mesh is :ref:`gf_mesh_imtime`.


Singularity
-------------

The singularity is a high frequency expansion,  :ref:`gf_tail`.


Evaluation method
---------------------

* Use a linear interpolation between the two closest point of the mesh.

* Return type: 

  * If Target==scalar_valued: a complex 
  * If Target==matrix_valued: an object modeling ImmutableMatrix concept.

* When the point is outside of the mesh, the evaluation of the gf returns: 

  * the evaluation of the high frequency tail


Data storage
---------------

* If Target==scalar_valued :
  
  * `data_t`: 1d array of complex<double>.

  * g.data()(i) is the value of g for the i-th point of the mesh.

* If Target==matrix_valued :

  * `data_t`: 3d array (C ordered) of complex<double>.

  * g.data()(i, range(), range()) is the value of g for the i-th point of the mesh.


TO DO: complex OR DOUBLE: FIX and document !!

HDF5 storage convention
---------------------------

h5 tag: `ImTime`


Examples
---------

.. literalinclude:: ./gf_imtime_0.cpp
   
