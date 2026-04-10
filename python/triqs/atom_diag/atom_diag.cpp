#include <c2py/c2py.hpp>
#include <triqs/atom_diag/atom_diag.hpp>
#include <triqs/atom_diag/functions.hpp>
#include <triqs/atom_diag/gf.hpp>
#include <triqs/c2py_converters/fundamental_operator_set.hpp>
#include <triqs/c2py_converters/real_or_complex.hpp>
#include <triqs/c2py_converters/gf.hpp>
#include <triqs/operators.hpp>

namespace c2py_module {

  // Classes to wrap: Real and Complex variants
  using AtomDiagReal    = triqs::atom_diag::atom_diag<false>;
  using AtomDiagComplex = triqs::atom_diag::atom_diag<true>;

} // namespace c2py_module

// Explicit instantiation declarations for all free functions.
// The definitions and instantiation *definitions* live in the triqs library
// (atom_diag/impl/functions.cpp and atom_diag/impl/gf.cpp).
namespace triqs::atom_diag {

  // functions.hpp
  extern template double partition_function(atom_diag<false> const &, double);
  extern template double partition_function(atom_diag<true> const &, double);

  extern template atom_diag<false>::block_matrix_t atomic_density_matrix(atom_diag<false> const &, double);
  extern template atom_diag<true>::block_matrix_t atomic_density_matrix(atom_diag<true> const &, double);

  extern template atom_diag<false>::scalar_t trace_rho_op(atom_diag<false>::block_matrix_t const &, atom_diag<false>::many_body_op_t const &,
                                                          atom_diag<false> const &);
  extern template atom_diag<true>::scalar_t trace_rho_op(atom_diag<true>::block_matrix_t const &, atom_diag<true>::many_body_op_t const &,
                                                         atom_diag<true> const &);

  extern template atom_diag<false>::full_hilbert_space_state_t act(atom_diag<false>::many_body_op_t const &,
                                                                   atom_diag<false>::full_hilbert_space_state_t const &, atom_diag<false> const &);
  extern template atom_diag<true>::full_hilbert_space_state_t act(atom_diag<true>::many_body_op_t const &,
                                                                  atom_diag<true>::full_hilbert_space_state_t const &, atom_diag<true> const &);

  extern template std::vector<std::vector<quantum_number_t>> quantum_number_eigenvalues(atom_diag<false>::many_body_op_t const &,
                                                                                        atom_diag<false> const &);
  extern template std::vector<std::vector<quantum_number_t>> quantum_number_eigenvalues(atom_diag<true>::many_body_op_t const &,
                                                                                        atom_diag<true> const &);

  extern template std::vector<std::vector<quantum_number_t>> quantum_number_eigenvalues_checked(atom_diag<false>::many_body_op_t const &,
                                                                                                atom_diag<false> const &);
  extern template std::vector<std::vector<quantum_number_t>> quantum_number_eigenvalues_checked(atom_diag<true>::many_body_op_t const &,
                                                                                                atom_diag<true> const &);

  // gf.hpp — only the overloads that take atom_diag (not Lehmann)
  extern template block_gf<imtime> atomic_g_tau(atom_diag<false> const &, double, gf_struct_t const &, int, excluded_states_t const &);
  extern template block_gf<imtime> atomic_g_tau(atom_diag<true> const &, double, gf_struct_t const &, int, excluded_states_t const &);

  extern template block_gf<imfreq> atomic_g_iw(atom_diag<false> const &, double, gf_struct_t const &, int, excluded_states_t const &);
  extern template block_gf<imfreq> atomic_g_iw(atom_diag<true> const &, double, gf_struct_t const &, int, excluded_states_t const &);

  extern template block_gf<legendre> atomic_g_l(atom_diag<false> const &, double, gf_struct_t const &, int, excluded_states_t const &);
  extern template block_gf<legendre> atomic_g_l(atom_diag<true> const &, double, gf_struct_t const &, int, excluded_states_t const &);

  extern template block_gf<refreq> atomic_g_w(atom_diag<false> const &, double, gf_struct_t const &, std::pair<double, double> const &, int, double,
                                              excluded_states_t const &);
  extern template block_gf<refreq> atomic_g_w(atom_diag<true> const &, double, gf_struct_t const &, std::pair<double, double> const &, int, double,
                                              excluded_states_t const &);

} // namespace triqs::atom_diag
