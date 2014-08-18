from tools import get_indices_in_dict
import _gf_plot 
import numpy

def init( mesh= None, shape =None, name = 'g', **d):
    """
    """
    if mesh is None : 
        from gf import MeshImTime
        if 'beta' not in d : raise ValueError, "beta not provided"
        beta = float(d.pop('beta'))
        n_max = d.pop('n_points',10000)
        stat = d.pop('statistic','Fermion')
        kind = d.pop('kind','half_bins') 
        mesh = MeshImTime(beta,stat,n_max,kind)
    
    indices_pack = get_indices_in_dict(d)
    if not shape : 
      assert indices_pack, "No shape, no indices !"
      indicesL, indicesR = indices_pack
      shape = len(indicesL),len(indicesR)
    
    #data = d.pop('data') if 'data' in d else numpy.zeros((len(mesh),N1,N2), self.dtype )
    #tail = d.pop('tail') if 'tail' in d else TailGf(shape = (N1,N2))
    #symmetry = d.pop('symmetry', Nothing())

    return (mesh, shape, indices_pack, name), {}
    #return mesh, data, tail, symmetry, indices_pack, name
    
#--------------   PLOT   ---------------------------------------

def plot(self, opt_dict):
    """ Plot protocol. opt_dict can contain : 
         * :param RI: 'R', 'I', 'RI' [ default] 
         * :param x_window: (xmin,xmax) or None [default]
         * :param name: a string [default ='']. If not '', it remplaces the name of the function just for this plot.
    """
    has_complex_value = False
    return _gf_plot.plot_base( self, opt_dict,  r'$\tau$', lambda name : r'%s$(\tau)$'%name, has_complex_value ,  list(self.mesh) )

