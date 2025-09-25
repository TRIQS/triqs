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

## Statical analysis tools

## Testing tools

## Utilities


