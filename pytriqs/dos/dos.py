
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

import types,string,itertools
from operator import isSequenceType
import numpy

class DOS :
    r"""
* Stores a density of state of fermions  

.. math::

 \rho (\epsilon) \equiv \sum'_k \delta( \epsilon - \epsilon_k)

* The sum is normalized 

.. math::

  \int_{-\infty}^{\infty} d\epsilon \rho (\epsilon) = 1

* Implement :ref:`Plot Protocol <plotting>`.

    """
    def __init__(self, eps, rho, name = ''):
        """  
Parameters 
------------
eps : 1d array-type
    eps[i] is value of epsilon.
rho : 1d array-type
    The corresponding value of the dos. 
name : string
     Name of the dos/orbital

        """
        self.name = name
        try :
            self.eps = numpy.array( eps )
            assert  len(self.eps.shape) ==1
        except :
            raise RuntimeError, "Argument eps mismatch"
        try :
            self.rho = numpy.array( rho )
            assert  len(self.rho.shape) ==1
        except :
            raise RuntimeError, "Argument rho mismatch"
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

    def _plot_(self, Options) : 
        return  [ {'type' : "XY", 'label' : self.name, 'xlabel' :r'$\epsilon$', 'ylabel' : r'%s$(\epsilon)$'%self.name, 'xdata' : self.eps,'ydata' : self.rho } ]

    def density(self,mu=0):
        """Calculates the density of free fermions for the given DOS for chemical potential mu."""

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
    """   
    Read the DOS from a file 

    :param Filename:  a string  : name of the file
    :param name: name of the DOS
    :param single_orbital: can be None or an integer.
                    
    :rtype: 
       * if single_orbital== None, returns a tuple of DOS (even if there is one dos !).
       * If single_orbital==i, return only ONE DOS corresponding to ith orbital (starting at 1).

    Format of the file :   
        * N_orbitals +1 columns, 
        * the first column is the value of epsilon
        * the N_orbitals other columns are the values of the dos for various orbitals
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
    """
    * A DOS class, but constructed from a function.
    
    * The number of points can be variable and self-adjusted in the Hilbert transform to adapt precision.
    """
    def __init__(self, function, x_min, x_max, n_pts=100, name=''):
        """
        :param function: * a function :math:`\\epsilon \\rightarrow \\rho(\\epsilon)`
                         * The result type can be anything from which a 1d-array can be constructed by numpy
        :param x_min,x_max: Bound of the mesh (domain of the function). 
        :param n_pts: Number of points in the mesh.
        :param name: Name of the DOS.
        """
        assert callable(function), "function is not callable"
        self.function,self.x_min,self.x_max = function,x_min,x_max
        try :
            e = function(0.001)
            len(numpy.array(e).shape) ==1
        except :
            raise RuntimeError , "Value of the function must be a 1d-array"
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

from pytriqs.archive.hdf_archive_schemes import register_class
register_class (DOS)

