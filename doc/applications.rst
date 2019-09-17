.. _applications:

**Applications based on TRIQS**
===============================

The TRIQS library provides all the necessary building blocks for the
development of applications for the study of interacting quantum systems.
These applications can be grouped into one of the following categories

* **TRIQS applications**

  These applications have readily been developed and are maintained by the TRIQS
  collaboration.

* **External applications**

  These applications were developed and are maintained externally but are still fundamentally
  relying on TRIQS library features.

* **Interfaces to external codes**

  These applications are thin wrappers around TRIQS independent codes that are developed and
  maintained externally. The maintenance of the interface layer is commonly shared between the
  TRIQS collaboration and the external developers.


**Impurity Solvers**
====================

**Hybridization-expansion solver**
----------------------------------

The hybridization-expansion solver is an official TRIQS application that allows
to solve the generic problem of a quantum impurity embedded in a conduction bath.
It is based on a quantum Monte Carlo algorithm that stochastically samples the
diagrams of a hybridization expansion of the partition function.

For publications based on results produced with this application please cite `Comput. Phys. Comm. 200, 274 (2016) <https://www.sciencedirect.com/science/article/pii/S001046551500404X>`_

*Homepage:* `<https://triqs.github.io/cthyb/>`_


**Interface to the w2dynamics impurity solver**
-----------------------------------------------

This application provides an interface to the continuous-time hybridization expansion
impurity solver contained in the `w2dynamics <https://github.com/w2dynamics/w2dynamics>`_ software package.

For publications based on results produced with this application please cite `Comput. Phys. Commun. 235, 388 (2019) <https://www.sciencedirect.com/science/article/pii/S0010465518303217>`_

*Homepage*: `<https://triqs.github.io/w2dynamics_interface/>`_


**Interface to the pomerol exact diagonalization solver (external)**
--------------------------------------------------------------------

TRIQS wrapper around the `Pomerol <http://aeantipov.github.io/pomerol>`_ exact diagonalization library.

For publications based on results produced with this application please cite `aeantipov/pomerol: 1.2 <https://zenodo.org/record/825870>`_

*Homepage*: `<https://github.com/krivenko/pomerol2triqs/>`_


**PYED - Exact diagonalization for finite quantum systems (external)**
----------------------------------------------------------------------

The TRIQS-based python application pyed implements exact diagonalization for finite fermionic many-body quantum systems,
together with calculations of several response functions in imagianary time.

*Homepage*: `<https://github.com/hugostrand/pyed/>`_


**Hubbard I solver (external)**
-------------------------------
.. warning::

   THIS APPLICATION CURRENTLY ONLY COMPATIBLE WITH TRIQS 1.4

This solver gives the Hubbard I solution of a quantum impurity problem,
i.e. it solves the atomic limit. This simple approximation allows for
very quick answers even for large multiband systems.

*Homepage:* `<https://triqs.github.io/triqs/1.4/applications/hubbardI/>`_


**Analytic Continuation**
=========================

**Maxent**
----------

This official TRIQS application provides a modular Maximum Entropy program to perform analytic continuation.

*Homepage:* `<https://triqs.github.io/maxent/>`_


**Interface to Omegamaxent**
----------------------------

This application provides an interface to the `OmegaMaxent <https://www.physique.usherbrooke.ca/MaxEnt/index.php/Main_Page>`_
Maximum Entropy code.

For publications based on results produced with this application please cite `Phys. Rev. E 94, 023303 (2016) <https://journals.aps.org/pre/abstract/10.1103/PhysRevE.94.023303>`_

*Homepage:* `<https://triqs.github.io/omegamaxent_interface/>`_


**Stochastic Optimization Method for Analytic Continuation (external)**
-----------------------------------------------------------------------

This external TRIQS application is an optimized implementation of an analytic
continuation method proposed by Andrey S. Mishchenko.
A detailed description of the method can be found `here <http://www.cond-mat.de/events/correl12/manuscripts/mishchenko.pdf>`_.

For publications based on results produced with this application please cite `Comput. Phys. Commun. 239, 166 (2019) <https://www.sciencedirect.com/science/article/pii/S0010465519300402?via%3Dihub>`_

*Homepage:* `<http://krivenko.github.io/som/>`_


**Other Applications**
======================

**DFT tools**
-------------

DFTTools is an official TRIQS application that provides an interface to DFT packages
such as Wien2k and VASP for DFT+DMFT calculation. It allows you to turn band-structure calculations
results to inputs for full-fledged DFT+DMFT calculations in a few lines.

For publications based on results produced with this application please cite `Comput. Phys. Comm. 204, 200 (2016) <https://www.sciencedirect.com/science/article/pii/S0010465516300728?via%3Dihub>`_

*Homepage:* `<https://triqs.github.io/dft_tools/>`_


**The Two-Particle Response Function tool box (TPRF)**
------------------------------------------------------

The two-particle response function tool box (TPRF) is an official TRIQS application that provides efficient (C++/OpenMP/MPI)
implementations of the basic operations for higher order response functions such as inversion, products,
the random phase approximation, the bethe salpeter equation, etc.

For publications based on results produced with this application please cite `TRIQS/tprf: Pre-release 0.1.0 <https://zenodo.org/record/2638059>`_

*Homepage:* `<https://triqs.github.io/tprf/>`_


**TRIQS Solver benchmarks**
---------------------------

This official TRIQS application provides systematic tests and benchmarks of various impurity solvers for a set of
reference impurity models. It uses the Python interface of the TRIQS library as a framework.

*Homepage:* `<https://github.com/triqs/benchmarks>`_

**App4Triqs Application Skeleton**
----------------------------------

This official TRIQS application provides a CMake based skeleton for TRIQS applications.
Most of the official TRIQS applications are continuously following this project structure.
It can be easily adapted for your own application.

*Homepage:* `<https://github.com/triqs/app4triqs>`_
