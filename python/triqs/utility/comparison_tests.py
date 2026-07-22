# Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2020 Simons Foundation
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
# Authors: Alexander Hampel, Olivier Parcollet, Nils Wentzell

r"""
Numerical equality assertions for arrays and Green's functions.

These helpers are used throughout the TRIQS test suite to compare
numerical objects up to a tolerance. Each function raises
:class:`AssertionError` when the inputs differ by more than
``precision`` in maximum absolute element-wise difference, and
returns ``None`` on success.
"""

import numpy as np

def assert_arrays_are_close(a, b, precision = 1.e-6):
    r"""
    Assert that two arrays are equal in maximum absolute difference.

    Computes :math:`d = \max_i |a_i - b_i|` and requires
    ``d < precision``. The assertion error message embeds both arrays
    and the computed difference, to aid debugging.

    Parameters
    ----------
    a, b : array-like
        Arrays of identical shape.
    precision : float, optional
        Maximum allowed value of the elementwise absolute difference.
        Default ``1e-6``.

    Raises
    ------
    AssertionError
        If ``max(abs(a - b)) >= precision``.
    """
    d = np.amax(np.abs(a - b))
    assert  d< precision, "Arrays are different. Difference is %s.\n %s \n\n --------- \n\n %s"%(d,a,b)

def assert_array_close_to_scalar(a, x, precision = 1.e-6):
    r"""
    Assert that a square matrix equals ``x`` times the identity.

    Compares ``a`` against :math:`x\, \mathbb{1}_n` where ``n`` is
    ``a.shape[0]``, via :func:`assert_arrays_are_close`.

    Parameters
    ----------
    a : numpy.ndarray
        Two-dimensional square array.
    x : scalar
        Scalar value to compare ``a`` against, after multiplication by
        the identity matrix.
    precision : float, optional
        Maximum allowed element-wise absolute difference. Default
        ``1e-6``.

    Raises
    ------
    AssertionError
        If ``a`` differs from ``x * identity(a.shape[0])`` by more than
        ``precision`` in any entry.
    """
    assert_arrays_are_close(a, np.identity(a.shape[0])*(x), precision)

def assert_gfs_are_close(a, b, precision = 1.e-6):
    r"""
    Assert that two Green's functions agree on their data array.

    Checks that ``a`` and ``b`` have the same mesh type and equal
    meshes, then compares ``a.data`` and ``b.data`` via
    :func:`assert_arrays_are_close`.

    Parameters
    ----------
    a, b : triqs.gfs.Gf
        Green's functions with identical mesh and target shape.
    precision : float, optional
        Maximum allowed element-wise absolute difference on the data
        array. Default ``1e-6``.

    Raises
    ------
    AssertionError
        If the mesh types differ, the meshes are not equal, or the
        data arrays differ by more than ``precision``.
    """
    assert type(a.mesh) == type(b.mesh), "GFs have different type of meshes !"
    assert a.mesh == b.mesh, "meshes are different"
    assert_arrays_are_close(a.data, b.data, precision)

def assert_block_gfs_are_close(a, b, precision = 1.e-6):
    r"""
    Assert that two :class:`BlockGf` objects are equal block by block.

    Iterates over the blocks of ``a`` and ``b`` in parallel, checks
    that the block names match, and defers the data comparison of each
    block to :func:`assert_gfs_are_close`.

    Parameters
    ----------
    a, b : triqs.gfs.BlockGf
        Block Green's functions with the same number of blocks and the
        same block names.
    precision : float, optional
        Maximum allowed element-wise absolute difference on each
        block's data array. Default ``1e-6``.

    Raises
    ------
    AssertionError
        If the number of blocks differs, the block names differ, or
        any block fails :func:`assert_gfs_are_close`.
    """
    assert len(a) == len(b), "Block GFs have different number of blocks"
    for (nx,x),(ny,y) in zip(a,b):
        # first check if the names of the two blocks match
        assert nx == ny, "block name {} does not match {}".format(nx,ny)
        assert_gfs_are_close(x, y, precision)

def assert_block2_gfs_are_close(a, b, precision = 1.e-6):
    r"""
    Assert that two :class:`Block2Gf` objects are equal block by block.

    Currently a thin wrapper around :func:`assert_block_gfs_are_close`
    -- the iteration interface of :class:`Block2Gf` is compatible with
    that of :class:`BlockGf`.

    Parameters
    ----------
    a, b : triqs.gfs.Block2Gf
        Two-index block Green's functions.
    precision : float, optional
        Maximum allowed element-wise absolute difference. Default
        ``1e-6``.

    Raises
    ------
    AssertionError
        If any inner block comparison fails.
    """
    assert_block_gfs_are_close(a, b, precision)
