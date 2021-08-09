.. _applications:

**Applications based on TRIQS**
###############################

The TRIQS library provides all the necessary building blocks for the
development of applications for the study of interacting quantum systems.
These applications can be grouped into one of the following categories

* **Official applications**

  These applications have readily been developed and are maintained by the TRIQS
  collaboration.

* **External applications**

  These applications were developed and are maintained externally but are still fundamentally
  relying on TRIQS library features.

* **Interfaces to external codes**

  These applications are thin wrappers around TRIQS independent codes that are developed and
  maintained externally. The maintenance of the interface layer is commonly shared between the
  TRIQS collaboration and the external developers.

If you would like to have your application listed please contact us directly.

.. contents:: Table of Contents
   :depth: 2

.. _impurity_solvers:

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

**Hubbard I solver (external)**
-------------------------------

This solver gives the Hubbard I solution of a quantum impurity problem,
i.e. it solves the atomic limit. This simple approximation allows for
very quick answers even for large multiband systems.

*Homepage:* `<https://triqs.github.io/hubbardI/>`_

**PYED - Exact diagonalization for finite quantum systems (external)**
----------------------------------------------------------------------

The TRIQS-based python application pyed implements exact diagonalization for finite fermionic many-body quantum systems,
together with calculations of several response functions in imagianary time.

*Homepage*: `<https://github.com/hugostrand/pyed/>`_


**Interface to the pomerol exact diagonalization solver (external)**
--------------------------------------------------------------------

TRIQS wrapper around the `Pomerol <http://aeantipov.github.io/pomerol>`_ exact diagonalization library.

For publications based on results produced with this application please cite `10.5281/zenodo.592291 <https://doi.org/10.5281/zenodo.592291>`_

*Homepage*: `<https://github.com/krivenko/pomerol2triqs/>`_

**Interface to the w2dynamics impurity solver**
-----------------------------------------------

This application provides an interface to the continuous-time hybridization expansion
impurity solver contained in the `w2dynamics <https://github.com/w2dynamics/w2dynamics>`_ software package.

For publications based on results produced with this application please cite `Comput. Phys. Commun. 235, 388 (2019) <https://www.sciencedirect.com/science/article/pii/S0010465518303217>`_

*Homepage*: `<https://triqs.github.io/w2dynamics_interface/>`_

**Interface to the NRG Ljubljana impurity solver**
--------------------------------------------------

This application provides an interface to the numerical renormalization group
impurity solver `NRG Ljubljana <http://nrgljubljana.ijs.si>`_.

For publications based on results produced with this application please cite `Phys. Rev. B 79, 085106 (2009) <https://doi.org/10.1103/PhysRevB.79.085106>`_

*Homepage*: `<https://triqs.github.io/nrgljubljana_interface/>`_


.. _abinitio_tools:

**Ab-Initio Tools**
===================

**DFT tools - Toolbox for ab initio calculations of correlated materials**
--------------------------------------------------------------------------

DFTTools is an official TRIQS application that provides an interface to DFT packages
such as Wien2k and VASP for DFT+DMFT calculation. It allows you to turn band-structure calculations
results to inputs for full-fledged DFT+DMFT calculations in a few lines.

For publications based on results produced with this application please cite `Comput. Phys. Comm. 204, 200 (2016) <https://www.sciencedirect.com/science/article/pii/S0010465516300728?via%3Dihub>`_

*Homepage:* `<https://triqs.github.io/dft_tools/>`_

**TPRF - The Two-Particle Response Function tool box**
------------------------------------------------------

The two-particle response function tool box (TPRF) is an official TRIQS application that provides efficient (C++/OpenMP/MPI)
implementations of the basic operations for higher order response functions such as inversion, products,
the random phase approximation, the Bethe Salpeter equation, etc.

For publications based on results produced with this application please cite `10.5281/zenodo.2638058 <https://doi.org/10.5281/zenodo.2638058>`_

*Homepage:* `<https://triqs.github.io/tprf/>`_


.. _analytic_continuation:

**Analytic Continuation**
=========================

**Maxent**
----------

This official TRIQS application provides a modular Maximum Entropy program to perform analytic continuation.

*Homepage:* `<https://triqs.github.io/maxent/>`_

**SOM - Stochastic Optimization Method for Analytic Continuation (external)**
-----------------------------------------------------------------------------

This external TRIQS application is an optimized implementation of an analytic
continuation method proposed by Andrey S. Mishchenko.
A detailed description of the method can be found `here <http://www.cond-mat.de/events/correl12/manuscripts/mishchenko.pdf>`_.

For publications based on results produced with this application please cite `Comput. Phys. Commun. 239, 166 (2019) <https://www.sciencedirect.com/science/article/pii/S0010465519300402?via%3Dihub>`_

*Homepage:* `<http://krivenko.github.io/som/>`_

**Interface to Omegamaxent**
----------------------------

This application provides an interface to the `OmegaMaxent <https://www.physique.usherbrooke.ca/MaxEnt/index.php/Main_Page>`_
Maximum Entropy code.

For publications based on results produced with this application please cite `Phys. Rev. E 94, 023303 (2016) <https://journals.aps.org/pre/abstract/10.1103/PhysRevE.94.023303>`_

*Homepage:* `<https://triqs.github.io/omegamaxent_interface/>`_



.. _other_applications:

**Other Applications**
======================

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

**Cpp2Py**
----------

Cpp2Py is the Python-C++ interfacing tool of the TRIQS project, provided here as a standalone project.

*Homepage:* `<https://github.com/triqs/cpp2py>`_
