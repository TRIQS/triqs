# Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018 Simons Foundation
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

from itertools import product

# Set function to make index for GF blocks given spin sn and orbital name on
def get_mkind(off_diag,map_operator_structure):
    r"""
    Set function that makes the index for operators and GF blocks for a given
    spin and orbital name.

    Parameters
    ----------
    off_diag : boolean
               Do we have (orbital) off-diagonal elements?
               If yes, the operators and blocks are denoted by ('spin', 'orbital'), otherwise by ('spin_orbital',0).
    map_operator_structure : dict 
                             Mapping of names of GF blocks names from one convention to another, 
                             e.g. {('up', 0): ('up_0', 0), ('down', 0): ('down_0',0)}.
                             If provided, the operators and blocks are denoted by the mapping of ``('spin', 'orbital')``.

    Returns
    -------
    mkind : lambda
            The function mapping spin and orbital names to a tuple.

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
def set_operator_structure(spin_names,orb_names,off_diag):
    r"""
    Set the operator_structure for given spin and orbital names, according to
    whether or not the Green's functions contain off-diagonal blocks.

    Parameters
    ----------
    spin_names : list of strings
                 Names of the spins, e.g. ['up','down'].
    orb_names : list of strings or int       
                Names of the orbitals, e.g. [0,1,2] or ['t2g','eg'].
    off_diag : boolean
               Do we have (orbital) off-diagonal elements?
               If yes, the operators and blocks are denoted by ('spin', 'orbital'),
               otherwise by ('spin_orbital',0).

    Returns
    -------
    op_struct : list
                The structure of the operators [block:[inner], ... ].

    """

    op_struct = []
    if off_diag: # outer blocks are spin blocks
        for sn in spin_names:
            op_struct.append([sn, [on for on in orb_names]])
    else: # outer blocks are spin-orbital blocks
        for sn, on in product(spin_names,orb_names):
            op_struct.append([sn+'_%s'%on, [0]])

    return op_struct
