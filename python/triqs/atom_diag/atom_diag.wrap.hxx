#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_atom_diag_GUARDS
#define C2PY_HXX_DECLARATION_atom_diag_GUARDS
template <> constexpr bool c2py::is_wrapped<triqs::atom_diag::atom_diag<false>>     = true;
template <> inline constexpr auto c2py::tp_name<triqs::atom_diag::atom_diag<false>> = "triqs.atom_diag.atom_diag.AtomDiagReal";
template <> constexpr bool c2py::is_wrapped<triqs::atom_diag::atom_diag<true>>      = true;
template <> inline constexpr auto c2py::tp_name<triqs::atom_diag::atom_diag<true>>  = "triqs.atom_diag.atom_diag.AtomDiagComplex";
#endif