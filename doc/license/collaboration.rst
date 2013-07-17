.. index:: TRIQS collaboration

.. _collaboration:

The TRIQS collaboration
========================

TRIQS core 
----------

Basic components and python libraries
.....................................

**Main developers**: M. Ferrero, O. Parcollet

**Contributors**: L. Boehnke (Legendre Green's functions)

  
C++ ``triqs::arrays`` library
.............................

**Developer**: O. Parcollet


Quantum impurity solvers
-------------------------

Continuous-time quantum Monte Carlo
...................................

The CTQMC algorithm implemented in TRIQS is based on a hybridization expansion
of the partition function as described in references [#ctqmc1]_ and [#ctqmc2]_.
The computation of the imaginary-time Green's function has been improved with
the use of Legendre polynomials following reference [#ctqmc3]_.

**Main developers**: M. Ferrero, O. Parcollet

**Contributors**: L. Boehnke (measures in Legendre basis)

**Related papers**:

.. [#ctqmc1] `P. Werner, A. Comanac, L. de’ Medici, M. Troyer, and A. J. Millis, Phys. Rev. Lett. 97, 076405 (2006) <http://link.aps.org/doi/10.1103/PhysRevLett.97.076405>`_ (:download:`bibtex file <ctqmc2.bib>`)
.. [#ctqmc2] `P. Werner and A. J. Millis, Phys. Rev. B 74, 155107 (2006) <http://link.aps.org/doi/10.1103/PhysRevB.74.155107>`_ (:download:`bibtex file <ctqmc3.bib>`)
.. [#ctqmc3] `L. Boehnke, H. Hafermann, M. Ferrero, F. Lechermann, and O. Parcollet, Phys. Rev. B 84, 075145 (2011) <http://link.aps.org/doi/10.1103/PhysRevB.84.075145>`_ (:download:`bibtex file <ctqmc1.bib>`)

Hubbard I
..........

**Main developer**: L. Pourovskii


Wien2TRIQS
-----------------

The developement of an interface between the Wien2k electronic structure
package and TRIQS has been motivated by a scientific collaboration between the
research groups of Antoine Georges, Silke Biermann (Ecole Polytechnique),
Olivier Parcollet (CEA Saclay).  A first step has been the definition of the
framework and the construction of the projective Wannier functions as input for
the DMFT calculations [#wien2k1]_.  This has been followed by the introduction
of full charge self-consistency [#wien2k2]_, necessary for total energy
calculations.

**Developers**: M. Aichhorn, L. Pourovskii, V. Vildosola, C. Martins

**Related papers**:

.. [#wien2k1] `M. Aichhorn, L. Pourovskii, V. Vildosola, M. Ferrero, O. Parcollet, T. Miyake, A. Georges, and S. Biermann, Phys. Rev. B 80, 085101 (2009) <http://link.aps.org/doi/10.1103/PhysRevB.80.085101>`_ (:download:`bibtex file <wien2k1.bib>`)
.. [#wien2k2] `M. Aichhorn, L. Pourovskii, and A. Georges, Phys. Rev. B 84, 054529 (2011) <http://link.aps.org/doi/10.1103/PhysRevB.84.054529>`_ (:download:`bibtex file <wien2k2.bib>`)


