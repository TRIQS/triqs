from dcomplex cimport * 
from shared_ptr cimport *
from arrays cimport *   
from libcpp.pair cimport pair
from libcpp.vector cimport vector
from libcpp.string cimport string as std_string
#from extractor cimport *
#from h5 cimport *
from cython.operator cimport dereference as deref, preincrement as inc #dereference and increment operator

import numpy

cdef extern from "triqs/lattice/bravais_lattice_and_brillouin_zone.hpp" namespace "triqs::gf" : 
    cdef cppclass bravais_lattice_c " triqs::lattice_tools::bravais_lattice" : 
        bravais_lattice_c(matrix[double] & units)
        int n_orbitals()
        matrix[double] units()
        int dim()
        void push_back(std_string &, tqa_vector[double] &)
        tqa_vector[double] lattice_to_real_coordinates (tqa_vector_view[double] &)

cdef extern from "triqs/lattice/tight_binding.hpp" namespace "triqs::gf" : 
    cdef cppclass tight_binding " triqs::lattice_tools::tight_binding" : 
        tight_binding(bravais_lattice_c & units)
        void push_back (vector[long] &, matrix[double] &) 

    #Fix the name conflict pv wiht a pxd, cf doc....?
    array_view[complex,THREE] hopping_stack_c "hopping_stack" (tight_binding  & TB, array_view[double,TWO] & k_stack)
    pair [array[double,ONE],array[double,TWO]]  dos_c "dos" (tight_binding & TB, size_t nkpts, size_t neps)
    pair [array[double,ONE],array[double,ONE]]  dos_patch_c "dos_patch" (tight_binding & TB, array_view[double,TWO] & triangles, size_t neps, size_t ndiv)
    array_view[double,TWO] energies_on_bz_path_c "energies_on_bz_path" (tight_binding & TB, tqa_vector[double] & K1, tqa_vector[double] &  K2, size_t n_pts)
    array_view[double,TWO] energies_on_bz_grid_c "energies_on_bz_grid" (tight_binding & TB, size_t n_pts)

cdef class BravaisLattice :
    """
    """
    cdef bravais_lattice_c * _c
    def __init__(self, units, orbital_positions = None ) :
        """  """
        self._c = new bravais_lattice_c( matrix[double](units))
        orbital_positions  = orbital_positions if orbital_positions else dict ("", (0,0,0) )
        for name, pos in orbital_positions.items():
            self._c.push_back( name, tqa_vector[double](pos))

    def __dealloc__(self):
        del self._c
        
    def lattice_to_real_coordinates(self, R) : 
        """
        Transform into real coordinates.
        :param x_in: coordinates in the basis of the unit vectors
        :rtype: Output coordinates in R^3 (real coordinates) as an array
        """
        return self._c.lattice_to_real_coordinates ( tqa_vector_view[double] (R)).to_python()
 
    property dim :
        """Dimension of the lattice"""
        def __get__(self) : 
            """Dimension of the lattice"""
            return self._c.dim()
 
    def n_orbitals(self) : 
        """Number of orbitals"""
        return self._c.n_orbitals()
  
cdef class TightBinding: 
    """
    """
    cdef tight_binding * _c
    def __init__(self, BravaisLattice bravais_lattice, hopping ) : 
        """  """
        self._c = new tight_binding(deref(bravais_lattice._c))
        cdef vector[long] d
        for displ, mat in hopping.items() :
            d = displ
            self._c.push_back( d,  matrix[double] (numpy.array(mat, dtype =float)))

    def __dealloc__(self):
        del self._c

# -------------------------------------------

def hopping_stack (TightBinding TB, k_stack) : 
    """ """
    return hopping_stack_c(deref(TB._c), array_view[double,TWO](k_stack)).to_python() 

def dos ( TightBinding TB, int nkpts, int neps):
    """ """
    a =  dos_c(deref(TB._c), nkpts, neps)
    return (a.first.to_python(), a.second.to_python())

def dos_patch ( TightBinding TB, triangles,  int neps, int ndiv): 
    """ """
    a = dos_patch_c(deref(TB._c), array_view[double,TWO] (triangles), neps, ndiv)
    return (a.first.to_python(), a.second.to_python())

def energies_on_bz_path ( TightBinding TB, K1, K2, n_pts) : 
    """ """
    return energies_on_bz_path_c (deref(TB._c), tqa_vector[double](K1), tqa_vector[double] (K2), n_pts).to_python()  

def energies_on_bz_grid ( TightBinding TB, n_pts) : 
    """ """ 
    return energies_on_bz_grid_c (deref(TB._c), n_pts).to_python() 


