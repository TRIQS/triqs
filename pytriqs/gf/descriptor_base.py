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

class LazyCTX:
    def __init__ (self, G): 
        self.G = G
    def _is_compatible_for_ops(self, g): 
        m1,m2  = self.G.mesh, g.mesh
        return m1 is m2 or m1 == m2
    def __eq__ (self, y):
        return isinstance(y, self.__class__) and self._is_compatible_for_ops(y.G)
    def __call__ (self, x): 
        if not isinstance(x, descriptor_base.Base): return x
        tmp = self.G.copy()
        x(tmp)
        return tmp


def is_lazy(y):
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

# The Base for any descriptor taking a BlockGf
class BaseBlock(Base):
    def __init__ (self, G, *args, **kw):
        Base.__init__(self, G = G)
        if self.is_block_descriptor():
            self.descriptor_iter = (self.__class__(g, *args, **kw) for i,g in G)

    def is_block_descriptor(self):
        return self.G.__class__.__name__ == 'BlockGf'

    def __iter__ (self):
        return self.descriptor_iter
                
#########################################################################

class Function (Base):
    r"""   
       Stores a python function 
       
       If the Green's function is defined on an array of points :math:`x_i`, then it will be initialized to :math:`F(x_i)`.
    """
    def __init__ (self, function): 
        r"""
        :param function: the function :math:`\omega \rightarrow function(\omega)`
        """
        Base.__init__(self, function=function)
        
    def __call__(self,G):
        if not(callable(self.function)): raise RuntimeError, "GFInitializer.Function: f must be callable"
        res = G.data[...]
        try:
            for n,om in enumerate(G.mesh): res[n,...] = self.function(om.value)
        except:
            print "The given function has a problem..."
            raise
        return G

#########################################################################

class Const(Base):
    def __init__ (self, C):
        Base.__init__(self, C=C)
         
    def __call__(self,G):
        C = self.C
        if G.mesh.__class__.__name__ not in ['MeshImFreq', 'MeshReFreq']:
            raise TypeError, "This initializer is only correct in frequency"

        if not isinstance(C,numpy.ndarray) and G.target_rank > 0: 
            assert G.target_shape[0]==G.target_shape[1], "Const only applies to square G"
            C = C*numpy.identity(G.target_shape[0]) 
        if G.target_rank > 0 and C.shape != (G.target_shape[0],G.target_shape[1]): raise RuntimeError, "Size of constant incorrect"
        
        Function(lambda om: C)(G)
        return G
    
#########################################################################

class Omega_(Base):
    r"""The function:math:`\omega \rightarrow \omega` """
    def __str__(self): return "Omega" 
    def __call__(self,G):
        if G.mesh.__class__.__name__ not in ['MeshImFreq', 'MeshReFreq']:
            raise TypeError, "This initializer is only correct in frequency"

        Id = 1. if G.target_rank == 0 else numpy.identity(G.target_shape[0])
        
        for n,om in enumerate(G.mesh): G.data[n,...] = om*Id
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

        if G.target_rank > 0:
            if not isinstance(A,numpy.ndarray): A = A*numpy.identity(G.target_shape[0]) 
            if not isinstance(B,numpy.ndarray): B = B*numpy.identity(G.target_shape[0]) 
            if A.shape != G.target_shape: raise RuntimeError, "Size of A incorrect"
            if B.shape != G.target_shape: raise RuntimeError, "Size of B incorrect"
        
        Function(lambda om: A*om + B, None)(G)

        if self.Invert: G.invert()
        return G


