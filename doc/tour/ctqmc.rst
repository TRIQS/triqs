
 
Solving a quantum impurity model with CTQMC 
-------------------------------------------

.. note::
  
   Requires TRIQS and the :doc:`application cthyb_matrix <../../applications>`

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

.. runblock:: python

   from pytriqs.gf.local import *
   from pytriqs.operators import *
   from pytriqs.applications.impurity_solvers.cthyb_matrix import Solver

   # Parameters
   D, V, U = 1.0, 0.2, 4.0
   e_f, beta = -U/2.0, 50

   # Construct the impurity solver with the inverse temperature
   # and the structure of the Green's functions
   S = Solver(beta = beta, gf_struct = [ ('up',[1]), ('down',[1]) ])

   # Initialize the non-interacting Green's function S.G0
   for spin, g0 in S.G0 :
       g0 <<= inverse( iOmega_n - e_f - V**2 * Wilson(D) ) 

   # Run the solver. The result will be in S.G
   S.solve(H_local = U * N('up',1) * N('down',1),   # Local Hamiltonian 
           quantum_numbers = {                      # Quantum Numbers 
             'Nup': N('up',1),                     # Operators commuting with H_Local
             'Ndown': N('down',1) },          
           n_cycles  = 500000,                      # Number of QMC cycles
           length_cycle = 200,                      # Length of one cycle 
           n_warmup_cycles = 10000,                 # Warmup cycles
           n_legendre = 50,                         # Number of Legendre coefficients
           random_name = 'mt19937',                 # Name of the random number generator
           use_segment_picture = True,              # Use the segment picture
           measured_operators = {                   # Operators to be averaged
             'Nimp': N('up',1)+N('down',1) }
           )

   # Save the results in an hdf5 file (only on the master node)
   from pytriqs.archive import HDFArchive
   import pytriqs.utility.mpi as mpi

   if mpi.is_master_node():
     Results = HDFArchive("solution.h5",'w')
     Results["G"] = S.G
     Results["Gl"] = S.G_legendre
     Results["Nimp"] = S.measured_operators_results['Nimp']


The result can be then read from the ``HDF5`` file and plotted using the ``oplot`` function:

.. literalinclude::  aim_plot.py

.. image:: aim_plot.png
   :width: 700
   :align: center


We go through this example in more details in the documentation of the cthyb_matrix application.

