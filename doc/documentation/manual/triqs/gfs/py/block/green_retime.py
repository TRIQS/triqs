from math import pi
from triqs.gf import *

gw = GfReFreq(indices = [1], window = (-5, 5), n_points = 1001, name = "egBlock")
gw << SemiCircular(2.0)

gt = GfReTime(indices = [1], mesh = make_adjoint_mesh(gw.mesh), name = "egBlock")
gt << Fourier(gw)

from triqs.plot.mpl_interface import oplot
oplot(gt.imag, '-o')
