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

__all__ = ['plt','oplot','subplots','figsize_default','use_amsmath']

import numpy, matplotlib as mpl, matplotlib.pylab as plt
from protocol import plot_protocol_apply
from matplotlib import rc

try:
  subplots = mpl.pyplot.subplots
except:
  def subplots(nrows=1, ncols=1, sharex=False, sharey=False, squeeze=True, subplot_kw=None, **fig_kw):
    print "subplots not supported"
    return plt.figure(1), [plt.subplot(nrows,ncols,x+1) for x in range(nrows*ncols)]

figsize_default = (12,8)


def oplot (*ob_list, **opt_dict) :
    """
    A thin layer above pyplot.plot function that allows plotting objects with
    plot protocol as well as arrays.
    Options are the same as for the pyplot.plot function.
    """
    plt.figure(num=opt_dict.pop('num', 1),
               figsize=opt_dict.pop('figsize', figsize_default))
    __oplot_impl(plt.plot, plt.xlabel,plt.ylabel,plt.legend, *ob_list,**opt_dict)
    # remove this in the notebook...
    #if hasattr(plt.figure(1), "show") : plt.figure(1).show()

mpl.axes.Axes.oplot = lambda self, *ob_list, **opt_dict : __oplot_impl(self.plot,self.set_xlabel, self.set_ylabel, self.legend, *ob_list,**opt_dict)

def __oplot_impl (plot_fct, xlabel_fct, ylabel_fct, legend_fct, *ob_list, **opt_dict) :
    """
    A thin layer above pyplot.plot function that allows plotting objects with
    plot protocol as well as arrays.
    Options are the same as for the pyplot.plot function.
    """

    def objs() : # filter the arguments for the format strings ...
        i, l = 0, []
        while i< len (ob_list) :
            if i < len(ob_list) - 1 and type(ob_list[i+1]) == type("") :
                res = ob_list[i], [ ob_list[i+1] ]
                i+=2
            else :
                res =  ob_list[i], [ ]
                i+=1
            yield res

    for ob, OptionsList in objs() :
        opt = opt_dict.copy() # the plot protocol will consume the dict....
        #ob2 = eval_expr_or_pass (ob) # if it is a lazy_expr, it is time to evaluate it !
        for curvedata in plot_protocol_apply(ob, opt, plt.xlim ) :
            X,Y = curvedata['xdata'],curvedata['ydata']
            d = { 'label' :  curvedata['label'] }
            d.update(opt)
            try :
                plot_fct(X,Y,*OptionsList,**d)
            except TypeError, e:
                import re
                m = re.search('(?<=There is no line property )"(.*)"', str(e) )
                if m :
                   raise RuntimeError, "Option %s is not understood in plot function : it is not an option of the object to be plotted, nor a matplotlib option"%m.group(0)
                else :
                   raise
            if 'xlabel' in curvedata : xlabel_fct(curvedata['xlabel'], fontsize=20)
            if 'ylabel' in curvedata : ylabel_fct(curvedata['ylabel'], fontsize=20)

    legend_fct(loc = 1) #legend is built from the label

def use_amsmath():
  rc('text', usetex=True)
  rc('text.latex', preamble="\usepackage{amsmath}")

