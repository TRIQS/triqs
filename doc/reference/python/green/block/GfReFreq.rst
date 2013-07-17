.. index::
  single: Green's functions; block Green's function
  module: gf_refreq

.. _GfReFreq:

Retarded Green's function in real frequencies (GfReFreq)
=====================================================================================

This object stores a matrix valued Green function in real frequencies : 

.. math::
  G_{\alpha \beta} (\omega) \equiv \int_{-\infty}^{\infty} G_{\alpha \beta} ( t ) e^{-i \omega t}

where :math:`G(t)` is the :ref:`Green function in real time <GfReTime>`.


Reference
---------------

.. autoclass:: pytriqs.gf.local.GfReFreq
  :members: set_from_fourier, inverse_fourier, transpose, conjugate, set_from_pade
 


HDF5 data scheme
^^^^^^^^^^^^^^^^^^^^^^

The GfReFreq (TRIQS_HDF5_data_scheme = "GfReFreq") is decomposed in the following objects : 

=========================   ===========================  ===========================================================================
Name                        Type                         Meaning
=========================   ===========================  ===========================================================================
Mesh                        MeshGf                       The mesh
Tail                        TailGf                       The tail
Data                        3d numpy of complex          Data[i1,i2,n] is the element of the Green function where :
                                                          * i1, i2 are the indices
                                                          * n is the frequency index 
IndicesL,IndicesR           string                       The Python repr of the indices, e.g. (1,2), or (1,)
                                                         repr(this_string) reproduces the indices 
Name                        string                       Name of the Green function block
Note                        string                       Note 
=========================   ===========================  ===========================================================================


Examples 
---------------

.. plot:: reference/python/green/block/green_refreq.py
   :include-source:
   :scale: 70

Note that `g` is a **retarded** Green's function.


The next example demonstrates how a real frequency Green's function can be
reconstructed from an imaginary frequency counterpart using set_from_pade()
method.

.. plot:: reference/python/green/block/green_pade.py
   :include-source:
   :scale: 70
