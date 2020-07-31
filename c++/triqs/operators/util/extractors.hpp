// Copyright (c) 2015-2018 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
// Copyright (c) 2015-2018 Centre national de la recherche scientifique (CNRS)
// Copyright (c) 2015-2017 Igor Krivenko
// Copyright (c) 2018-2020 Simons Foundation
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
// Authors: Igor Krivenko, Olivier Parcollet, Nils Wentzell, tayral

#pragma once
#include <variant>
#include <triqs/utility/first_include.hpp>
#include <triqs/utility/tuple_tools.hpp>
#include <triqs/operators/many_body_operator.hpp>
#include <triqs/hilbert_space/fundamental_operator_set.hpp>
#include <triqs/arrays.hpp>

namespace triqs::operators::utils {

  using triqs::arrays::array;

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

} // namespace triqs::operators::utils
