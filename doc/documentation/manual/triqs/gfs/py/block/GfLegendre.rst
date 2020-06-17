.. index::
  single: Green's functions; block Green's function
  module: gf_legendre

.. _GfLegendre:

**[GfLegendre]** Matsubara Green's function with Legendre polynomials 
=======================================================================

Reference
---------------

.. autoclass:: triqs.gf.GfLegendre
  :members: density, enforce_discontinuity

.. warning::

    Arguments of ``__init__()`` must be documented.

HDF5 data scheme
^^^^^^^^^^^^^^^^^^

The GfLegendre (Format= "GfLegendre") is decomposed in the following objects:

=========================   ===========================  ===========================================================================
Name                        Type                         Meaning
=========================   ===========================  ===========================================================================
Mesh                        MeshGf                       The mesh
Data                        3d numpy of complex          Data[n,i1,i2] is the element of the Green function where:
                                                          * n is the Legendre coefficient index 
                                                          * i1, i2 are the indices
IndicesL, IndicesR          string                       The Python repr of the indices, e.g. (1,2), or (1,)
                                                         repr(this_string) reproduces the indices 
Name                        string                       Name of the Green's function block
Note                        string                       Note
=========================   ===========================  ===========================================================================

Example
--------------- 

.. plot:: documentation/manual/triqs/gfs/py/block/green_legendre.py
   :include-source:
   :scale: 70 

