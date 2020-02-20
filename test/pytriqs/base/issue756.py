from math import pi
from pytriqs.gf import *

gw = GfReFreq(indices = [1], window = (-5, 5), n_points = 1001, name = "egBlock")
gw << SemiCircular(2.0)

gt = make_gf_from_fourier(gw)
