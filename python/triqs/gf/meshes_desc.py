from cpp2py.wrap_generator import *
import re

module = module_(full_name = "triqs.gf.meshes", doc = "All the meshes", app_name="triqs")

module.add_imports("triqs.lattice.lattice_tools")

module.add_include("<triqs/gfs.hpp>")

module.add_include("<cpp2py/converters/string.hpp>")
module.add_include("<cpp2py/converters/vector.hpp>")
module.add_include("<cpp2py/converters/function.hpp>")
module.add_include("<cpp2py/converters/optional.hpp>")

module.add_include("<triqs/cpp2py_converters.hpp>")

module.add_using("namespace triqs::arrays")
module.add_using("namespace triqs::mesh")
module.add_preamble("""
""")

## --------------------------------------------------
##                    WARNING
# The names of the Meshes MUST be MeshXXX, with XXX = ImFreq, ...
# where XXX is the name appearing in hdf5 in multivar mesh (gf/h5.hpp, gf_h5_name trait).
## --------------------------------------------------


########################
##   enums
########################

module.add_enum(c_name = "statistic_enum",
                c_namespace = "triqs::mesh",
                values = ["Fermion","Boson"])

module.add_enum(c_name = "triqs::mesh::imfreq::option",
         c_namespace = "",
         values = ["imfreq::option::all_frequencies","imfreq::option::positive_frequencies_only"])

########################
##   Mesh generic
########################

def make_mesh(py_type, c_tag, index_type='long', doc=""):

    m = class_( py_type = py_type,
            c_type = "%s"%c_tag,
            c_type_absolute = "triqs::mesh::%s"%c_tag,
            hdf5 = True,
            serializable= "tuple",
            is_printable= True,
            comparisons = "== !=",
            doc = doc
           )

    m.add_method("long index_to_linear(%s i)"%index_type, doc = "index -> linear index")
    m.add_len(calling_pattern = "int result = self_c.size()", doc = "Size of the mesh")
    m.add_iterator()
    m.add_method("PyObject * values()",
                 calling_pattern = """
                    static auto cls = pyref::get_class("triqs.gf", "MeshValueGenerator", /* raise_exception */ true);
                    pyref args = PyTuple_Pack(1, self);
                    auto result = PyObject_CallObject(cls, args);
                 """, doc = "A numpy array of all the values of the mesh points")

    m.add_method_copy()
    m.add_method_copy_from()

    return m

########################
##   MeshImFreq
########################

m = make_mesh( py_type = "MeshImFreq", c_tag = "imfreq",
        doc="""Mesh of Matsubara frequencies

        Parameters
        ----------
        beta : float
            Inverse temperature
        statistic : str
            'Fermion' or 'Boson'
        n_iw : int [default=1025]
            Number of positive Matsubara frequencies
        """)
m.add_constructor(signature = "(double beta, statistic_enum S, int n_iw=1025)")
m.add_method("""int last_index()""")
m.add_method("""int first_index()""")
m.add_method("""bool positive_only()""")
m.add_method("""void set_tail_fit_parameters(double tail_fraction, int n_tail_max = 30, std::optional<int> expansion_order = {})""")
m.add_property(name = "beta",
               getter = cfunction(calling_pattern="double result = self_c.domain().beta",
               signature = "double()",
               doc = "Inverse temperature"))
m.add_property(name = "statistic",
               getter = cfunction(calling_pattern="statistic_enum result = self_c.domain().statistic", signature = "statistic_enum()"),
               doc = "Statistic")
m.add_call(signature = "dcomplex (long n)", calling_pattern = " auto result = dcomplex{0, (2*n + self_c.domain().statistic)*M_PI/self_c.domain().beta}", doc = "")

module.add_class(m)

########################
##   MeshImTime
########################

