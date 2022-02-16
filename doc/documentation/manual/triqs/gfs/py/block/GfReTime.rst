.. index::
  single: Green's functions; block Green's function
  module: gf_retime

.. _GfReTime:

**[GfReTime]** Retarded Green's function in real time
=======================================================

Reference
---------------

.. autoclass:: triqs.gf.GfReTime
   :noindex:
   :members: set_from_fourier, fourier, transpose, conjugate

.. warning::

    Arguments of ``__init__()`` must be documented.

HDF5 data scheme
^^^^^^^^^^^^^^^^^^^^^^

The GfReTime (Format = "GfReTime") is decomposed in the following objects:

=========================   ===========================  ===========================================================================
Name                        Type                         Meaning
=========================   ===========================  ===========================================================================
Mesh                        MeshGf                       The mesh
Data                        3d numpy of complex          Data[n,i1,i2] is the element of the Green function where:
                                                          * n is the index of the time slice
                                                          * i1, i2 are the indices
IndicesL,IndicesR           string                       The Python repr of the indices, e.g. (1,2), or (1,)
                                                         repr(this_string) reproduces the indices
Name                        string                       Name of the Green function block
Note                        string                       Note
=========================   ===========================  ===========================================================================


Example
---------------

.. plot:: documentation/manual/triqs/gfs/py/block/green_retime.py
   :include-source:
   :scale: 70

