# Copyright (c) 2021-2024 Simons Foundation
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
# Authors: Alexander Hampel, Harrison LaBollita, Nils Wentzell

r"""Constrained-residual minimization (CRM) Dyson solver for DLR Green's functions.

Provides :func:`~triqs.gfs.dlr_crm_dyson_solver.minimize_dyson`, which solves Dyson's equation

.. math::

    G - G_0 - G_0\,\Sigma\,G = 0

as an optimization problem in the DLR (discrete Lehmann representation)
basis. The solver optimizes only the dynamic part of the self-energy
:math:`\Sigma_{\mathrm{dyn}}(i\nu) = \Sigma(i\nu) - \Sigma_0` and, when
available, uses higher-frequency moments as non-linear constraints. The
method is described in https://arxiv.org/abs/2310.01266.
"""

import numpy as np

from scipy.optimize import minimize, NonlinearConstraint

from triqs.gfs import (
    Gf,
    BlockGf,
    make_gf_dlr,
    make_gf_dlr_imfreq,
    inverse
)
from triqs.mesh import MeshDLRImFreq, MeshDLRImTime, MeshDLR

from triqs.utility import mpi

import warnings

warnings.filterwarnings('ignore', message='delta_grad == 0.0. Check if the approximated function is linear.')

def minimize_dyson(
    G0_dlr,
    G_dlr,
    Sigma_moments,
    method='trust-constr',
    options=dict(maxiter=5000, disp=True, gtol=1e-32, xtol=1e-100, finite_diff_rel_step=1e-20),
    **kwargs,
):
    r"""Solve Dyson's equation in the DLR basis by constrained residual minimization.

    Defines Dyson's equation as an optimization problem,

    .. math::

        G - G_0 - G_0\,\Sigma\,G = 0,

    and solves it on the DLR (discrete Lehmann representation) nodes
    via :func:`scipy.optimize.minimize`. The solver optimizes only the
    dynamic part of the self-energy
    :math:`\Sigma_{\mathrm{dyn}}(i\nu) = \Sigma(i\nu) - \Sigma_0`,
    where :math:`\Sigma_0` is the Hartree shift. When the second moment
    :math:`\Sigma_1` is supplied, it is enforced as a non-linear
    constraint on the optimizer.

    When ``G_dlr`` / ``G0_dlr`` are :class:`~triqs.gfs.block_gf.BlockGf`, the
    solve is dispatched block by block.

    Parameters
    ----------
    G0_dlr : Gf or BlockGf
        Non-interacting Green's function defined on a
        :class:`~triqs.mesh.meshes.MeshDLR`, :class:`~triqs.mesh.meshes.MeshDLRImTime`
        or :class:`~triqs.mesh.meshes.MeshDLRImFreq` mesh.
    G_dlr : Gf or BlockGf
        Interacting Green's function on the same mesh family as
        ``G0_dlr``.
    Sigma_moments : list of numpy.ndarray, or dict of list of numpy.ndarray
        High-frequency moments of :math:`\Sigma`. ``Sigma_moments[0]``
        is the Hartree shift :math:`\Sigma_0` (the constant part of
        :math:`\Sigma`). When supplied, ``Sigma_moments[1]`` is used as
        a non-linear constraint on the optimizer. For a
        :class:`~triqs.gfs.block_gf.BlockGf` input, a dict keyed by block name is expected.
    method : str, optional
        Optimization method forwarded to
        :func:`scipy.optimize.minimize`. Default ``'trust-constr'`` —
        one of the few methods that supports non-linear constraints.
    options : dict, optional
        Options forwarded to :func:`scipy.optimize.minimize`. Default
        ``dict(maxiter=5000, disp=True, gtol=1e-32, xtol=1e-100,
        finite_diff_rel_step=1e-20)``.

    Returns
    -------
    Sigma_DLR : Gf or BlockGf
        Optimized self-energy defined on a
        :class:`~triqs.mesh.meshes.MeshDLRImFreq` mesh.
    Sigma_0 : numpy.ndarray or dict of numpy.ndarray
        Hartree shift (per block when the input is a :class:`~triqs.gfs.block_gf.BlockGf`).
    residual : float or dict of float
        :math:`L_2` norm of the Dyson residual
        :math:`G - G_0 - G_0\,\Sigma\,G` (per block when the input is a
        :class:`~triqs.gfs.block_gf.BlockGf`).

    Notes
    -----
    The method is described in https://arxiv.org/abs/2310.01266. The
    moments can be computed directly in the impurity solver (see the
    `cthyb high frequency moments tutorial
    <https://triqs.github.io/cthyb/latest/guide/high_freq_moments.html>`_)
    or approximated by fitting the tail of the self-energy obtained
    from the usual Dyson equation.

    Examples
    --------
    Approximate the moments from a tail fit and pass them to the
    solver:

    >>> S_iw = inverse(G0_iw) - inverse(G_iw)
    >>> tail, err = S_iw.fit_hermitian_tail()
    >>> S_iw_dlr, Sigma_HF, residual = minimize_dyson(
    ...     G0_dlr=G0_dlr, G_dlr=G_dlr, Sigma_moments=tail[0:1])

    The input ``G_dlr`` can be obtained via ``fit_gf_dlr`` from a noisy
    imaginary-time Green's function, or by directly sampling the DLR
    mesh points from a full ``MeshImFreq`` ``G_iw`` object:

    >>> for iwn in G_dlr_iw.mesh:
    ...     G_dlr_iw[iwn] = G_full_iw(iwn)
    """

    # recursive call for BlockGf, could be MPI parallelized
    if isinstance(G_dlr, BlockGf) or isinstance(G0_dlr, BlockGf):
        assert isinstance(G_dlr, BlockGf) and isinstance(G0_dlr, BlockGf), 'G0_dlr and G_dlr must be both Gf or BlockGf'
        assert list(G_dlr.indices).sort() == list(G0_dlr.indices).sort(), 'G0_dlr and G_dlr must have the same block structure'
        Sig_dlr_list = []
        Sig_HF_list = {}
        residual_dict = {}
        for block, gtau in G_dlr:
            Sig_dlr, Sig_HF, res = minimize_dyson(G0_dlr[block], G_dlr[block], Sigma_moments[block], method, options, **kwargs)
            Sig_dlr_list.append(Sig_dlr)
            Sig_HF_list[block] = Sig_HF
            residual_dict[block] = res

        Bgf_Sigma_iw_fit = BlockGf(name_list=list(G_dlr.indices), block_list=Sig_dlr_list)

        return Bgf_Sigma_iw_fit, Sig_HF_list, residual_dict

    # initial checks
    if len(Sigma_moments) > 0:
        assert G_dlr.target_shape == G0_dlr.target_shape == Sigma_moments.shape[1:], 'number of orbs inconsistent across G, G0, and moments'
    else:
        raise ValueError('Provide self-energy moments for the Dyson solver as list of numpy.ndarray or dict of list of numpy.ndarray')

    # make sure we are working with matrix valued Green's functions
    if len(G_dlr.target_shape) == 0:
        G_dlr = Gf(mesh=G_dlr.mesh, data=G_dlr.data.reshape(-1, 1, 1))
        G0_dlr = Gf(mesh=G0_dlr.mesh, data=G0_dlr.data.reshape(-1, 1, 1))
        Sigma_moments = Sigma_moments.reshape(-1, 1, 1)
        scalar_output = True
    else:
        scalar_output = False

    # prepare meshes
    def to_dlr_imfreq(G):
        if isinstance(G.mesh, (MeshDLRImTime, MeshDLR)):
            return make_gf_dlr_imfreq(G)
        elif isinstance(G.mesh, MeshDLRImFreq):
            return G
        else:
            raise ValueError(f'minimize_dyson input Green functions must be defined on MeshDLRImFreq, MeshDLRImTime, or MeshDLR, but got {G.mesh}')

    g0_iwaa  = to_dlr_imfreq(G0_dlr)
    g_iwaa   = to_dlr_imfreq(G_dlr)
    assert g0_iwaa.mesh == g_iwaa.mesh, f'G0_dlr and G_dlr have incompatible dlr meshes {g0_iwaa.mesh} and {g_iwaa.mesh}'
    mesh_iw  = g_iwaa.mesh

    # Gf / mat -> vector conversion
    def flatten(arr):
        return arr.flatten().view(float)

    # vector > Gf / mat conversion
    def unflatten(vec):
        return vec.view(complex).reshape(G_dlr.data.shape)

    # setup constraints
    if len(Sigma_moments) == 1:
        constraints = ()
    else:  # len(Sigma_moments) >= 2, use only the second moment

        def constraint_func(x):
            r"""Non-linear constraint :math:`\sum_k \sigma_k = \Sigma_1` enforced on the solver.

            Parameters
            ----------
            x : numpy.ndarray
                Flattened real view of the dynamic self-energy DLR
                coefficients.

            Returns
            -------
            numpy.ndarray
                Flattened real view of :math:`\sum_k \sigma_k`, which
                the optimizer is constrained to equal the supplied
                second moment.
            """
            temp = Gf(mesh=mesh_iw, data=unflatten(x))
            sig = make_gf_dlr(temp)
            mat = sig.data.sum(axis=0)
            vec = flatten(mat)
            return vec

        bound = flatten(Sigma_moments[1])
        constraints = NonlinearConstraint(constraint_func, bound, bound)

    # target function for minimization
    def dyson_difference(x):
        r"""Objective function passed to :func:`scipy.optimize.minimize`.

        Parameters
        ----------
        x : numpy.ndarray
            Flattened real view of the dynamic self-energy DLR
            coefficients.

        Returns
        -------
        float
            Frobenius norm of the Dyson residual
            :math:`G - G_0 - G_0\,\Sigma\,G` evaluated on the DLR
            nodes.
        """
        sig_iwaa = Gf(mesh=mesh_iw, data=unflatten(x))
        sig_iwaa += Sigma_moments[0]
        #  G - G0 - G0*Σ*G = 0 done on the DLR nodes
        r_iwaa = g_iwaa - g0_iwaa - g0_iwaa * sig_iwaa * g_iwaa
        # the Frobeinus norm
        r = np.sqrt(np.sum(r_iwaa.tau_L2_norm() ** 2))
        return r

    # compute initial guess for Sigma from Dyson equation
    sig0_iwaa = inverse(g0_iwaa) - inverse(g_iwaa) - Sigma_moments[0]
    x_init = flatten(sig0_iwaa.data)

    # run solver to optimize Σ(iν)
    solution = minimize(dyson_difference, x_init, method=method, constraints=constraints, options=options)

    mpi.report(solution.message)
    if not solution.success:
        mpi.report('[WARNING] Minimization did not converge! Please proceed with caution!')

    # create optimized self-energy from minimizer
    sig_iwaa = Gf(mesh=mesh_iw, data=unflatten(solution.x))

    mpi.report(f'L2 norm of residual (G-G₀-G₀ΣG): {solution.fun:.4e}')
    if len(Sigma_moments) >= 2:
        constraint_violation = np.max(np.abs(make_gf_dlr(sig_iwaa).data.sum(axis=0) - Sigma_moments[1]))
        mpi.report(f'Σ1 constraint diff: {constraint_violation:.4e}')

    if scalar_output:
        return sig_iwaa[0, 0], Sigma_moments[0][0, 0], solution.fun
    else:
        return sig_iwaa, Sigma_moments[0], solution.fun
