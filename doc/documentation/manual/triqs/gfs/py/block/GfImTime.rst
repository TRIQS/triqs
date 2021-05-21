.. index::
  single: Green's functions; block Green's function
  module: gf_imtime

.. _GfImTime:

**[GfImTime]** Matsubara Green's function in imaginary time
==============================================================================

Reference
---------------

.. autoclass:: triqs.gf.GfImTime
  :members: set_from_fourier, set_from_legendre, transpose, conjugate

.. warning::

    Arguments of ``__init__()`` must be documented.

HDF5 data scheme
^^^^^^^^^^^^^^^^^^^^^^

The GfImTime (Format = "GfImTime") is decomposed in the following objects:

=========================   ===========================  ===========================================================================
Name                        Type                         Meaning
=========================   ===========================  ===========================================================================
Mesh                        MeshGf                       The mesh
Tail                        TailGf                       The tail
Data                        3d numpy of double           Data[n,i1,i2] is the element of the Green function where:
                                                          * n is the index of the time slice, starting at tau=0
                                                          * i1, i2 are the indices
IndicesL,IndicesR           string                       The Python repr of the indices, e.g. (1,2), or (1,)
                                                         repr(this_string) reproduces the indices 
Name                        string                       Name of the Green function block
Note                        string                       Note 
=========================   ===========================  ===========================================================================

Example
-------

.. plot:: documentation/manual/triqs/gfs/py/block/green_imtime.py
   :include-source:
   :scale: 70 

