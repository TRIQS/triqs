# Copyright (c) 2013-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2020 Simons Foundation
# Copyright (c) 2014 Igor Krivenko
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
# Authors: DerWeh, Michel Ferrero, Igor Krivenko, Mi-chael, Olivier Parcollet, Nils Wentzell


r"""Concrete Green's function descriptors used with the ``<<`` operator.

This module provides the ready-made descriptors that fill a Green's
function with a closed-form expression:

* :class:`~triqs.gfs.descriptors.SemiCircular` — Hilbert transform of a 
  semicircular DOS.
* :class:`~triqs.gfs.descriptors.Flat` (alias :class:`~triqs.gfs.descriptors.Wilson`) 
  — Hilbert transform of a flat DOS with cutoff.
* :class:`~triqs.gfs.descriptors.Fourier` — lazy Fourier transform between conjugate 
  meshes.
* :class:`~triqs.gfs.descriptors.LegendreToMatsubara`, 
  :class:`~triqs.gfs.descriptors.MatsubaraToLegendre` — lazy basis transforms.
* :class:`~triqs.gfs.descriptors.OneFermionInTime` — a one-fermion imaginary-time 
  propagator.

The frequency placeholders :data:`~triqs.gfs.descriptor_base.Omega` and 
:data:`~triqs.gfs.descriptor_base.iOmega_n` and the 
:class:`~triqs.gfs.descriptor_base.Function` / :class:`~triqs.gfs.descriptor_base.Const` 
descriptors are re-exported from :mod:`triqs.gfs.descriptor_base`.

**See Also:** :meth:`~triqs.gfs.gf.Gf.__lshift__` -- Operator that applies a descriptor.
"""

from .descriptor_base import *
from triqs.mesh import MeshImFreq, MeshDLRImFreq, MeshReFreq, MeshReFreqPts, MeshReFreqLog, MeshImTime, MeshDLRImTime
from .semicirc import g_semicirc_z, g_semicirc_w, g_semicirc_tau
import warnings

#######################################

class OneFermionInTime(Base):
    r"""Single-fermion propagator on an imaginary-time mesh.

    Fills the target with the imaginary-time Green's function of a
    single fermionic level at energy :math:`L`:

    .. math::

        G(\tau) = -\frac{e^{-L\tau}}{1 + e^{-L\beta}}.

    Parameters
    ----------
    l : float, optional
        Level energy :math:`L`. Default ``0``.
    """
    def __init__ (self, l = 0):
         Base.__init__(self, L=l)

    def __call__(self,G):
        """Fill ``G.data`` with the single-fermion imaginary-time propagator.

        Parameters
        ----------
        G : Gf
            Target Green's function on an imaginary-time mesh
            (modified in place).

        Returns
        -------
        Gf
            ``G``.

        Raises
        ------
        TypeError
            If ``G.mesh`` is not an imaginary-time mesh.
        """
        L = self.L
        if G.mesh.TypeGF not in [GF_Type.Imaginary_Time]:
            raise TypeError("This initializer is only correct in frequency")

        Id = 1. if len(G.target_shape) == 0 else numpy.identity(G.target_shape[0])

        fact = -1/(1+exp(-L*G.beta))
        Function(lambda t: fact* exp(-L*t) *Id)(G)
        return G

##################################################

def _SemiCircularDOS(half_bandwidth):
    r"""Return :math:`\omega \mapsto \rho(\omega)` for a semicircular DOS 
    with the given half-bandwidth.

    See also :mod:`triqs.gfs.semicirc`.
    
    Parameters
    ----------
    half_bandwidth : float
        :math:`D`.

    Returns
    -------
    callable
        ``rho(omega)`` evaluating the semicircular density of states.
    """
    from math import sqrt,pi
    larg = half_bandwidth
    def semi(x):
        if (abs(x)<larg): return sqrt( 1 - (x/larg)**2 )*2/pi/larg
        else: return 0.0
    return semi

def semi(x):
    r"""Return :math:`\omega \mapsto \rho(\omega)` for a semicircular DOS 
    with the given half-bandwidth.

    See also :mod:`triqs.gfs.semicirc`.
    
    Parameters
    ----------
    x : float
        Half-bandwidth.

    Returns
    -------
    callable
        ``rho(omega)`` evaluating the semicircular density of states.
    """
    return _SemiCircularDOS(x)

