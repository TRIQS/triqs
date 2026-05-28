# Copyright (c) 2017 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020-2023 Simons Foundation
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
# Authors: Michel Ferrero, Olivier Parcollet, Nils Wentzell

r"""Per-mesh plot functions for single-variable Green's functions.

Each function takes ``(self, opt_dict)`` where ``self`` is a
:class:`~triqs.gfs.gf.Gf` with the matching mesh type and ``opt_dict`` 
carries the plot options (``mode``, ``x_window``, ``name``, ...). They 
return the list of curve descriptors expected by 
:func:`~triqs.plot.mpl_interface.oplot`.
"""

from . import plot_base

#---------------------------------------------------------------
# A list of plot functions for
#---------------------------------------------------------------

def imfreq(self, opt_dict):
    r"""Plot protocol for :class:`~triqs.gfs.gf.Gf` objects on a 
    :class:`~triqs.mesh.meshes.MeshImFreq`.

    Parameters
    ----------
    self : Gf
        Green's function on a Matsubara mesh.
    opt_dict : dict
        Plot options. Recognised keys:

        * ``mode`` — ``'R'`` (real), ``'I'`` (imaginary). Default
          ``''`` (both).
        * ``x_window`` — ``(xmin, xmax)`` tuple clipping the frequency
          axis.
        * ``name`` — label / legend name for the curve.

    Returns
    -------
    list of dict
        Curve descriptors consumed by
        :func:`~triqs.plot.mpl_interface.oplot`.
    """
    return plot_base.plot_base( self, opt_dict,  r'$\omega_n$',
            lambda x : r'%s$(i\omega_n)$'%x, [x.imag for x in list(self.mesh.values())] )

#----------------------------------------------------------------

def imtime(self, opt_dict):
    r"""Plot protocol for :class:`~triqs.gfs.gf.Gf` objects on a 
    :class:`~triqs.mesh.meshes.MeshImTime`.

    Parameters
    ----------
    self : Gf
        Green's function on an imaginary-time mesh.
    opt_dict : dict
        Plot options. See :func:`~triqs.gfs.plot.one_var.imfreq` for 
        recognised keys (``mode`` accepts ``'R'`` or ``'I'`` here).

    Returns
    -------
    list of dict
        Curve descriptors for :func:`~triqs.plot.mpl_interface.oplot`.
    """
    return plot_base.plot_base( self, opt_dict,  r'$\tau$', lambda x : r'%s$(\tau)$'%x, list(self.mesh.values()))

#----------------------------------------------------------------

def dlr_imfreq(self, opt_dict):
    r"""Plot protocol for :class:`~triqs.gfs.gf.Gf` objects on a 
    :class:`~triqs.mesh.meshes.MeshDLRImFreq`.

    Sets ``linestyle='None'`` and ``marker='x'`` by default to render
    the DLR samples as discrete points.

    Parameters
    ----------
    self : Gf
        Green's function on a DLR Matsubara mesh.
    opt_dict : dict
        Plot options; see :func:`~triqs.gfs.plot.one_var.imfreq`.

    Returns
    -------
    list of dict
        Curve descriptors for :func:`~triqs.plot.mpl_interface.oplot`.
    """
    opt_dict['linestyle'] = 'None'
    if not 'marker' in opt_dict:
        opt_dict['marker'] = 'x'
    return plot_base.plot_base( self, opt_dict,  r'$\omega_n$',
            lambda x : r'%s$(i\omega_n)$'%x, [x.imag for x in list(self.mesh.values())] )

#----------------------------------------------------------------

def dlr_imtime(self, opt_dict):
    r"""Plot protocol for :class:`~triqs.gfs.gf.Gf` objects on a 
    :class:`~triqs.mesh.meshes.MeshDLRImTime`.

    Sets ``linestyle='None'`` and ``marker='x'`` by default to render
    the DLR samples as discrete points.

    Parameters
    ----------
    self : Gf
        Green's function on a DLR imaginary-time mesh.
    opt_dict : dict
        Plot options; see :func:`~triqs.gfs.plot.one_var.imfreq`.

    Returns
    -------
    list of dict
        Curve descriptors for :func:`~triqs.plot.mpl_interface.oplot`.
    """
    opt_dict['linestyle'] = 'None'
    if not 'marker' in opt_dict:
        opt_dict['marker'] = 'x'
    return plot_base.plot_base( self, opt_dict,  r'$\tau$', lambda x : r'%s$(\tau)$'%x, list(self.mesh.values()))

#----------------------------------------------------------------

def legendre(self, opt_dict):
    r"""Plot protocol for :class:`~triqs.gfs.gf.Gf` objects on a 
    :class:`~triqs.mesh.meshes.MeshLegendre`.

    Parameters
    ----------
    self : Gf
        Green's function expanded on Legendre polynomials.
    opt_dict : dict
        Plot options; see :func:`~triqs.gfs.plot.one_var.imfreq`.

    Returns
    -------
    list of dict
        Curve descriptors for :func:`~triqs.plot.mpl_interface.oplot`.
    """
    return plot_base.plot_base( self, opt_dict,  r'$l_n$', lambda x : r'%s$(l_n)$'%x, list(range(len(self.mesh))))

#----------------------------------------------------------------

def refreq(self, opt_dict):
    r"""Plot protocol for :class:`~triqs.gfs.gf.Gf` objects on a 
    :class:`~triqs.mesh.meshes.MeshReFreq`.

    Supports an additional ``mode='S'`` (spectral function,
    :math:`-\mathrm{Im}\,G / \pi`) on top of the usual modes.

    Parameters
    ----------
    self : Gf
        Green's function on a real-frequency mesh.
    opt_dict : dict
        Plot options. ``mode`` may be ``'R'``, ``'I'`` or ``'S'``;
        see :func:`~triqs.gfs.plot.one_var.imfreq` for the rest.

    Returns
    -------
    list of dict
        Curve descriptors for :func:`~triqs.plot.mpl_interface.oplot`.
    """
    return plot_base.plot_base(self, opt_dict,  r'$\omega$', lambda x : r'%s$(\omega)$'%x, list(self.mesh.values()), allow_spectral_mode = True)

#----------------------------------------------------------------

def retime (self, opt_dict):
    r"""Plot protocol for :class:`~triqs.gfs.gf.Gf` objects on a 
    :class:`~triqs.mesh.meshes.MeshReTime`.

    Parameters
    ----------
    self : Gf
        Green's function on a real-time mesh.
    opt_dict : dict
        Plot options; see :func:`~triqs.gfs.plot.one_var.imfreq`.

    Returns
    -------
    list of dict
        Curve descriptors for :func:`~triqs.plot.mpl_interface.oplot`.
    """
    return plot_base.plot_base(self, opt_dict,  r'$\t$', lambda x : r'%s$(\t)$'%x, list(self.mesh.values()))

