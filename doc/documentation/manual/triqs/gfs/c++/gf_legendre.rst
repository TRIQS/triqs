.. highlight:: c

.. _gf_legendre:

**[gf<legendre>]** Legendre representation
==========================================================

This is a specialisation of :ref:`gf<gf_and_view>` for Legendre polynomial expansion.

.. warning::
  TO BE COMPLETED

Synopsis
------------

.. code::

  gf<legendre, Target, Opt>

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

TO BE WRITTEN

Singularity
-------------

None


Evaluation method
---------------------

TO BE WRITTEN

Data storage
---------------

TO BE WRITTEN

HDF5 storage convention
---------------------------

h5 tag: `Legendre`

Examples
---------

.. literalinclude:: /documentation/examples/triqs/gfs/gf_legendre_0.cpp
   :language: cpp

.. literalinclude:: /documentation/examples/triqs/gfs/gf_legendre_0.output
