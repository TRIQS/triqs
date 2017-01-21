################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2017 by M. Ferrero, O. Parcollet
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
import itertools, warnings
from functools import reduce # Valid in Python 2.6+, required in Python 3
import operator
import numpy as np
import mesh_product
import lazy_expressions
from descriptor_base import *
from types import IntType, SliceType, StringType
import gf_fnt, wrapped_aux
from mesh_product import MeshProduct
import mesh as meshes
import singularities
import plot 

all_meshes = tuple(c for c in meshes.__dict__.values() if isinstance(c, type) and c.__name__.startswith('Mesh'))
all_call_proxies = dict( (c.__name__, c) for c in wrapped_aux.__dict__.values() if isinstance(c, type) and c.__name__.startswith('CallProxy'))

class CallProxyNone :
    def __init__(self, a):
        pass
    def __call__(self, a): 
        raise NotImplemented

def call_factory_from_dict(cl,name, dic):
    """Given a class cl and a dict dic, it calls cl.__factory_from_dict__(dic)"""
    return cl.__factory_from_dict__(name, dic)

import descriptor_base
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

# a metaclass that adds all functions of gf_fnt as methods 
# the C++ will take care of the dispatch
def add_method_helper(a,cls): 
    def _(self, *args, **kw):
       return a(self, *args, **kw)
#         try : 
            # return a(*args, **kw)
        # except TypeError:
            # print "No dispatch available for %s(%s, ...)"%(a.__name__, cls.__name__)
            # raise
    _.__doc__ = 50*'-' + '\n' + a.__doc__
    _.__name__ = a.__name__
    return _

class AddMethod(type): 
    def __init__(cls, name, bases, dct):
        super(AddMethod, cls).__init__(name, bases, dct)
        for a in [f for f in gf_fnt.__dict__.values() if callable(f)]:
            setattr(cls, a.__name__, add_method_helper(a,cls)) 

#Debug warning
#import traceback
#def warn_with_traceback(message, category, filename, lineno, file=None, line=None):
#    traceback.print_stack()
#    print warnings.formatwarning(message, category, filename, lineno, line)
#warnings.showwarning = warn_with_traceback

