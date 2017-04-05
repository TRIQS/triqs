import numpy as np

def assert_arrays_are_close(a, b, precision = 1.e-6):
    d = np.amax(np.abs(a - b))
    assert  d< precision, "Arrays are different. Difference is %s"%d

def assert_array_close_to_scalar(a, x, precision = 1.e-6):
    assert_arrays_are_close(a, np.identity(a.shape[0])*(x), precision)

def assert_tails_are_close(a, b, precision = 1.e-6):
    if a is None : 
        assert b is None
        return 
    assert a.n_valid_orders == b.n_valid_orders, "Tails have different n_valid_orders %s vs %s "%(a.n_valid_orders, b.n_valid_orders)
    n = a.n_valid_orders
    if n >0:
        assert_arrays_are_close(a.data[0:n], b.data[0:n], precision)

def assert_gfs_are_close(a, b, precision = 1.e-6):
    assert type(a.mesh) == type(b.mesh), "GFs have different type of meshes !"
    assert a.mesh == b.mesh, "meshes are different"
    assert_arrays_are_close(a.data, b.data, precision)
    assert_tails_are_close(a.singularity, b.singularity, precision);

def assert_block_gfs_are_close(a, b, precision = 1.e-6):
    assert len(a) == len(b), "Block GFs have different number of blocks"
    for (nx,x),(ny,y) in zip(a,b):
      assert_gfs_are_close(x, y, precision)

