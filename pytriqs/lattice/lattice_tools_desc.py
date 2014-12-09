from wrap_generator import *

module = module_(full_name = "pytriqs.lattice_tools", doc = "Lattice tools (to be improved)")
module.add_include("<triqs/lattice/brillouin_zone.hpp>")
module.add_include("<triqs/lattice/tight_binding.hpp>")
module.add_include("<triqs/python_tools/converters/pair.hpp>")
module.add_include("<triqs/python_tools/converters/string.hpp>")
module.add_include("<triqs/python_tools/converters/arrays.hpp>")
#module.add_include("<triqs/python_tools/converters/h5.hpp>")
module.add_include("<triqs/python_tools/converters/vector.hpp>")

module.add_using("namespace triqs::lattice")
module.add_using("namespace triqs::arrays")
module.add_using("namespace triqs")
module.add_using("r_t = arrays::vector<double>")
module.add_using("k_t = arrays::vector<double>")

# ---------   Bravais lattice ----------------------------------

bl = class_( py_type = "BravaisLattice",
        c_type = "bravais_lattice",
        c_type_absolute = "triqs::lattice::bravais_lattice",
        serializable= "tuple",
       )

bl.add_constructor(signature = "(matrix<double> units, std::vector<r_t> orbital_positions, std::vector<std::string> atom_orb_name)",
                   doc = "")
bl.add_constructor(signature = "(matrix<double> units, std::vector<r_t> orbital_positions)",
                   doc = "")
bl.add_constructor(signature = "(matrix<double> units)",
                   doc = "")
bl.add_constructor(signature = "()",
                   doc = "")

bl.add_property(getter = cfunction("int dim()"), doc = "Dimension of the lattice")
bl.add_property(getter = cfunction("int n_orbitals()"), doc = "Number of orbitals")
bl.add_property(getter = cfunction("matrix_const_view<double> units()"), doc = "Base vectors of the lattice")

bl.add_method(name = "lattice_to_real_coordinates", 
              signature = "r_t lattice_to_real_coordinates(r_t x)",
              doc = "Transform into real coordinates.")

module.add_class(bl)

# ---------   TightBinding ----------------------------------
tb = class_(py_type = "TightBinding",
        c_type = "tight_binding",
        c_type_absolute = "triqs::lattice::tight_binding",
        #serializable= "tuple",
       )

tb.add_constructor(signature = "(bravais_lattice latt, PyObject* hopping)",
                   calling_pattern = 
                   """
                    // We need to rewrite manually the call to the constructor : 
                    // hopping is a dict : displacement -> matrix
                    // we flatten it into 2 vector of vector and matrix resp.
                    // for the tight_binding constructor
                    // First of all, if it is not a dict, error
                    if (!PyDict_Check(hopping)) {PyErr_SetString(PyExc_TypeError, "hopping must be a mapping type (dict)"); return 0;}
                    // The arguments for the C++ constructor
                    std::vector<std::vector<long>> displs; 
                    std::vector<matrix<dcomplex>> mats;
                    // we loop on the dict // Cf python doc for PyDict_Next
                    PyObject *key, *value;
                    Py_ssize_t pos = 0;
                    while (PyDict_Next(hopping, &pos, &key, &value)) {
                     displs.push_back(convert_from_python<std::vector<long>>(key));
                     mats.push_back(convert_from_python<matrix<dcomplex>>(value));
                    }
                    auto result = tight_binding(*latt, displs, mats);
                   """, 
                   doc = " ")

module.add_class(tb)

# ---------   Module functions ----------------------------------

module.add_function(name = "hopping_stack",
                    signature = "array<dcomplex, 3> (tight_binding  TB, array_const_view<double, 2> k_stack)",
                    doc = """ """)
module.add_function(name = "dos",
                    signature = "std::pair<array<double, 1>, array<double, 2>> (tight_binding  TB, int nkpts, int neps)",
                    doc = """ """)
module.add_function(name = "dos_patch",
                    signature = "std::pair<array<double, 1>, array<double, 1>> (tight_binding  TB, array<double, 2> triangles, int neps, int ndiv)",
                    doc = """ """)
module.add_function(name = "energies_on_bz_path",
                    signature = "array<double, 2> (tight_binding  TB, k_t  K1, k_t  K2, int n_pts)",
                    doc = """ """)
module.add_function(name = "energy_matrix_on_bz_path",
                    signature = "array<dcomplex, 3> (tight_binding  TB, k_t  K1, k_t  K2, int n_pts)",
                    doc = """ """)
module.add_function(name = "energies_on_bz_grid",
                    signature = "array<double, 2> (tight_binding  TB, int n_pts)",
                    doc = """ """)

########################
##   Code generation
########################

if __name__ == '__main__' :
   module.generate_code()

