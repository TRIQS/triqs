// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2018-2021 Simons Foundation
// Copyright (c) 2015-2017 Igor Krivenko
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Thomas Ayral, Michel Ferrero, Igor Krivenko, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Provides utilities to extract coefficients (matrices and tensors) from many-body operators.
 */

#pragma once
#include <variant>
#include <triqs/utility/first_include.hpp>
#include <triqs/utility/tuple_tools.hpp>
#include <triqs/operators/many_body_operator.hpp>
#include <triqs/hilbert_space/fundamental_operator_set.hpp>
#include <triqs/arrays.hpp>

namespace triqs::operators::utils {

  /**
   * @addtogroup triqs-ops
   * @{
   */

  // Elevate `nda::array` to the `triqs::operators::utils` namespace.
  using nda::array;

  // Single particle state index type (see triqs::hilbert_space::fundamental_operator_set::indices_t).
  using indices_t = hilbert_space::fundamental_operator_set::indices_t;

  /// Shorthand for triqs::operators::many_body_operator_generic.
  template <typename scalar_t> using op_t = operators::many_body_operator_generic<scalar_t>;

  /// Map from an index pair \f$ (\alpha_i, \alpha_j) \f$ to a coefficient of type `T`.
  template <typename T> using dict2_t = std::map<std::tuple<indices_t, indices_t>, T>;

  /// Map from an index quadruple \f$ (\alpha_i, \alpha_j, \alpha_k, \alpha_l) \f$ to a coefficient of type `T`.
  template <typename T> using dict4_t = std::map<std::tuple<indices_t, indices_t, indices_t, indices_t>, T>;

  /// Rank-`N` array with `real_or_complex` element type, stored as a `std::variant`.
  template <int N> using real_or_complex_array = std::variant<array<double, N>, array<std::complex<double>, N>>;

  /**
   * @brief Extract the coefficients of a normal-ordered quadratic operator.
   *
   * @details Assumes that the operator \f$ \hat{h} \f$ has the normal-ordered quadratic form
   * \f[
   *   \hat{h} = \sum_{ij} h_{ij} \hat{c}_i^\dagger \hat{c}_j \; .
   * \f]
   * The coefficients \f$ h_{ij} \f$ are returned as a map from the index pair \f$ (\alpha_i,
   * \alpha_j) \f$ to the value \f$ h_{ij} \f$.
   *
   * If a term that is not of this form is encountered, an exception is thrown unless
   * `ignore_irrelevant` is `true`, in which case the offending term is silently skipped.
   *
   * @tparam scalar_t Scalar type of the coefficients.
   * @param h Many-body operator \f$ \hat{h} \f$.
   * @param ignore_irrelevant If `true`, terms that do not match the expected form are skipped
   * instead of triggering an exception.
   * @return Dictionary of non-vanishing coefficients \f$ h_{ij} \f$.
   */
  template <typename scalar_t> dict2_t<scalar_t> extract_h_dict(op_t<scalar_t> const &h, bool ignore_irrelevant = false) {

    auto h_dict = dict2_t<scalar_t>{};

    for (auto const &term : h) {
      auto const &coef = term.coef;
      auto const &m    = term.monomial;

      if (m.size() == 2) {
        if (!(m[0].dagger && !m[1].dagger)) {
          if (!ignore_irrelevant) TRIQS_RUNTIME_ERROR << "extract_h_dict: monomial is not of the form C^+(i) C(j)";
        } else { // everything ok
          h_dict.insert({std::make_tuple(m[0].indices, m[1].indices), coef});
        }
      } else {
        if (!ignore_irrelevant) TRIQS_RUNTIME_ERROR << "extract_h_dict: monomial must have 2 operators";
      }
    }

    return h_dict;
  }

