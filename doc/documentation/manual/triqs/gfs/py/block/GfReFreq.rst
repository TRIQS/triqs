.. index::
  single: Green's functions; block Green's function
  module: gf_refreq

.. _GfReFreq:

**[GfReFreq]** Retarded Green's function in real frequencies
=============================================================

Reference
---------------

.. autoclass:: triqs.gf.GfReFreq
   :noindex:
   :members: set_from_fourier, inverse_fourier, transpose, conjugate, set_from_pade

.. warning::

    Arguments of ``__init__()`` must be documented.

HDF5 data scheme
^^^^^^^^^^^^^^^^^^^^^^

The GfReFreq (Format = "GfReFreq") is decomposed in the following objects:

=========================   ===========================  ===========================================================================
Name                        Type                         Meaning
=========================   ===========================  ===========================================================================
Mesh                        MeshGf                       The mesh
Data                        3d numpy of complex          Data[n,i1,i2] is the element of the Green function where:
                                                          * n is the frequency index
                                                          * i1, i2 are the indices
IndicesL,IndicesR           string                       The Python repr of the indices, e.g. (1,2), or (1,)
                                                         repr(this_string) reproduces the indices
Name                        string                       Name of the Green function block
Note                        string                       Note
=========================   ===========================  ===========================================================================


Examples
---------------

.. plot:: documentation/manual/triqs/gfs/py/block/green_refreq.py
   :include-source:
   :scale: 70

Note that `g` is a **retarded** Green's function.


The next example demonstrates how a real frequency Green's function can be
reconstructed from an imaginary frequency counterpart using ``set_from_pade()``
method.

.. plot:: documentation/manual/triqs/gfs/py/block/green_pade.py
   :include-source:
   :scale: 70
