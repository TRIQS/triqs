from pytriqs.plot.protocol import clip_array
import numpy

class PlotWrapperPartialReduce:
    """ Internal Use"""
    def __init__(self, obj,  **opt) :
        self.obj, self.opt = obj,opt
    def _plot_(self,Options) :
        Options.update(self.opt)
        return self.obj._plot_(Options)

#----------------

def plot_base (self, opt_dict, xlabel, ylabel, use_ris, X):
    """ Plot protocol. opt_dict can contain :
         * :param RIS: 'R', 'I', 'S', 'RI' [ default]
         * :param x_window: (xmin,xmax) or None [default]
         * :param Name: a string [default ='']. If not '', it remplaces the name of the function just for this plot.
    """
    Name = opt_dict.pop('name', '' )  # consume it
    NamePrefix = opt_dict.pop('NamePrefix', '' )  # consume it
    if Name and NamePrefix : raise ValueError, 'Name and NamePrefix can not be used at the same time'
    if NamePrefix : name_save, self.name = self.name, Name or NamePrefix

    rx = opt_dict.pop('x_window',None ) # consume it
    X =  numpy.array(X).real
    sl = clip_array (X, *rx) if rx else slice(len(X)) # the slice due to clip option x_window

    def mdic( prefix, f) :
       return [{'type' : "XY",
                'xlabel' : xlabel,
                'ylabel' : ylabel (self.name),
                'xdata' : X[sl],
                'label' : Name if Name else prefix + "%s_%s"%(i,j), 
                'ydata' : f( self.data[sl,i,j] ) } for i in range(self.target_shape[0]) for j in range(self.target_shape[1])]

    if use_ris :
        ris = opt_dict.pop('RI','RI')
        if   ris == "R" :
            res = mdic( 'Re ', lambda x : x.real)
        elif ris == "I" :
            res = mdic( 'Im ', lambda x : x.imag)
        elif ris == "S" :
            res = mdic( '', lambda x : -1/numpy.pi *x.imag)
        elif ris == 'RI' :
             res = mdic( 'Re ', lambda x : x.real) + mdic( 'Im ', lambda x : x.imag)
        else :
             raise ValueError, "RIS flags meaningless %s"%ris
    else:
        res = mdic( '', lambda x : x)

    if NamePrefix: self.name = name_save
    return res

#------------------

def x_data_view(self, x_window = None, flatten_y = False):
    """
    :param x_window: the window of x variable (omega/omega_n/t/tau) for which data is requested
                      if None, take the full window
    :param flatten_y: If the Green function is of size (1, 1) flatten the array as a 1d array
    :rtype: a tuple (X, data) where
             * X is a 1d numpy of the x variable inside the window requested
             * data is a 3d numpy array of dim (:,:, len(X)), the corresponding slice of data
               If flatten_y is True and dim is (1, 1, *), returns a 1d numpy
    """
    X = [x.imag for x in self.mesh] if type(self.mesh) == MeshImFreq else [x for x in self.mesh]
    X, data = numpy.array(X), self.data
    if x_window:
      sl = clip_array (X, *x_window) if x_window else slice(len(X)) # the slice due to clip option x_window
      X, data = X[sl],  data[sl,:,:]
    if flatten_y and data.shape[1:3]==(1, 1): data = data[:,0,0]
    return X, data


