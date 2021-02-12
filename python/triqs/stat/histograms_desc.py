# Generated automatically using the command :
# c++2py.py ../../triqs/stat/histograms.hpp -p
from cpp2py.wrap_generator import *

# The module
module = module_(full_name = "triqs.stat.histograms", doc = "")

# All the triqs C++/Python modules

# Add here all includes beyond what is automatically included by the triqs modules
module.add_include("<triqs/stat/histograms.hpp>")

module.add_include("<cpp2py/converters/pair.hpp>")
module.add_include("<triqs/cpp2py_converters.hpp>")

# Add here anything to add in the C++ code at the start, e.g. namespace using
module.add_preamble("""
using namespace triqs;
using namespace triqs::stat;
""")

# The class histogram
c = class_(
        py_type = "Histogram",  # name of the python class
        c_type = "histogram",   # name of the C++ class
        c_type_absolute = "triqs::stat::histogram",
        is_printable = True,
        hdf5 = True,
        comparisons = "==",
        arithmetic = "add_only",
        serializable = "tuple",
        doc = r"""Statistical histogram

   This class serves to sample a continuous random variable, and to 'bin' it.
   It divides a given range of real values into a series of equal intervals,
   and counts amounts of samples falling into each interval.
   The histogram keeps track of the total number of the sampled values, as well
   as of the lost samples that lie outside the chosen range."""
)

c.add_constructor("""(int a, int b)""", doc = r"""Constructor

    Constructs a histogram over :math:`[a; b]` range with bin length equal to 1.

Parameters
----------
a
     Left end of the sampling range

b
     Right end of the sampling range""")

c.add_constructor("""(double a, double b, long n_bins)""", doc = r"""Constructor

    Constructs a histogram over :math:`[a; b]` range with a given number of bins.

Parameters
----------
a
     Left end of the sampling range

b
     Right end of the sampling range

n_bins
     Number of bins""")

c.add_constructor("""()""", doc = r"""Default constructor""")

c.add_method("""double mesh_point (int n)""",
             doc = r"""Get position of bin's center

Parameters
----------
n
     Bin index

Returns
-------
out
     Position of the center, :math:`n (b - a) / (n_\mathrm{bins} - 1)`""")

c.add_len(doc = """Number of bins""")

c.add_property(name = "limits",
               getter = cfunction("std::pair<double, double> limits ()"),
               doc = r"""Return boundaries of the histogram

    @return Pair of histogram boundaries, `(a,b)`""")

c.add_property(name = "data",
               getter = cfunction("arrays::vector<double> data ()"),
               doc = r"""Read-only access to the data storage

    @return Constant reference to the histogram data array""")

c.add_property(name = "n_data_pts",
               getter = cfunction("unsigned long long n_data_pts ()"),
               doc = r"""Get number of accumulated samples

    @return Number of accumulated data points""")

c.add_property(name = "n_lost_pts",
               getter = cfunction("unsigned long long n_lost_pts ()"),
               doc = r"""Get number of discarded samples

    @return Number of discarded data points""")

c.add_method("""void clear ()""",
               doc = r"""Reset all histogram values to 0

    Resets all data values and the total counts of accumulated and discarded points.""")

f = pyfunction(name = '__lshift__', arity = 2)
f.add_overload(calling_pattern = '<<', signature = 'self_t& (triqs::stat::histogram h, double x)')
f.treat_as_inplace = True
c.number_protocol['lshift'] = f


module.add_class(c)

module.add_function ("triqs::stat::histogram pdf (triqs::stat::histogram h)",
                    doc = r"""Normalise histogram to get probability density function (PDF)

Parameters
----------
h
     Histogram to be normalised

Returns
-------
out
     Probability density function""")

module.add_function ("triqs::stat::histogram cdf (triqs::stat::histogram h)",
                     doc = r"""Integrate and normalise histogram to get cumulative distribution function (CDF)

Parameters
----------
h
     Histogram to be integrated and normalised

Returns
-------
out
     Cumulative distribution function""")


module.generate_code()
