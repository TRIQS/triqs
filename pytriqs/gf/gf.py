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
import descriptors, descriptor_base
from types import IntType, SliceType, StringType
from mesh_product import MeshProduct
import meshes
import singularities
import plot 
import gf_fnt, wrapped_aux
from singularities import GfIndices

# list of all the meshes
all_meshes = (MeshProduct,) + tuple(c for c in meshes.__dict__.values() if isinstance(c, type) and c.__name__.startswith('Mesh'))
# list of call_proxies
all_call_proxies = dict( (c.__name__, c) for c in wrapped_aux.__dict__.values() if isinstance(c, type) and c.__name__.startswith('CallProxy'))

class CallProxyNone:
    """Default do nothing value"""
    def __init__(self, a):
        pass
    def __call__(self, a): 
        raise NotImplemented

# For IO later
def call_factory_from_dict(cl,name, dic):
    """Given a class cl and a dict dic, it calls cl.__factory_from_dict__(dic)"""
    return cl.__factory_from_dict__(name, dic)

# a metaclass that adds all functions of gf_fnt as methods 
# the C++ will take care of the dispatch
def add_method_helper(a,cls): 
    def _(self, *args, **kw):
       return a(self, *args, **kw)
    _.__doc__ = 50*'-' + '\n' + a.__doc__
    _.__name__ = a.__name__
    return _

