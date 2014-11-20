from scipy.optimize import leastsq
from tools import get_indices_in_dict
import _gf_plot 
import numpy

def init( mesh=None, shape=None, name='g', **kwargs):
    """
    """
    if mesh is None:
        from gf import MeshImFreq
        if 'beta' not in kwargs: raise ValueError, "beta not provided"
        beta = float(kwargs.pop('beta'))
        n_points = kwargs.pop('n_points',1025)
        stat = kwargs.pop('statistic','Fermion')
        positive_only = kwargs.pop('positive_only',True)
        mesh = MeshImFreq(beta,stat,n_points, positive_only)
    
    indices_pack = get_indices_in_dict(kwargs)
    if not shape: 
      assert indices_pack, "No shape, no indices !"
      indicesL, indicesR = indices_pack
      shape = len(indicesL),len(indicesR)
    if kwargs: raise ValueError, "GfImFreq: Unused parameters %s were passed."%kwargs.keys()


    # FIXME why is this here and uncommented?    
    #data = kwargs.pop('data') if 'data' in kwargs else numpy.zeros((len(mesh),N1,N2), self.dtype )
    #tail = kwargs.pop('tail') if 'tail' in kwargs else TailGf(shape = (N1,N2))
    #symmetry = kwargs.pop('symmetry', Nothing())
    #return mesh, data, tail, symmetry, indices_pack, name

    return (mesh, shape, indices_pack, name), {}

#--------------   PLOT   ---------------------------------------

def plot(self, opt_dict):
    """ Plot protocol. opt_dict can contain:
         *:param RIS: 'R', 'I', 'S', 'RI' [ default]
         *:param x_window: (xmin,xmax) or None [default]
         *:param name: a string [default = '']. If not '', it remplaces the name of the function just for this plot.
    """
    return _gf_plot.plot_base( self, opt_dict,  r'$\omega_n$',
            lambda name : r'%s$(i\omega_n)$'%name, True, [x.imag for x in self.mesh] )

#--------------   OTHER OPERATIONS -----------------------------------------------------

def replace_by_tail_depr(self,start):
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
