.. highlight:: c

.. _gf_imfreq: 

**[gf<imfreq>]** Green function on Matsubara frequencies
==========================================================

This is a specialisation of :ref:`gf<gf_and_view>` for imaginary Matsubara frequencies.

Synopsis
------------

.. code::

  gf<imfreq, Target, Opt>

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

The domain is triqs::gfs::matsubara_domain.

The Matsubara frequencies are :math:`\omega_n=\frac{(2n+1)\pi}{\beta}` for fermions and :math:`\omega_n=\frac{2n\pi}{\beta}` for bosons. 

The mesh is :ref:`gf_mesh_imfreq`. 


Evaluation method
---------------------

* No interpolation.

* Return type: 

  * If Target==scalar_valued: a complex 
  * If Target==matrix_valued: an object modeling ImmutableMatrix concept.

* When the point is outside of the mesh, the high-frequency moments of the
  Green function are automatically determined using a least-square fitting procedure.
  They are then used to calculate the value of the Green function at the point.


Data storage
---------------

* If Target==scalar_valued :
  
  * `data_t`: 1d array of complex<double>.

  * g.data()(i) is the value of g for the i-th point of the mesh.

* If Target==matrix_valued :

  * `data_t`: 3d array (C ordered) of complex<double>.

  * g.data()(i, range(), range()) is the value of g for the i-th point of the mesh.

HDF5 storage convention
---------------------------

h5 tag: `ImFreq`

TODO: DECIDE if we have 2 tag, one for scalar, one for matrix....

Examples
---------


.. literalinclude:: ./gf_imfreq_0.cpp

Free functions
---------------------

.. toctree::
  :maxdepth: 1

  /documentation/cpp_api/triqs/gfs/is_gf_real
  /documentation/cpp_api/triqs/gfs/is_gf_real_in_tau
