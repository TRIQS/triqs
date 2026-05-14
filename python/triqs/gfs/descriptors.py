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


r""" """

from .descriptor_base import *
from triqs.mesh import MeshImFreq, MeshDLRImFreq, MeshReFreq, MeshReFreqPts, MeshReFreqLog, MeshImTime, MeshDLRImTime
from .semicirc import g_semicirc_z, g_semicirc_w, g_semicirc_tau
import warnings

#######################################

class OneFermionInTime(Base):
    def __init__ (self, l = 0):
         Base.__init__(self, L=l)

    def __call__(self,G):
        L = self.L
        if G.mesh.TypeGF not in [GF_Type.Imaginary_Time]:
            raise TypeError("This initializer is only correct in frequency")

        Id = 1. if len(G.target_shape) == 0 else numpy.identity(G.target_shape[0])

        fact = -1/(1+exp(-L*G.beta))
        Function(lambda t: fact* exp(-L*t) *Id)(G)
        return G

##################################################

def _SemiCircularDOS(half_bandwidth):
    """
       Semi_Circular DOS function
       Input: the 1/2 bandwidth
       Returns: a function omega-> dos(omega)
    """
    from math import sqrt,pi
    larg = half_bandwidth
    def semi(x):
        if (abs(x)<larg): return sqrt( 1 - (x/larg)**2 )*2/pi/larg
        else: return 0.0
    return semi

def semi(x):
    return _SemiCircularDOS(x)

##################################################

class SemiCircular (Base):
    r"""Hilbert transform of a semicircular density of states, i.e.

     .. math::
        g(z) = \int \frac{A(\omega)}{z-\omega} d\omega

    where :math:`A(\omega) = \theta( D - |\omega|) 2 \sqrt{ D^2 - \omega^2}/(\pi D^2)`.

    (Works with frequency and imaginary-time Green's functions.)
    """
    def __init__ (self, half_bandwidth, chem_potential=0.):
        r""":param half_bandwidth: :math:`D`, the half bandwidth of the
semicircular density of states
        :param chem_potential: :math:`\mu`, the chemical potential of the    |
semicircular density of states, corresponds to minus the center of the
semicircle
"""
        Base.__init__(self, half_bandwidth=half_bandwidth, chem_potential=chem_potential)

    def __str__(self): return "SemiCircular(%s, %s)"%self.half_bandwidth, chem_potential

    def __call__(self,G):
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
    r"""The Hilbert transform of a flat density of states, with cut-off

    .. math::
        g(z) = \int \frac{A(\omega)}{z-\omega} d\omega

    where :math:`A(\omega) = \theta( D^2 - \omega^2)/(2D)`.

    (Only works in combination with frequency Green's functions.)
    """
    def __init__ (self, half_bandwidth):
        """:param half_bandwidth: :math:`D`, the half bandwidth """
        Base.__init__(self, half_bandwidth=half_bandwidth)

    def __str__(self): return "Flat(%s)"%self.half_bandwidth

    def __call__(self,G):

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
    r"""
    The Fourier transform as a lazy expression
    """
    def __init__ (self, G, *args, **kw):
        """:param G: :math:`G`, the function to be transformed. Must in the time domain"""
        BaseBlock.__init__(self, G, *args, **kw)
        self.args, self.kw = args, kw
    def __str__(self): return "Fourier of gf"

    def __call__(self,G2):
        G2.set_from_fourier(self.G, *self.args, **self.kw)
        return G2

class LegendreToMatsubara (BaseBlock):
    r"""
    The transformation from Legendre to Matsubara as a lazy expression
    """
    def __init__ (self, G):
        """:param G: :math:`G`, the function to be transformed. Must in the Legendre domain"""
        BaseBlock.__init__(self, G)

    def __str__(self): return "LegendreToMatsubara of gf"

    def __call__(self,G2):
        G2.set_from_legendre(self.G)
        return G2

class MatsubaraToLegendre (BaseBlock):
    r"""
    The transformation from Legendre to Matsubara as a lazy expression
    """
    def __init__ (self, G):
        """:param G: :math:`G`, the function to be transformed. Must in the Matsubara domain"""
        BaseBlock.__init__(self, G)

    def __str__(self): return "MatsubaraToLegendre of gf"

    def __call__(self,G2):
        G2.set_from_imfreq(self.G)
        return G2

