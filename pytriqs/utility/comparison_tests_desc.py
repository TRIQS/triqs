from wrap_generator import *

# The module
module = module_(full_name = "comparison_tests", doc = "Short module with comparison tests")
module.add_include("<pytriqs/converters/gf.hpp>")
module.add_include("<triqs/gfs/functions/gf_tests.hpp>")
module.add_using("namespace triqs::gfs")

# Module functions to make gf comparisons
for types in ['imfreq', 'refreq', 'imtime', 'retime', 'legendre']:
  module.add_function(name = "assert_gfs_are_close",
                      signature = "void(gf_view<%s> x, gf_view<%s> y, double precision=1.e-6)"%(2*(types,)), doc = "compare two gfs")
  module.add_function(name = "assert_block_gfs_are_close",
                      signature = "void(block_gf_view<%s> x, block_gf_view<%s> y, double precision=1.e-6)"%(2*(types,)), doc = "compare two block gfs")

# array comparisons
def assert_arrays_are_close(a, b, precision = 1.e-6):
    import numpy as np
    assert np.amax(np.abs(a - b)) < precision, "Arrays are different"

def assert_array_close_to_scalar(a, x, precision = 1.e-6):
    import numpy as np
    assert_arrays_are_close(a, np.identity(a.shape[0])*(x), precision)

# initialisation of the module
module.add_python_function(assert_arrays_are_close)
module.add_python_function(assert_array_close_to_scalar)

# generate wrapper code
if __name__ == '__main__' :
   module.generate_code()
