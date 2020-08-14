from cpp2py.wrap_generator import *

# The module
module = module_(full_name = "cpp2py.test_g", doc = " Doc of my_module ")
module.add_include("./g.hpp")

module.add_include("<triqs/cpp2py_converters.hpp>")
module.add_include("<cpp2py/converters/pair.hpp>")

module.add_function (name = "make_bgf", signature = "block_gf<imfreq> (double a)", doc = "")
module.add_function (name = "pass_bgf", signature = "void (block_gf_view<imfreq> g) ", doc = "")

module.add_function (name = "make_gf", signature = "gf<imfreq,scalar_valued> (double a)", doc = "")
module.add_function (name = "pass_gf", signature = "void (gf_view<imfreq,scalar_valued> g)", doc = "")

module.add_function (name = "make_pair_gf", signature = "std::pair<gf<imfreq,scalar_valued>, gf<imfreq,scalar_valued>> (double a)", doc = "")
module.add_function (name = "pass_two_gf", signature = "void (gf_view<imfreq,scalar_valued> g1, gf_view<imfreq,scalar_valued> g2)", doc = "")

if __name__ == '__main__' :
   module.generate_code()

