################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011 by M. Ferrero, O. Parcollet
#
# TRIQS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TRIQS. If not, see <http://www.gnu.org/licenses/>.
#
################################################################################

import lazy_expressions, descriptors
from block_gf import BlockGf
from gf import GfImFreq, TailGf
from descriptor_base import A_Omega_Plus_B
import numpy as np

def inverse(x):
    if descriptors.is_lazy(x):
        return lazy_expressions.lazy_function("inverse", inverse) (x)
    assert hasattr(x,'invert') and hasattr(x,'copy')
    cop = x.copy() 
    cop.invert()
    return cop


def delta(g):
    """Compute delta from G0"""
    if type(g) == BlockGf:
    	return BlockGf(name_block_generator = [ (n, delta(g0)) for n,g0 in g], make_copies=False)
    elif type(g) == GfImFreq:
    	g0_iw_inv = inverse(g)
    	delta_iw = g0_iw_inv.copy()
    	delta_iw << A_Omega_Plus_B(g0_iw_inv.tail[-1], g0_iw_inv.tail[0])
    	delta_iw -= g0_iw_inv
        return delta_iw
    else:
        raise TypeError, "No function delta for g0 object of type %s"%type(g) 


# Determine one of G0_iw, G_iw and Sigma_iw from other two using Dyson's equation
def dyson(**kwargs):
    """
    Solve Dyson's equation for given two of G0_iw, G_iw and Sigma_iw to yield the third.

    Parameters
    ----------
    G0_iw : Gf, optional
            Non-interacting Green's function.
    G_iw : Gf, optional
           Interacting Green's function.
    Sigma_iw : Gf, optional
               Self-energy.
    """
    if not (len(kwargs)==2 and set(kwargs.keys())<set(['G0_iw','G_iw', 'Sigma_iw'])):
        raise ValueError, 'dyson: Two (and only two) of G0_iw, G_iw and Sigma_iw must be provided to determine the third.'
    if 'G0_iw' not in kwargs:
        G0_iw = inverse(kwargs['Sigma_iw'] - inverse(kwargs['G_iw']))
        return G0_iw
    elif 'G_iw' not in kwargs:
        G_iw = inverse(inverse(kwargs['G0_iw']) - kwargs['Sigma_iw'])
        return G_iw
    elif 'Sigma_iw' not in kwargs:
        Sigma_iw = inverse(kwargs['G0_iw']) - inverse(kwargs['G_iw'])
        return Sigma_iw

# Fit tails for Sigma_iw and G_iw.
# Either give window to fix in terms of matsubara frequencies index (fit_min_n/fit_max_n) or value (fit_min_w/fit_max_w).
def tail_fit(Sigma_iw,G0_iw=None,G_iw=None,fit_min_n=None,fit_max_n=None,fit_min_w=None,fit_max_w=None,fit_n_moments=None,fit_known_moments=None):
    """
    Fit the tails of Sigma_iw and optionally G_iw.

    Parameters
    ----------
    Sigma_iw : Gf
               Self-energy.
    G0_iw : Gf, optional
            Non-interacting Green's function.
    G_iw : Gf, optional
           Interacting Green's function.
           If G0_iw and G_iw are provided, the tails of G_iw will also be fitted.
    fit_min_n : int, optional, default=int(0.8*len(Sigma_iw.mesh))
                Matsubara frequency index from which tail fitting should start.
    fit_max_n : int, optional, default=int(len(Sigma_iw.mesh))
                Matsubara frequency index at which tail fitting should end.
    fit_min_w : float, optional
                Matsubara frequency from which tail fitting should start.
    fit_max_w : float, optional
                Matsubara frequency at which tail fitting should end.
    fit_n_moments : int, optional
                    Number of moments to fit in the tail of Sigma_iw.
    fit_known_moments : dict{str:TailGf object}, optional, default = {block_name: TailGf(dim1, dim2, n_moments, order_min)}
                        Known moments of Sigma_iw, given as a TailGf object.
    """

    # Define default tail quantities
    if fit_known_moments is None:
        fit_known_moments = {name:TailGf(sig.N1,sig.N2,0,0) for name, sig in Sigma_iw} # TailGf(dim1, dim2, n_moments, order_min)
    if fit_min_w is not None: fit_min_n = int(0.5*(fit_min_w*Sigma_iw.mesh.beta/np.pi - 1.0))
    if fit_max_w is not None: fit_max_n = int(0.5*(fit_max_w*Sigma_iw.mesh.beta/np.pi - 1.0))
    if fit_min_n is None: fit_min_n = int(0.8*len(Sigma_iw.mesh))
    if fit_max_n is None: fit_max_n = int(len(Sigma_iw.mesh))
    if fit_n_moments is None: fit_n_moments = 3

    # Now fit the tails of Sigma_iw and G_iw
    for name, sig in Sigma_iw: sig.fit_tail(fit_known_moments[name], fit_n_moments, fit_min_n, fit_max_n)
    if (G_iw is not None) and (G0_iw is not None):
        for name, g in G_iw: g.tail = inverse( inverse(G0_iw[name].tail) - Sigma_iw[name].tail )

    return Sigma_iw, G_iw
