# Copyright (c) 2013-2016 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2013-2016 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020 Simons Foundation
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
# Authors: Thomas Ayral, Michel Ferrero, Olivier Parcollet, Priyanka Seth, Nils Wentzell


r"""
Density-of-states classes and a text-file reader.

Defines :class:`DOS` and :class:`DOSFromFunction`, plus the
:func:`dos_from_file` helper. Both classes implement the TRIQS plot
protocol so they can be passed directly to
:func:`triqs.plot.mpl_interface.oplot`.
"""

import types,string,itertools
import numpy

class DOS :
    r"""
    Density of states of fermions on a 1D epsilon grid.

    Stores a discrete representation of

    .. math::

        \rho(\epsilon) \equiv \sum_k^{'} \delta(\epsilon - \epsilon_k),

    normalised so that :math:`\int_{-\infty}^{\infty} d\epsilon\,
    \rho(\epsilon) = 1`.

    Parameters
    ----------
    eps : array-like
        1D array of energy values :math:`\epsilon_i`.
    rho : array-like
        1D array of DOS values :math:`\rho(\epsilon_i)`, same length as
        ``eps``.
    name : str, optional
        Name of the DOS / orbital, used as the plot label. Default ``''``.

    Attributes
    ----------
    eps : numpy.ndarray
        Energy grid.
    rho : numpy.ndarray
        DOS values on the grid.
    name : str
        Name of the DOS / orbital.

    Notes
    -----
    The class implements the TRIQS plot protocol, so
    instances can be passed directly to
    :func:`triqs.plot.mpl_interface.oplot`. HDF5 read/write is registered
    via :func:`h5.formats.register_class`.
    """
    def __init__(self, eps, rho, name = ''):
        self.name = name
        try :
            self.eps = numpy.array( eps )
            assert  len(self.eps.shape) ==1
        except :
            raise RuntimeError("Argument eps mismatch")
        try :
            self.rho = numpy.array( rho )
            assert  len(self.rho.shape) ==1
        except :
            raise RuntimeError("Argument rho mismatch")
        assert self.eps.shape[0] == self.rho.shape[0], "Dimensions of eps and rho do not match"
        
        
    #-------------------------------------------------------------

    def __reduce__(self) : 
        return self.__class__, (self.eps,self.rho, self.name)

    def __reduce_to_dict__(self) :
        return {'epsilon' : self.eps, 'rho': self.rho}

    @classmethod
    def __factory_from_dict__(cls,name, D) :
        return cls(D['epsilon'],D['rho'], name)
 
    def __repr__(self) : 
        return  """
        DOS object :
        """%self.__dict__ 

    def copy(self):
        """Return an independent :class:`DOS` with the same ``eps``, ``rho`` and ``name``."""
        return DOS(self.eps,self.rho,self.name)

    def _plot_(self, Options) :
        return  [ {'label' : self.name, 'xlabel' :r'$\epsilon$', 'ylabel' : r'%s$(\epsilon)$'%self.name, 'xdata' : self.eps,'ydata' : self.rho } ]

    def density(self,mu=0):
        r"""
        Integrated density of free fermions up to chemical potential ``mu``.

        Approximates :math:`\int_{-\infty}^{\mu} d\epsilon\, \rho(\epsilon)`
        by the trapezoidal rule on the stored ``eps`` / ``rho`` arrays,
        with a linear interpolation across the partial bin straddling
        ``mu``.

        Parameters
        ----------
        mu : float, optional
            Chemical potential. Default 0.

        Returns
        -------
        float
            Filling :math:`\int_{-\infty}^{\mu} \rho(\epsilon)\, d\epsilon`.
        """

        dens = 0.0
        a = [ (e>mu) for e in self.eps ]
        try:
            ind = a.index(True)
        except:
            ind = self.eps.shape[0]

        de = self.eps[1]-self.eps[0]
        #for e,r in itertools.izip(self.eps[0:ind],self.rho[0:ind]):
        #    dens += r
        dens = (sum(self.rho[0:ind]) - self.rho[0]/2.0 - self.rho[ind-1]/2.0) * de
        #dens2 = dens + (self.rho[ind-1]/2.0 + self.rho[ind]/2.0) * de
        if (ind<self.eps.shape[0]): dens += (mu-self.eps[ind-1]) * (self.rho[ind-1] + self.rho[ind])/2.0
        return dens 

