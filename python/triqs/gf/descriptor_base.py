# Copyright (c) 2014-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2014-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2020 Simons Foundation
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


r""" """

import numpy
from math import *
from .lazy_expressions import LazyExprTerminal, LazyExpr, transform

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
    return type(x) in [ type(1), type(1.0), type(1j), numpy.ndarray, int, numpy.int_, numpy.int8, numpy.int16, numpy.int32, float, numpy.float_, numpy.float32, numpy.float64, complex, numpy.complex_, numpy.complex64, numpy.complex128 ]

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
        if not(callable(self.function)): raise RuntimeError("GFInitializer.Function: f must be callable")
        res = G.data[...]
        try:
            for n,om in enumerate(G.mesh): res[n,...] = self.function(om.value)
        except:
            print("The given function has a problem...")
            raise
        return G

#########################################################################

class Const(Base):
    def __init__ (self, C):
        Base.__init__(self, C=C)
         
    def __call__(self,G):
        C = self.C
        if G.mesh.__class__.__name__ not in ['MeshImFreq', 'MeshReFreq', 'MeshDLRImFreq']:
            raise TypeError("This initializer is only correct in frequency")

        if not isinstance(C,numpy.ndarray) and G.target_rank > 0: 
            assert G.target_shape[0]==G.target_shape[1], "Const only applies to square G"
            C = C*numpy.identity(G.target_shape[0]) 
        if G.target_rank > 0 and C.shape != (G.target_shape[0],G.target_shape[1]): raise RuntimeError("Size of constant incorrect")
        
        Function(lambda om: C)(G)
        return G
    
#########################################################################

class Omega_(Base):
    r"""The function:math:`\omega \rightarrow \omega` """
    def __str__(self): return "Omega" 
    def __call__(self,G):
        if G.mesh.__class__.__name__ not in ['MeshImFreq', 'MeshReFreq', 'MeshDLRImFreq']:
            raise TypeError("This initializer is only correct in frequency")

        Id = 1. if G.target_rank == 0 else numpy.identity(G.target_shape[0])
        
        for n,om in enumerate(G.mesh): G.data[n,...] = om*Id
        return G

##########################################################################

Omega = Omega_()
iOmega_n = Omega_()

##########################################################################
