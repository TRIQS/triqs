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
import warnings, numpy as np
from pytriqs.gf.gf import Gf
from pytriqs.gf.meshes import MeshImFreq
from tool_gfloc import *

class GfImFreq(Gf) : 
    """
    Parameters (KEYWORD argument ONLY)
    ----------
    mesh: MeshImFreq, optional
          The mesh of the Green function
          If not present, it will be constructed from 
          the parameters beta, indices, [n_points], [statistic]

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

    singularity or tail : TailGfXXX, optional
                 One of the singularity of the module 'singularities'
                 If not present singularity is None.

    indices: GfIndices or list of str or list of list of str, Optional
             Optional string indices for the target space, to allow e.g g['eg', 'eg']
             list of list of str: the list of indices for each dimension.
             list of str: all indices are assumed to be the same for all dimensions.
   
    name: str
          The name of the Green function. For plotting.

    """
    def __init__(self, **kw): # enforce keyword only policy 
        """
          Same as Gf, but can rebuild the  mesh for backward compatibility
        """
        def delegate(self, mesh=None, data = None, target_shape=None, singularity = None, tail = None, indices = None, name='', n_points = 1025, beta = None, statistic = 'Fermion'):
            if mesh is None:
                assert isinstance(beta, (int, long, float)), "If the Mesh is not given, beta is mandatory and must be float"
                assert isinstance(n_points, int) and n_points >0, "n_points is crazy"
                mesh = MeshImFreq(beta, statistic, n_points)
        
            super(GfImFreq, self).__init__(
                      mesh = mesh, 
                      data = data, 
                      target_shape = target_shape,
                      singularity = tail or singularity,
                      _singularity_maker = make_singularity_maker(tail, singularity),
                      indices = indices, 
                      name = name) 

        delegate(self, **kw)

    #-------------- Old Tail operations. Deprecated -----------------------------------------------------

    def replace_by_tail_depr(self, start):
        d = self.data
        t = self.tail
        for n, om in enumerate(self.mesh):
            if n >= start: d[n,:,:] = t(om)

    def fit_tail_depr(self, fixed_coef, order_max, fit_start, fit_stop, replace_tail=True):
       """
       Fit the tail of the Green's function
       Input:
         - fixed_coef: a 3-dim array of known coefficients for the fit starting from the order -1
         - order_max: highest order in the fit
         - fit_start, fit_stop: fit the data between fit_start and fit_stop
       Output:
         On output all the data above fit_start is replaced by the fitted tail
         and the new moments are included in the Green's function
       """

       from scipy.optimize import leastsq
       # Turn known_coefs into a numpy array if ever it is not already the case
       known_coef = fixed_coef

       # Change the order_max
       # It is assumed that any known_coef will start at order -1
       self.tail.zero()
       self.tail.mask.fill(order_max)

       # Fill up two arrays with the frequencies and values over the range of interest
       ninit, nstop = 0, -1
       x = []
       for om in self.mesh:
         if (om.imag < fit_start): ninit = ninit+1
         if (om.imag <= fit_stop): nstop = nstop+1
         if (om.imag <= fit_stop and om.imag >= fit_start): x += [om]
       omegas = numpy.array(x)
       values = self.data[ninit:nstop+1,:,:]

       # Loop over the indices of the Green's function
       for n1,indR in enumerate(self.indicesR):
         for n2,indL in enumerate(self.indicesL):

           # Construct the part of the fitting functions which is known
           f_known = numpy.zeros((len(omegas)),numpy.complex)
           for order in range(len(known_coef[n1][n2])):
             f_known += known_coef[n1][n2][order]*omegas**(1-order)

           # Compute how many free parameters we have and give an initial guess
           len_param = order_max-len(known_coef[n1][n2])+2
           p0 = len_param*[1.0]

           # This is the function to be minimized, the diff between the original
           # data in values and the fitting function
           def fct(p):
             y_fct = 1.0*f_known
             for order in range(len_param):
               y_fct += p[order]*omegas**(1-len(known_coef[n1][n2])-order)
             y_fct -= values[:,n1,n2]
             return abs(y_fct)

           # Now call the minimizing function
           sol = leastsq(fct, p0, maxfev=1000*len_param)

           # Put the known and the new found moments in the tail
           for order in range(len(known_coef[n1][n2])):
             self.tail[order-1][n1,n2] = numpy.array([[ known_coef[n1][n2][order] ]])
           for order, moment in enumerate(sol[0]):
             self.tail[len(known_coef[n1][n2])+order-1][n1,n2] = numpy.array([[ moment ]])
       self.tail.mask.fill(order_max)
       # Replace then end of the Green's function by the tail
       if replace_tail: self.replace_by_tail_depr(ninit);
       