##########################################################################

def dos_from_file(Filename, name = '', single_orbital = None):
    r"""
    Read a (multi-)orbital DOS from a whitespace-separated text file.

    Parameters
    ----------
    Filename : str
        Path to the text file containing the DOS data.
    name : str, optional
        Name to attach to the resulting :class:`DOS` object(s). Default
        ``''``.
    single_orbital : int, optional
        If given, return only the DOS for the ``single_orbital``-th
        column (1-based). If ``None`` (default), return one :class:`DOS`
        per orbital column.

    Returns
    -------
    DOS or list of DOS
        A single :class:`DOS` when ``single_orbital`` is set, otherwise a
        list with one :class:`DOS` per orbital column (even when there is
        only one orbital).

    Notes
    -----
    The file format has ``N_orbitals + 1`` whitespace-separated columns
    per row: the first column holds energy values :math:`\epsilon_i` and
    the remaining columns hold the DOS values for each orbital.
    """
    f = open(Filename); s=''
    while not(s.strip()) :
        s= f.readline()
        assert s, "File is empty !"
    N_Orbitals = len (s.split()) - 1
    assert N_Orbitals >0, "File : wrong format"
    # not very safe :  fromfile routine can crashes if given non numerics
    r = numpy.fromfile(Filename,sep=' ')
    l,div  =  r.shape[0], N_Orbitals +1 
    assert l%(div)==0,"File does not contains N*%d numbers !"%(div)
    r.shape =  l//(div) , div # reshape the array
    eps = r[:,0]
    if single_orbital : 
        assert single_orbital>0 and single_orbital <= N_Orbitals, " single_orbital  "
        return DOS (r[:,0] ,r[:,single_orbital], name)
    else :
        return [  DOS (r[:,0] ,r[:,i +1 ], name) for i in range (N_Orbitals)]


##########################################################################

class DOSFromFunction(DOS):
    r"""
    :class:`DOS` constructed by sampling a callable :math:`\rho(\epsilon)`.

    Evaluates ``function`` on an equispaced 1D mesh of ``n_pts`` points
    between ``x_min`` and ``x_max`` and stores the result as a
    :class:`DOS`. The mesh can be re-sampled at runtime to refine
    precision -- :meth:`HilbertTransform.__call__` exploits this via its
    ``n_points_integral`` and ``test_convergence`` arguments.

    Parameters
    ----------
    function : callable
        Function :math:`\epsilon \mapsto \rho(\epsilon)`. The return value
        must be convertible to a 1D numpy array.
    x_min : float
        Lower bound of the mesh (domain of ``function``).
    x_max : float
        Upper bound of the mesh.
    n_pts : int, optional
        Number of points in the mesh. Default 100.
    name : str, optional
        Name of the DOS. Default ``''``.

    Attributes
    ----------
    function : callable
        The callable supplied at construction.
    x_min : float
        Lower mesh bound.
    x_max : float
        Upper mesh bound.
    """
    def __init__(self, function, x_min, x_max, n_pts=100, name=''):
        assert callable(function), "function is not callable"
        self.function,self.x_min,self.x_max = function,x_min,x_max
        try :
            e = function(0.001)
            len(numpy.array(e).shape) ==1
        except :
            raise RuntimeError("Value of the function must be a 1d-array")
        self.__f(n_pts) # compute arrays
        DOS.__init__(self,self.eps,self.rho,name) 
        
    #-------------------------------------------------------------
    
    def __reduce__(self) : 
        return  self.__class__, (self.function,self.x_min, self.x_max, len(self.eps), self.name)
    
    #-------------------------------------------------------------
  
    def __f(self,N) :
        r = (self.x_max - self.x_min)/float(N-1)
        self.eps  = numpy.array( [self.x_min + r* i for i in range(N) ] )
        self.rho  = numpy.array( [self.function(e) for e in self.eps])

#-----------------------------------------------------
#  Register the class for HDFArchive
#-----------------------------------------------------

from h5.formats import register_class
register_class (DOS)

