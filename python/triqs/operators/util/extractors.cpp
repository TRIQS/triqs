#include <c2py/c2py.hpp>
#include <triqs/c2py_converters/fundamental_operator_set.hpp>
#include <triqs/c2py_converters/real_or_complex.hpp>
#include <triqs/operators/util/extractors.hpp>
#include <triqs/utility/tuple_tools.hpp>

#include "../operators.wrap.hxx"

// The raw C++ extract_*_dict functions return maps keyed by
// std::tuple<indices_t, ...> where indices_t = std::vector<variant>.
// c2py converts std::vector to Python list, which is not hashable
// and thus cannot be a dict key. These wrappers convert each
// indices_t key to a std::tuple<variant, variant> (-> Python tuple, hashable).
//
// dict_to_matrix wraps dict_to_variant_matrix, accepting gf_struct_t
// (Python-friendly) instead of fundamental_operator_set.

namespace triqs::operators::utils::py {

  using triqs::utility::real_or_complex;
  using roc       = real_or_complex;
  using indices_t = hilbert_space::fundamental_operator_set::indices_t;
  using idx_var   = std::variant<long, std::string, double, std::array<long, 3>>;
  using idx_tup   = std::tuple<idx_var, idx_var>;

  C2PY_IGNORE static auto v2t(indices_t const &v) { return std::make_tuple(v[0], v[1]); }

  /**
   * @brief Extract the quadratic part \f$ \sum_{ij} h_{ij} \hat{c}^\dagger_i \hat{c}_j \f$ from a many-body operator.
   *
   * @param H The many-body operator.
   * @param ignore_irrelevant If `true`, silently skip non-quadratic terms.
   * @return Dictionary mapping index pairs \f$ (i, j) \f$ to coefficients \f$ h_{ij} \f$.
   */
  std::map<std::tuple<idx_tup, idx_tup>, roc> extract_h_dict(many_body_operator const &H, bool ignore_irrelevant = false) {
    std::map<std::tuple<idx_tup, idx_tup>, roc> result;
    for (auto const &kv : utils::extract_h_dict(H, ignore_irrelevant)) result[triqs::tuple::map(v2t, kv.first)] = kv.second;
    return result;
  }

  /**
   * @brief Extract the density-density interaction \f$ \frac{1}{2} \sum_{ij} U_{ij} \hat{n}_i \hat{n}_j \f$ from a
   * many-body operator.
   *
   * @param H The many-body operator.
   * @param ignore_irrelevant If `true`, silently skip irrelevant terms.
   * @return Dictionary mapping index pairs \f$ (i, j) \f$ to coefficients \f$ U_{ij} \f$.
   */
  std::map<std::tuple<idx_tup, idx_tup>, roc> extract_U_dict2(many_body_operator const &H, bool ignore_irrelevant = false) {
    std::map<std::tuple<idx_tup, idx_tup>, roc> result;
    for (auto const &kv : utils::extract_U_dict2(H, ignore_irrelevant)) result[triqs::tuple::map(v2t, kv.first)] = kv.second;
    return result;
  }

  /**
   * @brief Extract the two-body interaction part from a many-body operator.
   * 
   * @details The two body interaction is assumed to have the form
   * \f[
   *    \frac{1}{2} \sum_{ijkl} U_{ijkl} \hat{c}^\dagger_i \hat{c}^\dagger_j \hat{c}_l \hat{c}_k \; .
   * \f]
   *
   * @param H The many-body operator.
   * @param ignore_irrelevant If `true`, silently skip irrelevant terms.
   * @return Dictionary mapping index quadruples \f$ (i, j, k, l) \f$ to coefficients \f$ U_{ijkl} \f$.
   */
  std::map<std::tuple<idx_tup, idx_tup, idx_tup, idx_tup>, roc> extract_U_dict4(many_body_operator const &H, bool ignore_irrelevant = false) {
    std::map<std::tuple<idx_tup, idx_tup, idx_tup, idx_tup>, roc> result;
    for (auto const &kv : utils::extract_U_dict4(H, ignore_irrelevant)) result[triqs::tuple::map(v2t, kv.first)] = kv.second;
    return result;
  }

  /**
   * @brief Convert a coefficient dictionary to a matrix, given a GF structure.
   *
   * @param d The 2-index/4-index dictionary.
   * @param gf_struct The Green's function block structure.
   * @return A real or complex matrix/rank-4 tensor (as variant).
   */
  real_or_complex_array<2> dict_to_matrix(dict2_t<roc> d, gfs::gf_struct_t gf_struct) {
    return utils::dict_to_variant_matrix(std::move(d), hilbert_space::fundamental_operator_set{gf_struct});
  }

  real_or_complex_array<4> dict_to_matrix(dict4_t<roc> d, gfs::gf_struct_t gf_struct) {
    return utils::dict_to_variant_matrix(std::move(d), hilbert_space::fundamental_operator_set{gf_struct});
  }

} // namespace triqs::operators::utils::py

// Explicit instantiations for functions that can be wrapped directly
namespace triqs::operators::utils {

  using roc = triqs::utility::real_or_complex;

  template op_t<roc> quadratic_terms(op_t<roc> const &);
  template op_t<roc> quartic_terms(op_t<roc> const &);
  template nda::array<nda::matrix<roc>, 1> block_matrix_from_op(op_t<roc> const &, hilbert_space::gf_struct_t const &, bool);
  template op_t<roc> op_from_block_matrix(nda::array<nda::matrix<roc>, 1> const &, hilbert_space::gf_struct_t const &);

} // namespace triqs::operators::utils

#include "extractors.wrap.cxx"
