from cpp2py.wrap_generator import *

import pytriqs.gf

# The module
module = module_(full_name = "pytriqs.wrap_test.test_bl", doc = " Doc of my_module ")
module.add_include("<triqs/../test/pytriqs/wrap_test/blockgf.hpp>")

module.add_include("<cpp2py/converters/vector.hpp>")
module.add_include("<triqs/cpp2py_converters.hpp>")

module.add_function(name = "fct_2_blocks", signature = "void(block_gf_view<imfreq> x, block_gf_view<imfreq> y)", doc = "")
module.add_function(name = "fct_2_blocks", signature = "void(block_gf_view<imtime> x, block_gf_view<imtime> y)", doc = "")

if __name__ == '__main__':
   module.generate_code()
