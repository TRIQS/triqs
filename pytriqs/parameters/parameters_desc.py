from wrap_generator import *

# The module
module = module_(full_name = "pytriqs.parameters.parameters", doc = "TO BE WRITTEN")
module.add_include("<triqs/parameters/parameters.hpp>")
module.add_include("<triqs/utility/formatted_output.hpp>")
module.add_using("namespace triqs::params")

# one class
g = class_(
        py_type = "Parameters",
        c_type = "triqs::params::parameters",
        #serializable= "tuple",
        is_printable= True,
        hdf5 = True,
        # Add +  for merging
        arithmetic = ("only_add")
        )

#add a constructor
#g.add_constructor(doc = "DOC of constructor", args = [])

g.add_method(py_name = "help", calling_pattern = "auto result = triqs::utility::print_formatted(self_c.generate_help())", signature = "std::string()", doc = "help")

# add getitem/setitem ...
g.add_getitem(signature = "PyObject *(const char * key)", 
              calling_pattern = "PyObject * result = self_c[key].to_python()",
              doc = "")
g.add_setitem(signature = "void(const char * key, PyObject * ob)", 
              calling_pattern = """
                if (!self_c[key].from_python_convertible(ob)) return NULL; // early exit, the error is set by the converter
                self_c[key].set_from_python(ob);
                """,
              doc = "")

module.add_class(g)

if __name__ == '__main__' : 
   module.generate_code(mako_template = sys.argv[1], wrap_file = sys.argv[2])
   module.generate_py_converter_header(mako_template = sys.argv[3], wrap_file = sys.argv[4])

