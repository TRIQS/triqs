
.. index::
  single: Green's functions; initializers

.. _descriptors:
   
Descriptors
-----------

descriptors are simple objects that describe a function of frequencies 
and permits to compute it with its high frequencies expansion.

They can be combined into simple expressions.
Example:

.. literalinclude:: impinbath.py
   :lines: 1-13


Reference
^^^^^^^^^

.. class::  triqs.gf.descriptors.Omega

    The function :math:`\omega \rightarrow \omega`. If used with Matsubara Green's functions, the corresponding 
    class member is called `iOmega_n`.

.. automodule::  triqs.gf.descriptors
   :members: Function,SemiCircular, Flat 

   
