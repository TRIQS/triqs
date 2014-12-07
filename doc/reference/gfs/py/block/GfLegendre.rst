.. index::
  single: Green's functions; block Green's function
  module: gf_legendre

.. _GfLegendre:

Matsubara Green's function with Legendre polynomials (GfLegendre)
=====================================================================================

This object stores a matrix-valued Matsubara Green's function in imaginary time using
Legendre polynomials. It is connected to the usual imaginary-time Matsubara Green's
function through:

.. math::
  G_{l, \alpha \beta} = \sqrt{2l+1} \int_0^\beta d\tau P_l (x(\tau)) G_{\alpha \beta}(\tau),


where :math:`x(\tau) = 2\tau/\beta -1` and the :math:`P_l` are the Legendre
polynomials.  :math:`G_{\alpha \beta}(\tau)` is the :ref:`Matsubara Green's
function in imaginary time <GfImTime>`.


Reference
---------------

.. autoclass:: pytriqs.gf.local.GfLegendre
  :members: density, enforce_discontinuity

.. warning::

    Arguments of ``__init__()`` must be documented.

HDF5 data scheme
^^^^^^^^^^^^^^^^^^

The GfLegendre (TRIQS_HDF5_data_scheme= "GfLegendre") is decomposed in the following objects: 

=========================   ===========================  ===========================================================================
Name                        Type                         Meaning
=========================   ===========================  ===========================================================================
Mesh                        MeshGf                       The mesh
Tail                        TailGf                       The tail
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

.. plot:: reference/gfs/py/block/green_legendre.py
   :include-source:
   :scale: 70 