class AddMethod(type): 
    def __init__(cls, name, bases, dct):
        super(AddMethod, cls).__init__(name, bases, dct)
        for a in [f for f in gf_fnt.__dict__.values() if callable(f)]:
            setattr(cls, a.__name__, add_method_helper(a,cls)) 

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
             Incompatible with data

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
    
    _hdf5_data_scheme_ = 'Gf'

    def __init__(self, **kw): # enforce keyword only policy 
        
        #print "Gf construct args", kw

        def delegate(self, mesh, data=None, target_shape=None, singularity = None, indices = None, name = '', is_real = False, _singularity_maker = None):
            """
            target_shape and data  : must provide exactly one of them
            """
            # input check
            assert (target_shape is None) or (data is None), "data and target_shape : one must be None"
            assert (data is None) or (is_real is False), "is_real can not be True is data is not None"
            if target_shape : 
                for i in target_shape : 
                    assert i>0, "Target shape elements must be >0"

            # mesh
            assert isinstance(mesh, all_meshes), "Mesh is unknown. Possible type of meshes are  %s"%''.join(all_meshes)
            self._mesh = mesh

            # indices
            # if indices is not a list of list, but a list, then the target_rank is assumed to be 2 !
            # backward compatibility only, it is not very logical (what about vector_valued gf ???)
            assert isinstance(indices, (type(None), list, GfIndices)), "Type of indices incorrect : should be None, Gfindices, list of str, or list of list of str"
            if isinstance(indices, list):
                if not isinstance(indices[0], list): indices = [indices, indices]
                # indices : transform indices into string
                indices = [ [str(x) for x in v] for v in indices]
                indices = GfIndices(indices)
            self._indices = indices # now indices are None or Gfindices 

            # data
            if data is None:
                # if no data, we get the target_shape. If necessary, we find it from of the list of indices
                if target_shape is None : 
                    assert indices, "Without data, target_shape, I need the indices to compute the shape !"
                    target_shape = [ len(x) for x in indices.data]
                # we now allocate the data
                l = mesh.size_of_components() if isinstance(mesh, MeshProduct) else [len(mesh)]
                data = np.zeros(list(l) + list(target_shape), dtype = np.float64 if is_real else np.complex128)
            # Now we have the data at correct size. Set up a few short cuts
            self._data = data
            self._target_rank = len(self._data.shape) - (self._mesh.rank if isinstance(mesh, MeshProduct) else 1) 
            self._rank = len(self._data.shape) - self._target_rank
            assert self._rank >= 0

            # target_shape. Ensure it is correct in any case.
            assert target_shape is None or tuple(target_shape) == self._data.shape[self._rank:] # Debug only
            self._target_shape = self._data.shape[self._rank:]

            # If no indices was given, build the default ones
            if self._indices is None: 
                self._indices = GfIndices([list(str(i) for i in range(n)) for n in self._target_shape])

            # Check that indices  have the right size
            if self._indices is not None: 
                d,i =  self._data.shape[self._rank:], tuple(len(x) for x in self._indices.data)
                assert (d == i), "Indices are of incorrect size. Data size is %s while indices size is %s"%(d,i)
            # Now indices are set, and are always a GfIndices object, with the
            # correct size
           
            # singularity : given, or a maker is given (using the data, ...)
            assert (singularity is None) or (_singularity_maker is None), "Internal error"
            self.singularity = singularity or (_singularity_maker(self) if _singularity_maker else None)

            # Misc.
            self.name = name

            # NB : at this stage, enough checks should have been made in Python in order for the C++ view 
            # to be constructed without any INTERNAL exceptions.
            # Set up the C proxy for call operator for speed. The name has to
            # agree with the wrapped_aux module, it is of only internal use
            s = '_x_'.join( m.__class__.__name__[4:] for m in self.mesh._mlist) if isinstance(mesh, MeshProduct) else self._mesh.__class__.__name__[4:]
            proxyname = 'CallProxy%s_%s%s'%(s, self.target_rank,'_R' if data.dtype == np.float64 else '')
            self._c_proxy = all_call_proxies.get(proxyname, CallProxyNone)(self)
            
            # check all invariants. Debug.
            self.__check_invariants()

        delegate(self, **kw)

    def __check_invariants_tail(self):
        """Check invariant for singularity. Mainly for debug"""
        if self._singularity: 
            # The target size
            assert self._singularity.data.shape[1:] == self.data.shape[self._rank:], "Invariant broken for tail %s, %s"%(self._singularity.data.shape[1:], self.data.shape[self._rank:])
 
    def __check_invariants(self):
        """Check various invariant. Mainly for debug"""
        # rank
        assert self.rank == self._mesh.rank if isinstance (self._mesh, MeshProduct) else 1
        # The mesh size must correspond to the size of the data
        assert self._data.shape[:self._rank] == tuple(len(m) for m in self._mesh.components) if isinstance (self._mesh, MeshProduct) else (len(self._mesh),)
        self.__check_invariants_tail()
 
    @property
    def rank(self):
        """Number of variable"""
        return self._rank

    @property
    def target_rank(self): 
        """Dimension of the target shape"""
        return self._target_rank

    @property
    def target_shape(self): 
        """Shape of the target_space"""
        return self._target_shape

    @property
    def mesh(self):
        """The mesh"""
        return self._mesh

    @property
    def data(self):
        """
           The data array, as a numpy array.
           Storage convention is self.data[x,y,z, ..., n0,n1,n2] 
           where x,y,z correspond to the variables (the mesh) and 
           n0, n1, n2 to the target_space
        """
        return self._data

    @property
    def singularity(self) : 
        """
        The singularity 
        Possible types : (TailGf, None)
        """
        return self._singularity
 
    @singularity.setter
    def singularity(self, value): 
        self._singularity = value
        self.__check_invariants_tail()

    @property
    def tail(self) : 
        """
        Same as singularity
        """
        return self._singularity
   
    @tail.setter
    def tail(self, value):
        self._singularity = value
        self.__check_invariants_tail()

    @property
    def indices(self):
        """
        Access to the indices
        """
        return self._indices

    def copy(self) : 
        """
        Deep copy of the Green function
        """
        return Gf (mesh = self._mesh.copy(), 
                   data = self._data.copy(), 
                   singularity = self._singularity.copy() if self._singularity else None,
                   indices = self._indices.copy(), 
                   name = self.name)

    def copy_from(self, another):
        """
        Copy the data of another into self.
        """
        self._mesh.copy_from(another.mesh)
        assert self._data.shape == another._data.shape, "Shapes are incompatible"
        self._data[:] = another._data[:]
        if self._singularity : self._singularity.copy_from(another._singularity)
        self._indices = another._indices.copy()
        self.name = another.name
        self.__check_invariants()

    def __repr__(self):
        return "Green Function %s with mesh %s and range %s: \n"%(self.name, self.mesh, self.rank)

    def __str__ (self): 
        return self.name if self.name else repr(self)

    #--------------  Bracket operator []  -------------------------
    
    def __getitem__(self, key):

        # First case : g[:] = RHS ... will be g << RHS
        if key == slice(None, None, None) : return self

        # Second, we get a list g[[a,b]] : we are slicing the mesh
        if isinstance(key, list):
            assert len(key) == self.rank, "wrong number of arguments in [[ ]]. Expected %s, got %s"%(self.rank, len(key))
            mlist = self._mesh._mlist if self.rank > 1 else [self._mesh]
            # slice the data 
            _ = slice(0, None) # all
            k = [m.index_to_linear(i) if i is not all else _ for i,m in itertools.izip(key, mlist)] 
            k += self._target_rank * [_]
            dat = self._data[k]
            # list of the remaining lists
            mlist = [m for i,m in itertools.ifilter(lambda tup_im : tup_im[0] is all, itertools.izip(key, mlist))]
            # if no mlist, then we have a complete evaluation
            # assert len(mlist) > 0, "Oops"  # More conservative version
            if len(mlist) == 0 : return dat
            mesh = MeshProduct(*mlist) if len(mlist)>1 else mlist[0]
            sing = None # FIXME : slice the singularity, in one case
            r = Gf(mesh = mesh, data = dat, singularity = sing)
            r.__check_invariants()
            return r

        # In all other cases, we are slicing the target space
        else : 
            assert self.target_rank == len(key), "wrong number of arguments. Expected %s, got %s"%(self.target_rank, len(key))
            # transform the key in a list of slices
            if all(isinstance(x, str) for x in key):
                assert self._indices, "Got string indices, but I have no indices to convert them !"
                key_s = [self._indices.convert_index (s,i) for i,s in enumerate(key)] # convert returns a slice of len 1
                key = [x.start for x in key_s]
            if all(isinstance(x, slice) for x in key) : 
                key_s, key = list(key), [ x.start for x in key]
            else:
                key_s = map(lambda r: slice(r,r+1,1), key) # transform int into slice 
            # now the key is a list of slices
            dat = self._data[ self._rank * [slice(0,None)] + key_s ] 
            ind = GfIndices([ v[k]  for k,v in zip(key_s, self._indices.data)])
            sing = singularities._make_tail_view_from_data(self._singularity.data[[slice(0,None)] + key_s]) if self._singularity else None # Build a new view of the singularity
            r = Gf(mesh = self._mesh, data = dat, singularity = sing, indices = ind, name = "")
            r.__check_invariants()
            return r

    def __setitem__(self, key, val):
        self[key] << val

    # -------------- Various operations -------------------------------------
    
    @property
    def real(self) : 
        return Gf(mesh = self._mesh, data = self._data.real, singularity = None,name = "Re " + self.name) # Singularity is None for G(tau) ?

    @property
    def imag(self) : 
        return Gf(mesh = self._mesh, data = self._data.imag, singularity = None, name = "Im " + self.name) # Singularity is None for G(tau) ?

    # --------------  Lazy system -------------------------------------

    def __lazy_expr_eval_context__(self) : 
        return LazyCTX(self)

    def __lshift__(self, A):
        """ A can be two things:
          * G << any_init will init the GFBloc with the initializer
          * G << g2 where g2 is a GFBloc will copy g2 into self
        """
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
        return self._c_proxy(args) 

    # -------------- Various operations -------------------------------------
 
    def __le__(self, other): 
        raise RuntimeError, " Operator <= not defined "

    def __ilshift__(self, A):
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
           if not self._singularity and arg._singularity : self._singularity = - arg._singularity.copy()
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
        # Multiply by a GF !?
        self._data[:] *= arg
        if self._singularity : self._singularity *= arg
        return self

    def __mul__(self,y):
        c = self.copy()
        c *= y
        return c

    def __rmul__(self,x): return self.__mul__(x)

    # ---------- Division
    def __idiv__(self,arg):
        if descriptor_base.is_lazy(arg): return lazy_expressions.make_lazy(self) /arg
        self._data[:] /= arg
        if self._singularity : self._singularity /= arg
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
        """Inverts this Gf in place, in a matrix sense"""
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
        # FIXME Why this assert ?
        #assert any( (isinstance(self.mesh, x) for x in [meshes.MeshImFreq, meshes.MeshReFreq])), "Method invalid for this Gf" 
        d = np.transpose(self.data.copy(), (0, 2, 1))
        t = self.singularity.transpose()
        return Gf(mesh = self.mesh, data= d, singularity = t, indices = self.indices.transpose(), name = self.name)

    def conjugate(self):
        """
        Returns a new functions, with the conjugate.
        """
        #assert any( (isinstance(self.mesh, x) for x in [meshes.MeshImFreq, meshes.MeshReFreq])), "Method invalid for this Gf" 
        t = self.singularity.conjugate(isinstance(self.mesh,meshes.MeshImFreq)) if self.singularity else None
        d = np.conj(self.data)
        return Gf(mesh = self.mesh, data= d, singularity = t, indices = self.indices, name = self.name)

    def zero(self):
        """Put self to 0"""
        self._data[:] = 0
        if self.singularity: self.singularity.zero()

    def from_L_G_R(self, L, G, R):
        """self[:] =  l * g * r"""
        assert self.target_rank==2, "Function only makes sense for matrix valued Gf"
        assert self.rank==1, "Not implemented for more than one var" # A little generalization needed in C++ ?
        wrapped_aux.set_from_gf_data_mul_LR(self.data, L, G.data, R)
        if self.singularity : self.singularity.from_L_G_R(L, G.singularity, R) 

    def total_density(self):
        """Total density"""
        return np.trace(gf_fnt.density(self))

    #-----------------------------  IO  -----------------------------------
    
    def __reduce__(self):
        return call_factory_from_dict, (Gf, self.name, self.__reduce_to_dict__())

    def __reduce_to_dict__(self):
        d = {'mesh' : self._mesh, 'data' : self._data}
        if self.singularity : d['singularity'] = self.singularity 
        if self.indices : d['indices'] = self.indices 
        return d

    @classmethod
    def __factory_from_dict__(cls, name, d):
        #print "FACTORY", d
        r = cls(name = name, **d)
        # Backward compatibility layer
        # In the case of an ImFreq function, old archives did store only the >0
        # frequencies, we need to duplicate it for negative freq.
        # Same code as in the C++ h5_read for gf.
        need_unfold = isinstance(r.mesh, meshes.MeshImFreq) and r.mesh.positive_only() 
        return r if not need_unfold else wrapped_aux._make_gf_from_real_gf(r)
    
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

    #-------------- Deprecated. ---------------------------
    @property
    def beta(self) : 
        warnings.warn("g.beta is deprecated and not generic. Use g.mesh.beta instead")
        return self.mesh.beta
    
    @property
    def statistic(self) : 
        warnings.warn("g.statistic is deprecated and not generic. Use g.mesh.statistic instead")
        return self.mesh.statistic

    #-------------- Deprecated. NB works only in 1 var, matrix_valued anyway  ---------------------------
    @property
    def N1(self):
        assert self.target_rank == 2, "N1 only makes sense for rank 2 targets"
        warnings.warn("g.N1 is deprecated and not generic. Use g.target_shape[0] instead")
        return self.target_shape[0]

    @property
    def N2(self):
        assert self.target_rank == 2, "N2 only makes sense for rank 2 targets"
        warnings.warn("g.N2 is deprecated and not generic. Use g.target_shape[1] instead")
        return self.target_shape[1]

    @property
    def indicesL(self):
        warnings.warn("g.indicesL is deprecated. Use g.indices[0] instead")
        return self.indices.data[0]

    @property
    def indicesR(self):
        warnings.warn("g.indicesR is deprecated. Use g.indices[1] instead")
        return self.indices.data[1]

#---------------------------------------------------------

from pytriqs.archive.hdf_archive_schemes import register_class, register_backward_compatibility_method
register_class (Gf)

# A backward compatility function
def bckwd(hdf_scheme):
    # we know scheme is of the form GfM1_x_M2_s/tv3
    m, t= hdf_scheme[2:], '' # get rid of Gf
    for suffix in ['_s', 'Tv3', 'Tv4'] : 
        if m.endswith(suffix) :
            m, t = m[:-len(suffix)], suffix
            break
    return {'singularity': 'TailGf'+t, 'mesh': 'Mesh'+m, 'indices': 'GfIndices'}

register_backward_compatibility_method("Gf", "Gf", bckwd)

