from wrap_generator import *

# The module
mod = module_(full_name = "pytriqs.wrap_test.my_moduleB", imported_modules = ["my_module"], doc = " Doc of my_module ")
mod.add_include("../c++/b.hpp") # FIX PUT ASOLUTE NAME

mod.add_function (name = "print_a2", signature = "void(A a)", doc = "DOC of print_a")

mod.generate_code(mako_template = sys.argv[1], wrap_file = sys.argv[2])

