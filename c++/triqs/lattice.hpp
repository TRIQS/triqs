#pragma once

/**
 * @file
 * @brief Umbrella header for the TRIQS lattice / tight-binding tools.
 */

#include "./lattice/bravais_lattice.hpp"
#include "./lattice/brillouin_zone.hpp"
#include "./lattice/tight_binding.hpp"

#ifdef C2PY_INCLUDED
#include <triqs/lattice/lattice_tools.wrap.hxx>
#include "./c2py_converters/lattice.hpp"
#endif
