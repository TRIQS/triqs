#pragma once

#include "./lattice/bravais_lattice.hpp"
#include "./lattice/brillouin_zone.hpp"
#include "./lattice/tight_binding.hpp"

#ifdef C2PY_INCLUDED
template <> inline constexpr bool c2py::is_wrapped<triqs::lattice::bravais_lattice> = true;
template <> inline constexpr bool c2py::is_wrapped<triqs::lattice::brillouin_zone>  = true;
template <> inline constexpr bool c2py::is_wrapped<triqs::lattice::tight_binding>   = true;

template <> inline constexpr char *c2py::tp_name<triqs::lattice::bravais_lattice> = "BravaisLattice";
template <> inline constexpr char *c2py::tp_name<triqs::lattice::brillouin_zone>  = "BrillouinZone";
template <> inline constexpr char *c2py::tp_name<triqs::lattice::tight_binding>   = "TightBinding";
#include "./c2py_converters/lattice.hpp"
#endif
