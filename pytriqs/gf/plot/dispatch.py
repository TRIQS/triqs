import plot_base

#---------------------------------------------------------------
# A list of plot functions for 
#---------------------------------------------------------------

def imfreq(self, opt_dict):
    r"""
    Plot protocol for GfImFreq objects.

    Parameters
    ----------
    opt_dict: dictionary
              Can contain:
              - mode: string, default None
                      Mode to plot the Green's function in:
                      -- 'R': real part only
                      -- 'I': imaginary part only
              - x_window: tuple, default None 
                          (xmin,xmax)
              - name: string, default = ''
                      If not '', it remplaces the name of the function just for this plot.
    """
    return plot_base.plot_base( self, opt_dict,  r'$\omega_n$',
            lambda name : r'%s$(i\omega_n)$'%name, [x.imag for x in self.mesh] )

#-



#--------------   PLOT   ---------------------------------------


plot_fnt_table = {}

plot_fnt_table[((MeshImFreq,), 2)] = imfreq



def plot_dispatch(g, *args, **kw) :
    """ 
    Dispatch the plot according to type
    """
    
    
