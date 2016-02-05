# Generated automatically using the command :
# c++2py.py ../c++/block_matrix.hpp -p -m pytriqs.arrays.block_matrix -o block_matrix
from wrap_generator import *

# The module
module = module_(full_name = "pytriqs.arrays.block_matrix", doc = "")

# Add here all includes beyond what is automatically included by the triqs modules
module.add_include("<triqs/arrays/block_matrix.hpp>")
module.add_include("<triqs/python_tools/converters/h5.hpp>")
module.add_include("<triqs/python_tools/converters/pair.hpp>")
module.add_include("<triqs/python_tools/converters/arrays.hpp>")
module.add_include("<triqs/python_tools/converters/vector.hpp>")
module.add_include("<triqs/python_tools/converters/map.hpp>")
module.add_using("namespace triqs::arrays")

# The class block_matrix
c = class_(
        py_type = "BlockMatrix",  # name of the python class
        c_type = "block_matrix<double>",   # name of the C++ class
        c_type_absolute = "triqs::arrays::block_matrix<double>",
        hdf5 = True,
        arithmetic=("vector_space", "double"),
        serializable = "tuple",
        is_printable = True
)

c.add_member(c_name = "block_names",
             c_type = "std::vector<std::string>",
             read_only= False,
             doc = """ """)

c.add_member(c_name = "matrix_vec",
             c_type = "std::vector<matrix<double> >",
             read_only= False,
             doc = """ """)

c.add_constructor("""(std::vector<std::string> block_names_, std::vector<matrix<double>> matrix_vec_)""",
                  doc = """ """)
c.add_constructor("""()""",
                  doc = """ """)

c.add_method("""int size ()""",
             doc = """ """)
c.add_method(name="__call__",
             calling_pattern = "matrix<double> result = self_c(s)",
             signature="matrix<double>(std::string s)",
             doc = """ """)
c.add_getitem(
             signature="matrix<double>(int n)",
             doc = """ """)

module.add_class(c)
module.add_function ("void h5_write (triqs::h5::group fg, std::string subgroup_name, triqs::arrays::block_matrix<double> c)", doc = "")

module.add_function ("void h5_read (triqs::h5::group fg, std::string subgroup_name, triqs::arrays::block_matrix<double> c)", doc = "")

#module.add_function("void mpi_broadcast(triqs::arrays::block_matrix<double> m, triqs::mpi::communicator c={}, int root = 0)", doc="")

module.generate_code()
