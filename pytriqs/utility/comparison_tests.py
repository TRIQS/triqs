from __future__ import unicode_literals
from builtins import zip
import numpy as np

def assert_arrays_are_close(a, b, precision = 1.e-6):
    d = np.amax(np.abs(a - b))
    assert  d< precision, "Arrays are different. Difference is %s.\n %s \n\n --------- \n\n %s"%(d,a,b)

def assert_array_close_to_scalar(a, x, precision = 1.e-6):
    assert_arrays_are_close(a, np.identity(a.shape[0])*(x), precision)

def assert_gfs_are_close(a, b, precision = 1.e-6):
    assert type(a.mesh) == type(b.mesh), "GFs have different type of meshes !"
    assert a.mesh == b.mesh, "meshes are different"
    assert_arrays_are_close(a.data, b.data, precision)

def assert_block_gfs_are_close(a, b, precision = 1.e-6):
    assert len(a) == len(b), "Block GFs have different number of blocks"
    for (nx,x),(ny,y) in zip(a,b):
      assert_gfs_are_close(x, y, precision)

def assert_block2_gfs_are_close(a, b, precision = 1.e-6):
    assert_block_gfs_are_close(a, b, precision)
