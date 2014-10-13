################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011-2012 by M. Ferrero, O. Parcollet
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

import numpy
import lazy_expressions, descriptor_base
#from gf import MeshImFreq
from types import IntType, SliceType, StringType
from tools import LazyCTX #, IndicesConverter, get_indices_in_dict, py_deserialize
from _gf_plot import PlotWrapperPartialReduce
#from gf import TailGf

#---------------------   [  ] operator        ------------------------------------------

def __getitem__(self, key):
    """Key is a tuple of index (n1, n2) as defined at construction"""
    if len(key) !=2: raise KeyError, "[ ] must be given two arguments"
    sl1, sl2 = key
    if type(sl1) == StringType and type(sl2) == StringType:
        # Convert the indices to integer
        indices_converter = [ IndicesConverter(self.indicesL), IndicesConverter(self.indicesR)]
        sl1, sl2 =  [ indices_converter[i].convertToNumpyIndex(k) for i, k in enumerate(key) ]
    if type (sl1) != slice: sl1 = slice (sl1, sl1+1)
    if type (sl2) != slice: sl2 = slice (sl2, sl2+1)
    return self.__class__(indicesL = list(self.indicesL)[sl1],
                          indicesR = list(self.indicesR)[sl2],
                          name = self.name,
                          mesh = self.mesh,
                          data = self.data[:,sl1,sl2],
                          tail = self.tail._make_slice(sl1, sl2))

def __setitem__(self, key, val):
    g = self.__getitem__(key)
    g <<= val

#--------------   PLOT   ---------------------------------------

def _real_plot(self):
    """Use self.real in a plot to plot only the real part"""
    return PlotWrapperPartialReduce(self, RI='R')

def _imag_plot(self):
    """Use self.imag in a plot to plot only the imag part"""
    return PlotWrapperPartialReduce(self, RI='I')

#--------  LAZY expression system -----------------------------------------

def add_precall (self, y):
    if descriptor_base.is_lazy(y): return lazy_expressions.make_lazy(self) + y

def sub_precall (self, y):
    if descriptor_base.is_lazy(y): return lazy_expressions.make_lazy(self) - y

def mul_precall (self, y):
    if descriptor_base.is_lazy(y): return lazy_expressions.make_lazy(self) * y

def div_precall (self, y):
    if descriptor_base.is_lazy(y): return lazy_expressions.make_lazy(self) / y

def _lshift_(self, A):
    return _ilshift_(self, A)
 
def _ilshift_(self, A):
    """ A can be two things:
      * G <<= any_init will init the GFBloc with the initializer
      * G <<= g2 where g2 is a GFBloc will copy g2 into self
    """
    import descriptors
    if isinstance(A, self.__class__):
        if self is not A: self.copy_from(A) # otherwise it is useless AND does not work !!
    elif isinstance(A, lazy_expressions.LazyExpr): # A is a lazy_expression made of GF, scalars, descriptors
        A2= descriptors.convert_scalar_to_const(A)
        def e_t (x):
            if not isinstance(x, descriptors.Base): return x
            tmp = self.copy()
            x(tmp)
            return tmp
        self.copy_from (lazy_expressions.eval_expr_with_context(e_t, A2) )
    elif isinstance(A, lazy_expressions.LazyExprTerminal): #e.g. g<<= SemiCircular (...)
        self <<= lazy_expressions.LazyExpr(A)
    elif descriptors.is_scalar(A): #in the case it is a scalar ....
        self <<= lazy_expressions.LazyExpr(A)
    else:
        raise RuntimeError, " <<= operator: RHS  not understood"
    return self

#---------------------------------------------------

def from_L_G_R(self, L, G, R):

    N1 = self.data.shape[1]
    N2 = self.data.shape[2]
    assert L.shape[0] == N1
    assert L.shape[1] == G.data.shape[1]
    assert R.shape[0] == G.data.shape[2]
    assert R.shape[1] == N2

    MatrixStack(self.data).matmul_L_R(L, G.data, R)

    # this might be a bit slow
    for o in range(G.tail.order_min, G.tail.order_max+1):
      self.tail[o] = numpy.dot(L, numpy.dot(G.tail[o], R))
      self.tail.mask.fill(G.tail.order_max)

#---------------------------------------------------

def invert(self):
    """Invert the matrix for all arguments"""
    MatrixStack(self.data).invert()
    self.tail.invert()

#---------------------------------------------------

# FIXME NEVER USED CLEAN UP
#def transpose(self):
#    """Transposes the GF Bloc: return a new transposed view"""
#    ### WARNING: this depends on the C++ layering ....
#    return self.__class__(
#            indices = list(self.indices),
#            mesh = self.mesh,
#            data = self.data.transpose( (0, 2, 1) ),
#            tail = self.tail.transpose(),
#            name = self.name+'(t)')

#---------------------------------------------------

def conjugate(self):
    """Complex conjugate of the GF Bloc. It follow the policy of numpy and
    make a copy only if the Green function is complex valued"""

    return self.__class__(
            indices = list(self.indices),
            mesh = self.mesh,
            data = self.data.conjugate(),
            tail = self.tail.conjugate(),
            name = self.name+'*')

#------------------  Density -----------------------------------

def total_density(self):
    """Trace density"""
    return numpy.trace(self.density())

