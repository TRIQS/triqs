#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_histograms_GUARDS
#define C2PY_HXX_DECLARATION_histograms_GUARDS
template <> constexpr bool c2py::is_wrapped<triqs::stat::histogram>     = true;
template <> inline constexpr auto c2py::tp_name<triqs::stat::histogram> = "triqs.stat.histograms.Histogram";
#endif