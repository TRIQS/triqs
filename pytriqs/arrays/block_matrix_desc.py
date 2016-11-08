# Generated automatically using the command :
# c++2py.py ../c++/block_matrix.hpp -p -m pytriqs.arrays.block_matrix -o block_matrix
from wrap_generator import *

# The module
module = module_(full_name = "pytriqs.arrays.block_matrix", doc = "Block-diagonal matrix containers")

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
        py_type = "BlockMatrix",           # name of the python class
        c_type = "block_matrix<double>",   # name of the C++ class
        c_type_absolute = "triqs::arrays::block_matrix<double>",
        arithmetic = ("algebra", "double", "with_unary_minus"),
        hdf5 = True,
        serializable = "tuple",
        is_printable = True
)

c.add_constructor("""(std::vector<std::string> block_names, std::vector<matrix<double>> matrix_vec)""",
                  doc = """Construct on a list of block names and a list of blocks""")
c.add_constructor("""()""",
                  doc = """Construct empty block matrix""")
c.add_method_copy()

c.add_member(c_name = "block_names",
             c_type = "std::vector<std::string>",
             read_only = False,
             doc = """Names of diagonal blocks""")

c.add_member(c_name = "matrix_vec",
             c_type = "std::vector<matrix<double> >",
             read_only = False,
             doc = """List of diagonal blocks""")

c.add_method("""int size ()""", doc = """DEPRECATED: number of diagonal blocks""")

c.add_len(doc = """Number of diagonal blocks""")

c.add_call(calling_pattern = "matrix<double> result = self_c[n]",
           signature="matrix<double>(int n)",
           doc = """Access diagonal block by index""")

c.add_getitem(signature="matrix<double>(std::string name)",
              calling_pattern = "matrix<double> result = self_c(name)",
              doc = """Access diagonal block by index""")

c.add_setitem(signature = "void(std::string name, matrix<double> m)",
              calling_pattern = "self_c(name) = m",
              doc = """Access diagonal block by index""")

module.add_class(c)

# The class block_matrix (complex)
c = class_(
        py_type = "BlockMatrixComplex",                  # name of the python class
        c_type = "block_matrix<std::complex<double>>",   # name of the C++ class
        c_type_absolute = "triqs::arrays::block_matrix<std::complex<double>>",
        arithmetic = ("algebra", "double", "std::complex<double>", "with_unary_minus"),
        hdf5 = True,
        serializable = "tuple",
        is_printable = True
)

c.add_member(c_name = "block_names",
             c_type = "std::vector<std::string>",
             read_only = False,
             doc = """Names of diagonal blocks""")

c.add_member(c_name = "matrix_vec",
             c_type = "std::vector<matrix<std::complex<double>> >",
             read_only = False,
             doc = """List of diagonal blocks""")

c.add_constructor("""(std::vector<std::string> block_names, std::vector<matrix<std::complex<double>>> matrix_vec)""",
                  doc = """Construct on a list of block names and a list of blocks""")
c.add_constructor("""()""",
                  doc = """Construct empty block matrix""")
c.add_method_copy()

c.add_method("""int size ()""", doc = """DEPRECATED: number of diagonal blocks""")

c.add_len(doc = """Number of diagonal blocks""")

c.add_call(calling_pattern = "matrix<std::complex<double>> result = self_c[n]",
           signature="matrix<std::complex<double>>(int n)",
           doc = """Access diagonal block by index""")

c.add_getitem(signature="matrix<std::complex<double>>(std::string name)",
              calling_pattern = "matrix<std::complex<double>> result = self_c(name)",
              doc = """Access diagonal block by index""")

c.add_setitem(signature = "void(std::string name, matrix<std::complex<double>> m)",
              calling_pattern = "self_c(name) = m",
              doc = """Access diagonal block by index""")

module.add_class(c)

module.generate_code()
