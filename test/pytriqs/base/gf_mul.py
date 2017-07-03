from pytriqs.gf import *

gw=GfImFreq(n_points = 5, beta=1.,statistic="Fermion", indices=[''])

g = GfImTime(indices=[0], beta=10)
g2 = g.copy()
g2 << g*g

gs = GfImTime(target_shape = (), beta=10)
gs2 = gs.copy()
gs2 << gs * gs
