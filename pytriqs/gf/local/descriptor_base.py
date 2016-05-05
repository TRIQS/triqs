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

import numpy
from math import *
from lazy_expressions import LazyExprTerminal, LazyExpr, transform

def is_lazy(y):
    #return type(y) in [ Omega_, LazyExpr]
    return  isinstance(y,(Omega_, LazyExpr, LazyExprTerminal))

def is_scalar(x): 
    return type(x) in [ type(1), type(1.0), type(1j), numpy.ndarray, numpy.int, numpy.int_, numpy.int8, numpy.int16, numpy.int32, numpy.float, numpy.float_, numpy.float32, numpy.float64, numpy.complex, numpy.complex_, numpy.complex64, numpy.complex128 ]

def convert_scalar_to_const(expr): 

  # if the expression is a pure scalar, replace it by Const
  t = expr.get_terminal()
  if is_scalar(t): return LazyExpr( Const(t) )

  # otherwise: replace all scalar appearing in +/- operations by Const
  def act (tag, childs): 
        if tag in ["+", "-"]:
            for n,c in enumerate(childs): 
                t = c.get_terminal()
                if is_scalar(t): childs[n] =  Const (t)
        return (tag,childs)

  return transform(expr, act)
    
class Base (LazyExprTerminal):
    def __init__(self,**kargs):
        self.__dict__.update(kargs)
                
#########################################################################

class Function (Base):
    r"""   
       Stores a python function and a tail.
       
       If the Green's function is defined on an array of points :math:`x_i`, then it will be initialized to :math:`F(x_i)`.
    """
    def __init__ (self, function, tail=None): 
        r"""
        :param function: the function :math:`\omega \rightarrow function(\omega)`
        :param tail: The tail. Use None if you do not wish to use a tail (will be put to 0)
        """
        Base.__init__(self, function=function, tail=tail)
        
    def __call__(self,G):
        if not(callable(self.function)): raise RuntimeError, "GFInitializer.Function: f must be callable"
        res = G.data[:,:,:]
        try:
            for n,om in enumerate(G.mesh): res[n,:,:] = self.function(om)
        except:
            print "The given function has a problem..."
            raise
        if self.tail: G.tail.copy_from(self.tail)
        return G

#########################################################################

class Const(Base):
    def __init__ (self, C):
        Base.__init__(self, C=C)
         
    def __call__(self,G):
        C = self.C
        if G.mesh.__class__.__name__ not in ['MeshImFreq', 'MeshReFreq']:
            raise TypeError, "This initializer is only correct in frequency"

        if not isinstance(C,numpy.ndarray): 
            assert G.N1==G.N2, "Const only applies to square G"
            C = C*numpy.identity(G.N1) 
        if C.shape != (G.N1,G.N2): raise RuntimeError, "Size of constant incorrect"

        G.tail.reset(12)
        G.tail[0][:,:] = C
        
        Function(lambda om: C, None)(G)
        return G
    
#########################################################################

class Omega_(Base):
    r"""The function:math:`\omega \rightarrow \omega` """
    def __str__(self): return "Omega" 
    def __call__(self,G):
        if G.mesh.__class__.__name__ not in ['MeshImFreq', 'MeshReFreq']:
            raise TypeError, "This initializer is only correct in frequency"

        Id = numpy.identity(G.N1)
        G.tail.reset(10)
        G.tail[-1][:,:] = Id
        
        for n,om in enumerate(G.mesh): G.data[n,:,:] = om*Id
        return G

##########################################################################

Omega = Omega_()
iOmega_n = Omega_()

##########################################################################

class A_Omega_Plus_B(Base):
    "deprecated. do not use"
    def __init__ (self, A=1, B=0, Invert=False):
        Base.__init__(self, A=A, B=B,Invert=Invert)
         
    def __call__(self,G):
        A,B = self.A, self.B
        if G.mesh.__class__.__name__ not in ['MeshImFreq', 'MeshReFreq']:
            raise TypeError, "This initializer is only correct in frequency"

        if not isinstance(A,numpy.ndarray): A = A*numpy.identity(G.N1) 
        if not isinstance(B,numpy.ndarray): B = B*numpy.identity(G.N1) 
        if A.shape != (G.N1,G.N2): raise RuntimeError, "Size of A incorrect"
        if B.shape != (G.N1,G.N2): raise RuntimeError, "Size of B incorrect"

        G.tail.zero()
        G.tail[-1][:,:] = A
        G.tail[0][:,:] = B
        
        Function(lambda om: A*om + B, None)(G)

        if self.Invert: G.invert()
        return G


