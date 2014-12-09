from math import pi
from pytriqs.gf.local import *

gw = GfReFreq(indices = [1], window = (-5, 5), n_points = 101, name = "egBlock")
gw << SemiCircular(2.0)

gt = GfReTime(indices = [1], window = (-5*2*pi*(100.0/101.0),5*2*pi*(100.0/101.0)), n_points = 101, name = "egBlock")
gt << InverseFourier(gw)

from pytriqs.plot.mpl_interface import oplot
oplot(gt.imag, '-o')


