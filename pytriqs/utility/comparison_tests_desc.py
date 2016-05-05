from wrap_generator import *

# The module
module = module_(full_name = "comparison_tests", doc = "Short module with comparison tests")
module.add_include("<triqs/py_converters/gf.hpp>")
#module.add_include("<triqs/py_converters/multivar.hpp>")
module.add_include("<triqs/gfs/gf_tests.hpp>")
module.add_using("namespace triqs::gfs")

# Module functions to make gf comparisons
import itertools
descs= ['imfreq', 'refreq', 'imtime', 'retime', 'legendre']
target_types =['matrix_valued', 'scalar_valued']

### COMMENTED
d_t = [(x,y) for x,y in itertools.product(descs, target_types)]
#d_t.append(('cartesian_product<imfreq, imfreq>', 'tensor_valued<3>'))

for types, target_type in d_t:
  module.add_function(name = "assert_gfs_are_close",
                      signature = "void(gf_view<%s,%s> x, gf_view<%s,%s> y, double precision=1.e-6)"%(types, target_type, types, target_type), doc = "compare two gfs")
  module.add_function(name = "assert_block_gfs_are_close",
                      signature = "void(block_gf_view<%s,%s> x, block_gf_view<%s,%s> y, double precision=1.e-6)"%(types, target_type, types, target_type), doc = "compare two block gfs")

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
