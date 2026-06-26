@page documentation API Documentation

[TOC]

The **TRIQS** library provides various useful tools for studying interacting quantum systems.

The following provides a detailed reference documentation grouped into logical units.

If you are looking for a specific function, class, etc., try using the search bar in the top left corner.

## Atomic problem diagonalization

@ref triqs-atom-diag provides a lightweight exact diagonalization solver for finite fermionic Hamiltonians,
together with helpers that build derived quantities from a solved eigensystem:

- @ref triqs::atom_diag::atom_diag holds the block-diagonal Hamiltonian, its eigensystem, and the matrix
representations of the fundamental creation/annihilation operators in the eigenbasis. The Hilbert space is
partitioned into invariant subspaces by auto-partitioning, by user-supplied quantum numbers, or by a
particle-number window, and each block is then diagonalized with the QR algorithm.
- Thermodynamic averages of an observable are computed via triqs::atom_diag::partition_function,
triqs::atom_diag::atomic_density_matrix and triqs::atom_diag::trace_rho_op, and the action of an operator on a
state vector is provided by triqs::atom_diag::act.
- Eigenvalues of conserved quantities can be tabulated with triqs::atom_diag::quantum_number_eigenvalues and
triqs::atom_diag::quantum_number_eigenvalues_checked.
- The atomic Green's function on an imaginary-time, Matsubara, Legendre or real-frequency mesh is built by
triqs::atom_diag::atomic_g_tau, triqs::atom_diag::atomic_g_iw, triqs::atom_diag::atomic_g_l and
triqs::atom_diag::atomic_g_w, all of which derive from the Lehmann representation exposed by
triqs::atom_diag::atomic_g_lehmann.

## Determinant manipulation

## Experimental tools

## Green's functions

## Hilbert space

## Lattice tools

@ref triqs-lattice provide classes to describe crystal lattices and tight-binding Hamiltonians:

- @ref triqs::lattice::bravais_lattice represents a Bravais lattice in 1, 2 or 3 dimensions,
defined by its basis vectors and the positions of atomic orbitals within the unit cell.
- @ref triqs::lattice::brillouin_zone represents the first Brillouin zone of a given Bravais
lattice via its reciprocal-space basis vectors.
- @ref triqs::lattice::tight_binding represents a tight-binding Hamiltonian on a Bravais lattice.
It supports Fourier transforms to reciprocal space, dispersion (band-structure) calculations on
Brillouin zone meshes and produces matrix-valued Green's functions on those meshes.
- @ref triqs::lattice::grid_generator generates a regular grid of points inside the d-dimensional
unit cube, useful for Brillouin zone sampling.

## MC tools

The @ref triqs-mc module provides a generic, MPI-aware Markov-chain Monte Carlo framework. 
The user supplies moves (see @ref triqs-mc-moves), measurements (see @ref triqs-mc-measures) and a sign type satisfying 
the concepts in @ref triqs-mc-concepts.
These are registered with a triqs::mc_tools::mc_generic instance (see @ref triqs-mc-simulation) which drives the warmup 
and accumulation phases. 
Random number generation and other building blocks are documented under @ref triqs-mc-utils.

## Meshes

@ref triqs-meshes form the basis of the @ref triqs-gfs containers in the **TRIQS** library. They define the mathematical 
domain of the underlying function, how it is represented in memory, and how it is evaluated.

- @ref triqs-meshes-concepts specify the requirements for mesh types and mesh points (triqs::mesh::Mesh, 
triqs::mesh::MeshWithValues, triqs::mesh::MeshPoint).
- @ref triqs-meshes-types are the concrete mesh classes provided by **TRIQS**:
  - @ref triqs-meshes-imag for imaginary time and Matsubara frequency domains, including 
  @ref triqs-meshes-matsubara arithmetic.
  - @ref triqs-meshes-real for real time and frequency domains.
  - @ref triqs-meshes-lattice for Bravais lattices and Brillouin zones, with @ref triqs-meshes-kexpr for lazy k-vector 
  arithmetic.
  - @ref triqs-meshes-func for function-space representations such as Legendre polynomials and DLR.
  - @ref triqs-meshes-prod combine several meshes into a single product mesh.
  - @ref triqs-meshes-other contain base classes and rarely used mesh types.
- @ref triqs-meshes-utils provide common utilities used across mesh types (iteration, evaluation, helper functions).
- @ref triqs-meshes-tailfitting fits the high- and low-frequency tails of functions on real or imaginary frequency 
meshes.

## Operators

## Statistical analysis tools

@ref triqs-stat is a header-mostly toolkit for the statistical analysis of Monte Carlo data and other sampled
observables:

- @ref triqs-stat-accs provide streaming binning accumulators: @ref triqs::stat::lin_binning groups consecutive
samples into equally sized bins and keeps the bin data for downstream resampling, while @ref triqs::stat::log_binning
maintains one bin per power-of-two bin size and reports the standard error and integrated autocorrelation time at each
binning level.
- @ref triqs-stat-meanerr offers standalone functions triqs::stat::mean, triqs::stat::mean_and_err and their MPI
counterparts. The flavour of mean (sum vs arithmetic mean) and error (variance, standard error of data, standard error
of mean, jackknife error) is selected through the triqs::stat::mean_tag and triqs::stat::error_tag enums.
- @ref triqs-stat-resampling provides triqs::stat::jackknife for bias-corrected estimates and jackknife errors of an
arbitrary function of the averages of one or more sampled observables.
- @ref triqs-stat-utils contains triqs::stat::histogram for counting samples into equally spaced bins on a closed
interval, plus small helpers shared by the rest of the module.
- @ref triqs-stat-concepts specify the value and range types accepted by the API (triqs::stat::StatCompatible,
triqs::stat::AccCompatible, triqs::stat::StatCompatibleRange).

Every component has an MPI-aware variant so the same client code runs serially or in parallel.

## Test tools

@ref triqs-test-tools is a header-only collection of GoogleTest-based helpers used throughout the **TRIQS** C++ test 
suite.

It provides various macros and helper functions to simplify writing unit tests involving TRIQS components.

## Utilities

@ref triqs-utility collects the foundational building blocks shared across the **TRIQS** library:

- @ref triqs-utility-io provide verbosity-controlled and auto-indenting output streams.
- @ref triqs-utility-macros provide the exception hierarchy, various macros, and the compiler / platform glue used by 
every other component.
- @ref triqs-utility-math expose numerical helpers used across the library, such as interpolation points and weights, 
analytic continuation and runtime-typed numbers.
- @ref triqs-utility-other collects general-purpose helpers that do not fit the other categories.
- @ref triqs-utility-runtime contains process-level utilities such as timers and signal handling tools.
- @ref triqs-utility-tuple contain various useful functions when working with `std::tuple` and `std::variant` types.
- @ref triqs-utility-traits collect template-metaprogramming helpers, type traits and RAII helpers.
