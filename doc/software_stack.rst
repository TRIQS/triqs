.. _software_stack:

TRIQS Software Stack
####################

TRIQS is organized as a layered software stack: a set of general-purpose **core libraries** at the
bottom, the **TRIQS library** with its physics-oriented containers and numerical algorithms in the
middle, and a rich ecosystem of **applications** built on top.


Core libraries
==============

The TRIQS library is built on a set of stand-alone core libraries providing important functionalities:

* `TRIQS/h5 <https://github.com/triqs/h5>`_ : a high-level C++ interface to the hdf5 library
* `TRIQS/mpi <https://github.com/triqs/mpi>`_ : a high-level C++ interface to the Message Passing Interface
* `TRIQS/itertools <https://github.com/triqs/itertools>`_ : a C++ library to create and adapt C++ ranges
* `TRIQS/nda <https://github.com/triqs/nda>`_ : a C++ library providing an efficient and flexible multi-dimensional array class
* `TRIQS/app4triqs <https://github.com/TRIQS/app4triqs>`_ : a skeleton project to bootstrap new TRIQS-based applications
* `flatironinstitute/cppdlr <https://github.com/flatironinstitute/cppdlr>`_ : a C++ library implementing the Discrete Lehmann Representation (DLR) of imaginary-time Green's functions
* `flatironinstitute/c2py <https://github.com/flatironinstitute/c2py>`_ : a runtime library that exposes C++ types and functions to Python
* `flatironinstitute/clair <https://github.com/flatironinstitute/clair>`_ : a Clang-based tool that generates the Python bindings from the C++ source (used together with c2py)

These libraries are developed and released independently and have their own documentation.


The TRIQS library
=================

On top of the core libraries, the TRIQS library itself provides the physics-oriented building blocks
used throughout the ecosystem: Green's function containers and their meshes, second-quantization
operators, lattice and tight-binding tools, exact diagonalization, Monte Carlo tools, and more. These
are documented in the :ref:`User guide <triqs_userguide>` and the :ref:`API Documentation <documentation>`.


.. _numerical_algorithms:

Numerical Algorithms
====================

TRIQS provides implementations of advanced numerical algorithms that are directly relevant to
the study of interacting quantum systems, for example:

* Orthogonal polynomial representation of imaginary-time Green’s functions `Phys. Rev. B 84 075145 <https://doi.org/10.1103/PhysRevB.84.075145>`_

* Discrete Lehmann representation of imaginary time Green's functions `Phys. Rev. B 105 235115 <https://doi.org/10.1103/PhysRevB.105.235115>`_
  through the cppdlr library `J. Open Source Softw. 9(100) 6297 <https://doi.org/10.21105/joss.06297>`_

* Constrained residual minimization Dyson Solver `Phys. Rev. B 111 115155 <https://doi.org/10.1103/PhysRevB.111.115155>`_

Please cite the relevant papers when using these algorithms in your research.


Applications
============

Within the broader TRIQS ecosystem we provide various TRIQS applications
that assemble the TRIQS library components to provide full-fledged and cutting-edge implementations
of different many-body algorithms. They allow for example to solve a generic quantum impurity model
or to run a complete DFT+DMFT calculation.

The TRIQS library and applications have enabled researchers to address topics as diverse as:

* Spin-Orbit Coupling and Electronic Correlations in Sr2RuO4, `Phys. Rev. Lett. 120 126401 <https://doi.org/10.1103/PhysRevLett.120.126401>`_
* Late transition metal oxides with infinite-layer structure: Nickelates versus cuprates, `Phys. Rev. B 101 081110 <https://doi.org/10.1103/PhysRevB.101.081110>`_
* Many-Body Electronic Structure of NdNiO2 and CaCuO2 `Phys. Rev. X 10 021061 <https://doi.org/10.1103/PhysRevX.10.021061>`_
* Tracking the Footprints of Spin Fluctuations: A MultiMethod, MultiMessenger Study of the Two-Dimensional Hubbard Model `Phys. Rev. X 11, 011058 <https://doi.org/10.1103/PhysRevX.11.011058>`_
* Linear resistivity and Sachdev-Ye-Kitaev (SYK) spin liquid behavior in a quantum critical metal with spin-1/2 fermions `Proc. Natl. Acad. Sci. USA 117, 18341 <https://doi.org/10.1073/pnas.2003179117>`_

Various other TRIQS related research publications can be found through a targeted `Google Scholar search <https://scholar.google.com/scholar?cites=11179891035589079889&as_sdt=5,33&sciodt=0,33&hl=en>`_.

The complete list of applications is provided on the following page:

.. toctree::
   :maxdepth: 2

   applications
