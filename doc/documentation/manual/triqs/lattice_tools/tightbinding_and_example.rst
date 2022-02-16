The BravaisLattice and TightBinding classes: definitions and example
====================================================================


The following example is aimed at demonstrating the use of **TRIQS
Lattice tools**.

BravaisLattice
--------------

A ``BravaisLattice`` is constructed as
``BravaisLattice(units, orbital_positions )`` where

-  ``units`` is the list the coordinates (given as triplets) of the
   basis vectors :math:`\lbrace \mathbf{e}_i \rbrace _{i=1\dots d}`
   (:math:`d` is the dimension)
-  ``orbital_positions`` is a dictionary of the atoms forming the basis
   of the Bravais Lattice: the key is the name of the atom/orbital, the
   value is the triplet of its coordinates.

TightBinding
------------

A tight-binding lattice is defined by the relation:

.. math::  \mathbf{t}_k  = \sum_{\mathbf{R}\in \mathrm{BL}} e^{i \mathbf{k}\cdot \mathbf{R}} \mathbf{t}_\mathbf{R}

where :math:`\mathbf{t}_i` is the matrix of the hoppings from a
documentation/manual/triqs unit cell (:math:`\mathbf{R}=O`\ ) to a unit cell indexed by
:math:`\mathbf{R}`\ . :math:`(\mathbf{t}_\mathbf{R})_{n,m}` is the
tight-binding integral between atom :math:`n` of site :math:`O` and atom
:math:`m` of site :math:`\mathbf{R}`\ , ie

.. math:: (\mathbf{t}_\mathbf{R})_{n,m} \equiv \int d^3\mathbf{r} \phi_n(\mathbf{r})^{*} V(\mathbf{r}) \phi_m(\mathbf{r}-\mathbf{R})

where :math:`\phi_n(\mathbf{r}-\mathbf{R})` is the Wannier orbital of
atom :math:`n` centered at site :math:`\mathbf{R}`\ . The corresponding
class in **Lattice Tools** is the ``TightBinding`` class. Its instances
are constructed as follows:

``TightBinding ( bravais_lattice, hopping_dictionary)`` where

-  ``bravais_lattice`` is an instance of ``BravaisLattice``
-  ``hopping_dictionary`` is a dictionary of the hoppings
   :math:`\mathbf{t}_\mathbf{R}`\ , where the keys correspond to the
   coordinates of :math:`\mathbf{R}` in the unitary basis
   :math:`\lbrace \mathbf{e}_i \rbrace _{i=1\dots d}`\ , and the values
   to the corresponding matrix: :math:`(\mathbf{t}_\mathbf{R})_{n,m}`

energies_on_bz_path
-------------------

The function ``energies_on_bz_path (TB, start, end, n_pts)`` returns a
:math:`n_{at} \times n_{pts}` matrix :math:`E` such that

``E[n,k]``:math:`= \epsilon_n(\mathbf{k})`

where ``k`` indexes the ``n_pts`` :math:`\mathbf{k}`\ -points of the
line joining ``start`` and ``end``, and :math:`\epsilon_n(k)` is the
:math:`n`\ th eigenvector of :math:`t_\mathbf{k}`\ .

Example
-------

The following example illustrates the usage of the above tools for the
case of a two-dimensional, square lattice with various unit cells. We
successively construct three Bravais lattices ``BL_1``, ``BL_2`` and
``BL_4`` with, respectively, 1, 2 and 4 atoms per unit cell, as well as
three tight-binding models with hopping dictionaries ``hop_1``,
``hop_2`` and ``hop_4``

.. plot:: documentation/manual/triqs/lattice_tools/ex2.py
    :include-source:
    :scale: 70
