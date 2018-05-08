
Dynamical mean-field theory on a Bethe lattice
----------------------------------------------

.. note::

   Requires TRIQS and the :doc:`application cthyb <../../applications>`


In the case of Bethe lattice the dynamical mean-field theory (DMFT) self-consistency condition takes a particularly simple form

.. math::

  G^{-1}_{0,\sigma} (i \omega_n) = i \omega_n + \mu - t^2 G_{\sigma} (i \omega_n).


Hence, from a strictly technical point of view, in this case the DMFT cycle can be implemented by modifying 
the previous single-impurity example to the case of a bath with semi-circular density of states and adding a python loop to update :math:`G_0` as function of :math:`G`.

Here is a complete program doing this plain-vanilla DMFT on a half-filled one-band Bethe lattice:


.. runblock:: python

   from pytriqs.gf import *
   from pytriqs.gf import *
   from pytriqs.operators import *
   from pytriqs.archive import *
   import pytriqs.utility.mpi as mpi

   # Set up a few parameters
   U = 2.5
   half_bandwidth = 1.0
   chemical_potential = U/2.0
   beta = 100
   n_loops = 5

   # Construct the CTQMC solver
   from pytriqs.applications.impurity_solvers.cthyb import Solver
   S = Solver(beta = beta, gf_struct = { 'up':[0], 'down':[0] })

   # Set the solver parameters
   params = {}
   params['n_cycles'] = 1000000                # Number of QMC cycles
   params['length_cycle'] = 200                # Length of one cycle 
   params['n_warmup_cycles'] = 10000           # Warmup cycles

   # Initalize the Green's function to a semi-circular density of states
   g0_iw = GfImFreq(indices = [0], beta = 100)
   g0_iw << SemiCircular(half_bandwidth)
   for name, g0block in S.G_tau:
       g0block << InverseFourier(g0_iw)

   # Now do the DMFT loop
   for IterationNumber in range(n_loops):

       # Compute S.G0_iw with the self-consistency condition while imposing paramagnetism
       g = 0.5 * Fourier( S.G_tau['up'] + S.G_tau['down'] )
       for name, g0 in S.G0_iw:
           g0 << inverse( iOmega_n + chemical_potential - (half_bandwidth/2.0)**2  * g )

       # Run the solver
       S.solve(h_int = U * n('up',0) * n('down',0), **params)

       # Some intermediate saves
       if mpi.is_master_node():
         R = HDFArchive("single_site_bethe.h5")
         R["G_tau-%s"%IterationNumber] = S.G_tau
         del R
