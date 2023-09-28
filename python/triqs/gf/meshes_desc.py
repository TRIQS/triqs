from cpp2py.wrap_generator import *
import re

module = module_(full_name = "triqs.gf.meshes", doc = "All the meshes", app_name="triqs")

module.add_imports("triqs.lattice.lattice_tools")

module.add_include("<triqs/mesh.hpp>")

module.add_include("<cpp2py/converters/string.hpp>")
module.add_include("<cpp2py/converters/vector.hpp>")
module.add_include("<cpp2py/converters/function.hpp>")
module.add_include("<cpp2py/converters/optional.hpp>")
module.add_include("<cpp2py/converters/pair.hpp>")

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

module.add_enum(c_name = "triqs::mesh::imfreq::option",
         c_namespace = "",
         values = ["imfreq::option::all_frequencies","imfreq::option::positive_frequencies_only"])

########################
##   Mesh generic
########################

def make_mesh(py_type, c_tag, doc="", with_values=True):

    m = class_( py_type = py_type,
            c_type = "%s"%c_tag,
            c_type_absolute = "triqs::mesh::%s"%c_tag,
            hdf5 = True,
            serializable= "h5",
            is_printable= True,
            comparisons = "== !=",
            doc = doc
           )

    m.add_method(f"{c_tag}::data_index_t to_data_index({c_tag}::index_t index)", doc = "Function to convert an index to a data index")
    m.add_method(f"{c_tag}::index_t to_index({c_tag}::data_index_t data_index)", doc = "Function to convert a data index to an index")
    m.add_getitem(signature = f"{c_tag}::mesh_point_t operator[]({c_tag}::data_index_t data_index)", doc = "Get a mesh-point given the data index")
    m.add_call(signature = f"{c_tag}::mesh_point_t operator()({c_tag}::index_t index)", calling_pattern = " auto result = self_c(index)", doc = "Get a mesh-point given the index")
    m.add_len(calling_pattern = "int result = self_c.size()", doc = "Size of the mesh")
    m.add_property(name = "mesh_hash",
               getter = cfunction(calling_pattern="uint64_t result = self_c.mesh_hash()",
               signature = "uint64_t()",
               doc = "The hash encoding the mesh configuration"))

    m.add_iterator()

    if with_values:
        m.add_method("PyObject * values()",
                     calling_pattern = """
                        static auto cls = pyref::get_class("triqs.gf", "MeshValueGenerator", /* raise_exception */ true);
                        pyref args = PyTuple_Pack(1, self);
                        auto result = PyObject_CallObject(cls, args);
                     """, doc = "A numpy array of all the values of the mesh points")
        m.add_method(f"{c_tag}::value_t to_value({c_tag}::index_t index)", doc = "index -> value")

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
            Statistic, 'Fermion' or 'Boson'
        n_iw : int [default=1025]
            Number of positive Matsubara frequencies
        """)
m.add_constructor(signature = "(double beta, statistic_enum S, int n_iw=1025)")
m.add_constructor(signature = "(double beta, statistic_enum S, int n_max)")
m.add_constructor(signature = "(double beta, statistic_enum statistic, int n_iw)")
m.add_method("""int last_index()""")
m.add_method("""int first_index()""")
m.add_method("""bool positive_only()""")
m.add_method("""void set_tail_fit_parameters(double tail_fraction, int n_tail_max = 30, std::optional<int> expansion_order = {})""")
m.add_property(name = "beta",
               getter = cfunction(calling_pattern="double result = self_c.beta()",
               signature = "double()",
               doc = "Inverse temperature"))
m.add_property(name = "statistic",
               getter = cfunction(calling_pattern="statistic_enum result = self_c.statistic()", signature = "statistic_enum()"),
               doc = "Statistic")

module.add_class(m)

########################
##   MeshDLRImFreq
########################

m = make_mesh( py_type = "MeshDLRImFreq", c_tag = "dlr_imfreq",
        doc =  """Mesh of DLR imaginary times

        The MeshPoint for a given linear_index `i` can be otained
        through `m[i]` and for an index `n` through `m(n)`

        The associated Green function does not allow for evaluation on
        arbitrary Matsubara frequencies. For this use the Green function
        on the associated MeshDLR.

        Parameters
        ----------
        beta : float
            Inverse temperature
        statistic : str
            Statistic, 'Fermion' or 'Boson'
        w_max: float
            DLR energy cutoff, same as Lambda / beta
        eps: float
            Representation accuracy
        symmetrize: bool [default = false]
            Experimental! Whether to choose the frequencies symmetrically
            around 0. For fermionic/bosonic statistic enforces even/odd dlr-rank
            and number of frequencies
        """)
m.add_constructor(signature = "(double beta, statistic_enum statistic, double w_max, double eps, bool symmetrize = false)")
m.add_property(name = "beta",
               getter = cfunction(calling_pattern="double result = self_c.beta()",
               signature = "double()",
               doc = "Inverse temperature"))
m.add_property(name = "statistic",
               getter = cfunction(calling_pattern="statistic_enum result = self_c.statistic()",
               signature = "statistic_enum()"),
               doc = "Statistic")
m.add_property(name = "w_max",
               getter = cfunction(calling_pattern="double result = self_c.w_max()",
               signature = "double()",
               doc = "DLR energy cutoff"))
m.add_property(name = "eps",
               getter = cfunction(calling_pattern="double result = self_c.eps()",
               signature = "double()",
               doc = "Representation accuracy"))

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
            Statistic, 'Fermion' or 'Boson'
        n_tau : int
            Number of mesh-points
        """)
