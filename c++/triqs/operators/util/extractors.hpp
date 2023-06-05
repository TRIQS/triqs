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
// Authors: Michel Ferrero, Igor Krivenko, Olivier Parcollet, Nils Wentzell, Thomas Ayral

#pragma once
#include <variant>
#include <triqs/utility/first_include.hpp>
#include <triqs/utility/tuple_tools.hpp>
#include <triqs/operators/many_body_operator.hpp>
#include <triqs/hilbert_space/fundamental_operator_set.hpp>
#include <triqs/arrays.hpp>

namespace triqs::operators::utils {

  using nda::array;

  using indices_t = hilbert_space::fundamental_operator_set::indices_t;

  // Shorthand for many_body_operator_generic
  template <typename scalar_t> using op_t = operators::many_body_operator_generic<scalar_t>;

  // Mapping index pair -> coefficient of T
  template <typename T> using dict2_t = std::map<std::tuple<indices_t, indices_t>, T>;
  // Mapping quadruple -> coefficient of T
  template <typename T> using dict4_t = std::map<std::tuple<indices_t, indices_t, indices_t, indices_t>, T>;

  template <int N> using real_or_complex_array = std::variant<array<double, N>, array<std::complex<double>, N>>;

  /// Extract coefficients from an operator assuming it is a normal quadratic form of canonical operators
  /**
 * The normal quadratic form of canonical operators is defined as
 *
 * .. math:: \sum_{ij} h_{ij} c_i^\dagger c_j.
 *
 * An exception will be thrown if a term of a different form is met,
 * unless `ignore_irrelevant` argument is set to `true`.
 *
 * @param h subject operator for coefficient extraction.
 * @param ignore_irrelevant do not throw exception if an irrelevant term is met in `h`.
 * @return All non-vanishing coefficients :math:`h_{ij}` as dictionary object.
 * @include triqs/operators/util/extractors.hpp
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

  /// Extract coefficients from a density-density interaction operator
  /**
 * The density-density interaction operator is defined as
 *
 * .. math:: \frac{1}{2} \sum_{ij} U_{ij} n_i n_j.
 *
 * An exception will be thrown if a term of a different form is met,
 * unless `ignore_irrelevant` argument is set to `true`.
 *
 * @param h subject operator for coefficient extraction.
 * @param ignore_irrelevant do not throw exception if an irrelevant term is met in `h`.
 * @return All non-vanishing coefficients :math:`U_{ij}` as dictionary object.
 * @include triqs/operators/util/extractors.hpp
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

  /// Extract coefficients from a two-particle interaction operator
  /**
 * The two-particle interaction operator is defined as
 *
 * .. math:: \frac{1}{2} \sum_{ijkl} U_{ijkl} c_i^\dagger c_j^\dagger c_l c_k
 *
 * An exception will be thrown if a term of a different form is met,
 * unless `ignore_irrelevant` argument is set to `true`.
 *
 * @param h subject operator for coefficient extraction.
 * @param ignore_irrelevant do not throw exception if an irrelevant term is met in `h`.
 * @return All non-vanishing coefficients :math:`U_{ijkl}` as dictionary object.
 * @include triqs/operators/util/extractors.hpp
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

  /// Convert dictionary of coefficients to matrix/tensor, given a fundamental operator set
  /**
 * For a given dictionary `dict: (ind_1,ind_2,...,ind_N) -> x` returns an array of rank `N`
 * with elements of type `ValueType`. Each key `(ind_1,ind_2,...,ind_N)` is mapped to a
 * tuple of integers `(i_1,i_2,...,i_N)` using a given fundamental operator set `fs`.
 * The element of the resulting array addressed by `(i_1,i_2,...,i_N)` is then set to `x`.
 * The rest of elements, which have no corresponding keys in `dict`, are value-initialized
 * (set to `ValueType{}`).
 *
 * An exception is thrown if some index `ind_n` is missing from `fs`.
 *
 * @tparam ValueType element type of the resulting matrix/tensor
 * @tparam DictType type of the dictionary to convert
 * @param dict dictionary to convert
 * @param fs fundamental operator set used for conversion
 * @return Matrix/tensor, result of conversion
 * @include triqs/operators/util/extractors.hpp
 */
  template <typename ValueType = double, typename DictType>
  array<ValueType, std::tuple_size<typename DictType::key_type>::value> dict_to_matrix(DictType const &dict,
                                                                                       hilbert_space::fundamental_operator_set const &fs) {

    using namespace triqs::tuple;
    using matrix_t = array<ValueType, std::tuple_size<typename DictType::key_type>::value>;

    auto indices_to_linear = [&fs](indices_t const &indices) {
      if (!fs.has_indices(indices))
        TRIQS_RUNTIME_ERROR << "dict_to_matrix: key [" << indices << "] of dict not in fundamental_operator_set/gf_struct";
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

  /// Convert dictionary of real_or_complex coefficients to variant of real/complex matrix, given a fundamental operator set
  ///
  /// dict: dictionary to convert
  /// fs: fundamental operator set used for conversion
  template <typename DictType>
  real_or_complex_array<std::tuple_size<typename DictType::key_type>::value>
  dict_to_variant_matrix(DictType const &dict, hilbert_space::fundamental_operator_set const &fs) {

    for (auto const &kv : dict) {
      if (!kv.second.is_real()) return dict_to_matrix<std::complex<double>>(dict, fs);
    }
    return dict_to_matrix<double>(dict, fs);
  }

  /**
   * Filter out terms of given length from an operator
   *
   * @param h subject operator
   * @param The length of the operator terms to filter
   * @param ignore_irrelevant do not throw exception if an irrelevant term is met in `h`.
   * @return Operator containing only the terms of given length
   * @include triqs/operators/util/extractors.hpp
   */
  template <typename scalar_t> op_t<scalar_t> filter_op(op_t<scalar_t> const &h, long len) {

    auto h_filtered = op_t<scalar_t>{};

    for (auto const &term : h) {
      if (term.monomial.size() == len) h_filtered += term;
    }

    return h_filtered;
  }

  /**
   * Filter out quadratic terms from an operator
   *
   * @param h subject operator
   * @param ignore_irrelevant do not throw exception if an irrelevant term is met in `h`.
   * @return Operator containing only the quadratic terms
   * @include triqs/operators/util/extractors.hpp
   */
  template <typename scalar_t> op_t<scalar_t> quadratic_terms(op_t<scalar_t> const &h) { return filter_op(h, 2); }

  /**
   * Filter out quartic terms from an operator
   *
   * @param h subject operator
   * @param ignore_irrelevant do not throw exception if an irrelevant term is met in `h`.
   * @return Operator containing only the quartic terms
   * @include triqs/operators/util/extractors.hpp
   */
  template <typename scalar_t> op_t<scalar_t> quartic_terms(op_t<scalar_t> const &h) { return filter_op(h, 4); }

  /**
   * Convert the quadratic operator
   *
   * .. math:: \sum_{\sigma ij} h_{\sigma ij} c_{\sigma, i}^\dagger c_{\sigma, j}.
   *
   * into its block-matrix representation
   *
   * @param h subject operator
   * @param gf_struct The object defining the block-structure
   * @param ignore_irrelevant do not throw exception if an irrelevant term is met in `h`.
   * @return The block-matrix representation h_{\sigma ij}
   * @include triqs/operators/util/extractors.hpp
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
          or m[1].indices.size() == 2) {
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
   * Convert the block-matrix h_{\sigma ij} into the associated operator
   *
   * .. math:: \sum_{\sigma ij} h_{\sigma ij} c_{\sigma, i}^\dagger c_{\sigma, j}.
   *
   * @param bl_mat subject block_matrix
   * @param gf_struct The object defining the block-structure
   * @return The associated operator
   * @include triqs/operators/util/extractors.hpp
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

} // namespace triqs::operators::utils
