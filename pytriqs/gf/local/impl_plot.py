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
                'label' : Name if Name else prefix + B.name ,
                'ydata' : f( B.data[sl,0,0] ) } for (i,j,B) in self ]

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