m.add_constructor(signature = "(double beta, statistic_enum S, int n_tau)")
m.add_constructor(signature = "(double beta, statistic_enum S, int n_max)")
m.add_constructor(signature = "(double beta, statistic_enum statistic, int n_tau)")
m.add_property(name = "beta",
               getter = cfunction(calling_pattern="double result = self_c.beta()",
               signature = "double()",
               doc = "Inverse temperature"))
m.add_property(name = "statistic",
               getter = cfunction(calling_pattern="statistic_enum result = self_c.statistic()", signature = "statistic_enum()"),
               doc = "Statistic")

module.add_class(m)

########################
##   MeshDLRImTime
########################

m = make_mesh(py_type = "MeshDLRImTime", c_tag = "dlr_imtime",
        doc =  """Mesh of DLR imaginary times

        The MeshPoint for a given linear_index `i` can be otained
        through `m[i]` and for an index `n` through `m(n)`

        The associated Green function does not allow for evaluation on
        arbitrary tau-points. For this use the Green function
        on the associated MeshDLR

        Parameters
        ----------
        beta : float
            Inverse temperature
        statistic : str
            Statistic, 'Fermion' or 'Boson'
        w_max: float
            DLR energy cutoff, same as Lambda / beta
        eps: float
            Representation accuracy
        symmetrize: bool [default = false]
            Experimental! Whether to choose the time-points symmetrically
            around beta/2. For fermionic/bosonic statistic enforces even/odd dlr-rank
            and number of tau-points
        """)
m.add_constructor(signature = "(double beta, statistic_enum statistic, double w_max, double eps, bool symmetrize = false)")
m.add_property(name = "beta",
               getter = cfunction(calling_pattern="double result = self_c.beta()",
               signature = "double()",
               doc = "Inverse temperature"))
m.add_property(name = "statistic",
               getter = cfunction(calling_pattern="statistic_enum result = self_c.statistic()",
               signature = "statistic_enum()"),
               doc = "Statistic")
m.add_property(name = "w_max",
               getter = cfunction(calling_pattern="double result = self_c.w_max()",
               signature = "double()",
               doc = "DLR energy cutoff"))
m.add_property(name = "eps",
               getter = cfunction(calling_pattern="double result = self_c.eps()",
               signature = "double()",
               doc = "Representation accuracy"))

module.add_class(m)

########################
##   MeshDLR
########################

m = make_mesh(py_type = "MeshDLR", c_tag = "dlr",
        doc =  """Mesh of DLR coefficients

        The MeshPoint for a given linear_index `i` can be otained
        through `m[i]` and for an index `n` through `m(n)`

        The associated Green function allows for evaluation on
        both arbitrary Matsubara frequencies and tau-points.

        Parameters
        ----------
        beta : float
            Inverse temperature
        statistic : str
            Statistic, 'Fermion' or 'Boson'
        w_max: float
            DLR energy cutoff, same as Lambda / beta
        eps: float
            Representation accuracy
        symmetrize: bool [default = false]
            Experimental! Whether to choose the time-points and frequencies
            of the associated MeshDLRImTime and MeshDLRImFreq symmetrically.
            For fermionic/bosonic statistic enforces even/odd dlr-rank [default = false]
        """)
