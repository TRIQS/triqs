# Copyright (c) 2017-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2017-2018 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2018-2023 Simons Foundation
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
# Authors: Michel Ferrero, Olivier Gingras, Alexander Hampel, Jonathan Karp, Manuel, Olivier Parcollet, Markus Richter, Hugo U. R. Strand, Nils Wentzell

import itertools, warnings, numbers
from functools import reduce # Valid in Python 2.6+, required in Python 3
import operator
import numpy as np
from . import mesh_product
from . import lazy_expressions
from . import descriptors, descriptor_base
from .mesh_product import MeshProduct
from triqs.plot.protocol import clip_array
from . import meshes
from . import plot 
from . import gf_fnt, wrapped_aux
from .mesh_point import MeshPoint
from operator import mul

# list of all the meshes
all_meshes = (MeshProduct,) + tuple(c for c in list(meshes.__dict__.values()) if isinstance(c, type) and c.__name__.startswith('Mesh'))
# list of call_proxies
all_call_proxies = dict( (c.__name__, c) for c in list(wrapped_aux.__dict__.values()) if isinstance(c, type) and c.__name__.startswith('CallProxy'))

class CallProxyNone:
    """Default do nothing value"""
    def __init__(self, *args):
        pass
    def __call__(self, *args):
        raise NotImplementedError

# For IO later
def call_factory_from_dict(cl,name, dic):
    """Given a class cl and a dict dic, it calls cl.__factory_from_dict__(dic)"""
    return cl.__factory_from_dict__(name, dic)

# a metaclass that adds all functions of gf_fnt as methods 
# the C++ will take care of the dispatch
def add_method_helper(a,cls): 
    def _(self, *args, **kw):
       return a(self, *args, **kw)
    _.__doc__ =  a.__doc__
    #_.__doc__ = 50*'-' + '\n' + a.__doc__
    _.__name__ = a.__name__
    return _

class AddMethod(type): 
    def __init__(cls, name, bases, dct):
        super(AddMethod, cls).__init__(name, bases, dct)
        for a in [f for f in list(gf_fnt.__dict__.values()) if callable(f)]:
            if not hasattr(cls, a.__name__):
                setattr(cls, a.__name__, add_method_helper(a,cls))

class Idx:
    def __init__(self, *x):
        self.idx = x[0] if len(x)==1 else x

