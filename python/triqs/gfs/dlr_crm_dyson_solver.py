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

import numpy as np

from scipy.optimize import minimize, NonlinearConstraint

from triqs.gf import (
    Gf,
    BlockGf,
    make_gf_dlr,
    make_gf_dlr_imfreq,
    inverse
)
from triqs.gf.meshes import MeshDLRImFreq, MeshDLRImTime, MeshDLR

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
    """
    Contrained Residual Minimization Dyson solver as described in https://arxiv.org/abs/2310.01266

    Defines the Dysons equation as an optimization problem:

        G - G0 - G0*Σ*G = 0

    and solves it using scipy.optimize.minimize using the DLR representation for the Green's functions.

    The solver optimizes only the dynamic part of the self-energy Σ_dyn(iν)= Σ(iν) - Σ_0.
    Here, Σ_0 is the Hartree shift. If provided the second moment Σ_1 is used as a non-linear constraint in the solver.

    The moments can be explicitly calculated in the impurity solver, see for example the `cthyb high frequency moments tutorial <https://triqs.github.io/cthyb/latest/guide/high_freq_moments.html>`_ .

    Alternatively the moments can be approximated by fitting the tail of the self-energy calculated via normal Dyson equation first:

    >>> S_iw = inverse(G0_iw) - inverse(G_iw)
    >>> tail, err = S_iw.fit_hermitian_tail()

    and then used as input for the Dyson solver:

    >>> S_iw_dlr, Sigma_HF, residual = minimize_dyson(G0_dlr=G0_dlr, G_dlr=G_dlr, Sigma_moments=tail[0:1])

    The input G_dlr can for example obtained via `fit_gf_dlr` from a noisy imaginary time Green's function or by directly setting the DLR mesh points from a full `MeshImFreq` G_iw object:

    >>> for iwn in G_dlr_iw.mesh:
    >>>     G_dlr_iw[iwn] = G_full_iw(iwn)

    Parameters
    ----------
    G0_dlr : triqs.gf.Gf or triqs.gf.BlockGf
        non-interacting Green's function defined on a DLR, DLRImTime, or DLRImFreq mesh
    G_dlr : triqs.gf.Gf or triqs.gf.BlockGf
        interacting Green's function defined on a DLR, DLRImTime, or DLRImFreq mesh
    Sigma_moments : list of numpy.ndarray or dict of list of numpy.ndarray
        moments of Σ. The first moment is the Hartree shift, i.e. the constant part of Σ.
        If provdided, use the second moment as a non-linear constraint for the Dyson solver.
    method : str, optional
        optimization method, defaults to 'trust-constr'
        Note: For non-linear constraints this is one of the few available methods
    options : dict, optional
        optimization options, defaults to dict(maxiter=5000, disp=True, gtol=1e-32, xtol=1e-100, finite_diff_rel_step=1e-20)

    Returns
    -------
    Sigma_DLR : triqs.gf.Gf or triqs.gf.BlockGf
        optimized self-energy defined on a DLRImFreq mesh
    Sigma_0 : numpy.ndarray
        Hartree shift
    residual : float
        L2 norm of residual (G-G₀-G₀ΣG)

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
            """
            constraint condition: ∑σk =  Σ_1
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
        """
        target function for minimize
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
