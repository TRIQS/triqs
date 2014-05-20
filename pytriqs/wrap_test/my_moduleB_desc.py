from wrap_generator import *

# The module
mod = module_(full_name = "pytriqs.wrap_test.my_moduleB", doc = " Doc of my_module ")
mod.add_include("<triqs/../pytriqs/wrap_test/b.hpp>") 
mod.add_include("<pytriqs/converters/my_module.hpp>")

mod.add_function (name = "print_a2", signature = "void(A a)", doc = "DOC of print_a")

mod.generate_code(mako_template = sys.argv[1], wrap_file = sys.argv[2])
mod.generate_py_converter_header(mako_template = sys.argv[3], wrap_file = sys.argv[4])

