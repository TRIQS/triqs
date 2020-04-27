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
        is_printable = True,
        hdf5 = True,
        comparisons = "==",
        arithmetic = "add_only",
        serializable = "tuple",
        doc = "This class serves to sample a continuous random variable, and to 'bin' it. "
              "It divides a given range of real values into a series of equal intervals, "
              "and counts amounts of samples falling into each interval. "
              "The histogram keeps track of the total number of the sampled values, as well "
              "as of the lost samples that lie outside the chosen range."
)

c.add_constructor("""(int a, int b)""",
                  doc = """Constructor with mesh of integer values""")

c.add_constructor("""(double a, double b, long n_bins)""",
                  doc = """Constructor with mesh of floating point values""")

c.add_method("""double mesh_point (int n)""",
             doc = """Get position of bin's center""")

c.add_len(doc = """Number of bins""")

c.add_property(name = "limits",
               getter = cfunction("std::pair<double,double> limits ()"),
               doc = """Return boundaries of the histogram""")

c.add_property(name = "data",
               getter = cfunction("arrays::vector<double> data ()"),
               doc = """Read-only access to the data storage""")

c.add_property(name = "n_data_pts",
               getter = cfunction("unsigned long long n_data_pts ()"),
               doc = """Number of accumulated data points""")

c.add_property(name = "n_lost_pts",
               getter = cfunction("unsigned long long n_lost_pts ()"),
               doc = """Number of discarded data points""")

c.add_method("""void clear ()""",
             doc = """Reset all histogram values to 0""")

f = pyfunction(name = '__lshift__', arity = 2)
f.add_overload(calling_pattern = '<<', signature = 'self_t& (triqs::statistics::histogram h, double x)')
f.treat_as_inplace = True
c.number_protocol['lshift'] = f


module.add_class(c)

module.add_function ("triqs::statistics::histogram pdf (triqs::statistics::histogram h)",
                     doc = """Normalise histogram to get probability density function (PDF)""")

module.add_function ("triqs::statistics::histogram cdf (triqs::statistics::histogram h)",
                     doc = """Integrate and normalise histogram to get cumulative distribution function (CDF)""")

module.generate_code()
