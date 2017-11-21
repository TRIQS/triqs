# Generated automatically using the command :
# c++2py.py ../../triqs/statistics/histograms.hpp -p
from cpp2py.wrap_generator import *

# The module
module = module_(full_name = "pytriqs.statistics.histograms", doc = "")

# All the triqs C++/Python modules

# Add here all includes beyond what is automatically included by the triqs modules
module.add_include("<triqs/statistics/histograms.hpp>")

module.add_include("<cpp2py/converters/pair.hpp>")
module.add_include("<triqs/cpp2py_converters.hpp>")

# Add here anything to add in the C++ code at the start, e.g. namespace using
module.add_preamble("""
using namespace triqs;
using namespace statistics;
""")

# The class histogram
c = class_(
        py_type = "Histogram",  # name of the python class
        c_type = "histogram",   # name of the C++ class
        c_type_absolute = "triqs::statistics::histogram",
        doc = r"Histogram",   # doc of the C++ class
        is_printable = True,
        hdf5 = True,
        arithmetic = "add_only",
        serializable = "tuple"
)

c.add_constructor("""(int a, int b)""",
                  doc = """Constructor with mesh of integer values """)

c.add_constructor("""(double a, double b, long n_bins)""",
                  doc = """Constructor with mesh of double values """)

c.add_constructor("""()""",
                  doc = """Default constructor """)

c.add_method("""double mesh_point (int n)""",
             doc = """Point on the mesh """)

c.add_len(doc = """Number of bins """)

c.add_property(name = "limits",
               getter = cfunction("std::pair<double,double> limits ()"),
               doc = """Returns boundaries of the histogram """)

c.add_property(name = "data",
               getter = cfunction("arrays::vector<double> data ()"),
               doc = """Read-only access to the data storage """)

c.add_property(name = "n_data_pts",
               getter = cfunction("uint64_t n_data_pts ()"),
               doc = """Norm of the stored data """)

c.add_property(name = "n_lost_pts",
               getter = cfunction("uint64_t n_lost_pts ()"),
               doc = """Number of discarded points """)

c.add_method("""void clear ()""",
             doc = """Reset all values to 0 """)

f = pyfunction(name = '__lshift__', arity = 2)
f.add_overload(calling_pattern = '<<', signature = 'self_t& (triqs::statistics::histogram h, double x)')
f.treat_as_inplace = True
c.number_protocol['lshift'] = f

c.add_pure_python_method("pytriqs.statistics.histogram.plot", rename = "_plot_")

module.add_class(c)

module.add_function ("triqs::statistics::histogram pdf (triqs::statistics::histogram h)", doc = """Normalise histogram to get PDF""")

module.add_function ("triqs::statistics::histogram cdf (triqs::statistics::histogram h)", doc = """Integrate and normalise histogram to get CDF""")

module.generate_code()
