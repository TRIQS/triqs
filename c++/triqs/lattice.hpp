#pragma once

// FIXME : includer of lattice to be completed.
// Here put the includes

#include "lattice/brillouin_zone.hpp"
#ifdef C2PY_INCLUDED
#include "./c2py_converters/mesh.hpp"
template <> constexpr bool c2py::is_wrapped<triqs::lattice::bravais_lattice> = true;
template <> constexpr bool c2py::is_wrapped<triqs::lattice::brillouin_zone>  = true;

template <> constexpr char *c2py::tp_name<triqs::lattice::bravais_lattice> = "BravaisLattice";
template <> constexpr char *c2py::tp_name<triqs::lattice::brillouin_zone>  = "BrillouinZone";

#endif