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
import itertools, warnings, numbers
from functools import reduce # Valid in Python 2.6+, required in Python 3
import operator
import numpy as np
import mesh_product
import lazy_expressions
import descriptors, descriptor_base
from types import IntType, SliceType, StringType
from mesh_product import MeshProduct
from pytriqs.plot.protocol import clip_array
import meshes
import plot 
import gf_fnt, wrapped_aux
from gf_fnt import GfIndices
from mesh_point import MeshPoint
from operator import mul

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

class Idx:
    def __init__(self, *x):
        self.idx = x[0] if len(x)==1 else x

class Gf(object):
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

    indices: GfIndices or list of str or list of list of str, Optional
             Optional string indices for the target space, to allow e.g g['eg', 'eg']
             list of list of str: the list of indices for each dimension.
             list of str: all indices are assumed to be the same for all dimensions.
   
    name: str 
          The name of the Green function. For plotting.

    NB : One of target_shape, data and indices must be set, and the other must be None.
    """
    __metaclass__ = AddMethod
    
    _hdf5_data_scheme_ = 'Gf'

    def __init__(self, **kw): # enforce keyword only policy 
        
        #print "Gf construct args", kw

        def delegate(self, mesh, data=None, target_shape=None, indices = None, name = '', is_real = False):
            """
            target_shape and data  : must provide exactly one of them
            """
            # FIXME ? name is deprecated
            #if name: 
            #    warnings.warn("constructor parameter 'name' is deprecated in gf constructor.\n It is only used in plots.\n Pass the name to the oplot function directly")
            self.name = name

            # input check
            assert (target_shape is None) or (data is None), "data and target_shape : one must be None"
            assert (data is None) or (is_real is False), "is_real can not be True is data is not None"
            if target_shape : 
                for i in target_shape : 
                    assert i>0, "Target shape elements must be >0"
     
            # mesh
            assert isinstance(mesh, all_meshes), "Mesh is unknown. Possible type of meshes are %s" % ', '.join(map(lambda m: m.__name__,all_meshes))
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
            len_data_shape = len(self._data.shape) 
            self._target_rank = len_data_shape - (self._mesh.rank if isinstance(mesh, MeshProduct) else 1)  
            self._rank = len_data_shape - self._target_rank 
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
           
            # NB : at this stage, enough checks should have been made in Python in order for the C++ view 
            # to be constructed without any INTERNAL exceptions.
            # Set up the C proxy for call operator for speed. The name has to
            # agree with the wrapped_aux module, it is of only internal use
            s = '_x_'.join( m.__class__.__name__[4:] for m in self.mesh._mlist) if isinstance(mesh, MeshProduct) else self._mesh.__class__.__name__[4:]
            proxyname = 'CallProxy%s_%s%s'%(s, self.target_rank,'_R' if data.dtype == np.float64 else '') 
            try:
                self._c_proxy = all_call_proxies.get(proxyname, CallProxyNone)(self)
            except:
                self._c_proxy = None

            # check all invariants. Debug.
            self.__check_invariants()

        delegate(self, **kw)

    def __check_invariants(self):
        """Check various invariant. Mainly for debug"""
        # rank
        assert self.rank == self._mesh.rank if isinstance (self._mesh, MeshProduct) else 1
        # The mesh size must correspond to the size of the data
        assert self._data.shape[:self._rank] == tuple(len(m) for m in self._mesh.components) if isinstance (self._mesh, MeshProduct) else (len(self._mesh),)
 
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
                   indices = self._indices.copy(), 
                   name = self.name)

    def copy_from(self, another):
        """
        Copy the data of another into self.
        """
        self._mesh.copy_from(another.mesh)
        assert self._data.shape == another._data.shape, "Shapes are incompatible: " + str(self._data.shape) + " vs " + str(another._data.shape)
        self._data[:] = another._data[:]
        self._indices = another._indices.copy()
        self.__check_invariants()

    def __repr__(self):
        return "Green Function %s with mesh %s and target_rank %s: \n"%(self.name, self.mesh, self.target_rank)
 
    def __str__ (self): 
        return self.name if self.name else repr(self)

    #--------------  Bracket operator []  -------------------------
    
    _full_slice = slice(None, None, None) 

    def __getitem__(self, key):

        # First case : g[:] = RHS ... will be g << RHS
        if key == self._full_slice:
            return self

        # Only one argument. Must be a mesh point
        if not isinstance(key, tuple):
            assert isinstance(key, (MeshPoint, Idx))
            return self.data[key.linear_index if isinstance(key, MeshPoint) else self._mesh.index_to_linear(key.idx)]

        # If all arguments are MeshPoint, we are slicing the mesh or evaluating
        if all(isinstance(x, (MeshPoint, Idx)) for x in key):
            assert len(key) == self.rank, "wrong number of arguments in [ ]. Expected %s, got %s"%(self.rank, len(key))
            return self.data[tuple(x.linear_index if isinstance(x, MeshPoint) else m.index_to_linear(x.idx) for x,m in itertools.izip(key,self._mesh._mlist))]

        # If any argument is a MeshPoint, we are slicing the mesh or evaluating
        elif any(isinstance(x, (MeshPoint, Idx)) for x in key):
            assert len(key) == self.rank, "wrong number of arguments in [[ ]]. Expected %s, got %s"%(self.rank, len(key))
            assert all(isinstance(x, (MeshPoint, Idx, slice)) for x in key), "Invalid accessor of Green function, please combine only MeshPoints, Idx and slice"
            assert self.rank > 1, "Internal error : impossible case" # here all == any for one argument
            mlist = self._mesh._mlist 
            for x in key:
                if isinstance(x, slice) and x != self._full_slice: raise NotImplementedError, "Partial slice of the mesh not implemented" 
            # slice the data 
            k = [x.linear_index if isinstance(x, MeshPoint) else m.index_to_linear(x.idx) if isinstance(x, Idx) else x for x,m in itertools.izip(key,mlist)] + self._target_rank * [slice(0, None)]
            dat = self._data[k]
            # list of the remaining lists
            mlist = [m for i,m in itertools.ifilter(lambda tup_im : not isinstance(tup_im[0], (MeshPoint, Idx)), itertools.izip(key, mlist))]
            assert len(mlist) > 0, "Internal error" 
            mesh = MeshProduct(*mlist) if len(mlist)>1 else mlist[0]
            sing = None 
            r = Gf(mesh = mesh, data = dat)
            r.__check_invariants()
            return r

        # In all other cases, we are slicing the target space
        else : 
            assert self.target_rank == len(key), "wrong number of arguments. Expected %s, got %s"%(self.target_rank, len(key))

            # Assume empty indices (scalar_valued)
            ind = GfIndices([])

            # String access: transform the key into a list integers
            if all(isinstance(x, str) for x in key):
                assert self._indices, "Got string indices, but I have no indices to convert them !"
                key_lst = [self._indices.convert_index(s,i) for i,s in enumerate(key)] # convert returns a slice of len 1

            # Slicing with ranges -> Adjust indices
            elif all(isinstance(x, slice) for x in key): 
                key_lst = list(key)
                ind = GfIndices([ v[k]  for k,v in zip(key_lst, self._indices.data)])

            # Integer access
            elif all(isinstance(x, int) for x in key):
                key_lst = list(key)

            # Invalid Access
            else:
                raise NotImplementedError, "Partial slice of the target space not implemented"

            dat = self._data[ self._rank * [slice(0,None)] + key_lst ] 
            r = Gf(mesh = self._mesh, data = dat, indices = ind)

            r.__check_invariants()
            return r

    def __setitem__(self, key, val):

        # Only one argument. Must be a mesh point
        if not isinstance(key, tuple):
            assert isinstance(key, (MeshPoint, Idx))
            self.data[key.linear_index if isinstance(key, MeshPoint) else self._mesh.index_to_linear(key.idx)] = val

        # If all arguments are MeshPoint, we are slicing the mesh or evaluating
        elif all(isinstance(x, (MeshPoint, Idx)) for x in key):
            assert len(key) == self.rank, "wrong number of arguments in [ ]. Expected %s, got %s"%(self.rank, len(key))
            self.data[tuple(x.linear_index if isinstance(x, MeshPoint) else m.index_to_linear(x.idx) for x,m in itertools.izip(key,self._mesh._mlist))] = val

        else:
            self[key] << val

    # -------------- Various operations -------------------------------------
    
    @property
    def real(self): 
        """A Gf with only the real part of data."""
        return Gf(mesh = self._mesh, data = self._data.real, name = ("Re " + self.name) if self.name else '') 

    @property
    def imag(self): 
        """A Gf with only the imag part of data."""
        return Gf(mesh = self._mesh, data = self._data.imag, name = ("Im " + self.name) if self.name else '') 
 
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
    
    def __call__(self, *args) : 
        assert self._c_proxy, " no proxy"
        return self._c_proxy(*args) 

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
            assert type(self.mesh) == type(arg.mesh), "Can not add two Gf with meshes of different type"
            assert self.mesh == arg.mesh, "Can not add two Gf with different mesh"
            self._data += arg._data 
        else:
            if self._target_rank != 2 and not isinstance(arg, np.ndarray):
                 self._data[:] += arg
            elif self._target_rank == 2:
                 wrapped_aux._iadd_g_matrix_scalar(self, arg) 
            else:
                 raise NotImplemented
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
           assert type(self.mesh) == type(arg.mesh), "Can not subtract two Gf with meshes of different type"
           assert self.mesh == arg.mesh, "Can not subtract two Gf with different mesh"
           self._data -= arg._data 
       else:
            if self._target_rank != 2 and not isinstance(arg, np.ndarray):
               self._data[:] -= arg
            elif self._target_rank == 2:
               wrapped_aux._isub_g_matrix_scalar(self, arg) 
            else:
               raise NotImplemented
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
    # Naive implementation of G1 *= G2 without checks
    def __imul__impl(self,arg): # need to separate for the ImTime case

        # reshaping the data. Smash the mesh indices into one
        rh = lambda d : np.reshape(d, (reduce(mul, d.shape[:self.rank]),) + (d.shape[self.rank:]))
        d_self =  rh(self.data)
        d_args =  rh(arg.data)
        if self.target_rank == 2:
            for n in range (d_self.shape[0]):
               d_self[n] = np.dot(d_self[n], d_args[n]) # put to C if too slow.
        else:
            d_self *= d_args # put to C if too slow.

    def __imul__(self,arg):
        if descriptor_base.is_lazy(arg): return lazy_expressions.make_lazy(self) * arg
        # If arg is a Gf
        if isinstance(arg, Gf):
            assert type(self.mesh) == type(arg.mesh), "Can not multiply two Gf with meshes of different type"
            assert self.mesh == arg.mesh, "Can not use in-place multiplication for two Gf with different mesh"
            self.__imul__impl(arg)
        elif isinstance(arg, numbers.Number):
            self._data[:] *= arg
        elif isinstance(arg, np.ndarray):
	    assert len(arg.shape) == 2, "Multiplication only supported for matrices"
	    assert len(self.target_shape) == 2, "Multiplication only supported for matrix_valued Gfs"
	    self.data[:] = np.dot(self.data, arg)
        else:
            assert False, "Invalid operand type for Gf in-place multiplication"
        return self
    
    @staticmethod
    def _combine_mesh_mul(l, r):
        """ Apply the Fermion/Boson rules for ImTime mesh, and recursively for MeshProduct"""
        assert type(l) == type(r), "Can not multiply two Gf with meshes of different type"
        
        if type(l) is MeshProduct:
            return MeshProduct(*[Gf._combine_mesh_mul(l,r) for (l,r) in zip(l.components, r.components)])

        if not type(l) is meshes.MeshImTime: #regular case
            assert l==r, "Can not multiply two Gf with different mesh"
            return l.copy()
        else:
            assert abs(l.beta-r.beta) < 1.e-15 and len(l)== len(r), "Can not multiply two Gf with different mesh"
            return meshes.MeshImTime(l.beta, 'Boson' if l.statistic == r.statistic else 'Fermion', len(l)) 

    def __mul__(self,y):
        if isinstance(y, Gf):
            # make a copy, but special treatment of the mesh in the Imtime case.
            c = Gf(mesh = Gf._combine_mesh_mul(self._mesh, y.mesh),
                   data = self._data.copy(), 
                   indices = self._indices.copy(), 
                   name = self.name)
            c.__imul__impl(y)
        elif isinstance(y, (numbers.Number, np.ndarray)):
            c = self.copy()
            c *= y
        else:
            assert False, "Invalid operand type for Gf multiplication"
        return c

    def __rmul__(self,y):
	c = self.copy()
        if isinstance(y, np.ndarray):
	    assert len(y.shape) == 2, "Multiplication only supported for matrices"
	    assert len(self.target_shape) == 2, "Multiplication only supported for matrix_valued Gfs"
	    c.data[:] = np.tensordot(y, self.data, axes=([-1], [-2]))
	elif isinstance(y, numbers.Number):
            c *= y
        else:
            assert False, "Invalid operand type for Gf multiplication"
        return c
    # ---------- Division
    def __idiv__(self,arg):
        if descriptor_base.is_lazy(arg): return lazy_expressions.make_lazy(self) /arg
        self._data[:] /= arg
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
        if self.target_rank==0:
            self.data[:] = 1. / self.data
            return

        """Inverts this Gf in place, in a matrix sense"""
        assert self.target_rank==2, "Inversion only makes sense for matrix or scalar_valued Gf"
        d = self.data.view() # Cf https://docs.scipy.org/doc/numpy/reference/generated/numpy.reshape.html
        d.shape = (np.prod(d.shape[:-2]),) + d.shape[-2:] # reshaped view, guarantee no copy
        wrapped_aux._gf_invert_data_in_place(d)   

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
        return Gf(mesh = self.mesh, data= d, indices = self.indices.transpose())

    def conjugate(self):
        """
        Returns a new functions, with the conjugate.
        """
        return Gf(mesh = self.mesh, data= np.conj(self.data), indices = self.indices)

    def zero(self):
        """Put self to 0"""
        self._data[:] = 0

    def from_L_G_R(self, L, G, R):
        """self[:] =  l * g * r"""
        assert self.target_rank==2, "Function only makes sense for matrix valued Gf"
        assert self.rank==1, "Not implemented for more than one var" # A little generalization needed in C++ ?
        wrapped_aux.set_from_gf_data_mul_LR(self.data, L, G.data, R)

    def total_density(self, *args, **kwargs):
        """Total density"""
        return np.trace(gf_fnt.density(self, *args, **kwargs))

    #-----------------------------  IO  -----------------------------------
    
    def __reduce__(self):
        return call_factory_from_dict, (Gf, self.name, self.__reduce_to_dict__())

    def __reduce_to_dict__(self):
        d = {'mesh' : self._mesh, 'data' : self._data}
        if self.indices : d['indices'] = self.indices 
        return d

    @classmethod
    def __factory_from_dict__(cls, name, d):
        # Backward compatibility layer
        # Drop singularity from the element and ignore it
        d.pop('singularity', None)
        #
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
        X, data = np.array(X), self.data
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
    return { 'mesh': 'Mesh'+m, 'indices': 'GfIndices'}

register_backward_compatibility_method("Gf", "Gf", bckwd)

