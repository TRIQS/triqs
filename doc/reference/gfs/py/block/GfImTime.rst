.. index::
  single: Green's functions; block Green's function
  module: gf_imtime

.. _GfImTime:

Matsubara Green's function in imaginary time (GfImTime)
==============================================================================

This object stores a matrix valued Green function in imaginary time
in the Matsubara formalism: 

.. math::

  G_{\alpha \beta} (\tau) \equiv - \mathcal{T} <c_{\alpha}(\tau) c^{\dagger}_{\beta} (0)> \qquad \text{ for } 0 \leq \tau \leq \beta

 
Reference
---------------

.. autoclass:: pytriqs.gf.local.GfImTime
  :members: set_from_inverse_fourier, set_from_legendre, transpose, conjugate


HDF5 data scheme
^^^^^^^^^^^^^^^^^^^^^^

The GfImTime (TRIQS_HDF5_data_scheme = "GfImTime") is decomposed in the following objects: 

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

.. plot:: reference/gfs/py/block/green_imtime.py
   :include-source:
   :scale: 70 

