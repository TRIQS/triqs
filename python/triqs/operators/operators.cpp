#include <c2py/c2py.hpp>
#include <triqs/c2py_converters/fundamental_operator_set.hpp>
#include <triqs/c2py_converters/real_or_complex.hpp>
#include <triqs/operators/many_body_operator.hpp>
#include <triqs/utility/real_or_complex.hpp>

namespace c2py {

  // Converter for the _cdress struct used for iterators.
  template <> struct py_converter<triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>::_cdress> {
    using scalar_t  = triqs::utility::real_or_complex;
    using indices_t = triqs::operators::indices_t;
    using c_t       = triqs::operators::many_body_operator_generic<scalar_t>::_cdress;
    using pair_t    = std::pair<std::vector<std::pair<bool, indices_t>>, scalar_t>;
    using conv_t    = py_converter<pair_t>;

    static constexpr const char *tp_name = "_Cdress";

    static PyObject *c2py(c_t const &dress) { return conv_t::c2py(static_cast<pair_t>(dress)); }
  };

} // namespace c2py

namespace c2py_module {

  // explicit class template instantiations
  using Operator = triqs::operators::many_body_operator_generic<triqs::utility::real_or_complex>;

} // namespace c2py_module

namespace triqs::operators {

  // indices_idx_t_t matches the variant type used in the Python bindings
  using idx_t = std::variant<long, std::string, double, std::array<long, 3>>;
  using roc_t = triqs::utility::real_or_complex;

  // c, c_dag, n with 0 to 4 variant index arguments
  template many_body_operator_generic<roc_t> c();
  template many_body_operator_generic<roc_t> c(idx_t);
  template many_body_operator_generic<roc_t> c(idx_t, idx_t);
  template many_body_operator_generic<roc_t> c(idx_t, idx_t, idx_t);
  template many_body_operator_generic<roc_t> c(idx_t, idx_t, idx_t, idx_t);

  template many_body_operator_generic<roc_t> c_dag();
  template many_body_operator_generic<roc_t> c_dag(idx_t);
  template many_body_operator_generic<roc_t> c_dag(idx_t, idx_t);
  template many_body_operator_generic<roc_t> c_dag(idx_t, idx_t, idx_t);
  template many_body_operator_generic<roc_t> c_dag(idx_t, idx_t, idx_t, idx_t);

  template many_body_operator_generic<roc_t> n();
  template many_body_operator_generic<roc_t> n(idx_t);
  template many_body_operator_generic<roc_t> n(idx_t, idx_t);
  template many_body_operator_generic<roc_t> n(idx_t, idx_t, idx_t);
  template many_body_operator_generic<roc_t> n(idx_t, idx_t, idx_t, idx_t);

  template bool is_op_hermitian(many_body_operator_generic<roc_t> const &, double);

} // namespace triqs::operators
