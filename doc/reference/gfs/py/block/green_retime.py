from pytriqs.gf.local import *

gw = GfReFreq(indices = [1], window = (-5, 5), n_points = 300, name = "egBlock")
gw << SemiCircular(2.0)

# This is a GfReTime
gt = gw.inverse_fourier()

from pytriqs.plot.mpl_interface import oplot
oplot(gt.imag, '-o')


