from gf import GfImFreq_cython, MeshImFreq, TailGf
from gf_generic import GfGeneric
import numpy
from scipy.optimize import leastsq
from tools import get_indices_in_dict
from nothing import Nothing
import impl_plot

class GfImFreq ( GfGeneric, GfImFreq_cython ) :
    def __init__(self, **d):
        """
        The constructor have two variants : you can either provide the mesh in
        Matsubara frequencies yourself, or give the parameters to build it.
        All parameters must be given with keyword arguments.

        GfImFreq(indices, beta, statistic, n_points, data, tail, name)

              * ``indices``:  a list of indices names of the block
              * ``beta``:  Inverse Temperature
              * ``statistic``:  'F' or 'B'
              * ``n_points``:  Number of Matsubara frequencies
              * ``data``:   A numpy array of dimensions (len(indices),len(indices),n_points) representing the value of the Green function on the mesh.
              * ``tail``:  the tail
              * ``name``:  a name of the GF

        GfImFreq(indices, mesh, data, tail, name)

              * ``indices``:  a list of indices names of the block
              * ``mesh``:  a MeshGf object, such that mesh.TypeGF== GF_Type.Imaginary_Frequency
              * ``data``:   A numpy array of dimensions (len(indices),len(indices),:) representing the value of the Green function on the mesh.
              * ``tail``:  the tail
              * ``name``:  a name of the GF

        .. warning::

          The Green function take a **view** of the array data, and a **reference** to the tail.

        """
        mesh = d.pop('mesh',None)
        if mesh is None :
            if 'beta' not in d : raise ValueError, "beta not provided"
            beta = float(d.pop('beta'))
            n_max = d.pop('n_points',1025)
            stat = d.pop('statistic','F')
            mesh = MeshImFreq(beta,stat,n_max)

        self.dtype = numpy.complex_
        indices_pack = get_indices_in_dict(d)
        indicesL, indicesR = indices_pack
        N1, N2 = len(indicesL),len(indicesR)
        data = d.pop('data') if 'data' in d else numpy.zeros((len(mesh),N1,N2), self.dtype )
        tail = d.pop('tail') if 'tail' in d else TailGf(shape = (N1,N2), size=10, order_min=-1)
        symmetry = d.pop('symmetry', Nothing())
        name =  d.pop('name','g')
        assert len(d) ==0, "Unknown parameters in GFBloc constructions %s"%d.keys()

        GfGeneric.__init__(self, mesh, data, tail, symmetry, indices_pack, name, GfImFreq)
        GfImFreq_cython.__init__(self, mesh, data, tail)

    #--------------   PLOT   ---------------------------------------

    def _plot_(self, opt_dict):
        """ Plot protocol. opt_dict can contain :
             * :param RIS: 'R', 'I', 'S', 'RI' [ default]
             * :param x_window: (xmin,xmax) or None [default]
             * :param name: a string [default ='']. If not '', it remplaces the name of the function just for this plot.
        """
        return impl_plot.plot_base( self, opt_dict,  r'$\omega_n$',
                lambda name : r'%s$(i\omega_n)$'%name, True, [x.imag for x in self.mesh] )

    #--------------   OTHER OPERATIONS -----------------------------------------------------

    def replace_by_tail(self,start) :
        d = self.data
        t = self.tail
        for n, om in enumerate(self.mesh) :
            if n >= start : d[n,:,:] = t(om)

    def fit_tail(self, fixed_coef, order_max, fit_start, fit_stop, replace_tail = True):
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
       self.tail = TailGf(shape = (self.N1,self.N2), size = order_max+2, order_min = -1)

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

       # Replace then end of the Green's function by the tail
       if replace_tail: self.replace_by_tail(ninit);
