from pytriqs.gf import *
from pytriqs.utility.comparison_tests import *
import numpy as np

gw = GfImFreq(indices = [1], beta = 50)
g2w = GfImFreq(indices = [1], beta = 50)

gw << SemiCircular(half_bandwidth = 1)
print 'gw.tail =', gw.tail, '\n'

g2w << gw * gw
print 'g2w.tail =', g2w.tail

t = gw.tail.copy()
t = t*t
assert_arrays_are_close(t.data[:7,:,:], g2w.tail.data[:7,:,:])


