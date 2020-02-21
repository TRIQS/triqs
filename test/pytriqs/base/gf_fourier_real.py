from __future__ import unicode_literals
from pytriqs.gf import *

# ==== Matrix-valued Green functions

gw = GfReFreq(indices = [1], window = (-5, 5), n_points = 1001, name = "egBlock")
gw << SemiCircular(2.0)

gt = make_gf_from_inverse_fourier(gw)
