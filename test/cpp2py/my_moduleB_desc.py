from cpp2py.wrap_generator import *

# The module
mod = module_(full_name = "my_moduleB", doc = " Doc of my_module ", app_name="triqs")
mod.add_include("<triqs/../test/cpp2py/b.hpp>") 

mod.add_function (name = "print_a2", signature = "void(A a)", doc = "DOC of print_a")

mod.generate_code()

