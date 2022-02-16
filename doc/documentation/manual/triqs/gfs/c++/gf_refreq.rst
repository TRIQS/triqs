.. highlight:: c

.. _gf_refreq:

**[gf<refreq>]** Real frequencies
==========================================================

This is a specialisation of :ref:`gf<gf_and_view>` for real frequencies.


Synopsis
------------

.. code::

  gf<refreq, Target, Opt>

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



Evaluation method
---------------------

* Linear interpolation on the mesh.

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

HDF5 storage convention
---------------------------

h5 tag: `ReFreq`

Examples
---------

.. literalinclude:: /documentation/examples/triqs/gfs/gf_refreq_0.cpp
   :language: cpp

.. literalinclude:: /documentation/examples/triqs/gfs/gf_refreq_0.output
