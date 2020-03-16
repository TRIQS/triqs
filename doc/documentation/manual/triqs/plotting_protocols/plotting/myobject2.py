import numpy

class myObject:
  def _plot_(self, options):
    PI = numpy.pi
    xdata = numpy.arange(-PI,PI,0.1)
    phi = options.pop('phi',0)      # Crucial : removes the entry from the dict
    ydata1 = numpy.cos(xdata + phi)
    ydata2 = numpy.sin(xdata + phi)
    dict1 = {'xdata': xdata, 'ydata': ydata1, 'label': r'$x \rightarrow \cos (x + \phi), \quad \phi = %s$'%phi}
    dict2 = {'xdata': xdata, 'ydata': ydata2, 'label': r'$x \rightarrow \sin (x + \phi), \quad \phi = %s$'%phi}
    dict1.update(options)
    dict2.update(options)
    return [dict1, dict2]

X = myObject()

from pytriqs.plot.mpl_interface import oplot
oplot(X,'-o')
oplot(X,'-x', phi = 0.3)