  /**
   * @brief Extract the coefficients of a density-density interaction operator.
   *
   * @details Assumes that the operator \f$ \hat{h} \f$ has the density-density form
   * \f[
   *   \hat{h} = \frac{1}{2} \sum_{ij} U_{ij} \hat{n}_i \hat{n}_j \; ,
   * \f]
   * with \f$ \hat{n}_i = \hat{c}_i^\dagger \hat{c}_i \f$. Internally each input term is matched against the
   * canonical normal-ordered pattern \f$ \hat{c}_i^\dagger \hat{c}_j^\dagger \hat{c}_j \hat{c}_i \f$ and both \f$ (i, j)
   * \f$ and \f$ (j, i) \f$ entries are written to the output map, so that the returned dictionary
   * is symmetric in its index pair.
   *
   * If a term that is not of this form is encountered, an exception is thrown unless
   * `ignore_irrelevant` is `true`, in which case the offending term is silently skipped.
   *
   * @tparam scalar_t Scalar type of the coefficients.
   * @param h Many-body operator \f$ \hat{h} \f$.
   * @param ignore_irrelevant If `true`, terms that do not match the expected form are skipped
   * instead of triggering an exception.
   * @return Dictionary of non-vanishing coefficients \f$ U_{ij} \f$.
   */
  template <typename scalar_t> dict2_t<scalar_t> extract_U_dict2(op_t<scalar_t> const &h, bool ignore_irrelevant = false) {

    auto U_dict = dict2_t<scalar_t>{};

    for (auto const &term : h) {
      auto const &coef = term.coef;
      auto const &m    = term.monomial;

      if (m.size() == 4) {
        if (!(m[0].dagger && m[1].dagger && !m[2].dagger && !m[3].dagger) || (m[0].indices != m[3].indices) || (m[1].indices != m[2].indices)) {
          if (!ignore_irrelevant) TRIQS_RUNTIME_ERROR << "extract_U_dict2: monomial is not of the form C^+(i) C^+(j) C(j) C(i)";
        } else { //everything ok
          U_dict.insert({std::make_tuple(m[0].indices, m[1].indices), coef});
          U_dict.insert({std::make_tuple(m[1].indices, m[0].indices), coef});
        }
      } else {
        if (!ignore_irrelevant) TRIQS_RUNTIME_ERROR << "extract_U_dict2: monomial must have 4 operators";
      }
    }

    return U_dict;
  }

  /**
   * @brief Extract the coefficients of a general two-particle interaction operator.
   *
   * @details Assumes that the operator \f$ \hat{h} \f$ has the two-particle form
   * \f[
   *   \hat{h} = \frac{1}{2} \sum_{ijkl} U_{ijkl} \hat{c}_i^\dagger \hat{c}_j^\dagger \hat{c}_l \hat{c}_k \; .
   * \f]
   * Each input term is matched against the canonical normal-ordered pattern \f$ \hat{c}_i^\dagger
   * \hat{c}_j^\dagger \hat{c}_l \hat{c}_k \f$ and the four index permutations equivalent under fermionic
   * antisymmetry are written to the output map with the appropriate sign, so that the returned
   * dictionary respects the antisymmetry of \f$ U_{ijkl} \f$ in \f$ (i, j) \f$ and in \f$ (k, l)
   * \f$.
   *
   * If a term that is not of this form is encountered, an exception is thrown unless
   * `ignore_irrelevant` is `true`, in which case the offending term is silently skipped.
   *
   * @tparam scalar_t Scalar type of the coefficients.
   * @param h Many-body operator \f$ \hat{h} \f$.
   * @param ignore_irrelevant If `true`, terms that do not match the expected form are skipped
   * instead of triggering an exception.
   * @return Dictionary of non-vanishing coefficients \f$ U_{ijkl} \f$.
   */
  template <typename scalar_t> dict4_t<scalar_t> extract_U_dict4(op_t<scalar_t> const &h, bool ignore_irrelevant = false) {

    auto U_dict = dict4_t<scalar_t>{};

    for (auto const &term : h) {
      scalar_t const &coef = term.coef;
      auto const &m        = term.monomial;

      if (m.size() == 4) {
        if (!(m[0].dagger && m[1].dagger && !m[2].dagger && !m[3].dagger)) {
          if (!ignore_irrelevant) TRIQS_RUNTIME_ERROR << "extract_U_dict4: monomial is not of the form C^+(i) C^+(j) C(l) C(k)";
        } else { // everything ok
          U_dict.insert({std::make_tuple(m[0].indices, m[1].indices, m[3].indices, m[2].indices), scalar_t(0.5) * coef});
          U_dict.insert({std::make_tuple(m[1].indices, m[0].indices, m[2].indices, m[3].indices), scalar_t(0.5) * coef});
          U_dict.insert({std::make_tuple(m[0].indices, m[1].indices, m[2].indices, m[3].indices), scalar_t(-0.5) * coef});
          U_dict.insert({std::make_tuple(m[1].indices, m[0].indices, m[3].indices, m[2].indices), scalar_t(-0.5) * coef});
        }
      } else {
        if (!ignore_irrelevant) TRIQS_RUNTIME_ERROR << "extract_U_dict4: monomial must have 4 operators";
      }
    }

    return U_dict;
  }

