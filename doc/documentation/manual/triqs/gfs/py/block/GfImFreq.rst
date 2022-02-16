.. index::
  single: Green's functions; block Green's function
  module: gf_imfreq

.. _GfImFreq:

**[GfImFreq]** Matsubara Green's function in imaginary frequencies
=====================================================================================

Reference
---------------

.. autoclass:: triqs.gf.GfImFreq
   :noindex:
   :members: density, set_from_fourier, set_from_legendre

.. warning::

    Arguments of ``__init__()`` must be documented.

HDF5 data scheme
^^^^^^^^^^^^^^^^^^^^^^

The GfImFreq (Format= "GfImFreq") is decomposed in the following objects:

=========================   ===========================  ===========================================================================
Name                        Type                         Meaning
=========================   ===========================  ===========================================================================
Mesh                        MeshGf                       The mesh
Data                        3d numpy of complex          Data[n,i1,i2] is the element of the Green function where:
                                                          * n is the Matsubara index
                                                          * i1, i2 are the indices

                                                         For real GF in time, we store only starting from n=0 and the
                                                         rest is deduced by symmetry
IndicesL,IndicesR           string                       The Python repr of the indices, e.g. (1,2), or (1,)
                                                         repr(this_string) reproduces the indices
Name                        string                       Name of the Green function block
Note                        string                       Note
=========================   ===========================  ===========================================================================


Example
---------------

.. plot:: documentation/manual/triqs/gfs/py/block/green_imfreq.py
   :include-source:
   :scale: 70
