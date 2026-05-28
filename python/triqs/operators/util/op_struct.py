# Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2021 Simons Foundation
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You may obtain a copy of the License at
#     https:#www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Michel Ferrero, Priyanka Seth, Nils Wentzell

"""Block/index helpers for many-body operators and Green's functions."""

from itertools import product

# Set function to make index for GF blocks given spin sn and orbital name on
def get_mkind(off_diag, map_operator_structure):
    """Build the index function that maps a (spin, orbital) pair to an operator/GF-block index.

    Parameters
    ----------
    off_diag : bool
        If ``True``, the operators and Green's-function blocks are denoted by
        ``('spin', 'orbital')``; otherwise by ``('spin_orbital', 0)``.
    map_operator_structure : dict
        Mapping of GF-block names from one convention to another, e.g.
        ``{('up', 0): ('up_0', 0), ('down', 0): ('down_0', 0)}``. If provided,
        the operators and blocks are labelled by the image of
        ``('spin', 'orbital')`` under this mapping.

    Returns
    -------
    callable
        Function ``mkind(spin_name, orbital_name)`` returning a tuple suitable
        for indexing :func:`~triqs.operators.operators.c` / 
        :func:`~triqs.operators.operators.c_dag` and the GF blocks.
    """

    if (off_diag is None) and (map_operator_structure is None):
        raise ValueError("hamiltonians: provide either off_diag or map_operator_structure.")

    if map_operator_structure is None:
        if off_diag:
            mkind = lambda sn, on: (sn, on)
        else:
            mkind = lambda sn, on: (sn+'_%s'%on, 0)
    else:
        mkind = lambda sn, on: map_operator_structure[(sn,on)]

    return mkind

# Set block structure of GF
def set_operator_structure(spin_names, n_orb, off_diag):
    """Build the operator block structure for the given spin and orbital names.

    The shape of the structure follows the same convention as
    :func:`get_mkind`: one block per spin when ``off_diag`` is ``True``, one
    block per (spin, orbital) pair otherwise.

    Parameters
    ----------
    spin_names : list of str
        Names of the spins, e.g. ``['up', 'down']``.
    n_orb : int
        Number of orbitals.
    off_diag : bool
        If ``True``, blocks carry orbital off-diagonal elements; otherwise the
        blocks are pure ``('spin_orbital', 0)`` blocks of size one.

    Returns
    -------
    list
        Block structure of the operators in the form
        ``[[block_name, block_size], ...]``.
    """

    if isinstance(n_orb, list):
        import warnings
        warnings.warn("set_operator_structure takes as a second argument the number of orbitals, not a list of orbital names")
        n_orb = len(n_orb)

    op_struct = []
    if off_diag: # outer blocks are spin blocks
        for sn in spin_names:
            op_struct.append([sn, n_orb])
    else: # outer blocks are spin-orbital blocks
        for sn, on in product(spin_names,range(n_orb)):
            op_struct.append([sn+'_%s'%on, 1])

    return op_struct
