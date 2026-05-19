#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_lattice_GUARDS
#define C2PY_HXX_DECLARATION_lattice_GUARDS
template <> constexpr bool c2py::is_wrapped<triqs::experimental::lattice::bz_int_options>     = true;
template <> inline constexpr auto c2py::tp_name<triqs::experimental::lattice::bz_int_options> = "triqs.experimental.lattice.lattice.BzIntOptions";
template <> constexpr bool c2py::is_wrapped<triqs::experimental::lattice::superlattice>       = true;
template <> inline constexpr auto c2py::tp_name<triqs::experimental::lattice::superlattice>   = "triqs.experimental.lattice.lattice.Superlattice";
template <> constexpr bool c2py::is_wrapped<triqs::experimental::lattice::tb_hk>              = true;
template <> inline constexpr auto c2py::tp_name<triqs::experimental::lattice::tb_hk>          = "triqs.experimental.lattice.lattice.TbHk";
#endif