##################################################

class SemiCircular (Base):
    r"""Hilbert transform of a semicircular density of states.

    .. math::

        g(z) = \int \frac{A(\omega)}{z-\omega}\, d\omega,
        \qquad
        A(\omega) = \theta(D - |\omega|)\,
            \frac{2 \sqrt{D^2 - \omega^2}}{\pi D^2}.

    Supports Matsubara, real-frequency and imaginary-time meshes.

    Parameters
    ----------
    half_bandwidth : float
        Half-bandwidth :math:`D`.
    chem_potential : float, optional
        Chemical potential :math:`\mu`, equal to minus the centre of the
        semicircle. Default ``0``.

    Examples
    --------
    >>> from triqs.gfs import SemiCircular
    >>> g << SemiCircular(half_bandwidth=1.0)
    """
    def __init__ (self, half_bandwidth, chem_potential=0.):
        Base.__init__(self, half_bandwidth=half_bandwidth, chem_potential=chem_potential)

    def __str__(self):
        """Return ``"SemiCircular(D, mu)"``.

        Returns
        -------
        str
        """
        return "SemiCircular(%s, %s)"%self.half_bandwidth, chem_potential

    def __call__(self,G):
        """Fill ``G.data`` with the semicircular Hilbert transform on its mesh.

        Parameters
        ----------
        G : Gf
            Target Green's function (modified in place).

        Returns
        -------
        Gf
            ``G``.

        Raises
        ------
        TypeError
            If ``G.mesh`` is not a supported mesh type.
        NotImplementedError
            For imaginary-time meshes with a non-zero chemical
            potential.
        """
        D = self.half_bandwidth
        mu = self.chem_potential
        Id = 1. if len(G.target_shape) == 0 else numpy.identity(G.target_shape[0])
        if type(G.mesh) in [MeshImFreq, MeshDLRImFreq]:
            vals = g_semicirc_z(G.mesh.values() + mu, D)
        elif type(G.mesh) in [MeshReFreq, MeshReFreqPts, MeshReFreqLog]:
            vals = g_semicirc_w(G.mesh.values().real + mu, D)
        elif type(G.mesh) in [MeshImTime, MeshDLRImTime]:
            if mu != 0.:
                raise NotImplementedError("SemiCircular on imaginary-time mesh with non-zero chemical potential is not supported")
            vals = g_semicirc_tau(G.mesh.values(), G.mesh.beta, D)
        else:
            raise TypeError(f"SemiCircular: mesh type not supported: {type(G.mesh)}")
        G.data[:] = numpy.multiply.outer(vals, Id)
        return G

##################################################

class Flat (Base):
    r"""Hilbert transform of a flat density of states with cutoff.

    .. math::

        g(z) = \int \frac{A(\omega)}{z-\omega}\, d\omega,
        \qquad
        A(\omega) = \frac{\theta(D^2 - \omega^2)}{2D}.

    Supports Matsubara and real-frequency meshes.

    Parameters
    ----------
    half_bandwidth : float
        Half-bandwidth :math:`D`.

    Examples
    --------
    >>> from triqs.gfs import Flat
    >>> g << Flat(half_bandwidth=2.0)
    """
    def __init__ (self, half_bandwidth):
        Base.__init__(self, half_bandwidth=half_bandwidth)

    def __str__(self):
        """Return ``"Flat(D)"``.

        Returns
        -------
        str
        """
        return "Flat(%s)"%self.half_bandwidth

    def __call__(self,G):
        """Fill ``G.data`` with the flat-DOS Hilbert transform on its mesh.

        Parameters
        ----------
        G : Gf
            Target Green's function on a frequency mesh (modified in
            place).

        Returns
        -------
        Gf
            ``G``.

        Raises
        ------
        TypeError
            If ``G.mesh`` is not a frequency mesh.
        """

        D = self.half_bandwidth
        Id = 1. if len(G.target_shape) == 0 else numpy.identity(G.target_shape[0], numpy.complex128)

        if type(G.mesh) in [MeshImFreq, MeshDLRImFreq]:
            f = lambda om: (-1/(2.0*D)) * numpy.log(numpy.divide(om-D,om+D)) * Id
        elif type(G.mesh) in [MeshReFreq, MeshReFreqPts, MeshReFreqLog]:
            def f(om):
              if (om.real > -D) and (om.real < D):
                return -numpy.log(numpy.divide(abs(om-D),abs(om+D)))*Id/(2*D) - 1j*pi*Id/(2*D)
              else:
                return -numpy.log(numpy.divide(abs(om-D),abs(om+D)))*Id/(2*D)
        else:
            raise TypeError("This initializer is only correct in frequency")

        # Silence "RuntimeWarning: divide by zero encountered in divide"
        old_err = numpy.seterr(divide='ignore')

        Function(f)(G)
        numpy.seterr(**old_err)
        return G

