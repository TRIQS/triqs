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
# Authors: Nils Wentzell, H. U.R. Strand, P. Seth, I. Krivenko, M. Ferrero, O. Parcollet

import numpy as np

from triqs.gf.gf_fnt import fit_hermitian_tail_on_window, replace_by_tail


def tail_fit(
        Sigma_iw,
        fit_min_n=None, fit_max_n=None,
        fit_min_w=None, fit_max_w=None,
        fit_max_moment=None, fit_known_moments=None
        ):
    """
    Fit a high frequency 1/(iw)^n expansion of Sigma_iw
    and replace the high frequency part with the fitted tail.

    Either give the frequency window to fit on in terms of Matsubara frequency
    index (fit_min_n/fit_max_n) or value (fit_min_w/fit_max_w).

    Parameters
    ----------
    Sigma_iw : BlockGf
               Self-energy.
    fit_min_n : int, optional, default=int(0.8*n_iw)
                Matsubara frequency index from which tail fitting should start.
    fit_max_n : int, optional, default=n_iw
                Matsubara frequency index at which tail fitting should end.
    fit_min_w : float, optional
                Matsubara frequency from which tail fitting should start.
    fit_max_w : float, optional
                Matsubara frequency at which tail fitting should end.
    fit_max_moment : int, optional, default=3
                     Highest moment to fit in the tail of Sigma_iw.
    fit_known_moments : dict, optional, default=None
                        Known moments of Sigma_iw per block, given as numpy ndarrays.

    Returns
    -------
    Sigma_iw : BlockGf
               Self-energy (modified in-place).
    """

    if fit_min_w is not None: fit_min_n = int(0.5*(fit_min_w*Sigma_iw.mesh.beta/np.pi - 1.0))
    if fit_max_w is not None: fit_max_n = int(0.5*(fit_max_w*Sigma_iw.mesh.beta/np.pi - 1.0))
    if fit_min_n is None: fit_min_n = int(0.8*len(Sigma_iw.mesh)/2)
    if fit_max_n is None: fit_max_n = int(len(Sigma_iw.mesh)/2)
    if fit_max_moment is None: fit_max_moment = 3

    if fit_known_moments is None:
        fit_known_moments = {}
        for name, sig in Sigma_iw:
            shape = [0] + list(sig.target_shape)
            fit_known_moments[name] = np.zeros(shape, dtype=complex)

    for name, sig in Sigma_iw:

        tail, err = fit_hermitian_tail_on_window(
            sig,
            n_min = fit_min_n,
            n_max = fit_max_n,
            known_moments = fit_known_moments[name],
            n_tail_max = 10 * len(sig.mesh),
            expansion_order = fit_max_moment
            )

        replace_by_tail(sig, tail, n_min=fit_min_n)

    return Sigma_iw
