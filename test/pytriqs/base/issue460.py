from __future__ import unicode_literals
from pytriqs.gf import *
from pytriqs.utility.comparison_tests import *
import numpy as np

gw = GfImFreq(indices = [1], beta = 50)
g2w = GfImFreq(indices = [1], beta = 50)

gw << SemiCircular(half_bandwidth = 1)

g2w << gw * gw



