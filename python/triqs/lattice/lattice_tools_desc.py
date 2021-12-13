from cpp2py.wrap_generator import *

module = module_(full_name = "triqs.lattice.lattice_tools", doc = "Lattice tools (to be improved)")
module.add_include("<triqs/lattice/brillouin_zone.hpp>")
module.add_include("<triqs/lattice/tight_binding.hpp>")

module.add_include("<cpp2py/converters/pair.hpp>")
module.add_include("<cpp2py/converters/vector.hpp>")
module.add_include("<cpp2py/converters/map.hpp>")
module.add_include("<triqs/cpp2py_converters.hpp>")

module.add_using("namespace triqs::lattice")
module.add_using("namespace triqs::arrays")
module.add_using("namespace triqs::gfs")
module.add_using("namespace triqs")
module.add_using("r_cvt = nda::vector_const_view<double>")
module.add_using("k_cvt = nda::vector_const_view<double>")

# ---------   Bravais lattice ----------------------------------

bl = class_(py_type = "BravaisLattice",
        c_type = "bravais_lattice",
        c_type_absolute = "triqs::lattice::bravais_lattice",
        is_printable = True,
        hdf5 = True,
        comparisons = "== !=",
        serializable= "tuple",
       )

bl.add_constructor(signature = "(triqs::lattice::brillouin_zone bz)",
                   doc = "")
bl.add_constructor(signature = "(matrix<double> units, std::vector<r_t> orbital_positions, std::vector<std::string> atom_orb_name)",
                   doc = "")
bl.add_constructor(signature = "(matrix<double> units, std::vector<r_t> orbital_positions)",
                   doc = "")
bl.add_constructor(signature = "(matrix<double> units)",
                   doc = "")
bl.add_constructor(signature = "()",
                   doc = "")

bl.add_property(getter = cfunction("matrix_const_view<double> units()"), doc = "Matrix containing lattice basis vectors as rows")
bl.add_property(getter = cfunction("int ndim()"), doc = "Number of dimensions")
bl.add_property(getter = cfunction("int n_orbitals()"), doc = "Number of orbitals in the unit cell")
bl.add_property(getter = cfunction("std::vector<r_t> orbital_positions()"), doc = "Return the list of orbital positions")
bl.add_property(getter = cfunction("std::vector<std::string> orbital_names()"), doc = "Return the list of orbital names")


bl.add_method(name = "lattice_to_real_coordinates", 
              signature = "r_t lattice_to_real_coordinates(r_t x)",
              doc = "Transform into real coordinates.")

module.add_class(bl)

# ------  brillouin_zone ----------------------
c = class_(
        py_type = "BrillouinZone",  # name of the python class
        c_type = "brillouin_zone",   # name of the C++ class
        c_type_absolute = "triqs::lattice::brillouin_zone",
        doc = r"",   # doc of the C++ class
        is_printable = True,
        hdf5 = True,
        comparisons = "== !=",
        serializable= "tuple",
)

c.add_constructor("""(triqs::lattice::bravais_lattice bl_)""",
                  doc = """Construct from a bravais_lattice """)

c.add_property(getter = cfunction("matrix_const_view<double> units()"),
               doc = "Matrix containing reciprocal basis vectors as rows")

c.add_property(getter = cfunction("triqs::lattice::bravais_lattice lattice()"),
               doc = """Access to the underlying bravais lattice """)

module.add_class(c)

# ---------   TightBinding ----------------------------------
tb = class_(py_type = "TightBinding",
        c_type = "tight_binding",
        c_type_absolute = "triqs::lattice::tight_binding",
        is_printable = True,
        hdf5 = True,
        comparisons = "== !="
       )

