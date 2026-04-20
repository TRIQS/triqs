# Copyright (c) 2024-2026 Simons Foundation
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
#     https://www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Nils Wentzell

import numpy as np

from triqs.gf import (
    Gf, BlockGf, MeshImFreq, MeshDLRImFreq,
    make_gf_dlr, make_gf_imfreq,
)


def make_gf_dlr_imfreq(g_iw, w_max=None, eps=1e-10, w_max_init=1.0, w_max_max=200.0):
    """Sample a Green's function from MeshImFreq onto a MeshDLRImFreq.

    Parameters
    ----------
    g_iw : Gf or BlockGf
        Input Green's function on MeshImFreq.
    w_max : float, optional
        DLR frequency cutoff. If provided, used directly.
        If None, automatically determined by increasing w_max until the
        DLR round-trip reproduces the input within eps.
    eps : float, optional
        DLR precision parameter. Default: 1e-10.
    w_max_init : float, optional
        Starting w_max for automatic search. Only used when w_max is None. Default: 1.0.
    w_max_max : float, optional
        Upper bound for automatic w_max search. Only used when w_max is None. Default: 200.0.

    Returns
    -------
    g_dlr_iw : Gf or BlockGf
        Green's function on MeshDLRImFreq. The w_max used is available
        as ``g_dlr_iw.mesh.w_max``.
    """
    if isinstance(g_iw, BlockGf):
        return _make_blockgf_dlr_imfreq(g_iw, w_max, eps, w_max_init, w_max_max)
    elif isinstance(g_iw, Gf):
        return _make_gf_dlr_imfreq(g_iw, w_max, eps, w_max_init, w_max_max)
    else:
        raise TypeError(f"Expected Gf or BlockGf, got {type(g_iw)}")


def _sample_on_dlr_mesh(g_iw, dlr_mesh):
    """Sample a single Gf onto a DLR ImFreq mesh."""
    g_dlr_iw = Gf(mesh=dlr_mesh, target_shape=g_iw.target_shape)
    for w in dlr_mesh:
        g_dlr_iw[w] = g_iw(w)
    return g_dlr_iw


def _round_trip_error(g_iw, dlr_mesh):
    """Compute the max error of a DLR round-trip on a single Gf."""
    g_dlr_iw = _sample_on_dlr_mesh(g_iw, dlr_mesh)
    g_dlr = make_gf_dlr(g_dlr_iw)
    n_iw = len(g_iw.mesh) // 2
    g_rec = make_gf_imfreq(g_dlr, n_iw)
    return np.max(np.abs(g_rec.data - g_iw.data))


def _find_w_max(g_iw, eps, w_max_init, w_max_max):
    """Find the smallest w_max such that DLR round-trip error < eps."""
    mesh = g_iw.mesh
    beta = mesh.beta
    statistic = str(mesh.statistic)

    w_max = w_max_init
    max_err = float('inf')
    while w_max <= w_max_max:
        dlr_mesh = MeshDLRImFreq(beta, statistic, w_max, eps, True)

        if isinstance(g_iw, BlockGf):
            max_err = max(_round_trip_error(g_bl, dlr_mesh) for _, g_bl in g_iw)
        else:
            max_err = _round_trip_error(g_iw, dlr_mesh)

        if max_err < eps:
            return w_max, dlr_mesh

        w_max *= 1.5

    raise RuntimeError(
        f"make_gf_dlr_imfreq: could not find suitable w_max <= {w_max_max} "
        f"for eps={eps} (last error: {max_err:.2e})"
    )


def _validate_dlr_mesh(g_iw_mesh, dlr_mesh):
    """Check that all DLR mesh indices fit within the ImFreq mesh."""
    dlr_idx = np.array([iw.index for iw in dlr_mesh])
    max_dlr_idx = max(abs(dlr_idx[0]), abs(dlr_idx[-1]))
    n_iw = g_iw_mesh.n_iw
    if max_dlr_idx > n_iw:
        raise ValueError(
            f"make_gf_dlr_imfreq: g_iw.mesh.n_iw = {n_iw} < "
            f"maximum DLR ImFreq index ({max_dlr_idx}). "
            f"Increase n_iw or reduce w_max."
        )


def _make_gf_dlr_imfreq(g_iw, w_max, eps, w_max_init, w_max_max):
    """Implementation for a single Gf."""
    assert isinstance(g_iw.mesh, MeshImFreq), \
        "make_gf_dlr_imfreq: input must be on MeshImFreq."

    if w_max is None:
        w_max, dlr_mesh = _find_w_max(g_iw, eps, w_max_init, w_max_max)
    else:
        beta = g_iw.mesh.beta
        statistic = str(g_iw.mesh.statistic)
        dlr_mesh = MeshDLRImFreq(beta, statistic, w_max, eps, True)

    _validate_dlr_mesh(g_iw.mesh, dlr_mesh)
    return _sample_on_dlr_mesh(g_iw, dlr_mesh)


def _make_blockgf_dlr_imfreq(g_iw, w_max, eps, w_max_init, w_max_max):
    """Implementation for a BlockGf."""
    mesh = g_iw.mesh
    assert isinstance(mesh, MeshImFreq), \
        "make_gf_dlr_imfreq: input must be on MeshImFreq."

    if w_max is None:
        w_max, dlr_mesh = _find_w_max(g_iw, eps, w_max_init, w_max_max)
    else:
        beta = mesh.beta
        statistic = str(mesh.statistic)
        dlr_mesh = MeshDLRImFreq(beta, statistic, w_max, eps, True)

    _validate_dlr_mesh(mesh, dlr_mesh)

    names, blocks = [], []
    for bl, g_bl in g_iw:
        names.append(bl)
        blocks.append(_sample_on_dlr_mesh(g_bl, dlr_mesh))
    return BlockGf(name_list=names, block_list=blocks)
