#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_operators_GUARDS
#define C2PY_HXX_DECLARATION_operators_GUARDS
template <> constexpr bool c2py::is_wrapped<triqs::operators::canonical_ops_t>     = true;
template <> inline constexpr auto c2py::tp_name<triqs::operators::canonical_ops_t> = "triqs.operators.operators.CanonicalOpsT";
template <> constexpr bool c2py::is_wrapped<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>> = true;
template <>
inline constexpr auto c2py::tp_name<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>> =
   "triqs.operators.operators.Operator";
#endif