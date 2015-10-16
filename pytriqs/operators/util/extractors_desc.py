from wrap_generator import *

# The many_body_operators module
module = module_(full_name = "pytriqs.operators.util.extractors",
                 doc = "Functions to extract coefficients from many-body operators")

module.use_module("operators")
module.add_include("<triqs/operators/util/extractors.hpp>")
module.add_include("<triqs/python_tools/converters/map.hpp>")
module.add_include("<triqs/python_tools/converters/tuple.hpp>")
module.add_include("<triqs/python_tools/converters/arrays.hpp>")
module.add_using("namespace triqs::utility")
module.add_using("namespace triqs::operators::util")

module.add_function("h_dict_t<double> extract_h_dict(many_body_operator<double> H, bool ignore_irrelevant = false)",
                    doc = """
Extracts coefficients of the quadratic part of a Hamiltonian H as a 2-index dictionary.

Parameters
----------
H : Operator
    The interaction Hamiltonian.
ignore_irrelevant : bool
    If True, ignore all irrelevant terms in H.
    Otherwise raise an exception when such terms are met.

Returns
-------
h_dict : {(i,j) : float}
         The 2-index dictionary of the quadratic part coefficients.
""")

module.add_function("U_dict2_t<double> extract_U_dict2(many_body_operator<double> H, bool ignore_irrelevant = false)",
                    doc = """
Extracts the density-density interaction matrix elements from a Hamiltonian H as a 2-index dictionary.

Parameters
----------
H : Operator
    The interaction Hamiltonian.
ignore_irrelevant : bool
    If True, ignore all irrelevant terms in H.
    Otherwise raise an exception when such terms are met.

Returns
-------
U_dict : {(i,j) : float}
         The 2-index interaction dictionary.
""")

module.add_function("U_dict4_t<double> extract_U_dict4(many_body_operator<double> H, bool ignore_irrelevant = false)",
                    doc = """
Extracts the interaction matrix elements from a Hamiltonian H as a 4-index dictionary.

Parameters
----------
H : Operator
    The interaction Hamiltonian.
ignore_irrelevant : bool
    If True, ignore all irrelevant terms in H.
    Otherwise raise an exception when such terms are met.

Returns
-------
U_dict : {(i,j,k,l) : float}
         The 4-index interaction dictionary.
""")

module.add_function("triqs::arrays::array<double,2> dict_to_matrix(h_dict_t<double> h_dict, gf_struct_t gf_struct)",
                    doc = """
Converts a 2-index dictionary {(i,j) : float} into a 2-dimensional NumPy array
given the structure of the Green's function.
The elements missing from the dictionary are assumed to be zero.

Parameters
----------
h_dict : {(i,j) : float}
         The 2-index dictionary.
gf_struct : {block_index : [inner indices]}
         The structure of the Green's function.

Returns
-------
array : 2-dimensional NumPy array
        The resulting array.
""")

module.add_function("triqs::arrays::array<double,4> dict_to_matrix(U_dict4_t<double> U_dict4, gf_struct_t gf_struct)",
                    doc = """
Converts a 4-index dictionary {(i,j,k,l) : float} into a 4-dimensional NumPy array
given the structure of the Green's function.
The elements missing from the dictionary are assumed to be zero.

Parameters
----------
U_dict4 : {(i,j,k,l) : float}
         The 4-index dictionary.
gf_struct : {block_index : [inner indices]}
         The structure of the Green's function.

Returns
-------
array : 4-dimensional NumPy array
        The resulting array.
""")

module.generate_code()