#backward compatibility
Wilson = Flat

##################################################

class Fourier(BaseBlock):
    r"""Lazy Fourier transform between conjugate meshes.

    Applying ``g_new << Fourier(g_old)`` runs the C++ Fourier transform
    from ``g_old``'s mesh to ``g_new``'s mesh. The transform direction
    (time → frequency or vice versa) is inferred from the mesh pair.

    Parameters
    ----------
    G : Gf or BlockGf
        Source Green's function.
    *args, **kw
        Forwarded to :func:`~triqs.gfs.gf_fnt.set_from_fourier` (e.g. tail moments for
        Matsubara → imaginary-time transforms).

    Attributes
    ----------
    G : Gf or BlockGf
        Captured source Green's function (inherited from
        :class:`~triqs.gfs.descriptor_base.BaseBlock`).
    args : tuple
        Positional arguments forwarded to :func:`~triqs.gfs.gf_fnt.set_from_fourier` at
        evaluation time.
    kw : dict
        Keyword arguments forwarded to :func:`~triqs.gfs.gf_fnt.set_from_fourier` at
        evaluation time.

    Examples
    --------
    >>> from triqs.gfs import Fourier, Gf
    >>> g_tau << Fourier(g_iw)
    """
    def __init__ (self, G, *args, **kw):
        BaseBlock.__init__(self, G, *args, **kw)
        self.args, self.kw = args, kw

    def __str__(self):
        """Return ``"Fourier of gf"``.

        Returns
        -------
        str
        """
        return "Fourier of gf"

    def __call__(self,G2):
        """Run the Fourier transform from ``self.G`` into ``G2``.

        Parameters
        ----------
        G2 : Gf
            Target Green's function on the conjugate mesh (modified in
            place).

        Returns
        -------
        Gf
            ``G2``.
        """
        G2.set_from_fourier(self.G, *self.args, **self.kw)
        return G2

class LegendreToMatsubara (BaseBlock):
    r"""Lazy Legendre → Matsubara basis transform.

    Parameters
    ----------
    G : Gf or BlockGf
        Source Green's function on a Legendre mesh.

    Examples
    --------
    >>> g_iw << LegendreToMatsubara(g_l)
    """
    def __init__ (self, G):
        BaseBlock.__init__(self, G)

    def __str__(self):
        """Return ``"LegendreToMatsubara of gf"``.

        Returns
        -------
        str
        """
        return "LegendreToMatsubara of gf"

    def __call__(self,G2):
        """Run the Legendre → Matsubara transform from ``self.G`` into ``G2``.

        Parameters
        ----------
        G2 : Gf
            Target Green's function on a Matsubara mesh (modified in
            place).

        Returns
        -------
        Gf
            ``G2``.
        """
        G2.set_from_legendre(self.G)
        return G2

class MatsubaraToLegendre (BaseBlock):
    r"""Lazy Matsubara → Legendre basis transform.

    Parameters
    ----------
    G : Gf or BlockGf
        Source Green's function on a Matsubara mesh.

    Examples
    --------
    >>> g_l << MatsubaraToLegendre(g_iw)
    """
    def __init__ (self, G):
        BaseBlock.__init__(self, G)

    def __str__(self):
        """Return ``"MatsubaraToLegendre of gf"``.

        Returns
        -------
        str
        """
        return "MatsubaraToLegendre of gf"

    def __call__(self,G2):
        """Run the Matsubara → Legendre transform from ``self.G`` into ``G2``.

        Parameters
        ----------
        G2 : Gf
            Target Green's function on a Legendre mesh (modified in
            place).

        Returns
        -------
        Gf
            ``G2``.
        """
        G2.set_from_imfreq(self.G)
        return G2