  /**
   * @brief Convert a coefficient dictionary into a dense rank-`N` array indexed by integers from a
   * fundamental operator set.
   *
   * @details For a dictionary mapping each key tuple \f$ (\alpha_{i_1}, \dots, \alpha_{i_N}) \f$ to
   * a value \f$ x \f$, the result is a rank-`N` array of element type `ValueType` whose entry at
   * \f$ (\mathtt{fs}[\alpha_{i_1}], \dots, \mathtt{fs}[\alpha_{i_N}]) \f$ is set to \f$ x \f$. All
   * other entries are value-initialized to `ValueType{}`.
   *
   * The dimension along every axis is `fs.size()`. The function throws if any index from `dict` is
   * not present in `fs`.
   *
   * @tparam ValueType Element type of the resulting array.
   * @tparam DictType Type of the input dictionary; its `key_type` must be a `std::tuple` of
   * triqs::operators::utils::indices_t.
   * @param dict Coefficient dictionary to convert.
   * @param fs Fundamental operator set used to map each index to an integer.
   * @return Dense rank-`N` array containing the converted coefficients.
   */
  template <typename ValueType = double, typename DictType>
  array<ValueType, std::tuple_size<typename DictType::key_type>::value> dict_to_matrix(DictType const &dict,
                                                                                       hilbert_space::fundamental_operator_set const &fs) {

    using namespace triqs::tuple;
    using triqs::hilbert_space::format_indices;
    using matrix_t = array<ValueType, std::tuple_size<typename DictType::key_type>::value>;

    auto indices_to_linear = [&fs](indices_t const &indices) {
      if (!fs.has_indices(indices))
        TRIQS_RUNTIME_ERROR << "dict_to_matrix: key [" << format_indices(indices) << "] of dict not in fundamental_operator_set/gf_struct";
      return fs[indices];
    };

    auto dims = make_tuple_repeat<std::tuple_size<typename DictType::key_type>::value>(fs.size());
    auto mat  = apply_construct_parenthesis<matrix_t>(dims);
    mat()     = ValueType{};

    for (auto const &kv : dict) triqs::tuple::apply(mat, map(indices_to_linear, kv.first)) = ValueType(kv.second);

    return mat;
  }

  ///////////////////////////////////////////////////
  // Functions for scalar_t = real_or_complex only //
  ///////////////////////////////////////////////////

  /**
   * @brief Convert a `real_or_complex`-valued coefficient dictionary into a `std::variant` of a real
   * and a complex dense array.
   *
   * @details If every value in `dict` is purely real, the result holds a `array<double, N>`;
   * otherwise it holds a `array<std::complex<double>, N>`. The shape of the array follows the same
   * convention as triqs::operators::utils::dict_to_matrix().
   *
   * @tparam DictType Type of the input dictionary.
   * @param dict Coefficient dictionary to convert.
   * @param fs Fundamental operator set used to map each index to an integer.
   * @return Variant containing either a real or a complex dense array, depending on `dict`.
   */
  template <typename DictType>
  real_or_complex_array<std::tuple_size<typename DictType::key_type>::value>
  dict_to_variant_matrix(DictType const &dict, hilbert_space::fundamental_operator_set const &fs) {

    for (auto const &kv : dict) {
      if (!kv.second.is_real()) return dict_to_matrix<std::complex<double>>(dict, fs);
    }
    return dict_to_matrix<double>(dict, fs);
  }

  /**
   * @brief Keep only the terms of a given length from a many-body operator.
   *
   * @details Returns a copy of \f$ \hat{h} \f$ consisting of those monomials whose length (number
   * of canonical operators) is exactly `len`.
   *
   * @tparam scalar_t Scalar type of the coefficients.
   * @param h Many-body operator \f$ \hat{h} \f$.
   * @param len Required monomial length.
   * @return Many-body operator containing only the matching terms.
   */
  template <typename scalar_t> op_t<scalar_t> filter_op(op_t<scalar_t> const &h, long len) {

    auto h_filtered = op_t<scalar_t>{};

    for (auto const &term : h) {
      if (term.monomial.size() == len) h_filtered += term;
    }

    return h_filtered;
  }

  /**
   * @brief Keep only the quadratic terms of a many-body operator \f$ \hat{h} \f$.
   *
   * @tparam scalar_t Scalar type of the coefficients.
   * @param h Many-body operator \f$ \hat{h} \f$.
   * @return Many-body operator containing only the quadratic terms.
   */
  template <typename scalar_t> op_t<scalar_t> quadratic_terms(op_t<scalar_t> const &h) { return filter_op(h, 2); }

  /**
   * @brief Keep only the quartic terms of a many-body operator \f$ \hat{h} \f$.
   *
   * @tparam scalar_t Scalar type of the coefficients.
   * @param h Many-body operator \f$ \hat{h} \f$.
   * @return Many-body operator containing only the quartic terms.
   */
  template <typename scalar_t> op_t<scalar_t> quartic_terms(op_t<scalar_t> const &h) { return filter_op(h, 4); }

