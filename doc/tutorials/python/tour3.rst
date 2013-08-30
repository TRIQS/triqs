
 
Tour 3: Solving a quantum impurity model with QMC 
----------------------------------------------------------

`[Requires TRIQS and the application cthyb_matrix]`

Free electrons are nice, but the `I` in TRIQS means `interacting`.
So let us solve a simple one-band Anderson impurity model

.. math::

  \mathcal{H}_\mathrm{loc} = U n_\uparrow n_\downarrow,

where the non-interacting Green's function is:

.. math::

  G^{-1}_{0,\sigma} (i \omega_n) = i \omega_n - \epsilon_f - V^2 \Gamma_\sigma(i \omega_n).

In this example, an impurity with the non-interacting level position at energy :math:`\epsilon_f` and on-site Coulomb repulsion :math:`U` is embedded into an electronic bath.
The 
electronic bath has a flat density of states over the interval
:math:`[-1,1]` and hybridizes  with the impurity with the amplitude :math:`V`.

We solve this model using the hybridization expansion Continuous Time Quantum Monte Carlo method (CT-Hyb)
proposed by `P. Werner et al. <http://link.aps.org/doi/10.1103/PhysRevLett.97.076405>`_

To this end we first initialize the ``Solver`` class of the TRIQS CT-Hyb implementaion 
``pytriqs.applications.impurity_solvers.cthyb_matrix``.
Then, after having constructed the non-interacting Green's function :math:`G^{-1}_{0,\sigma}`, 
we launch the impurity solver calculations by calling the ``Solve`` method.
Finally, the resulting interacting Green's function as well as average impurity occupancy 
is stored in the :ref:`HDF5 format<hdf5_base>`.


.. literalinclude:: ./aim.py

The result can be then read from the ``HDF5`` file and plotted using the ``oplot`` function:

.. literalinclude::  aim_plot.py

.. image:: aim_plot1.png
   :width: 700
   :align: center


We go through this example in more details in the documentation of the cthyb_matrix application.

