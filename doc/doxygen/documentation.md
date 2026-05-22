@page documentation API Documentation

[TOC]

The **TRIQS** library provides various useful tools for studying interacting quantum systems.

The following provides a detailed reference documentation grouped into logical units.

If you are looking for a specific function, class, etc., try using the search bar in the top left corner.

## Atom Diag

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

## Operators

## Statistical analysis

## Testing tools

## Utilities


