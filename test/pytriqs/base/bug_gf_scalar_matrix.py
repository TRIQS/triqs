
import numpy as np
from pytriqs.gf import GfImFreq
from pytriqs.utility.comparison_tests import assert_gfs_are_close

scalar = 1.0
matrix = np.random.random((2, 2))

A = GfImFreq(name='up', statistic='Fermion', beta=1.0, n_points=400, indices=[0, 1])
B = A.copy()

A << scalar
A << A + matrix

B << scalar + matrix

assert_gfs_are_close(A, B)
