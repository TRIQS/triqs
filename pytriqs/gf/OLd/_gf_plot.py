from pytriqs.plot.protocol import clip_array
import numpy


class PlotWrapperPartialReduce:
    """ Internal Use"""

    def __init__(self, obj,  **opt):
        self.obj, self.opt = obj, opt

    def _plot_(self, Options):
        Options.update(self.opt)
        return self.obj._plot_(Options)

#----------------


def plot_base(self, opt_dict, xlabel, ylabel, X, allow_spectral_mode=False):
    r"""
    Plot protocol for Green's function objects.

    Parameters
    ----------
    opt_dict: dictionary
              Can contain:
              - mode: string, default None
                      Mode to plot the Green's function in:
                      -- 'R': real part only
                      -- 'I': imaginary part only
                      -- 'S': spectral function
              - x_window: tuple, default None
                          (xmin,xmax)
              - name: string, default = ''
                      If not '', it remplaces the name of the function just for this plot.
    xlabel: string
            Label to apply to the x axis.
    ylabel: string
            Label to apply to the y axis.
    X: list
       The x values the object can take, i.e. the mesh.
    allow_spectral_mode: boolean, default False
                         Can the spectral function be measured for this type of Green's function?

    Returns
    -------
    plot_data: list of dict
               Object passed to oplot to plot.
    """
    name = opt_dict.pop('name', self.name)  # consume it
    name_prefix = opt_dict.pop('name_prefix', '')  # consume it
    if name and name_prefix:
        raise ValueError, 'name and name_prefix cannot be used at the same time'
    if name_prefix:
        name_save, self.name = self.name, name or name_prefix

    rx = opt_dict.pop('x_window', None)  # consume it
    X = numpy.array(X).real
    sl = clip_array(X, *rx) if rx else slice(len(X)) # the slice due to clip option x_window

    def mdic(prefix, f):
        from itertools import product
        ind_range = product(*map(range,reversed(self.target_shape)))
        make_label = lambda ind: "%s%s %s" % (prefix,name,"_".join(map(str, reversed(ind))))
        make_data_sl = lambda ind: (sl,) + tuple(reversed(ind))
        return [{'xlabel': xlabel,
                 'ylabel': ylabel(self.name or name),
                 'xdata': X[sl],
                 'label': make_label(ind),
                 'ydata': f(self.data[make_data_sl(ind)])} for ind in ind_range]

    # backward compat.
    if 'RI' in opt_dict:
        assert 'mode' not in opt_dict, "Can not have both flags RI and mode."
        import warnings
        warnings.warn("oplot: 'RI' flag is deprecated, use 'mode' instead")
        opt_dict['mode'] = opt_dict.pop('RI', '')

    mode = opt_dict.pop('mode', '')
    if mode == '':
        res = mdic('Re ', lambda x: x.real) + mdic('Im ', lambda x: x.imag)
    elif mode == 'R':
        res = mdic('Re ', lambda x: x.real)
    elif mode == 'I':
        res = mdic('Im ', lambda x: x.imag)
    elif mode == 'S':
        if allow_spectral_mode:
            res = mdic('', lambda x: -1 / numpy.pi * x.imag)
        else:
            raise ValueError, "Cannot measure the spectral function for this type of Green's function."
    else:
        raise ValueError, "The 'mode' flag is meaningless. Expected 'R', 'I', or 'S' and I got %s." % mode

    res[0].update(opt_dict) # Add all other unused parameters to the dict
    if name_prefix:
        self.name = name_save
    return res

#------------------


def x_data_view(self, x_window=None, flatten_y=False):
    """
    :param x_window: the window of x variable (omega/omega_n/t/tau) for which data is requested
                      if None, take the full window
    :param flatten_y: If the Green function is of size (1, 1) flatten the array as a 1d array
    :rtype: a tuple (X, data) where
             * X is a 1d numpy of the x variable inside the window requested
             * data is a 3d numpy array of dim (:,:, len(X)), the corresponding slice of data
               If flatten_y is True and dim is (1, 1, *), returns a 1d numpy
    """
    from pytriqs.gf.local.gf import MeshImFreq
    X = [x.imag for x in self.mesh] if type(self.mesh) == MeshImFreq else [
        x for x in self.mesh]
    X, data = numpy.array(X), self.data
    if x_window:
        # the slice due to clip option x_window
        sl = clip_array(X, *x_window) if x_window else slice(len(X))
        X, data = X[sl],  data[sl, :, :]
    if flatten_y and data.shape[1:3] == (1, 1):
        data = data[:, 0, 0]
    return X, data
