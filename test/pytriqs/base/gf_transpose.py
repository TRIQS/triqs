from pytriqs.gf import *

g = GfImFreq(indices = [list(range(3)), list(range(5))], beta = 40, n_points = 1000)
gt = g.transpose()

assert g.data.shape == (2000, 3, 5)
assert gt.data.shape == (2000, 5, 3)