m.add_constructor(signature = "(double beta, statistic_enum statistic, double w_max, double eps, bool symmetrize = false)")
m.add_property(name = "beta",
               getter = cfunction(calling_pattern="double result = self_c.beta()",
               signature = "double()",
               doc = "Inverse temperature"))
m.add_property(name = "statistic",
               getter = cfunction(calling_pattern="statistic_enum result = self_c.statistic()",
               signature = "statistic_enum()"),
               doc = "Statistic")
m.add_property(name = "w_max",
               getter = cfunction(calling_pattern="double result = self_c.w_max()",
               signature = "double()",
               doc = "DLR energy cutoff"))
m.add_property(name = "eps",
               getter = cfunction(calling_pattern="double result = self_c.eps()",
               signature = "double()",
               doc = "Representation accuracy"))

module.add_class(m)

########################
##   MeshLegendre
########################


# the mesh
m = make_mesh( py_type = "MeshLegendre", c_tag = "triqs::mesh::legendre", with_values = False,
        doc =  """Mesh representing Legendre polynomials with degrees in the interval [0,max_n]

        Parameters
        ----------
        beta : float
            Inverse temperature
        statistic : str
            Statistic, 'Fermion' or 'Boson'
        max_n : int
            Largest degree
        """)
m.add_constructor(signature = "(double beta, statistic_enum S, int max_n)")
m.add_constructor(signature = "(double beta, statistic_enum S, int n_max)")
m.add_constructor(signature = "(double beta, statistic_enum statistic, int max_n)")
m.add_property(name = "beta",
               getter = cfunction(calling_pattern="double result = self_c.beta()",
               signature = "double()",
               doc = "Inverse temperature"))
m.add_property(name = "statistic",
               getter = cfunction(calling_pattern="statistic_enum result = self_c.statistic()", signature = "statistic_enum()"),
               doc = "Statistic")

module.add_class(m)

########################
##   MeshReFreq
########################

m = make_mesh(py_type = "MeshReFreq", c_tag = "refreq",
        doc =  """Mesh of real frequencies

        Frequencies are evenly distributed in the interval [w_min, w_max]
        including values at both edges.

        Parameters (Option 1)
        ---------------------
        window : pair of float
            The frequency interval
        n_w : int
            Number of frequencies

        Parameters (Option 2)
        ---------------------
        w_min : float
            Smallest frequency
        w_max : float
            Largest frequency
        n_w : int
            Number of frequencies
        """)
m.add_constructor(signature = "(double w_min, double w_max, int n_w)")
m.add_constructor(signature = "(double w_min, double w_max, int n_max)")

m.add_constructor(signature = "(std::pair<double, double> window, int n_w)")

m.add_property(name = "w_min",
               getter = cfunction(calling_pattern="double result = self_c.w_min()",
               signature = "double()",
               doc = "Smallest frequency"))

