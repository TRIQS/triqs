r"""Experimental lattice tools.

.. warning::

   Experimental and unstable -- the API may change without notice.

This module provides tools for tight-binding calculations on a lattice:
tight-binding Hamiltonians :math:`H(\mathbf{k})`, Brillouin-zone integration,
local (k-integrated) Green's functions and the associated chemical-potential
search, folding onto a superlattice, and loaders for Wannier90 output files.
"""

from .lattice import TbHk, BzIntOptions, Superlattice, fold
from .lattice import gloc, find_chemical_potential
from .lattice import read_wannier90_tb_data, make_tb_hk_from_w90_tb_file, read_wannier90_hr_data, make_tb_hk_from_w90_hr_file


__all__ = ["TbHk", "BzIntOptions", "Superlattice",
           "gloc", "find_chemical_potential", "fold",
           "read_wannier90_tb_data", "make_tb_hk_from_w90_tb_file",
           "read_wannier90_hr_data", "make_tb_hk_from_w90_hr_file"]
