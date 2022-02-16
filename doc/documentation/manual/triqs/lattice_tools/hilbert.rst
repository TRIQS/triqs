.. _hilbert_transform:

.. module:: triqs.dos.hilbert_transform
   :noindex:


Hilbert Transform
=======================================
TRIQS comes with a Hilbert transform. Let us look at an example:

.. code-block:: python

   from triqs.lattice.tight_binding import *
   from triqs.dos import HilbertTransform
   from triqs.gf import GfImFreq

   # Define a DOS (here on a square lattice)
   BL = BravaisLattice(units = [(1,0,0) , (0,1,0) ], orbital_positions= [(0,0,0)] )
   t   = -1.00                # First neighbour Hopping
   tp  =  0.0*t               # Second neighbour Hopping
   hop= {  (1,0) :  [[ t]],
           (-1,0):  [[ t]],
           (0,1) :  [[ t]],
           (0,-1):  [[ t]],
           (1,1) :  [[ tp]],
           (-1,-1):  [[ tp]],
           (1,-1):  [[ tp]],
           (-1,1):  [[ tp]]}

   TB = TightBinding (BL, hop)
   d = dos(TB, n_kpts= 500, n_eps = 101, name = 'dos')[0]

   #define a Hilbert transform
   H = HilbertTransform(d)

   #fill a Green function
   G = GfImFreq(indices = ['up','down'], beta = 20)
   Sigma0 = GfImFreq(indices = ['up','down'], beta = 20); Sigma0.zero()
   G << H(Sigma = Sigma0,mu=0.)

Given a density of states `d` (here for a tight-binding model), the Hilbert transform `H` is defined is defined in the following way::

    H = HilbertTransform(d)

To construct a Green's function::

    G = GfImFreq(indices = ['up','down'], beta = 20)
    Sigma0 = GfImFreq(indices = ['up','down'], beta = 20); Sigma0.zero()
    G << H(Sigma = Sigma0, mu=0.)


.. autoclass:: triqs.dos.HilbertTransform
   :noindex:
   :members: __call__
   :undoc-members:


