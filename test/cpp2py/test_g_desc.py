from __future__ import unicode_literals
from cpp2py.wrap_generator import *

# The module
module = module_(full_name = "cpp2py.test_g", doc = " Doc of my_module ")
module.add_include("<triqs/../test/cpp2py/g.hpp>")

module.add_include("<triqs/cpp2py_converters.hpp>")
module.add_include("<cpp2py/converters/variant.hpp>")


module.add_function (name = "make_bgf", signature = "block_gf<imfreq> (double a)", doc = "DOC of print_a")
module.add_function (name = "pass_bgf", signature = "void (block_gf_view<imfreq> g) ", doc = "DOC of print_a")

module.add_function (name = "make_sgf", signature = "gf<imfreq,scalar_valued> (double a)", doc = "DOC of print_a")
module.add_function (name = "pass_sgf", signature = "void (gf_view<imfreq,scalar_valued> g)", doc = "DOC of print_a")


if __name__ == '__main__' :
   module.generate_code()

