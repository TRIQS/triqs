#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_lattice_tools_GUARDS
#define C2PY_HXX_DECLARATION_lattice_tools_GUARDS
template <> constexpr bool c2py::is_wrapped<triqs::lattice::bravais_lattice>     = true;
template <> inline constexpr auto c2py::tp_name<triqs::lattice::bravais_lattice> = "triqs.lattice.lattice_tools.BravaisLattice";
template <> constexpr bool c2py::is_wrapped<triqs::lattice::brillouin_zone>      = true;
template <> inline constexpr auto c2py::tp_name<triqs::lattice::brillouin_zone>  = "triqs.lattice.lattice_tools.BrillouinZone";
template <> constexpr bool c2py::is_wrapped<triqs::lattice::tight_binding>       = true;
template <> inline constexpr auto c2py::tp_name<triqs::lattice::tight_binding>   = "triqs.lattice.lattice_tools.TightBinding";
#endif