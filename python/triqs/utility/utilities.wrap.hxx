#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_utilities_GUARDS
#define C2PY_HXX_DECLARATION_utilities_GUARDS
template <> constexpr bool c2py::is_wrapped<triqs::utility::tau_t>     = true;
template <> inline constexpr auto c2py::tp_name<triqs::utility::tau_t> = "triqs.utility.utilities.TauT";
#endif