class Gf(object):
    __metaclass__ = AddMethod
    """
    Green function class

    Parameters
    ----------
    mesh: MeshXXX
          One of the meshes of the module 'meshes'
          The mesh of the Green function

    data: numpy.array, optional
          The data of the Gf.
          Must be of dimension mesh.rank + target_rank.
          Incompatible with target_shape

    target_shape: list of int, optional
                  Shape of the target space.
                  Incompatible with data

    is_real: bool
             Is the Green function real valued ?
             If true, and target_shape is set, the data will be real.
             No effect with the parameter data.

    singularity: TailGfXXX, optional
                 One of the singularity of the module 'singularities'
                 If not present singularity is None.

    indices: GfIndices or list of str or list of list of str, Optional
             Optional string indices for the target space, to allow e.g g['eg', 'eg']
             list of list of str: the list of indices for each dimension.
             list of str: all indices are assumed to be the same for all dimensions.
   
    name: str
          The name of the Green function. For plotting.

    NB : One of target_shape, data and indices must be set, and the other must be None.
    """
    def __init__(self, **kw): # enforce keyword only policy 
         
        def delegate(self, mesh, data=None, target_shape=None, singularity = None, indices = None, name = '', is_real = False, _singularity_maker = None):
            """
            target_shape and data  : must provide exactly one of them
            """
            # input check
            assert (target_shape is None) or (data is None), "data and target_shape : one must be None"
                
            # mesh
            assert isinstance(mesh, all_meshes), "Unknown mesh !"
            self._mesh = mesh

            # FIXME ??
            # if indices is not a list of list, but a list, then the target_rank is assumed to be 2 !
            # backward compatibility only, it is not very logical (what about vector_valued gf ???)
            assert isinstance(indices, (type(None), list,  singularities.GfIndices)) 
            if isinstance(indices, list):
                if not isinstance(indices[0], list): indices = [indices, indices]
                # indices : transform indices into string
                indices = [ [str(x) for x in v] for v in indices]
                indices = singularities.GfIndices(indices)
            self._indices = indices 

            #data
            if data is None:
                if target_shape is None : 
                    assert indices, "Without data, target_shape, I need the indices to compute the shape !"
                    target_shape = [ len(x) for x in indices.data]
                data = np.zeros([len(mesh)]  + list(target_shape), dtype = np.float64 if is_real else np.complex128)
            self._data = data
            self._target_rank = len(self._data.shape) - (self._mesh.rank if isinstance(mesh, MeshProduct) else 1) 
            self._rank = len(self._data.shape) - self._target_rank
            assert self._rank >= 0
            
            #singularity
            assert (singularity is None) ^ (_singularity_maker is None), "Internal error"
            self._singularity = singularity or (_singularity_maker(self) if _singularity_maker else None)
            self.name = name

            # compute the hdf5 tag 
            ## MUST correspond to C++. [4:] removes the 'Mesh'. Assumes that meshes are called
            ## MeshXXX where XXX is the hdf5 name
            ext = '' if self._target_rank == 2 else ('_s' if self._target_rank == 0 else 'Tv%s'%self._target_rank)
            s = '_x_'.join( m.__class__.__name__[4:] for m in self.mesh._mlist) if isinstance(mesh, MeshProduct) else self._mesh.__class__.__name__[4:]
            self._hdf5_data_scheme_mesh_ =  s
            self._hdf5_data_scheme_ = 'Gf' + s + ext
       
            # C proxy for call operator for speed ...
            proxyname = 'CallProxy%s_%s%s'%(s, self.target_rank,'_R' if data.dtype == np.float64 else '')
            self.c_proxy = all_call_proxies.get(proxyname, CallProxyNone)(self)
            
        delegate(self, **kw)

    @property
    def rank(self): 
        return self._rank

    @property
    def target_rank(self): 
        return self._target_rank

    @property
    def target_shape(self): 
        return self._data.shape[self._rank:]

    @property
    def beta(self) : 
        return self.mesh.beta
    
    @property
    def statistic(self) : 
        return self.mesh.statistic
    
    @property
    def mesh(self): 
        return self._mesh

    @property
    def data(self):
        return self._data

    @property
    def singularity(self) : 
        return self._singularity

    @property
    def tail(self) : 
        return self._singularity

    @property
    def indices(self):
        return self._indices

    def copy(self) : 
        return self.__class__(mesh = self._mesh.copy(), data = self._data.copy(), 
                singularity = self._singularity.copy() if self._singularity else None,
                indices = self._indices.copy(), name = self.name)

    def copy_from(self, another):
        self._mesh.copy_from(another.mesh)
        self._data[:] = another._data[:]
        if self._singularity : self._singularity.copy_from(another._singularity)
        self._indices = another._indices.copy()
        self.name = another.name

    def __repr__(self):
        return "Green Function %s with mesh %s and range %s: \n"%(self.name, self.mesh, self.rank)

    def __str__ (self): 
        return self.name if self.name else repr(self)

    #--------------  Bracket operator []  -------------------------
    
    def __getitem__(self, key):
        if type(key) == list : # This is a slice !
        
            assert self.rank > 1, "Slicing makes no sense for single variable gf"
            _ = slice(0, None) # all
            k = [m.index_to_linear(i) if type(i) != var_t else _ for i,m in itertools.izip(key, self._mesh._mlist)]
            k += self._target_rank * [_]
            dat = self._data[k]
            mlist = [m for i,m in itertools.ifilter(lambda i,m : type(i) == var_t, itertools.izip(key, self._mesh._mlist))]
            assert len(mlist) > 0, "Slice does not make sense if all variable are var_t"
            mesh = MeshProduct(*mlist) if len(mlist)>1 else mlist[0]
            sing = None # FIXME : slice the singularity, in one case
            return self.__class__(mesh = mesh, data = dat, singularity = sing)

        else : # This is a target slice
            assert self.target_rank == len(key), "wrong number of arguments. Expected %s, got %s"%(self.target_rank, len(key))
            # transform the key in a list of slices
            if all(isinstance(x, str) for x in key):
                assert  self._indices, "Got string indices, but I have no indices to convert them !"
                key_s = [self._indices.convert_index (s,i) for i,s in enumerate(key)] # convert returns a slice of len 1
                key = [x.start for x in key_s]
            if all(isinstance(x, slice) for x in key) : 
                key_s, key = list(key), [ x.start for x in key]
            else:
                key_s = map(lambda r: slice(r,r+1,1), key) # transform int into slice 
            # now the key is a list of slices
            dat = self._data[ self._rank * [slice(0,None)] + key_s ] 
            ind = singularities.GfIndices([ [v[k]]  for k,v in zip(key, self._indices.data)])
            sing = singularities.slice_target_sing(self._singularity, *key) if self._singularity else None
            return self.__class__(mesh = self._mesh, data = dat, singularity = sing, indices = ind, name = "")

    def __setitem__(self, key, val):
        self[key] << val

    # -------------- Various operations -------------------------------------
    
    @property
    def real(self) : 
        return self.__class__(mesh = self._mesh, data = self._data.real, singularity = None,name = "Re " + self.name) # Singularity is None for G(tau) ?

    @property
    def imag(self) : 
        return self.__class__(mesh = self._mesh, data = self._data.imag, singularity = None, name = "Im " + self.name) # Singularity is None for G(tau) ?

    # --------------  Lazy system -------------------------------------

    def __lazy_expr_eval_context__(self) : 
        return LazyCTX(self)

    def __lshift__(self, A):
        """ A can be two things:
          * G << any_init will init the GFBloc with the initializer
          * G << g2 where g2 is a GFBloc will copy g2 into self
        """
        import descriptors
        if isinstance(A, Gf):
            if self is not A: self.copy_from(A) # otherwise it is useless AND does not work !!
        elif isinstance(A, lazy_expressions.LazyExpr): # A is a lazy_expression made of GF, scalars, descriptors
            A2 = descriptors.convert_scalar_to_const(A)
            def e_t (x):
                if not isinstance(x, descriptors.Base): return x
                tmp = self.copy()
                x(tmp)
                return tmp
            self.copy_from (lazy_expressions.eval_expr_with_context(e_t, A2) )
        elif isinstance(A, lazy_expressions.LazyExprTerminal): #e.g. g<< SemiCircular (...)
            self << lazy_expressions.LazyExpr(A)
        elif descriptors.is_scalar(A): #in the case it is a scalar ....
            self << lazy_expressions.LazyExpr(A)
        else:
            raise NotImplemented
        return self

    # -------------- call -------------------------------------
    
    def __call__(self, args) : 
        return self.c_proxy(args) 

    # -------------- Various operations -------------------------------------
 
    def __le__(self, other): 
        raise RuntimeError, " Operator <= not defined "

    def __ilshift__(self, A):
        import warnings
        warnings.warn("The operator <<= is deprecated : update your code to use << instead", UserWarning, stacklevel=2)
        return self << A

    # ---------- Addition   

    def __iadd__(self,arg):
        if descriptor_base.is_lazy(arg): return lazy_expressions.make_lazy(self) + arg
        if isinstance(arg, Gf):
           self._data += arg._data 
           if self._singularity and arg._singularity : self._singularity += arg._singularity
           if not self._singularity and arg._singularity : self._singularity = arg._singularity.copy()
           if self._singularity and not arg._singularity : self._singularity = None
        else:
           wrapped_aux._iadd_g_matrix_scalar(self, arg) 
        return self

    def __add__(self,y):
        c = self.copy()
        c += y
        return c

    def __radd__(self,y): return self.__add__(y)

    # ---------- Substraction

    def __isub__(self,arg):
       if descriptor_base.is_lazy(arg): return lazy_expressions.make_lazy(self) - arg
       if isinstance(arg, Gf):
           self._data -= arg._data 
           if self._singularity and arg._singularity : self._singularity -= arg._singularity
           if not self._singularity and arg._singularity : self._singularity = arg._singularity.copy()
           if self._singularity and not arg._singularity : self._singularity = None
       else:
           wrapped_aux._isub_g_matrix_scalar(self, arg) 
       return self

    def __sub__(self,y):
        c = self.copy()
        c -= y
        return c

    def __rsub__(self,y):
        c = (-1)*self.copy()
        c += y
        return c

    # ---------- Multiplication
    def __imul__(self,arg):
        if descriptor_base.is_lazy(arg): return lazy_expressions.make_lazy(self) * arg
        self._data[:] *= arg
        self._singularity *= arg
        return self

    def __mul__(self,y):
        c = self.copy()
        c *= y
        return c

    def __rmul__(self,x): return self.__mul__(x)

    # ---------- Division
    def __idiv__(self,arg):
        if descriptor_base.is_lazy(arg): return lazy_expressions.make_lazy(self) / arg
        if operator.isSequenceType(arg):
            assert len(arg) == len(self.__GFlist) , "list of incorrect length"
            for l,g in izip(arg,self.__GFlist): g /=l 
        else: 
            for i,g in self: self[i] /= arg
        return self

    def __div__(self,y):
        c = self.copy()
        c /= y
        return c
  
    # ---------- unary -
    def __neg__(self):
        c = self.copy()
        c *= -1
        return c

   #----------------------------- other operations -----------------------------------

    def invert(self):
        """Inverts this Gf, in a matrix sense"""
        assert self.target_rank==2, "Inversion only makes sense for matrix valued Gf"
        wrapped_aux._gf_invert_data_in_place(self.data)   
        if self._singularity : self._singularity.invert()

    def inverse(self): 
        """Returns a new Gf, inverse of self."""
        r = self.copy()
        r.invert()
        return r

    def transpose(self): 
        """ Returns a NEW gf, with transposed data, i.e. it is NOT a transposed view."""
        assert any( (isinstance(self.mesh, x) for x in [meshes.MeshImFreq, meshes.MeshReFreq])), "Method invalid for this Gf" 
        d = np.transpose(self.data.copy(), (0, 2, 1))
        t = self.singularity.transpose()
        return self.__class__(mesh = self.mesh, data= d, singularity = t, indices = self.indices, name = self.name)

    def conjugate(self):
        """Returns a new functions, with the conjugate.
           Only for Gf of mesh MeshImFreq, and MeshReFreq
        """
        # REWRITTEN to have all matrix valued cases... I wrapped singularity conjufate
        assert any( (isinstance(self.mesh, x) for x in [meshes.MeshImFreq, meshes.MeshReFreq])), "Method invalid for this Gf" 
        t = self.singularity.conjugate(isinstance(self.mesh,meshes.MeshImFreq)) if self.singularity else None
        d = np.conj(self.data)
        return self.__class__(mesh = self.mesh, data= d, singularity = t, indices = self.indices, name = self.name)

    def zero(self):
        self._data[:] = 0
        if self.singularity: self.singularity.zero()

    def from_L_G_R(self, L, G, R):
        """self << l * g * r"""
        assert self.target_rank==2, "Inversion only makes sense for matrix valued Gf"
        assert self.rank==1, "Not implemented for more than one var" # A little generalization needed in C++ ?
        wrapped_aux.set_from_gf_data_mul_LR(self.data, L, G.data, R)
        if self.singularity : self.singularity.from_L_G_R(L, G.singularity, R) 

    #----------------------------- wrapped functions -----------------------------------

    def total_density(self):
        """Total density"""
        return np.trace(gf_fnt.density(self))

    #-----------------------------  IO  -----------------------------------
    
    def __reduce__(self):
        return call_factory_from_dict, (self.__class__, self.name, self.__reduce_to_dict__())

    def __reduce_to_dict__(self):
        d = {'mesh' : self._mesh, 'data' : self._data}
        if self.singularity : d['singularity'] = self.singularity 
        if self.indices : d['indices'] = self.indices 
        return d

    @classmethod
    def __factory_from_dict__(cls, name, d):
        return cls(name = name, **d)

    @classmethod
    def __group_scheme_map__(cls, hdf_scheme):
        # TODO: hdf_scheme needs to be boned
        # we know scheme is of the form GfM1_x_M2_s/tv3
        m, t= hdf_scheme[2:], '' # get rid of Gf
        for suffix in ['_s', '_tv3', '_tv4'] : 
            if m.endswith(suffix) :
                m, t = s[:-len(suffix)], suffix
                break
        return {'singularity': 'TailGf'+t, 'mesh': 'Mesh'+m, 'indices': 'GfIndices'}
    
    #-----------------------------plot protocol -----------------------------------

    def _plot_(self, opt_dict):
        """ Implement the plot protocol"""
        return plot.dispatcher(self)(self, opt_dict)

    def x_data_view(self, x_window=None, flatten_y=False):
        """
        :param x_window: the window of x variable (omega/omega_n/t/tau) for which data is requested
                          if None, take the full window
        :param flatten_y: If the Green function is of size (1, 1) flatten the array as a 1d array
        :rtype: a tuple (X, data) where
                 * X is a 1d numpy of the x variable inside the window requested
                 * data is a 3d numpy array of dim (:,:, len(X)), the corresponding slice of data
                   If flatten_y is True and dim is (1, 1, *), returns a 1d numpy
        """
        X = [x.imag for x in self.mesh] if isinstance(self.mesh, meshes.MeshImFreq) else [x for x in self.mesh]
        X, data = numpy.array(X), self.data
        if x_window:
            # the slice due to clip option x_window
            sl = clip_array(X, *x_window) if x_window else slice(len(X))
            X, data = X[sl],  data[sl, :, :]
        if flatten_y and data.shape[1:3] == (1, 1):
            data = data[:, 0, 0]
        return X, data

#---------------------------------------------------------

from pytriqs.archive.hdf_archive_schemes import register_class
register_class (Gf)

