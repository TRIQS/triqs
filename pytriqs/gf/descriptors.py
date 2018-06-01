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

r""" """

from descriptor_base import *
from meshes import MeshImFreq, MeshReFreq
                
#######################################

class OneFermionInTime(Base):
    def __init__ (self, l = 0):
         Base.__init__(self, L=l)
         
    def __call__(self,G):
        L = self.L
        if G.mesh.TypeGF not in [GF_Type.Imaginary_Time]: 
            raise TypeError, "This initializer is only correct in frequency"

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
      
    (Only works in combination with frequency Green's functions.)
    """
    def __init__ (self, half_bandwidth, chem_potential=0.):
        """:param half_bandwidth: :math:`D`, the half bandwidth of the
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
        Id = complex(1,0) if len(G.target_shape) == 0 else numpy.identity(G.target_shape[0],numpy.complex_)
        from cmath import sqrt
        if type(G.mesh) == MeshImFreq:
            def f(om_):
                om = om_ + mu
                return (om - 1j*copysign(1,om.imag)*sqrt(D*D - om**2))/D/D*2*Id
        elif type(G.mesh) == MeshReFreq:
            def f(om_):
              om = om_.real + mu
              if (om > -D) and (om < D):
                return (2.0/D**2) * (om - 1j* sqrt(D**2 - om**2))
              else:
                return (2.0/D**2) * (om - copysign(1,om) * sqrt(om**2 - D**2))
        else:
            raise TypeError, "This initializer is only correct in frequency"

        Id = 1. if len(G.target_shape) == 0 else numpy.identity(G.target_shape[0])
        Function(f)(G)

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
        Id = 1. if len(G.target_shape) == 0 else numpy.identity(G.target_shape[0], numpy.complex_)

        if type(G.mesh) == MeshImFreq:
            f = lambda om: (-1/(2.0*D)) * numpy.log(numpy.divide(om-D,om+D)) * Id
        elif type(G.mesh) == MeshReFreq:
            def f(om):
              if (om.real > -D) and (om.real < D):
                return -numpy.log(numpy.divide(abs(om-D),abs(om+D)))*Id/(2*D) - 1j*pi*Id/(2*D)
              else:
                return -numpy.log(numpy.divide(abs(om-D),abs(om+D)))*Id/(2*D)
        else:
            raise TypeError, "This initializer is only correct in frequency"

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

class InverseFourier (BaseBlock):
    r"""
    The Inverse Fourier transform as a lazy expression
    """
    def __init__ (self, G, *args, **kw):
        """:param G: :math:`G`, the function to be transformed. Must in the frequency domain"""
        BaseBlock.__init__(self, G, *args, **kw)
        self.args, self.kw = args, kw

    def __str__(self): return "InverseFourier of gf"

    def __call__(self,G2):
        G2.set_from_inverse_fourier(self.G, *self.args, **self.kw)
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

