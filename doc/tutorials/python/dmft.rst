
Dynamical mean-field theory on a Bethe lattice
----------------------------------------------

  `Requires TRIQS and the application cthyb_matrix`
  

In the case of Bethe lattice the dynamical mean-field theory (DMFT) self-consistency condition takes a particularly simple form

.. math::

  G^{-1}_{0,\sigma} (i \omega_n) = i \omega_n + \mu - t^2 G_{\sigma} (i \omega_n).


Hence, from a strictly technical point of view, in this case the DMFT cycle can be implemented by modifying 
the previous single-impurity example to the case of a bath with semi-circular density of states and adding a python loop to update :math:`G_0` as function of :math:`G`.

Here is a complete program doing this plain-vanilla DMFT  on a half-filled one-band Bethe lattice:


.. literalinclude:: ./dmft.py


A general introduction to DMFT calculations with TRIQS can be found :ref:`here <dmftloop>`.

Chapter :ref:`Wien2TRIQS <Wien2k>` discusses the TRIQS implementation for DMFT calculations of real materials and the interface between TRIQS and the Wien2k band structure code.