m = make_mesh(py_type = "MeshImTime", c_tag = "imtime",
        doc =  """Mesh of imaginary times

        Mesh-points are evenly distributed in the interval [0,beta]
        including points at both edges.

        Parameters
        ----------
        beta : float
            Inverse temperature
        statistic : str
            'Fermion' or 'Boson'
        n_tau : int
            Number of mesh-points
        """)
m.add_constructor(signature = "(double beta, statistic_enum S, int n_tau)")
m.add_property(name = "beta",
               getter = cfunction(calling_pattern="double result = self_c.domain().beta",
               signature = "double()",
               doc = "Inverse temperature"))
m.add_property(name = "statistic",
               getter = cfunction(calling_pattern="statistic_enum result = self_c.domain().statistic", signature = "statistic_enum()"),
               doc = "Statistic")

module.add_class(m)

########################
##   MeshLegendre
########################


# the domain
dom = class_( py_type = "GfLegendreDomain",
        c_type = "legendre_domain",
        c_type_absolute = "triqs::mesh::legendre_domain",
        serializable= "tuple",
       )
dom.add_constructor(signature = "(double beta, statistic_enum S, int max_n)")
module.add_class(dom)

# the mesh
m = make_mesh( py_type = "MeshLegendre", c_tag = "triqs::mesh::legendre",
        doc =  """Mesh representing Legendre polynomials with degrees in the interval [0,max_n]

        Parameters
        ----------
        beta : float
            Inverse temperature
        S : str
            Statistic, 'Fermion' or 'Boson'
        max_n : int
            Largest degree
        """)
m.add_constructor(signature = "(double beta, statistic_enum S, int max_n)")
m.add_property(name = "beta",
               getter = cfunction(calling_pattern="double result = self_c.domain().beta",
               signature = "double()",
               doc = "Inverse temperature"))
m.add_property(name = "statistic",
               getter = cfunction(calling_pattern="statistic_enum result = self_c.domain().statistic", signature = "statistic_enum()"),
               doc = "Statistic")

module.add_class(m)

########################
##   MeshReFreq
########################

m = make_mesh(py_type = "MeshReFreq", c_tag = "refreq",
        doc =  """Mesh of real frequencies

        Frequencies are evenly distributed in the interval [omega_min, omega_max]
        including values at both edges.

        Parameters
        ----------
        omega_min : float
            Smallest frequency
        omega_max : float
            Largest frequency
        n_w : int
            Number of frequencies
        """)
m.add_constructor(signature = "(double omega_min, double omega_max, int n_w)")

m.add_property(name = "omega_min",
               getter = cfunction(calling_pattern="double result = self_c.omega_min()",
               signature = "double()",
               doc = "Smallest frequency"))

m.add_property(name = "omega_max",
               getter = cfunction(calling_pattern="double result = self_c.omega_max()",
               signature = "double()",
               doc = "Largest frequency"))

m.add_property(name = "delta",
               getter = cfunction(calling_pattern="double result = self_c.delta()",
               signature = "double()",
               doc = "The mesh-spacing"))

module.add_class(m)

########################
##   MeshReTime
########################

m = make_mesh(py_type = "MeshReTime", c_tag = "retime",
        doc =  """Mesh of real times

        Times are evenly distributed in the interval [t_min, t_max]
        including values at both edges.

        Parameters
        ----------
        t_min : float
            Smallest time
        t_max : float
            Largest time
        n_t : int
            Number of time-points
        """)
m.add_constructor(signature = "(double t_min, double t_max, int n_t)")

m.add_property(name = "t_min",
               getter = cfunction(calling_pattern="double result = self_c.x_min()",
               signature = "double()",
               doc = "Smallest time"))

m.add_property(name = "t_max",
               getter = cfunction(calling_pattern="double result = self_c.x_max()",
               signature = "double()",
               doc = "Largest time"))

m.add_property(name = "delta",
               getter = cfunction(calling_pattern="double result = self_c.delta()",
               signature = "double()",
               doc = "The mesh-spacing"))

module.add_class(m)

########################
##   MeshBrZone
########################