class Gf(metaclass=AddMethod):
    r""" TRIQS Greens function container class

    Parameters
    ----------

    mesh: Types defined in triqs.gf beginning with 'Mesh'
          The mesh on which the Green function is defined.

    data: numpy.array, optional
          The data of the Greens function.
          Must be of dimension ``mesh.rank + target_rank``.

    target_shape: list of int, optional
                  Shape of the target space.

    is_real: bool
             Is the Greens function real valued?
             If true, and target_shape is set, the data will be real.
             Mutually exclusive with argument ``data``.

    name: str 
          The name of the Greens function for plotting.

    Notes
    -----

    One of ``target_shape`` or ``data`` must be set, and the other must be `None`.

    """
    
    _hdf5_data_scheme_ = 'Gf'
    __array_priority__ = 10000 # Makes sure the operations of this class are applied as priority

    def __init__(self, **kw): # enforce keyword only policy 
        
        #print "Gf construct args", kw

        def delegate(self, mesh, data=None, target_shape=None, name = '', is_real = False, indices=None):
            """
            target_shape and data  : must provide exactly one of them
            """
            # FIXME ? name is deprecated
            #if name: 
            #    warnings.warn("constructor parameter 'name' is deprecated in gf constructor.\n It is only used in plots.\n Pass the name to the oplot function directly")
            self.name = name

            # input check
            assert (target_shape is None) or (data is None), "data and target_shape : one must be None"
            assert (data is None) or (is_real is False), "is_real can not be True if data is not None"
            if target_shape : 
                for i in target_shape : 
                    assert i>0, "Target shape elements must be >0"
     
            # mesh
            assert isinstance(mesh, all_meshes), "Mesh is unknown. Possible type of meshes are %s" % ', '.join([m.__name__ for m in all_meshes])
            self._mesh = mesh

            # indices backward compat layer
            if indices is not None:
                warnings.warn("The use of string indices is no longer supported, converting to target_shape instead.", DeprecationWarning)
                assert target_shape is None, "target_shape must be None if indices is not None"
                if isinstance(indices[0], (list, range)):
                    target_shape = [len(li) for li in indices]
                else:
                    target_shape = [len(indices), len(indices)]

            # data
            if data is None:
                # if no data, we need target_shape
                assert target_shape is not None, "target_shape must be provided if data is None"
                # we now allocate the data
                l = mesh.size_of_components() if isinstance(mesh, MeshProduct) else [len(mesh)]
                data = np.zeros(list(l) + list(target_shape), dtype = np.float64 if is_real else np.complex128)
            else:
                l = tuple(mesh.size_of_components()) if isinstance(mesh, MeshProduct) else (len(mesh),)
                assert l == data.shape[0:len(l)], "Mismatch between data shape %s and sizes of mesh(es) %s\n " % (data.shape, l)

            # Now we have the data at correct size. Set up a few short cuts
            self._data = data
            len_data_shape = len(self._data.shape) 
            self._target_rank = len_data_shape - (self._mesh.rank if isinstance(mesh, MeshProduct) else 1)  
            self._rank = len_data_shape - self._target_rank 
            assert self._rank >= 0

            # target_shape. Ensure it is correct in any case.
            assert target_shape is None or tuple(target_shape) == self._data.shape[self._rank:] # Debug only
            self._target_shape = self._data.shape[self._rank:]

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

    def density(self, *args, **kwargs):
        r"""Compute the density matrix of the Greens function

        Parameters
        ----------

        beta : float, optional
            Used for finite temperature density calculation with ``MeshReFreq``.

        Returns
        -------

        density_matrix : ndarray
            Single particle density matrix with shape ``target_shape``.

        Notes
        -----

        Only works for single mesh Greens functions with a, Matsubara,
        real-frequency, or Legendre mesh.
        """

        return gf_fnt.density(self, *args, **kwargs)

    @property
    def rank(self):
        r"""int : The mesh rank (number of meshes)."""
        return self._rank

    @property
    def target_rank(self): 
        """int : The rank of the target space."""
        return self._target_rank

    @property
    def target_shape(self): 
        """(int, ...) : The shape of the target space."""
        return self._target_shape

    @property
    def target_indices(self):
        """(int, ...) : A generator for the target space integer tuples"""
        return itertools.product(*[range(i) for i in self._target_shape])

    @property
    def indices(self):
        warnings.warn("Gf.indices is deprecated, use Gf.target_shape", DeprecationWarning)
        """(int, ...) : Deprecated. Use target_indices instead."""
        return [range(d) for d in self.target_shape]

    @property
    def mesh(self):
        """gf_mesh : The mesh of the Greens function."""
        return self._mesh

    @property
    def data(self):
        """ndarray : Raw data of the Greens function.

           Storage convention is ``self.data[x,y,z, ..., n0,n1,n2]``
           where ``x,y,z`` correspond to the mesh variables (the mesh) and 
           ``n0, n1, n2`` to the ``target_space``.
        """
        return self._data

    def copy(self) : 
        """Deep copy of the Greens function.

        Returns
        -------
        G : Gf
            Copy of self.
        """
        return Gf (mesh = self._mesh.copy(), 
                   data = self._data.copy(), 
                   name = self.name)

    def copy_from(self, another):
        """Copy the data of another Greens function into self."""
        self._mesh.copy_from(another.mesh)
        assert self._data.shape == another._data.shape, "Shapes are incompatible: " + str(self._data.shape) + " vs " + str(another._data.shape)
        self._data[:] = another._data[:]
        self.__check_invariants()

    def __repr__(self):
        return "Greens Function %s with mesh %s and target_shape %s: \n"%(self.name, self.mesh, self.target_shape)
 
    def __str__ (self): 
        return self.__repr__()

    #--------------  Bracket operator []  -------------------------

    # Helper: Convert Idx or MeshPoint into data_index, else forward argument
    @staticmethod
    def _to_data_index(x, m):
        if isinstance(x, MeshPoint):
            assert x.mesh_hash == m.mesh_hash, "Green function Mesh and MeshPoint have incompatible hash"
            return x.data_index
        elif isinstance(x, Idx):
            return m.to_data_index(x.idx)
        else:
            return x
    
    _full_slice = slice(None, None, None) 

    def __getitem__(self, key):

        # First case : g[:] = RHS ... will be g << RHS
        if key == self._full_slice:
            return self

        # Only one argument. Must be a mesh point, idx or slicing rank1 target space
        if not isinstance(key, tuple):
            if isinstance(key, (Idx, MeshPoint)):
                return self.data[self._to_data_index(key, self._mesh)]
            else: key = (key,)

        # If all arguments are MeshPoint, we are slicing the mesh or evaluating
        if all(isinstance(x, (MeshPoint, Idx)) for x in key):
            assert len(key) == self.rank, "wrong number of arguments in [ ]. Expected %s, got %s"%(self.rank, len(key))
            return self.data[tuple(self._to_data_index(x,m) for x,m in zip(key,self._mesh._mlist))]

        # If any argument is a MeshPoint, we are slicing the mesh or evaluating
        elif any(isinstance(x, (MeshPoint, Idx)) for x in key):
            assert len(key) == self.rank, "wrong number of arguments in [[ ]]. Expected %s, got %s"%(self.rank, len(key))
            assert all(isinstance(x, (MeshPoint, Idx, slice)) for x in key), "Invalid accessor of Greens function, please combine only MeshPoints, Idx and slice"
            assert self.rank > 1, "Internal error : impossible case" # here all == any for one argument
            mlist = self._mesh._mlist 
            for x in key:
                if isinstance(x, slice) and x != self._full_slice: raise NotImplementedError("Partial slice of the mesh not implemented")
            # slice the data
            k = tuple(self._to_data_index(x,m) for x,m in zip(key,mlist)) + self._target_rank * (slice(0, None),)
            dat = self._data[k]
            # list of the remaining lists
            mlist = [m for i,m in filter(lambda tup_im : not isinstance(tup_im[0], (MeshPoint, Idx)), zip(key, mlist))]
            assert len(mlist) > 0, "Internal error" 
            mesh = MeshProduct(*mlist) if len(mlist)>1 else mlist[0]
            sing = None 
            r = Gf(mesh = mesh, data = dat)
            r.__check_invariants()
            return r

        # In all other cases, we are slicing the target space
        else : 
            assert self.target_rank == len(key), "wrong number of arguments. Expected %s, got %s"%(self.target_rank, len(key))

            # Slicing with ranges
            if all(isinstance(x, slice) for x in key): 
                key_tpl = tuple(key)

            # Integer access
            elif all(isinstance(x, int) for x in key):
                key_tpl = tuple(key)

            # Invalid Access
            elif any(isinstance(x, str) for x in key):
                raise RuntimeError("String indices are no longer supported. Please use integer indices for the target.")
            else:
                raise NotImplementedError("Partial slice of the target space not implemented")

            dat = self._data[ self._rank*(slice(0,None),) + key_tpl ]
            r = Gf(mesh = self._mesh, data = dat)

            r.__check_invariants()
            return r

    def __setitem__(self, key, val):

        # Only one argument and not a slice. Must be a mesh point, Idx
        if isinstance(key, (MeshPoint, Idx)):
            self.data[self._to_data_index(key, self._mesh)] = val

        # If all arguments are MeshPoint, we are slicing the mesh or evaluating
        elif isinstance(key, tuple) and all(isinstance(x, (MeshPoint, Idx)) for x in key):
            assert len(key) == self.rank, "wrong number of arguments in [ ]. Expected %s, got %s"%(self.rank, len(key))
            self.data[tuple(self._to_data_index(x,m) for x,m in zip(key,self._mesh._mlist))] = val

        else:
            self[key] << val

    # -------------- Various operations -------------------------------------
    
    @property
    def real(self): 
        """Gf : A Greens function with a view of the real part."""
        return Gf(mesh = self._mesh, data = self._data.real, name = ("Re " + self.name) if self.name else '') 

    @property
    def imag(self): 
        """Gf : A Greens function with a view of the imaginary part."""
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
            if self is not A: # otherwise it is useless AND does not work !!
                assert self.mesh == A.mesh, "Green function meshes are not compatible:\n  %s\nand\n  %s" % (self.mesh, A.mesh)
                self.copy_from(A)
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
        def filt(x):
            if isinstance(x, MeshPoint):
                return x.value
            if isinstance(x, Idx):
                return x.idx
            return x
        return self._c_proxy(*[filt(x) for x in args])

    # -------------- Various operations -------------------------------------
 
    def __le__(self, other): 
        raise RuntimeError(" Operator <= not defined ")

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

    # ---------- Matrix Multiplication (@)

    def __imatmul__(self,arg):
        if descriptor_base.is_lazy(arg): return lazy_expressions.make_lazy(self) * arg
        # If arg is a Gf
        if isinstance(arg, Gf):
            assert type(self.mesh) == type(arg.mesh), "Can not multiply two Gf with meshes of different type"
            assert self.mesh == arg.mesh, "Can not use in-place multiplication for two Gf with different mesh"

            if self.target_rank == 2:
                if arg.target_rank == 2:
                    assert arg.target_shape[0] != arg.target_shape[1], "In place multiplication not supported if"
                    "the argument is a nonsquare matrix. Use regular multiplication instead."
                    np.matmul(self.data, arg.data, out=self.data)
                elif arg.target_rank == 0:
                    self.data[:] *= arg.data[..., None, None]
                else:
                    raise NotImplementedError("argument of in place multiplication must be rank 0 or 2")

            elif self.target_rank == 0:
                assert arg.target_rank == 0, "argument of in place multiplication must have rank 0 if self does"
                self.data[:] = self.data * arg.data

            else:
                raise NotImplementedError("Green's functions must be of rank 0 or 2 for multiplication")

        elif isinstance(arg, numbers.Number):
            self._data[:] *= arg
        elif isinstance(arg, np.ndarray):
            assert len(arg.shape) == 2, "Multiplication only supported for matrices"
            assert len(self.target_shape) == 2, "Multiplication only supported for matrix_valued Gfs"
            self.data[:] = np.tensordot(self.data, arg, axes=([-1], [-2]))
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

    def __matmul__(self,y):
        if isinstance(y, Gf):
            # make a copy, but special treatment of the mesh in the Imtime case.
            result_mesh = Gf._combine_mesh_mul(self._mesh, y.mesh)
            if self.target_rank == 2 and y.target_rank == 2:
                c = Gf(mesh=result_mesh, target_shape=[self.target_shape[0], y.target_shape[1]])
                np.matmul(self.data, y.data, out=c.data)
            elif self.target_rank == 0 and y.target_rank == 0:
                c = Gf(mesh=result_mesh, target_shape=[])
                c.data[:] = self.data * y.data
            elif self.target_rank == 2 and y.target_rank == 0:
                c = Gf(mesh=result_mesh, target_shape=self.target_shape)
                c.data[:] = self.data * y.data[..., None, None]
            elif self.target_rank == 0 and y.target_rank == 2:
                c = Gf(mesh=result_mesh, target_shape=y.target_shape)
                c.data[:] = self.data[..., None, None] * y.data

            else:
                raise NotImplementedError("Green's functions must be of rank 0 or 2 for multiplication")

        elif isinstance(y, (numbers.Number, np.ndarray)):
            c = self.copy()
            c *= y
        else:
            assert False, "Invalid operand type for Gf multiplication"
        return c

    def __rmatmul__(self,y):
        c = self.copy()
        if isinstance(y, np.ndarray):
            assert len(y.shape) == 2, "Multiplication only supported for matrices"
            assert len(self.target_shape) == 2, "Multiplication only supported for matrix_valued Gfs"
            c.data[:] = np.moveaxis(np.tensordot(y, self.data, axes=([-1], [-2])), 0, -2)
        elif isinstance(y, numbers.Number):
            c *= y
        else:
            assert False, "Invalid operand type for Gf multiplication"
        return c

    # ---------- Multiplication (with *, for now it's equivalent to the matrix multiplication)
    def __imul__(self,arg):
        return self.__imatmul__(arg)

    def __mul__(self,y):
        return self.__matmul__(y)

    def __rmul__(self,y):
        return self.__rmatmul__(y)

    # ---------- Division
    def __itruediv__(self,arg):
        self._data[:] /= arg
        return self

    def __truediv__(self,y):
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
        """Inverts the Greens function (in place)."""

        if self.target_rank == 0: # Scalar target space
            self.data[:] = 1. / self.data
        elif self.target_rank == 2: # Matrix target space
            # TODO: Replace by np.linag.inv, since v1.8
            # Cf https://docs.scipy.org/doc/numpy/reference/generated/numpy.reshape.html
            d = self.data.view()
            d.shape = (np.prod(d.shape[:-2]),) + d.shape[-2:] # reshaped view, guarantee no copy
            wrapped_aux._gf_invert_data_in_place(d)
        else:
            raise TypeError(
                "Inversion only makes sense for matrix or scalar_valued Greens functions")

    def inverse(self): 
        """Computes the inverse of the Greens function.

        Returns
        -------
        G : Gf (copy)
            The matrix/scalar inverse of the Greens function.
        """
        r = self.copy()
        r.invert()
        return r

    def transpose(self): 
        """Take the transpose of a matrix valued Greens function.

        Returns
        -------

        G : Gf (copy)
            The transpose of the Greens function.

        Notes
        -----

        Only implemented for single mesh matrix valued Greens functions.

        """

        # FIXME Why this assert ?
        #assert any( (isinstance(self.mesh, x) for x in [meshes.MeshImFreq, meshes.MeshReFreq])), "Method invalid for this Gf"

        assert self.rank == 1, "Transpose only implemented for single mesh Greens functions"
        assert self.target_rank == 2, "Transpose only implemented for matrix valued Greens functions"

        d = np.transpose(self.data.copy(), (0, 2, 1))
        return Gf(mesh = self.mesh, data= d)

    def conjugate(self):
        """Conjugate of the Greens function.

        Returns
        -------
        G : Gf (copy)
            Conjugate of the Greens function.
        """
        return Gf(mesh = self.mesh, data= np.conj(self.data))

    def zero(self):
        """Set all values to zero."""
        self._data[:] = 0

    def from_L_G_R(self, L, G, R):
        r"""Matrix transform of the target space of a matrix valued Greens function.

        Sets the current Greens function :math:`g_{ab}` to the matrix transform of :math:`G_{cd}`
        using the left and right transform matrices :math:`L_{ac}` and :math:`R_{db}`.

        .. math::
            g_{ab} = \sum_{cd} L_{ac} G_{cd} R_{db}

        Parameters
        ----------

        L : (a, c) ndarray
            Left side transform matrix.
        G : Gf matrix valued target_shape == (c, d)
            Greens function to transform.
        R : (d, b) ndarray
            Right side transform matrix.

        Notes
        -----

        Only implemented for Greens functions with a single mesh.
        """

        assert self.rank == 1, "Only implemented for Greens functions with one mesh"
        assert self.target_rank == 2, "Matrix transform only valid for matrix valued Greens functions"

        assert len(L.shape) == 2, "L needs to be two dimensional"
        assert len(R.shape) == 2, "R needs to be two dimensional"

        assert L.shape[1] == G.target_shape[0], "Dimension mismatch between L and G"
        assert R.shape[0] == G.target_shape[1], "Dimension mismatch between G and R"

        assert L.shape[0] == self.target_shape[0], "Dimension mismatch between L and self"
        assert R.shape[1] == self.target_shape[1], "Dimension mismatch between R and self"

        if not L.strides == sorted(L.strides):
            L = L.copy(order='C')

        if not R.strides == sorted(R.strides):
            R = R.copy(order='C')

        wrapped_aux.set_from_gf_data_mul_LR(self.data, L, G.data, R)

    def total_density(self, *args, **kwargs):
        """Compute total density.

        Returns
        -------
        density : float
            Total density of the Greens function.

        Notes
        -----
        Only implemented for single mesh Greens function with a,
        Matsubara, real-frequency, or Legendre mesh.

        """
        return np.trace(gf_fnt.density(self, *args, **kwargs))

    #-----------------------------  IO  -----------------------------------
    
    def __reduce__(self):
        return call_factory_from_dict, (Gf, self.name, self.__reduce_to_dict__())

    def __reduce_to_dict__(self):
        d = {'mesh' : self._mesh, 'data' : self._data}
        return d

    _hdf5_format_ = 'Gf'

    @classmethod
    def __factory_from_dict__(cls, name, d):
        # Backward compatibility layer
        # Drop singularity from the grp and ignore it
        d.pop('singularity', None)
        # Backward compatibility layer
        # Drop indices from the grp and ignore it
        d.pop('indices', None)
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
        """Helper method for getting a view of the data.

        Parameters
        ----------

        x_window : optional
            The window of x variable (omega/omega_n/t/tau) for which data is requested.
        flatten_y: bool, optional
            If the Greens function is of size (1, 1) flatten the array as a 1d array.

        Returns
        -------

        (X, data) : tuple
            X is a 1d numpy array of the x variable inside the window requested.
            data is a 3d numpy array of dim (:,:, len(X)), the corresponding slice of data.
            If flatten_y is True and dim is (1, 1, *) it returns a 1d numpy array.
        """
        
        X = [x.imag for x in self.mesh] if isinstance(self.mesh, meshes.MeshImFreq) \
            else [x for x in self.mesh]
        
        X, data = np.array(X), self.data
        if x_window:
            # the slice due to clip option x_window
            sl = clip_array(X, *x_window) if x_window else slice(len(X))
            X, data = X[sl],  data[sl, :, :]
        if flatten_y and data.shape[1:3] == (1, 1):
            data = data[:, 0, 0]
        return X, data

#---------------------------------------------------------

from h5.formats import register_class, register_backward_compatibility_method
register_class (Gf)

# A backward compatility function
def bckwd(hdf_scheme):
    # we know scheme is of the form GfM1_x_M2_s/tv3
    m, t= hdf_scheme[2:], '' # get rid of Gf
    for suffix in ['_s', 'Tv3', 'Tv4'] : 
        if m.endswith(suffix) :
            m, t = m[:-len(suffix)], suffix
            break
    return { 'mesh': 'Mesh'+m }

# backward compatibility: suppress unrecognized GfIndices warnings
# when reading a Gf stored with triqs <3.2.x
warnings.filterwarnings("ignore", message="The hdf5 format GfIndices")

register_backward_compatibility_method("GfImFreq", "Gf", bckwd)
register_backward_compatibility_method("GfImTime", "Gf", bckwd)
register_backward_compatibility_method("GfLegendre", "Gf", bckwd)
register_backward_compatibility_method("GfReFreq", "Gf", bckwd)
register_backward_compatibility_method("GfReTime", "Gf", bckwd)

