.. index:: introduction
.. role:: red

Python modules
==================================

TRIQS (**T**\oolbox for **R**\esearch on **I**\nteracting **Q**\uantum **S**\ystems)
is a free software scientific project 
that aims at providing tools for the theoretical study of strongly correlated quantum systems.
It is based on some of the codes originally developed and used at IPhT Saclay and Ecole Polytechnique since 2005
(see :ref:`credits <collaboration>`).
It is implemented as a `Python <http://www.python.org>`_  library, with Python, C++ and Fortran codes and contains : 

* Various basic objects e.g. : 

  * Local Green's functions of various kinds.
  * Bravais lattices, tight-binding hoppings, local density of states, superlattices (for cluster methods)
  * Some simple *k*-sums over the Brillouin zone.

* Optional extension modules :

  * Quantum impurity solvers :

     * :ref:`Continuous-time hybridization Quantum Monte Carlo  <ctqmc_hyb>`.
     * :ref:`Hubbard I <hubbardI>`.

  * :ref:`Wien2TRIQS <Wien2k>` : an interface to the Wien2k electronic structure code for performing LDA+DMFT computations.

The rest of the introduction is a little demo of examples used elsewhere in the documentation.
The goal is not to explain them here in details but to quickly show some of TRIQS's current capabilities.

.. toctree::
   :maxdepth: 1

   tour1
   tour2
   tour3
   tour4
   ipt/ipt
   reading
 
