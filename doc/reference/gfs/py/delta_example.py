#FIXME PS: find appropriate place for this!

from pytriqs.gf.local import *
from pytriqs.plot.mpl_interface import *

g = GfImFreq(beta=100.0, indices=[0])
d = GfImFreq(beta=100.0, indices=[0])
g <<= inverse(iOmega_n - Wilson(1.0))
d <<= Wilson(1.0)

oplot(delta(g))
oplot(d)

G = BlockGf(name_list = ('0','1'), block_list = (g,g), make_copies = False)
delta(G)
