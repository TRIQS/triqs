from cpp2py.wrap_generator import *

module = module_(full_name = "pytriqs.random_generator", doc = "")
module.add_include("<triqs/mc_tools/random_generator.hpp>")
module.add_using("namespace triqs::mc_tools")

module.add_include("<h5/h5.hpp>")

module.add_include("<triqs/arrays.hpp>")

module.add_include("<cpp2py/converters/vector.hpp>")
module.add_include("<triqs/cpp2py_converters.hpp>")

# ---------  RandomGenerator  ----------------------------------

r = class_(py_type = "RandomGenerator",
           c_type = "random_generator",
           c_type_absolute = "triqs::mc_tools::random_generator",
          )

r.add_constructor(signature = "(std::string name, int seed)", 
                  doc = 
                  """
                  This is a random number generator class based on boost.

                  name Name of the random number generator
                  seed Random number seed
                  """)

r.add_call(signature = "int(int N)", doc = """Generate an integer random number in [0,N-1]""") 
r.add_call(signature = "double()", doc = """Generate a float random number in [0,1[""")
 
module.add_class(r)

# ---------   Module functions ----------------------------------

module.add_function(name = "random_generator_names_list",
                    signature = "std::vector<std::string>()",
                    doc = """Get a list of available random generator names"""
                    )

########################
##   Code generation
########################

if __name__ == '__main__' :
   module.generate_code()