m = make_mesh( py_type = "MeshBrZone", c_tag = "brzone", index_type = 'std::array<long,3>',
        doc =  """Momentum mesh on a BrillouinZone

        The unit vectors $U$ of the mesh are constructed such that

        $$K = N * U$$

        where $K$ is the reciprocal matrix and $N$ the periodization matrix.

        Parameters (Option 1)
        ---------------------
        bz : BrillouinZone
            The underlying Brillouin Zone (domain)
        N: Diagonal numpy.ndarray of integers, shape=(3,3)
            The periodization matrix

        Parameters (Option 2)
        ---------------------
        bz : BrillouinZone
            The underlying Brillouin Zone (domain)
        n_l : int
            Number of mesh-points in each reciprocal direction
        """)
m.add_constructor(signature = "(triqs::lattice::brillouin_zone bz, matrix_view<long> N)")
m.add_constructor(signature = "(triqs::lattice::brillouin_zone bz, int n_k)")
m.add_method(name="closest_index", signature="std::array<long,3> closest_index(triqs::arrays::vector<double> x)")

m.add_property(getter = cfunction("std::array<long,3> dims()"), doc = "Linear dimensions")
m.add_property(getter = cfunction("matrix_const_view<double> units()"), doc = "Matrix containing mesh basis vectors as rows")
m.add_property(getter = cfunction("triqs::lattice::brillouin_zone domain()"), doc = "The brillouin_zone domain")

module.add_class(m)

########################
##   MeshCycLat
########################

m = make_mesh( py_type = "MeshCycLat", c_tag = "cyclat", index_type = 'std::array<long,3>',
        doc =  """Mesh with finite periodicity on a BravaisLattice

        Parameters (Option 1)
        ---------------------
        bl : BravaisLattice
            The underlying Bravais Lattice (domain)
        N : diagonal numpy.ndarray of integers, shape=(3,3)
            The periodization matrix

        Parameters (Option 2)
        ---------------------
        bl : BravaisLattice
            The underlying Bravais Lattice (domain)
        L : int
            Number of mesh-points in each spacial direction
        """)
m.add_constructor(signature = "(triqs::lattice::bravais_lattice b, matrix_view<long> periodization_matrix)")
m.add_constructor(signature = "(triqs::lattice::bravais_lattice b, int L)")
m.add_constructor(signature = "(int L1, int L2, int L3)")
m.add_method(name="closest_index", signature="std::array<long,3> closest_index(triqs::arrays::vector<double> x)")

m.add_property(getter = cfunction("std::array<long,3> dims()"), doc = "Extent of each dimension")
m.add_property(getter = cfunction("matrix_const_view<double> units()"), doc = "Matrix containing mesh basis vectors as rows")
m.add_property(getter = cfunction("triqs::lattice::bravais_lattice domain()"), doc = "The bravais_lattice domain")

module.add_class(m)

############################
##   Mesh Factory Functions
############################

# ---------------------- make_adjoint_mesh --------------------
module.add_function("brzone triqs::gfs::make_adjoint_mesh(cyclat m)", doc = "Create the adjoint k-mesh")
module.add_function("cyclat triqs::gfs::make_adjoint_mesh(brzone m)", doc = "Create the adjoint r-mesh")
module.add_function("imfreq triqs::gfs::make_adjoint_mesh(imtime m, int n_iw = -1)", doc = "Create the adjoint iw-mesh")
module.add_function("imtime triqs::gfs::make_adjoint_mesh(imfreq m, int n_tau = -1)", doc = "Create the adjoint tau-mesh")
module.add_function("refreq triqs::gfs::make_adjoint_mesh(retime m, bool shift_half_bin = false)", doc = "Create the adjoint w-mesh")
module.add_function("retime triqs::gfs::make_adjoint_mesh(refreq m, bool shift_half_bin = false)", doc = "Create the adjoint t-mesh")

##   Code generation
module.generate_code()
