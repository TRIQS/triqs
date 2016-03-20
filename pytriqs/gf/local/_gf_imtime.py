from _imp_tools import get_indices_in_dict, get_indices_in_dict_tv
import _gf_plot 
import numpy

def init( mesh = None, shape = None, name = 'g', **kwargs):
    """
    """
    if mesh is None: 
        from gf import MeshImTime
        if 'beta' not in kwargs: raise ValueError, "beta not provided"
        beta = float(kwargs.pop('beta'))
        n_max = kwargs.pop('n_points',10000)
        stat = kwargs.pop('statistic','Fermion')
        mesh = MeshImTime(beta,stat,n_max)
    
    indices_pack = get_indices_in_dict(kwargs)
    if not shape: 
      assert indices_pack, "No shape, no indices !"
      indicesL, indicesR = indices_pack
      shape = len(indicesL),len(indicesR)
    if kwargs: raise ValueError, "GfImTime: Unused parameters %s were passed."%kwargs.keys()


    # FIXME 
    #data = kwargs.pop('data') if 'data' in kwargs else numpy.zeros((len(mesh),N1,N2), self.dtype )
    #tail = kwargs.pop('tail') if 'tail' in kwargs else TailGf(shape = (N1,N2))
    #symmetry = kwargs.pop('symmetry', Nothing())

    return (mesh, shape, indices_pack, name), {}
    #return mesh, data, tail, symmetry, indices_pack, name

def init_tv( mesh = None, shape = None, name = 'g', **kwargs):
    """
    """
    if mesh is None: 
        from gf import MeshImTime
        if 'beta' not in kwargs: raise ValueError, "beta not provided"
        beta = float(kwargs.pop('beta'))
        n_max = kwargs.pop('n_points',10000)
        stat = kwargs.pop('statistic','Fermion')
        mesh = MeshImTime(beta,stat,n_max)
    
    indices_pack = get_indices_in_dict_tv(kwargs)
    if not shape: 
      assert indices_pack, "No shape, no indices !"
      indices = indices_pack
      shape = [len(x) for x in indices]
    if kwargs: raise ValueError, "GfImTime: Unused parameters %s were passed."%kwargs.keys()

    return (mesh, shape, indices_pack, name), {}

#--------------   PLOT   ---------------------------------------

def plot(self, opt_dict):
    """ Plot protocol. opt_dict can contain: 
         *:param RI: 'R', 'I', 'RI' [ default] 
         *:param x_window: (xmin,xmax) or None [default]
         *:param name: a string [default = '']. If not '', it remplaces the name of the function just for this plot.
    """
    return _gf_plot.plot_base( self, opt_dict,  r'$\tau$', lambda name : r'%s$(\tau)$'%name, True ,  list(self.mesh) )


