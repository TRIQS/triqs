
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


.. code-block::  python

   from triqs.gf import *
   from triqs.operators import *
   from h5 import *
   import triqs.utility.mpi as mpi
   from triqs_cthyb import Solver

   # Parameters of the model
   U = 2.5
   t = 0.5
   mu = U/2.0
   beta = 100.0
   n_loops = 10

   # Construct the impurity solver
   S = Solver(beta = beta, gf_struct = [('up',[0]), ('down',[0])] )

   # This is a first guess for G
   S.G_iw << SemiCircular(2*t)

   # DMFT loop with self-consistency
   for i in range(n_loops):

       print "\n\nIteration = %i / %i" % (i+1, n_loops)

       # Symmetrize the Green's function imposing paramagnetism and use self-consistency
       g = 0.5 * ( S.G_iw['up'] + S.G_iw['down'] )
       for name, g0 in S.G0_iw:
           g0 << inverse( iOmega_n + mu - t**2 * g )

       # Solve the impurity problem
       S.solve(h_int = U * n('up',0) * n('down',0),   # Local Hamiltonian
           n_cycles  = 100000,                        # Number of QMC cycles
           length_cycle = 200,                        # Length of one cycle
           n_warmup_cycles = 5000                     # Warmup cycles
           )

       # Save iteration in archive
       with HDFArchive("single_site_bethe.h5", 'a') as A:
           A['G-%i'%i] = S.G_iw
           A['Sigma-%i'%i] = S.Sigma_iw
