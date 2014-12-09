
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

from pytriqs.gf.local import *
import pytriqs.utility.mpi as mpi
from itertools import *
import inspect
import copy,numpy

class SumkDiscrete:
    """
      INTERNAL USE
      The function to compute \[ G \leftarrow \sum_k (\omega + \mu - eps_k - Sigma(k,\omega))^{-1} \]
      for GF functions with blocks of the size of the matrix eps_k with a discrete sum.
      The class contains the discretized hoppings and points in the arrays
      Hopping, BZ_Points,BZ_weights,Mu_Pattern,Overlap (IF non orthogonal)
      It can also generate a grid (ComputeGrid) for a regular grid or a Gauss-Legendre sum.
    """
    def __init__ (self, dim, gf_struct, orthogonal_basis = True ):
	"""  
	Just constructs the arrays, but without initializing them
	- dim is the dimension
	- gf_struct : Indices of the Green function
	- orthogonal_basis : True by default
	"""
        self.__GFBLOC_Structure = copy.deepcopy(gf_struct)
        self.orthogonal_basis,self.dim = orthogonal_basis,dim

   #-------------------------------------------------------------

    def resize_arrays (self, nk):
	"""  
	Just constructs the arrays, but without initializing them
	- nk : total number of k points
	"""
        # constructs the arrays.
        no = len(self.__GFBLOC_Structure)
        self.Hopping    = numpy.zeros([nk,no,no],numpy.complex_)   # t(k_index,a,b)
        self.BZ_Points  = numpy.zeros([nk,self.dim],numpy.float_)      # k(k_index,:)
        self.BZ_weights = numpy.ones([nk],numpy.float_)/ float(nk) # w(k_kindex) ,  default normalisation 
        self.Mu_Pattern  =  numpy.identity(no,numpy.complex_) if self.orthogonal_basis else numpy.zeros([no,no,nk],numpy.complex_)
        self.Overlap = numpy.array(self.Mu_Pattern, copy=True)

   #-------------------------------------------------------------
    
    def __get_GFBloc_Structure(self) :
        """Returns the ONLY block indices accepted for the G and Sigma argument of the 
        SumK function"""
        return self.__GFBLOC_Structure

    GFBlocIndices = property(__get_GFBloc_Structure)

    #-------------------------------------------------------------

    def __call__ (self, Sigma, mu=0, eta=0, field=None, epsilon_hat=None, result=None, selected_blocks=()):
	""" 
	- Computes :
	   result <- \[ \sum_k (\omega + \mu - field - t(k) - Sigma(k,\omega)) \]
           if result is None, it returns a new GF with the results.
           otherwise, result must be a GF, in which the calculation is done, and which is then returned.
           (this allows chain calculation : SK(mu = mu,Sigma = Sigma, result = G).total_density()
           which computes the sumK into G,  and returns the density of G.
  
        - Sigma can be a X, or a function k-> X or a function k,eps ->X where  : 
	    - k is expected to be a 1d-numpy array of size self.dim of float, 
	      containing the k vector in the basis of the RBZ  (i.e.  -0.5< k_i <0.5)
            - eps is t(k)
	    - X is anything such that X[BlockName] can be added/subtracted to a GFBloc for BlockName in selected_blocks.
	      e.g. X can be a BlockGf(with at least the selected_blocks), or a dictionnary Blockname -> array
	      if the array has the same dimension as the GF blocks (for example to add a static Sigma).

        - field : Any k independant object to be added to the GF 

        - epsilon_hat : a function of eps_k returning a matrix, the dimensions of Sigma

        - selected_blocks : The calculation is done with the SAME t(k) for all blocks. If this list is not None
	  only the blocks in this list are calculated.
	  e.g. G and Sigma have block indices 'up' and 'down'. 
	       if selected_blocks ==None : 'up' and 'down' are calculated
	       if selected_blocks == ['up'] : only 'up' is calculated. 'down' is 0.


        """
        S = Sigma.view_selected_blocks(selected_blocks) if selected_blocks else Sigma
        Gres = result if result else Sigma.copy() 
        G = Gres.view_selected_blocks(selected_blocks) if selected_blocks else Gres

        # check input
        assert self.orthogonal_basis, "Local_G : must be orthogonal. non ortho cases not checked."
        assert isinstance(G,BlockGf), "G must be a BlockGf"
        assert len(list(set([g.N1 for i,g in G]))) == 1
        assert self.BZ_weights.shape[0] == self.n_kpts(), "Internal Error"
        no = list(set([g.N1 for i,g in G]))[0]
        Sigma_Nargs = len(inspect.getargspec(Sigma)[0]) if callable (Sigma) else 0
        assert Sigma_Nargs <=2 , "Sigma function is not of the correct type. See Documentation"

        # Initialize
        G.zero()
        tmp,tmp2 = G.copy(),G.copy()
        mupat = mu * numpy.identity(no, numpy.complex_)
        tmp << iOmega_n
        if field != None : tmp -= field 
        if Sigma_Nargs==0: tmp -= Sigma  # substract Sigma once for all

        # Loop on k points...
        for w, k, eps_k in izip(*[mpi.slice_array(A) for A in [self.BZ_weights, self.BZ_Points, self.Hopping]]):

            eps_hat = epsilon_hat(eps_k) if epsilon_hat else eps_k
            tmp2 << tmp
            tmp2 -= tmp2.n_blocks * [eps_hat - mupat]

            if Sigma_Nargs == 1: tmp2 -= Sigma (k)
            elif Sigma_Nargs ==2: tmp2 -= Sigma (k,eps_k)

            tmp2.invert()
            tmp2 *= w
            G += tmp2

        G << mpi.all_reduce(mpi.world,G,lambda x,y : x+y)
        mpi.barrier()

        return Gres

    #-------------------------------------------------------------

    def n_kpts(self) : 
	""" Returns the number of k points"""
	return self.BZ_Points.shape[0]
