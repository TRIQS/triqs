import numpy

class myObject:
  def _plot_(self, options):
    PI = numpy.pi
    xdata = numpy.arange(-PI,PI,0.1)
    ydata1 = numpy.cos(xdata)
    ydata2 = numpy.sin(xdata)
    return( [
              {'xdata': xdata, 'ydata': ydata1, 'label': 'Cos'},
              {'xdata': xdata, 'ydata': ydata2, 'label': 'Sin'}
            ] )

X = myObject()

from pytriqs.plot.mpl_interface import oplot
oplot(X,'-o')
