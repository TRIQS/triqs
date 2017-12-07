.. _ipt:

Iterated perturbation theory: a simple DMFT solver
==================================================

Introduction
------------

The iterated perturbation theory (IPT) was one of the first methods used to solve the
DMFT equations [#ipt1]_. In spite of its simplistic nature, IPT gives a qualitatively
correct description of a Mott metal-insulator transition in the Hubbard model on
infinite-dimensional lattices (on the quantitative level it tends to underestimate
correlations though). In IPT one iteratively solves the DMFT equations using the
second-order perturbation theory in Hubbard interaction :math:`U` to approximate
the impurity self-energy. For the particle-hole symmetric case it reads

.. math::

    \Sigma(i\omega_n) \approx \frac{U}{2} +
        U^2 \int_0^\beta d\tau e^{i\omega_n\tau} G_0(\tau)^3.

A Hartree-Fock contribution :math:`U/2` in the self-energy cancels with a term
from :math:`G_0(i\omega_n)^{-1}` when the functions are substituted into the
Dyson's equation. For this reason this contribution is usually omitted from
both functions.

The success of IPT is caused by the fact that it becomes exact not only in the
weak coupling limit (by construction), but also reproduces an atomic-limit
expression for :math:`\Sigma(i\omega_n)` as :math:`U` grows large [#ipt2]_.

IPT solver
----------

We start by writing an IPT solver that implements the weak-coupling
perturbation theory for a symmetric single-band Anderson model.
All Green's functions in the calculations have just one index because
*up* and *down* components are the same.

.. runblock:: python

   from pytriqs.gf import *

   class IPTSolver:

       def __init__(self, **params):

           self.U = params['U']
           self.beta = params['beta']

           # Matsubara frequency
           self.g_iw = GfImFreq(indices=[0], beta=self.beta)
           self.g0_iw = self.g_iw.copy()
           self.sigma_iw = self.g_iw.copy()

           # Imaginary time
           self.g0_tau = GfImTime(indices=[0], beta = self.beta)
           self.sigma_tau = self.g0_tau.copy()

       def solve(self):

           self.g0_tau << InverseFourier(self.g0_iw)
           self.sigma_tau << (self.U**2) * self.g0_tau * self.g0_tau * self.g0_tau
           self.sigma_iw << Fourier(self.sigma_tau)

           # Dyson equation to get G
           self.g_iw << inverse(inverse(self.g0_iw) - self.sigma_iw)


Visualization of a Mott transition
----------------------------------

We can now use this solver to run DMFT calculations and scan a range of
values of :math:`U`. 

.. plot:: tour/ipt_full.py
   :include-source:
   :scale: 70

Alternatively, in this :download:`script <./ipt_dmft.py>`, at every iteration the resulting data is plotted
and saved into PNG files using the :ref:`TRIQS matplotlib interface<plotting>`.
Note that :math:`G(i\omega_n)` is analytically continued to the real axis using
:ref:`Padé approximant<GfReFreq>`.

At the end of the script an external utility `convert` is invoked to join the
DOS plots into a single animated GIF file which illustrates how a metallic
solution evolves towards an insulator.
The result of this script is the following animated gif:

  .. image:: mott.gif
     :width: 700
     :align: center


Journal references
------------------

.. [#ipt1] A. Georges and G. Kotliar,
           Phys. Rev. B 45, 6479–6483 (1992).
.. [#ipt2] X. Y. Zhang, M. J. Rozenberg, and G. Kotliar,
           Phys. Rev. Lett. 70, 1666–1669 (1993)
