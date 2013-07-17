.. index::
  single: Green's functions; block Green's function
  module: gf_imfreq

.. _GfImFreq:

Matsubara Green's function in imaginary frequencies (GfImFreq)
=====================================================================================

This object stores a matrix valued Green function in imaginary frequencies
in the Matsubara formalism : 

.. math::
  G_{\alpha \beta} (i \omega_n) \equiv \int_0^\beta G_{\alpha \beta} ( \tau) e^{-i \omega_n \tau}

where :math:`G(\tau)` is the :ref:`Matsubara Green function in imaginary time <GfImTime>`.


Reference
---------------

.. autoclass:: pytriqs.gf.local.GfImFreq
  :members: density, set_from_fourier, set_from_legendre
 

HDF5 data scheme
^^^^^^^^^^^^^^^^^^^^^^

The GfImFreq (TRIQS_HDF5_data_scheme= "GfImFreq") is decomposed in the following objects : 

=========================   ===========================  ===========================================================================
Name                        Type                         Meaning
=========================   ===========================  ===========================================================================
Mesh                        MeshGf                       The mesh
Tail                        TailGf                       The tail
Data                        3d numpy of complex          Data[i1,i2,n] is the element of the Green function where :
                                                          * i1, i2 are the indices
                                                          * n is the Matsubara index 
                                                         
                                                         For real GF in time, we store only starting from n=0 and the
                                                         rest is deduced by symmetry
IndicesL,IndicesR           string                       The Python repr of the indices, e.g. (1,2), or (1,)
                                                         repr(this_string) reproduces the indices 
Name                        string                       Name of the Green function block
Note                        string                       Note 
=========================   ===========================  ===========================================================================


Example 
--------------- 

.. plot:: reference/python/green/block/green_imfreq.py
   :include-source:
   :scale: 70 