  /**
   * @brief Convert a block-diagonal quadratic operator into its block-matrix representation.
   *
   * @details Assumes that \f$ \hat{h} \f$ has the form
   * \f[
   *   \hat{h} = \sum_{\sigma ij} h_{\sigma ij} \hat{c}_{\sigma, i}^\dagger \hat{c}_{\sigma, j} \; ,
   * \f]
   * where the first element of each canonical operator's index is interpreted as the block label \f$ \sigma \f$ (a 
   * string) and the second element as the in-block integer index \f$ i \f$.
   *
   * If a term that is not of this form is encountered, an exception is thrown unless ``ignore_irrelevant`` is `true`, 
   * in which case the offending term is silently skipped.
   *
   * @tparam scalar_t Scalar type of the coefficients.
   * @param h Many-body operator \f$ \hat{h} \f$.
   * @param gf_struct Block structure specifying the block labels and the size of each block.
   * @param ignore_irrelevant If `true`, terms that do not match the expected form are skipped instead of triggering an 
   * exception.
   * @return One matrix per block, packaged as a one-dimensional array of matrices.
   */
  template <typename scalar_t>
  nda::array<nda::matrix<scalar_t>, 1> block_matrix_from_op(op_t<scalar_t> const &h, hilbert_space::gf_struct_t const &gf_struct,
                                                            bool ignore_irrelevant = false) {

    int n_bl    = gf_struct.size();
    auto bl_mat = nda::array<nda::matrix<scalar_t>, 1>(n_bl);
    for (auto bl : range(n_bl)) {
      auto bl_size = gf_struct[bl].second;
      bl_mat[bl]   = nda::zeros<scalar_t>(bl_size, bl_size);
    }

    auto name_to_bl = [&](auto &bl_name) {
      auto it =
         std::find_if(cbegin(gf_struct), cend(gf_struct), [&bl_name](auto const &blname_and_size) { return bl_name == blname_and_size.first; });
      return std::distance(cbegin(gf_struct), it);
    };

    for (auto const &term : h) {
      auto const &m        = term.monomial;
      scalar_t const &coef = term.coef;

      if (m.size() == 2 and m[0].dagger and not m[1].dagger and m[0].indices[0] == m[1].indices[0] and m[0].indices.size() == 2
          and m[1].indices.size() == 2) {
        auto bl_name = std::get<std::string>(m[0].indices[0]);
        auto op1_idx = std::get<long>(m[0].indices[1]);
        auto op2_idx = std::get<long>(m[1].indices[1]);

        bl_mat[name_to_bl(bl_name)](op1_idx, op2_idx) = coef;
      } else {
        if (!ignore_irrelevant) TRIQS_RUNTIME_ERROR << "block_matrix_from_op: Operator term is not of the form 'coeff * c_dag{bl,i} * c_{bl,j}'";
        ;
      }
    }

    return bl_mat;
  }

  /**
   * @brief Build a block-diagonal quadratic operator from its block-matrix representation.
   *
   * @details Given the block matrices \f$ h_{\sigma ij} \f$ and the block structure, returns
   * \f[
   *   \hat{h} = \sum_{\sigma ij} h_{\sigma ij} \hat{c}_{\sigma, i}^\dagger \hat{c}_{\sigma, j} \; .
   * \f]
   *
   * @tparam scalar_t Scalar type of the matrices and of the resulting operator.
   * @param bl_mat One matrix \f$ h_{\sigma ij} \f$ per block \f$ \sigma \f$, packaged as a one-dimensional array of 
   * matrices.
   * @param gf_struct Block structure.
   * @return Many-body operator \f$ \hat{h} \f$.
   */
  template <typename scalar_t>
  op_t<scalar_t> op_from_block_matrix(nda::array<nda::matrix<scalar_t>, 1> const &bl_mat, hilbert_space::gf_struct_t const &gf_struct) {

    EXPECTS(bl_mat.size() == gf_struct.size());

    auto h = op_t<scalar_t>{};

    int n_bl = gf_struct.size();
    for (auto bl : range(n_bl)) {

      auto [bl_name, bl_size] = gf_struct[bl];

      EXPECTS(bl_mat[bl].shape() == (std::array{bl_size, bl_size}));

      for (auto [i, j] : itertools::product_range(bl_size, bl_size)) h += bl_mat[bl](i, j) * c_dag(bl_name, i) * c(bl_name, j);
    }

    return h;
  }

  /** @} */

} // namespace triqs::operators::utils
