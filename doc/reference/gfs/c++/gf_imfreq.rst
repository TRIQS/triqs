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


Singularity
-------------

The singularity is a high frequency expansion,  :ref:`gf_tail`.


Evaluation method
---------------------

* No interpolation.

* Return type: 

  * If Target==scalar_valued: a complex 
  * If Target==matrix_valued: an object modeling ImmutableMatrix concept.

* When the point is outside of the mesh, the evaluation of the gf returns: 

  * the evaluation of the high frequency tail if no_tail is not set.
  * 0 otherwise


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


.. triqs_example:: ./gf_imfreq_0.cpp

Free functions
---------------------

.. toctree::
  :maxdepth: 1

  /cpp2rst_generated/triqs/gfs/is_gf_real
  /cpp2rst_generated/triqs/gfs/is_gf_real_in_tau