m.add_property(name = "w_max",
               getter = cfunction(calling_pattern="double result = self_c.w_max()",
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

        Parameters (Option 1)
        ---------------------
        window : pair of float
            The time interval
        n_t : int
            Number of time-points

        Parameters (Option 2)
        ---------------------
        t_min : float
            Smallest time
        t_max : float
            Largest time
        n_t : int
            Number of time-points
        """)
m.add_constructor(signature = "(double t_min, double t_max, int n_t)")
m.add_constructor(signature = "(double t_min, double t_max, int n_max)")

m.add_constructor(signature = "(std::pair<double, double> window, int n_t)")

m.add_property(name = "t_min",
               getter = cfunction(calling_pattern="double result = self_c.t_min()",
               signature = "double()",
               doc = "Smallest time"))

m.add_property(name = "t_max",
               getter = cfunction(calling_pattern="double result = self_c.t_max()",
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

m = make_mesh( py_type = "MeshBrZone", c_tag = "brzone",
        doc =  """Momentum mesh on a BrillouinZone)

        The unit vectors $U$ of the mesh are constructed such that

        $$K = N * U$$

        where $K$ is the reciprocal matrix and $N$ the periodization matrix.

        Parameters (Option 1)
        ---------------------
        bz : BrillouinZone
            The underlying Brillouin Zone
        dims : numpy.ndarray of integers, shape=(3,)
            The extent of each dimension

        Parameters (Option 2)
        ---------------------
        bz : BrillouinZone
            The underlying Brillouin Zone
        n_k : int
            Number of mesh-points in each reciprocal direction
        """)
m.add_constructor(signature = "(triqs::lattice::brillouin_zone bz, std::array<long, 3> dims)")
m.add_constructor(signature = "(triqs::lattice::brillouin_zone bz, int n_k)")

m.add_method("std::array<long,3> closest_index(vector_const_view<double> v)", doc = "Mesh Index closest to the vector")

m.add_property(getter = cfunction("std::array<long, 3> dims()"), doc = "Linear dimensions")
m.add_property(getter = cfunction("matrix_const_view<double> units()"), doc = "Matrix containing mesh basis vectors as rows")
m.add_property(getter = cfunction("triqs::lattice::brillouin_zone bz()"), doc = "The brillouin_zone")

module.add_class(m)

########################
##   MeshCycLat
########################

m = make_mesh( py_type = "MeshCycLat", c_tag = "cyclat",
        doc =  """Mesh with finite periodicity on a BravaisLattice

        Parameters (Option 1)
        ---------------------
        lattice : BravaisLattice
            The underlying Bravais Lattice
        dims : numpy.ndarray of integers, shape=(3,)
            The extent of each dimension

        Parameters (Option 2)
        ---------------------
        lattice : BravaisLattice
            The underlying Bravais Lattice
        L : int
            Number of mesh-points in each spacial direction
        """)
m.add_constructor(signature = "(triqs::lattice::bravais_lattice bl, std::array<long, 3> dims)")
m.add_constructor(signature = "(triqs::lattice::bravais_lattice bl, int L)")
m.add_constructor(signature = "(triqs::lattice::bravais_lattice lattice, std::array<long, 3> dims)")
m.add_constructor(signature = "(triqs::lattice::bravais_lattice lattice, int L)")
m.add_constructor(signature = "(int L1, int L2, int L3)")
m.add_constructor(signature = "()")

m.add_property(getter = cfunction("std::array<long, 3> dims()"), doc = "Extent of each dimension")
m.add_property(getter = cfunction("matrix_const_view<double> units()"), doc = "Matrix containing mesh basis vectors as rows")
m.add_property(getter = cfunction("triqs::lattice::bravais_lattice lattice()"), doc = "The bravais_lattice")

module.add_class(m)

############################
##   Mesh Factory Functions
############################

# ---------------------- make_adjoint_mesh --------------------
module.add_function("imfreq triqs::mesh::make_adjoint_mesh(imtime m, int n_iw = -1)", doc = "Create the adjoint iw-mesh")
module.add_function("imtime triqs::mesh::make_adjoint_mesh(imfreq m, int n_tau = -1)", doc = "Create the adjoint tau-mesh")
module.add_function("dlr_imfreq triqs::mesh::make_adjoint_mesh(dlr_imtime m)", doc = "Create the adjoint dlr iw mesh")
module.add_function("dlr_imtime triqs::mesh::make_adjoint_mesh(dlr_imfreq m)", doc = "Create the adjoint dlr imtime mesh")
module.add_function("refreq triqs::mesh::make_adjoint_mesh(retime m, bool shift_half_bin = false)", doc = "Create the adjoint w-mesh")
module.add_function("retime triqs::mesh::make_adjoint_mesh(refreq m, bool shift_half_bin = false)", doc = "Create the adjoint t-mesh")
module.add_function("brzone triqs::mesh::make_adjoint_mesh(cyclat m)", doc = "Create the adjoint k-mesh")
module.add_function("cyclat triqs::mesh::make_adjoint_mesh(brzone m)", doc = "Create the adjoint r-mesh")

##   Code generation
module.generate_code()
