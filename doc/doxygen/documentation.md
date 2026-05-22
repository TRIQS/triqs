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


