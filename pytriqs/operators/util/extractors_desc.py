from cpp2py.wrap_generator import *

# triqs.operators.util.extractors module
module = module_(full_name = "triqs.operators.util.extractors",
                 doc = "Functions to extract coefficients from many-body operators",
                 app_name = "triqs")

module.add_imports("triqs.operators")

module.add_include("<triqs/operators/util/extractors.hpp>")

module.add_include("<cpp2py/converters/vector.hpp>")
module.add_include("<cpp2py/converters/map.hpp>")
module.add_include("<cpp2py/converters/variant.hpp>")
module.add_include("<cpp2py/converters/pair.hpp>")
module.add_include("<cpp2py/converters/tuple.hpp>")

module.add_include("<triqs/cpp2py_converters.hpp>")
module.add_include("<triqs/cpp2py_converters/real_or_complex.hpp>")

module.add_using("namespace triqs::operators")
module.add_using("namespace triqs::operators::utils")
module.add_using("namespace triqs::hilbert_space")

module.add_preamble("""
using variant_int_string = std::variant<int, std::string>;
using indices_t_t = std::tuple<variant_int_string, variant_int_string>;
auto v2t = [](indices_t const& v) { return std::make_tuple(v[0],v[1]); };
using triqs::tuple::map;
""")

module.add_function("dict2_t<real_or_complex> extract_h_dict(many_body_operator H, bool ignore_irrelevant = false)",
                    calling_pattern = """
                    std::map<std::tuple<indices_t_t,indices_t_t>,real_or_complex> result;
                    for(auto const& kv : extract_h_dict(H,ignore_irrelevant)) result[map(v2t,kv.first)] = kv.second;
                    """,
                    doc = r"""
    Extract coefficients of the normal quadratic part :math:`\sum_{ij}h_{ij} c^\dagger_i c_j` from a Hamiltonian H as a 2-index dictionary.

    Parameters
    ----------
    H : Operator
        The Hamiltonian.
    ignore_irrelevant : bool
        If True, ignore all irrelevant terms in H.
        Otherwise raise an exception when such terms are met.

    Returns
    -------
    h_dict : dict
        The 2-index dictionary of the quadratic part coefficients, {(i,j) \: float}.

""")

module.add_function("dict2_t<real_or_complex> extract_U_dict2(many_body_operator H, bool ignore_irrelevant = false)",
                    calling_pattern = """
                    std::map<std::tuple<indices_t_t,indices_t_t>,real_or_complex> result;
                    for(auto const& kv : extract_U_dict2(H,ignore_irrelevant)) result[map(v2t,kv.first)] = kv.second;
                    """,
                    doc = r"""
    Extract U-matrix of the density-density interaction part :math:`\frac{1}{2}\sum_{ij} U_{ij} n_i n_j`
    from a Hamiltonian H as a 2-index dictionary.

    Parameters
    ----------
    H : Operator
        The Hamiltonian.
    ignore_irrelevant : bool
        If True, ignore all irrelevant terms in H.
        Otherwise raise an exception when such terms are met.

    Returns
    -------
    U_dict : dict
        The 2-index interaction dictionary, {(i,j) \: float}.

""")

module.add_function("dict4_t<real_or_complex> extract_U_dict4(many_body_operator H, bool ignore_irrelevant = false)",
                    calling_pattern = """
                    std::map<std::tuple<indices_t_t,indices_t_t,indices_t_t,indices_t_t>,real_or_complex> result;
                    for(auto const& kv : extract_U_dict4(H,ignore_irrelevant)) result[map(v2t,kv.first)] = kv.second;
                    """,
                    doc = r"""
    Extract U-matrix of the interaction part :math:`\frac{1}{2}\sum_{ijkl} U_{ijkl} c^\dagger_i c^\dagger_j c_l c_k`
    from a Hamiltonian H as a 4-index dictionary.

    Parameters
    ----------
    H : Operator
        The Hamiltonian.
    ignore_irrelevant : bool
        If True, ignore all irrelevant terms in H.
        Otherwise raise an exception when such terms are met.

    Returns
    -------
    U_dict : dict
        The 4-index interaction dictionary, {(i,j,k,l) \: float}.

""")

dict_to_matrix_docstring = r"""
Convert a 2/4-index dictionary to a 2/4-dimensional NumPy array given the structure of the Green's function.
The elements missing from the dictionary are assumed to be zero.

Parameters
----------
d : dict
    The 2/4-index dictionary.
gf_struct : dict
    The structure of the Green's function, {block_index \: [inner indices]}.

Returns
-------
arr : array
    The resulting NumPy array.

"""

module.add_function("real_or_complex_array<2> dict_to_variant_matrix(dict2_t<real_or_complex> d, gf_struct_t gf_struct)",
                    name = "dict_to_matrix", doc = dict_to_matrix_docstring)
module.add_function("real_or_complex_array<4> dict_to_variant_matrix(dict4_t<real_or_complex> d, gf_struct_t gf_struct)",
                    name = "dict_to_matrix", doc = dict_to_matrix_docstring)

module.generate_code()