tb.add_constructor(signature = "(bravais_lattice latt, PyObject* hoppings)",
                   calling_pattern = 
                   """
                    // We need to rewrite manually the call to the constructor : 
                    // hoppings is a dict : displacement -> matrix
                    // we flatten it into 2 vector of vector and matrix resp.
                    // for the tight_binding constructor
                    // First of all, if it is not a dict, error
                    if (!PyDict_Check(hoppings)) {PyErr_SetString(PyExc_TypeError, "hoppings must be a mapping type (dict)"); return 0;}
                    // The arguments for the C++ constructor
                    std::vector<nda::vector<long>> displs;
                    std::vector<matrix<dcomplex>> mats;
                    // we loop on the dict // Cf python doc for PyDict_Next
                    PyObject *key, *value;
                    Py_ssize_t pos = 0;
                    while (PyDict_Next(hoppings, &pos, &key, &value)) {
                     displs.push_back(convert_from_python<nda::vector<long>>(key));
                     mats.push_back(convert_from_python<matrix<dcomplex>>(value));
                    }
                    auto result = tight_binding(latt, displs, mats);
                   """, 
                   doc = " ")

tb.add_constructor(signature = "(bravais_lattice bl, std::vector<nda::vector<long>> displ_vec, std::vector<matrix<dcomplex>> overlap_mat_vec)",
                   doc = """Construct a TightBinding object given the bravais lattice, the vector of displacements, and the vector of overlap matrices""")

tb.add_property(getter = cfunction("std::vector<nda::vector<long>> displ_vec()"),
                doc = """A list containing displacement vectors""")

tb.add_property(getter = cfunction("std::vector<matrix<dcomplex>> overlap_mat_vec()"),
                doc = """A list containing overlap matrices""")

tb.add_property(getter = cfunction("triqs::lattice::bravais_lattice lattice()"),
                doc = """The underlying bravais lattice""")

tb.add_method(name = "lattice_to_real_coordinates",
              signature = "r_t lattice_to_real_coordinates(r_t x)",
              doc = "Transform into real coordinates.")

tb.add_method(name = "fourier",
              signature = "matrix<dcomplex> (k_cvt K)",
              doc = """Evaluate the fourier transform for a momentum vector k in units of the reciprocal lattice vectors""")

tb.add_method(name = "fourier",
              signature = "nda::array<dcomplex, 3> (nda::array_const_view<double, 2> K)",
              doc = """Evaluate the fourier transform for an array of momentum vectors k in units of the reciprocal lattice vectors""")

tb.add_method(name = "fourier",
              signature = "gf<mesh::brzone, matrix_valued> (mesh::brzone k_mesh)",
              doc = """Evaluate the fourier transform on the k_mesh and return the associated Green-function object""")

tb.add_method(name = "dispersion",
              signature = "nda::array<double, 1> (k_cvt K)",
              doc = """Evaluate the dispersion relation for a momentum vector k in units of the reciprocal lattice vectors""")

tb.add_method(name = "dispersion",
              signature = "nda::array<double, 2> (nda::array_const_view<double, 2> K)",
              doc = """Evaluate the dispersion relation for an array of momentum vectors k in units of the reciprocal lattice vectors""")

tb.add_method(name = "dispersion",
              signature = "gf<mesh::brzone, tensor_real_valued<1>> (mesh::brzone k_mesh)",
              doc = """Evaluate the dispersion relation on the k_mesh and return the associated Green-function object""")


module.add_class(tb)

# ---------   Module functions ----------------------------------

module.add_function(name = "dos",
                    signature = "std::pair<array<double, 1>, array<double, 2>> (tight_binding  TB, int nkpts, int neps)",
                    doc = """ """)
module.add_function(name = "dos_patch",
                    signature = "std::pair<array<double, 1>, array<double, 1>> (tight_binding  TB, array<double, 2> triangles, int neps, int ndiv)",
                    doc = """ """)
module.add_function(name = "hopping_stack",
                    signature = "array<dcomplex, 3> (tight_binding  TB, array_const_view<double, 2> k_stack)",
                    doc = """ """)
module.add_function(name = "energies_on_bz_path",
                    signature = "array<double, 2> (tight_binding  TB, k_cvt  K1, k_cvt  K2, int n_pts)",
                    doc = """ """)
module.add_function(name = "energy_matrix_on_bz_path",
                    signature = "array<dcomplex, 3> (tight_binding  TB, k_cvt  K1, k_cvt  K2, int n_pts)",
                    doc = """ """)
module.add_function(name = "energies_on_bz_grid",
                    signature = "array<double, 2> (tight_binding  TB, int n_pts)",
                    doc = """ """)

########################
##   Code generation
########################

if __name__ == '__main__' :
   module.generate_code()

